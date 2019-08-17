//
//  psm.c
//  hmmtest
//
//  Created by Matthew Fonken on 2/12/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//


#ifdef __PSM__

#include "psm.h"

void InitializePSM( psm_t * model )
{
    GMMFunctions.Model.Initialize( &model->gmm );
    HMMFunctions.Initialize( &model->hmm, MANY_SYMBOL );
    KumaraswamyFunctions.Initialize( &model->kumaraswamy, NUM_STATES + 1 );
    model->state_bands.length = NUM_STATE_GROUPS;
    
#ifdef __PSM__
    floating_t default_bands_intervals[] = STATE_KUMARASWAMY_INTERVALS;
    floating_t prev_boundary = 0., vertical_center = 0;
    for( uint8_t i = 0; i < NUM_STATE_GROUPS; i++ )
    {
        band_t * band = &model->state_bands.band[i];
        band->lower_boundary = prev_boundary;
        band->upper_boundary = default_bands_intervals[i];
        vertical_center = ( ( (floating_t)( band->upper_boundary + band->lower_boundary ) ) * THRESH_RANGE ) / 2;
        band->true_center = (vec2){ 0, vertical_center };
    }
#else
    memset( model->state_bands.band, 0, sizeof(model->state_bands.band) );
#endif
}

void ReportObservationsPSM( psm_t * model, observation_list_t * observation_list )
{
    /* Cycle through and add observations to gaussian mixture model */
    vec2 value = { 0 };
    for( uint8_t i = 0; i < observation_list->length && i < MAX_OBSERVATIONS; i++ )
    {
        observation_t * observation = &observation_list->observations[i];
        value = (vec2){ (double)observation->density, (double)observation->thresh };
        GMMFunctions.Model.AddValue( &model->gmm, observation, &value );
    }
    HMMFunctions.ReportObservation( &model->hmm, model->current_observation );
    
    if( observation_list->length > 0 )
        model->previous_thresh = observation_list->observations[0].thresh;
    /// TODO: Analyze value
//    printf("a:%p\n", &model->gmm);
}

void UpdateStateIntervalsPSM( psm_t * model, floating_t nu )
{
    floating_t observation_set[NUM_STATE_GROUPS], cumulative = 0.;
    LOG_PSM(PSM_DEBUG, "Hidden:");
    for( uint8_t i = 0; i < NUM_STATE_GROUPS; i++ )
    {
        cumulative += model->hmm.B.expected[model->current_observation][i];
        observation_set[i] = cumulative;
        LOG_PSM_BARE(PSM_DEBUG, "[%.4f]", model->hmm.B.expected[model->current_observation][i]);
    }
    LOG_PSM_BARE(PSM_DEBUG, "\n");
    KumaraswamyFunctions.GetVector( &model->kumaraswamy, nu, model->state_intervals, observation_set, NUM_STATE_GROUPS );
    
    LOG_PSM(PSM_DEBUG, "Update:");
    for( uint8_t i = 0; i < NUM_STATE_GROUPS; i++ )
    {
        LOG_PSM_BARE(PSM_DEBUG, "[%.4f]", model->state_intervals[i]);
    }
    LOG_PSM_BARE(PSM_DEBUG, "\n");
}

void UpdatePSM( psm_t * model, observation_list_t * observation_list, floating_t nu )
{
    /* Report tracking observations & update state band knowledge  */
    if( observation_list->length > 0 )
    {
        model->current_observation = round(nu);
        if(model->current_observation >= MANY_SYMBOL)
            model->current_observation = MANY_SYMBOL;
        PSMFunctions.ReportObservations( model, observation_list );
    }
    
    /* Calculate current observation and update observation matrix */
    PSMFunctions.DiscoverStateBands( model, &model->state_bands );
    model->observation_state = PSMFunctions.GetCurrentBand( model, &model->state_bands );
    
    ///TODO: Compare model->observation_state with hmm recommendation
    
    /* Update state path prediction to best cluster */
    HMMFunctions.BaumWelchGammaSolve( &model->hmm );
    HMMFunctions.UpdateObservationMatrix( &model->hmm );
    
//    if(count++ % 10 == 0)
    {
        /* Update state bands */
        PSMFunctions.UpdateStateIntervals( model, nu );
        
        /* Update states/transition matrix */
        FSMFunctions.Sys.Update( &model->hmm.A, model->state_intervals );
        model->current_state = model->hmm.A.state;
    }
    
    /* Generate proposals to complete update */
    PSMFunctions.GenerateProposals( model );
}

