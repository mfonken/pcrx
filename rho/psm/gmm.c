//
//  gmm.c
//  GaussianMixtureModel
//
//  Created by Matthew Fonken on 2/9/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//
#ifdef __PSM__
#include "gmm.h"

void InitializeGaussianMixtureCluster( gaussian_mixture_cluster_t * cluster, observation_t * observation, vec2 * output )
{
    if(cluster == NULL) return;
    cluster->gaussian_in.mean.a = observation->density;
    cluster->gaussian_in.mean.b = observation->thresh;
    cluster->gaussian_out.mean.a = output->a;
    cluster->gaussian_out.mean.b = output->b;
    
    memset( &cluster->gaussian_out.covariance, 0, sizeof(mat2x2) );
    cluster->gaussian_in.covariance = (mat2x2){ INITIAL_VARIANCE, 0, 0, INITIAL_VARIANCE };
    cluster->inv_covariance_in = (mat2x2){ INV_INITIAL_VARIANCE, 0, 0, INV_INITIAL_VARIANCE };
    cluster->score = 1.;
    
    memset( &cluster->labels, 0, sizeof(cluster->labels) );
    cluster->labels.average[observation->label] = 1;
    cluster->labels.count[observation->label]++;
    
    GetMat2x2LLT( &cluster->gaussian_in.covariance, &cluster->llt_in );
    
    GMMFunctions.Cluster.UpdateNormal( cluster );
}
void UpdateGaussianMixtureCluster( gaussian_mixture_cluster_t * cluster, observation_t * observation, vec2 * output )
{
    if (isnan(cluster->log_gaussian_norm_factor))
        return;
//    LOG_GMM(GMM_DEBUG, "m_maha: %.2f\n", cluster->mahalanobis_sq);
    if (cluster->mahalanobis_sq > MAX_MAHALANOBIS_SQ_FOR_UPDATE)
        return;
    double score_weight = ALPHA * safe_exp( -BETA * cluster->mahalanobis_sq );
    cluster->score += score_weight * ( cluster->probability_condition_input - cluster->score );
    
    double weight = ALPHA * cluster->probability_condition_input;
    
    vec2 delta_mean_in = WeightedIncreaseMean( (vec2 *)observation, &cluster->gaussian_in, weight );
    vec2 delta_mean_out = WeightedIncreaseMean( output, &cluster->gaussian_out, weight );
    
    LOG_GMM(GMM_DEBUG, "m_mean: [%.2f %.2f]\n", cluster->gaussian_in.mean.a, cluster->gaussian_in.mean.b);
    
    UpdateGaussianWithWeight( &delta_mean_in, &delta_mean_in,  &cluster->gaussian_in,  weight );
    UpdateGaussianWithWeight( &delta_mean_in, &delta_mean_out, &cluster->gaussian_out, weight );
    
    GetMat2x2LLT( &cluster->gaussian_in.covariance, &cluster->llt_in );
    getMat2x2Inverse( &cluster->gaussian_in.covariance, &cluster->inv_covariance_in );
    
    GMMFunctions.Cluster.UpdateNormal( cluster );
    GMMFunctions.Cluster.UpdateLimits( cluster );
    
    ReportLabel( &cluster->labels, observation->label );
    
    cluster->timestamp = TIMESTAMP();
}
void GetScoreOfGaussianMixtureCluster( gaussian_mixture_cluster_t * cluster, vec2 * input)
{
    vec2 input_delta = { 0 };
    vec2SubVec2(input, &cluster->gaussian_in.mean, &input_delta);
    cluster->mahalanobis_sq = CalculateMahalanobisDistanceSquared( &cluster->inv_covariance_in, &input_delta);
    if(cluster->mahalanobis_sq < 1)
    {
        //printf("#");
    }
    cluster->probability_of_in = safe_exp( cluster->log_gaussian_norm_factor - 0.5 * cluster->mahalanobis_sq );
}
void UpdateNormalOfGaussianMixtureCluster( gaussian_mixture_cluster_t * cluster )
{
    LOG_GMM(GMM_DEBUG, "m_norm: [%.2f %.2f | %.2f %.2f]", cluster->llt_in.a, cluster->llt_in.b, cluster->llt_in.c, cluster->llt_in.d);
    double cholesky_dms = cluster->llt_in.a * cluster->llt_in.d,
    norm_factor = -log( 2 * M_PI * sqrt( cluster->llt_in.a ) * sqrt( cluster->llt_in.d ) );
    LOG_GMM(GMM_DEBUG, " %.2f %.2f\n", cholesky_dms, norm_factor);
    cluster->log_gaussian_norm_factor = norm_factor;
    if(isnan(norm_factor))
    {
        //printf("!");
    }
}
void UpdateInputProbabilityOfGaussianMixtureCluster( gaussian_mixture_cluster_t * cluster, double total_probability )
{
    cluster->probability_condition_input = total_probability > MIN_TOTAL_MIXTURE_PROBABILITY
    ? ZDIV( cluster->probability_of_in, total_probability ) : 0.f;
}
void ContributeToOutputOfGaussianMixtureCluster( gaussian_mixture_cluster_t * cluster, vec2 * input, vec2 * output )
{
    vec2 input_delta;
    vec2SubVec2(input, &cluster->gaussian_in.mean, &input_delta);
    vec2 inv_covariance_delta = { 0 };
    mat2x2DotVec2(&cluster->inv_covariance_in, &input_delta, &inv_covariance_delta);
    
    vec2 input_covariance, pre_condition = { 0 }, pre_output;
    mat2x2 cov_out_T = { cluster->gaussian_out.covariance.a, cluster->gaussian_out.covariance.c,
                         cluster->gaussian_out.covariance.b, cluster->gaussian_out.covariance.d };
    mat2x2DotVec2( &cov_out_T, &inv_covariance_delta, &input_covariance );
    vec2AddVec2( &cluster->gaussian_out.mean, &input_covariance, &pre_condition);
    scalarMulVec2( cluster->probability_condition_input, &pre_condition, &pre_output );
    vec2AddVec2( output, &pre_output, output);
}
void UpdateLimitsOfGaussianMixtureCluster( gaussian_mixture_cluster_t * cluster )
{
    double radius_y = cluster->gaussian_in.covariance.d * VALID_CLUSTER_STD_DEV;
    cluster->max_y = cluster->gaussian_in.mean.b + radius_y;
    cluster->min_y = cluster->gaussian_in.mean.b - radius_y;
}
void WeighGaussianMixtureCluster( gaussian_mixture_cluster_t * cluster )
{
    /* Find best two label contributes */
    double first = cluster->labels.average[0], second = 0.;
    for( uint8_t i = 1; i < MAX_LABELS; i++ )
    {
        double check = cluster->labels.average[i];
        if( check > first )
        {
            second = first;
            first = check;
        }
        else if ( check > second )
            second = check;
    }
    double a = ( cluster->gaussian_in.covariance.b * cluster->gaussian_in.covariance.c ),
    b = ( cluster->gaussian_in.covariance.a * cluster->gaussian_in.covariance.d );
    double eccentricity_factor = ZDIV( a, b );
    cluster->weight = ( first + second ) * eccentricity_factor;
    cluster->primary_id = first;
    cluster->secondary_id = second;
}

void InitializeGaussianMixtureModel( gaussian_mixture_model_t * model )
{
    memset( model, 0, sizeof(gaussian_mixture_model_t) );
    for( uint16_t i = 0; i < MAX_CLUSTERS; i++ )
        model->cluster[i] = &(model->cluster_mem[i]);
}
double GetScoreSumOfClustersInGaussianMixtureModel( gaussian_mixture_model_t * model, vec2 * input )
{
    double score_sum = 0;
    gaussian_mixture_cluster_t * cluster;
    for( uint8_t i = 0; i < model->num_clusters; i++)
    {
        cluster = model->cluster[i];
        GMMFunctions.Cluster.GetScore( cluster, input );
        score_sum += cluster->probability_of_in;
    }
    return score_sum;
}
double GetOutputAndBestDistanceOfGaussianMixtureModel( gaussian_mixture_model_t * model, double total_probability, vec2 * input, vec2 * output )
{
    double best_match_distance = MAX_DISTANCE;
    gaussian_mixture_cluster_t * cluster;
    for( uint8_t i = 0; i < model->num_clusters; i++)
    {
        cluster = model->cluster[i];
        GMMFunctions.Cluster.UpdateInputProbability( cluster, total_probability );
        if( cluster->score > MIN_CLUSTER_SCORE)
            GMMFunctions.Cluster.ContributeToOutput( cluster, input, output );
        if( cluster->mahalanobis_sq < best_match_distance )
            best_match_distance = cluster->mahalanobis_sq;
    }
    return best_match_distance;
}
double GetMaxErrorOfGaussianMixtureModel( gaussian_mixture_model_t * model, vec2 * output, vec2 * value, vec2 * min_max_delta )
{
    vec2 output_delta;
    vec2SubVec2( value, output, &output_delta );
    double a_error = fabs( ZDIV( output_delta.a, min_max_delta->a ) ),
    b_error = fabs( ZDIV( output_delta.b, min_max_delta->b ) );
    return MAX( a_error, b_error );
}
void AddClusterToGaussianMixtureModel( gaussian_mixture_model_t * model, observation_t * observation, vec2 * value )
{
    uint16_t i = 0;
    for( ; i < model->num_clusters; i++ )
        if( model->cluster[i] == NULL ) break;
    GMMFunctions.Cluster.Initialize( model->cluster[i], observation, value );
    model->num_clusters++;
}
void UpdateGaussianMixtureModel( gaussian_mixture_model_t * model, observation_t * observation, vec2 * value )
{
    gaussian_mixture_cluster_t * cluster;
    for( uint8_t i = 0; i < model->num_clusters; i++ )
    {
        cluster = model->cluster[i];
        GMMFunctions.Cluster.Update( cluster, observation, value );
    }
    for( uint8_t i = 0; i < model->num_clusters; i++ )
    {
        gaussian_mixture_cluster_t * cluster = model->cluster[i];
        if( cluster->score < MIN_CLUSTER_SCORE
           || ISTIMEDOUT( cluster->timestamp, MAX_CLUSTER_LIFETIME )
           || isnan(cluster->log_gaussian_norm_factor))
            GMMFunctions.Model.RemoveCluster( model, i );
    }
}
void AddValueToGaussianMixtureModel( gaussian_mixture_model_t * model, observation_t * observation, vec2 * value )
{
    if( !model->num_clusters )
    {
        model->min_in = *(vec2 *)( observation );
        model->max_in = model->min_in;
        model->min_out = *value;
        model->max_out = model->min_out;
    }
    else
    {
        model->min_in  = (vec2){ MIN( model->min_in.a, observation->density ), MIN( model->min_in.b, observation->thresh ) };
        model->max_in  = (vec2){ MAX( model->max_in.a, observation->density ), MAX( model->max_in.b, observation->thresh ) };
        model->min_out = (vec2){ MIN( model->min_out.a, value->a ), MIN( model->min_out.b, value->b ) };
        model->max_out = (vec2){ MAX( model->max_out.a, value->a ), MAX( model->max_out.b, value->b ) };
    }
    vec2 output = { 0., 0. };
    vec2 observation_vec = (vec2){ (double)observation->density, (double)observation->thresh };
    double total_probability = GMMFunctions.Model.GetScoreSumOfClusters( model, &observation_vec );
    double best_distance = GMMFunctions.Model.GetOutputAndBestDistance( model, total_probability, &observation_vec, &output);
    
    vec2 min_max_delta;
    vec2SubVec2( &model->max_out, &model->min_out, &min_max_delta );
    double max_error = GMMFunctions.Model.GetMaxError( model, &output, value, &min_max_delta );
    
    GMMFunctions.Model.Update( model, observation, value );
    LOG_GMM(GMM_DEBUG, "Max error: %.2f\n", max_error);
    /* Add cluster if error or distance is to high for a cluster match */
    if( model->num_clusters < MAX_CLUSTERS
       && ( !model->num_clusters
           || ( ( max_error > MAX_ERROR )
               && ( best_distance > MAX_MAHALANOBIS_SQ ) ) ) )
        GMMFunctions.Model.AddCluster( model, observation, value );
}

void RemovClusterFromGaussianMixtureModel( gaussian_mixture_model_t * model, uint16_t index )
{
    // Replace pointer to be removed by the last pointer
    model->cluster[index] = model->cluster[--model->num_clusters];
}

#endif