void UpdateStateBandPSM( band_list_t * band_list, uint8_t i, int8_t c, gaussian2d_t * band_gaussian )
{
    if( i > band_list->length ) i = band_list->length - 1;
    if( c == 0 )
    { /* If no gaussian for band, zero state info */
        if( !i )
            band_list->band[i] = (band_t){ THRESH_MAX, THRESH_MAX,  0., (vec2){ 0., 0. } };
        else
            memcpy( &band_list->band[i], &band_list->band[i-1], sizeof(band_t) );
    }
    else if( c == -1 )
    {
        floating_t boundary = band_list->band[i-1].upper_boundary;
        band_list->band[i].lower_boundary = boundary;
        band_list->band[i].upper_boundary = boundary;
        band_list->band[i].true_center = (vec2){ band_list->band[i-1].true_center.a, boundary };
        band_list->band[i].variance = band_list->band[i-1].variance;
    }
    else
    { /* Otherwise set using cumulated band gaussian */
        floating_t radius = band_gaussian->covariance.d * VALID_CLUSTER_STD_DEV * 5;
        band_list->band[i].lower_boundary = band_gaussian->mean.b + radius;
        band_list->band[i].upper_boundary = band_gaussian->mean.b - radius;
        band_list->band[i].true_center = band_gaussian->mean;
        band_list->band[i].variance = band_gaussian->covariance.d;
        if( i )
            band_list->band[i-1].upper_boundary = band_list->band[i].lower_boundary;
    }
}

void DiscoverStateBandsPSM( psm_t * model, band_list_t * band_list )
{
#ifdef SPOOF_STATE_BANDS
    band_list->length = NUM_STATE_GROUPS;
    floating_t prev = 0, curr, center;
    floating_t spoof_bands[] = { 0.25, 0.5, 0.75, 1. };
    floating_t spoof_deviation = 40.;
    for( uint8_t i = 0; i < NUM_STATE_GROUPS; i++ )
    {
        curr = spoof_bands[i] * CAPTURE_HEIGHT;
        center = ( curr + prev ) / 2;
        band_list->band[NUM_STATE_GROUPS - 1 - i] = (band_t){ curr, prev, spoof_deviation, (vec2){ (1 - spoof_bands[i]) * CAPTURE_WIDTH, center } };
        prev = curr;
    }
    return;
#else
    uint32_t processed_clusters = { 0 };
    for( uint8_t i = 0; i < model->gmm.num_clusters; i++ )
        GMMFunctions.Cluster.UpdateLimits( model->gmm.cluster[i] );
    
    gaussian2d_t band_gaussian = { 0 };
    band_list->band[0].lower_boundary = 0;
    /// NOTE: Minimum boundary has greatest y
    int8_t current_band_id = 0, num_clusters_in_band = 0, num_to_process = model->gmm.num_clusters;
    uint32_t running_label_vector = 0;
    while(num_to_process-- > 0)
    {
        /* Get next min cluster */
        uint8_t min_id = 0;
        floating_t min_boundary = 0, check_boundary;
        for(uint32_t i = 0, m = 1; i < model->gmm.num_clusters; i++, m <<= 1 )
        {
            if( processed_clusters & m ) continue;
            if( model->gmm.cluster[i]->gaussian_in.mean.a > CAPTURE_WIDTH ) continue;
            check_boundary = model->gmm.cluster[i]->max_y;
            if( check_boundary > min_boundary )
            {
                min_boundary = check_boundary;
                min_id = i;
            }
        }
        
        /* Check if new cluster has new label(s) */
        uint32_t current_label_vector = GetValidLabels( &model->gmm.cluster[min_id]->labels );
        uint32_t check = current_label_vector & ~running_label_vector;
        if( check )
        { /* If new labels in cluster */
            uint32_t new_label_vector = running_label_vector | current_label_vector;
            /* If new update skipped bands, if any */
            current_band_id = CountSet(new_label_vector);
            
            for( uint8_t i = CountSet(running_label_vector); i < current_band_id; i++ )
            {
                PSMFunctions.UpdateStateBand( band_list, i, num_clusters_in_band, &band_gaussian );
                num_clusters_in_band = -1;
            }
            band_gaussian = model->gmm.cluster[min_id]->gaussian_in;
            num_clusters_in_band = 1;
            running_label_vector = new_label_vector;
        }
        else
        { /* Otherwise cumulate current gaussian into band gaussian */
            double stddevs = NumStdDevsFromYMean( &band_gaussian, model->gmm.cluster[min_id]->gaussian_in.mean.b );
            LOG_PSM(PSM_DEBUG_2,  "bcy: %.4f vs min: %d | stddevs: %.4f\n", band_gaussian.covariance.d, MIN_VARIANCE_SPAN_TO_REJECT_FOR_BAND_CALC, stddevs);
            if( band_gaussian.covariance.d < MIN_VARIANCE_SPAN_TO_REJECT_FOR_BAND_CALC
               || NumStdDevsFromYMean( &band_gaussian, model->gmm.cluster[min_id]->gaussian_in.mean.b )
               < MAX_STD_DEVS_TO_BE_INCLUDED_IN_BAND_CALC )
            {
                LOG_PSM(PSM_DEBUG_2, "Combining cluster %d\n", min_id);
                mulGaussian2d( &band_gaussian, &model->gmm.cluster[min_id]->gaussian_in, &band_gaussian );
                num_clusters_in_band++;
            }
        }
        
        LOG_PSM(PSM_DEBUG_2, "Band %d gaussian is <%.4f %.4f> [%.4f %.4f %.4f %.4f]\n", current_band_id, band_gaussian.mean.a, band_gaussian.mean.b, band_gaussian.covariance.a, band_gaussian.covariance.b, band_gaussian.covariance.c, band_gaussian.covariance.d );
        
        if( !num_to_process )
        { /* Always update band on last cluster */
            PSMFunctions.UpdateStateBand( band_list, current_band_id, num_clusters_in_band, &band_gaussian );
        }

        processed_clusters |= 1 << min_id;
    }
    for( uint8_t i = current_band_id+1; i < band_list->length; i++ )
        PSMFunctions.UpdateStateBand( band_list, i, -1, NULL );
    band_list->band[band_list->length-1].upper_boundary = 0;
    
    if(band_list->length > 0)
    {
        LOG_PSM(PSM_DEBUG, "State bands: \n");
        for( uint8_t i = 0; i < band_list->length; i++ )
            LOG_PSM(PSM_DEBUG, " %d: (%.3f %.3f)  C<%.3f %.3f>[%.3f]\n", i, band_list->band[i].lower_boundary, band_list->band[i].upper_boundary, band_list->band[i].true_center.a, band_list->band[i].true_center.b, band_list->band[i].variance);
        LOG_PSM(PSM_DEBUG, "\n");
    }
#endif
}

uint8_t FindMostLikelyHiddenStatePSM( psm_t * model, uint8_t observation_state, floating_t * confidence )
{
    uint8_t best_observation_id = 0;
    floating_t best_observation_weight = 0.;
    
    /* Determine target observation band */
    for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
    {
        floating_t check = model->hmm.B.expected[observation_state][i];
        if( check > best_observation_weight )
        {
            best_observation_id = i;
            best_observation_weight = check;
        }
    }
    if( confidence != NULL) *confidence = best_observation_weight;
    return best_observation_id;
}

void UpdateBestClusterPSM( psm_t * model, band_list_t * band_list )
{
    floating_t
        lower_bound = band_list->band[model->best_state].lower_boundary,
        upper_bound = ( model->best_state + 1 >= NUM_STATE_GROUPS ) ?
            MAX_THRESH : band_list->band[model->best_state + 1].lower_boundary,
        best_cluster_weight = 0.;
    int8_t best_cluster_id = -1;
    for( uint8_t i = 0; i < model->gmm.num_clusters; i++ )
    {
        gaussian_mixture_cluster_t * cluster = model->gmm.cluster[i];
        if( IN_RANGE( cluster->gaussian_in.mean.b, upper_bound, lower_bound ) )
        {
            GMMFunctions.Cluster.Weigh( cluster );
            if( cluster->weight > best_cluster_weight )
            {
                best_cluster_weight = cluster->weight;
                best_cluster_id = i;
            }
        }
    }
    model->best_cluster_id = best_cluster_id;
    model->best_cluster_weight = best_cluster_weight;
}

uint8_t GetCurrentBandPSM( psm_t * model, band_list_t * band_list )
{
    uint8_t state = 0;
    for( ; state < band_list->length-1; state++ )
    {
        /* If next boundary is above thresh, band is in current thresh */
        if( model->previous_thresh > band_list->band[state+1].lower_boundary )
            break;
    }
    return state;
}

void GenerateProposalsPSM( psm_t * model )
{
    if( !model->gmm.num_clusters ) return;
    
    /* Update current state */
    model->best_state = PSMFunctions.FindMostLikelyHiddenState( model, TARGET_STATE, &model->best_confidence );
    PSMFunctions.UpdateBestCluster( model, &model->state_bands );
    
    if( model->best_cluster_id >= model->gmm.num_clusters
       || model->best_cluster_id < 0) return;
    
    /* Update predictions */
    vec2 * proposed_center = &model->state_bands.band[model->best_state].true_center;
    model->proposed_avg_den = proposed_center->a;
    model->proposed_thresh = proposed_center->b;
    
    /* Update primary & secondary to be reconstructed */
    gaussian_mixture_cluster_t * cluster = model->gmm.cluster[model->best_cluster_id];
    model->proposed_primary_id = cluster->primary_id;
    model->proposed_secondary_id = cluster->secondary_id;
    model->proposed_nu = GetNumberOfValidLabels( &cluster->labels );
}

#endif
