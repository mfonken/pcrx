//
//  hmm.c
//  hmmtest
//
//  Created by Matthew Fonken on 2/10/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifdef __PSM__
#include "hmm.h"

//#define NORMALIZE_BY_STATE

static const char * observation_strings[] = { "O0", "O1", "O2", "O3", "OM" };

void InitializeHiddenMarkovModel( hidden_markov_model_t * model, observation_symbol_t initial_observation )
{
    FSMFunctions.Sys.Initialize( &model->A, UNDER_POPULATED );
    memset( &model->B, 0, sizeof(observation_matrix_t) );
    model->T = 0;
    model->N = NUM_STATE_GROUPS;
    model->M = NUM_OBSERVATION_SYMBOLS;
    
    model->O.prev = initial_observation;

    /* Initialize each observation set as Kumarawwamy distribution */
    kumaraswamy_t kumaraswamy;
    KumaraswamyFunctions.Initialize( &kumaraswamy, NUM_STATE_GROUPS );
    double alpha_step = 1. ;/// kumaraswamy.beta;
    
    double x = alpha_step, observation_alpha[NUM_OBSERVATION_SYMBOLS] = { 0. }, bands[NUM_STATE_GROUPS], intervals[NUM_STATE_GROUPS];
    for( uint8_t i = 0.; i < NUM_OBSERVATION_SYMBOLS; i++, x += alpha_step )
        observation_alpha[i] = x;
        
    for( uint8_t i = 0; i < NUM_STATE_GROUPS; i++)
        bands[i] = (double)(i+1)/(double)(NUM_STATE_GROUPS);
    
    for( uint8_t i = 0; i < NUM_STATE_GROUPS; i++ )
    {
        KumaraswamyFunctions.GetVector( &kumaraswamy, observation_alpha[i], intervals, bands, NUM_STATE_GROUPS);
        memcpy( model->A.probabilities.map[i], intervals, sizeof(double) * NUM_STATE_GROUPS );
    }
    
    kumaraswamy.beta = NUM_OBSERVATION_SYMBOLS;
    for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
    {
        KumaraswamyFunctions.GetVector( &kumaraswamy, observation_alpha[i], intervals, bands, NUM_STATE_GROUPS);
        memcpy( model->B.expected[i], intervals, sizeof(double) * NUM_STATE_GROUPS );
    }
    
    /* Set equal initial probabilities */
    double v = 1./(double)NUM_STATE_GROUPS;
    for(uint8_t i = 0; i < NUM_STATE_GROUPS; i++)
        model->p[i] = v;
    
    HMMFunctions.PrintObservationMatrix( model );
}

void UpdateObservationMatrixHiddenMarkovModel(  hidden_markov_model_t * model )
{
    /* Observation matrix update */
    LOG_HMM(HMM_DEBUG, "Observation matrix update:\n");
    LOG_HMM(HMM_DEBUG, "    ");
    for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
        LOG_HMM_BARE(HMM_DEBUG, "  %s    ", observation_strings[i]);
    LOG_HMM_BARE(HMM_DEBUG, "\n");
#ifdef NORMALIZE_BY_STATE
      /* Normalize by state */
    for( uint8_t j = 0; j < NUM_STATES; j++ )
    {
        LOG_HMM(HMM_DEBUG, "%s ", stateString(j));
        double row_sum = 0., curr = 0., row_s = 0.;
        for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
        {
            curr = model->G.cumulative_value[i][j];
            if( curr > MAX_SINGLE_CONFIDENCE )
                return;
            row_sum += curr;
        }

        for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
        {
            if( row_sum != 0. )
            {
                model->B.expected[i][j] = ZDIV( model->G.cumulative_value[i][j], row_sum );
            }
            LOG_HMM_BARE(HMM_DEBUG, "|%.4f|", model->B.expected[i][j]);
            row_s += model->B.expected[i][j];
        }
        LOG_HMM_BARE(HMM_DEBUG, " = %.3f\n", row_s);
    }
#else
    double observation_sums[NUM_OBSERVATION_SYMBOLS] = { 0. };
    /* Normalize by symbol */
    for( uint8_t j = 0; j < NUM_OBSERVATION_SYMBOLS; j++ )
    {
        double row_sum = 0.;
        for( uint8_t i = 0; i < NUM_STATES; i++ )
        {
            row_sum += model->G.cumulative_value[j][i];
        }
        
        if( row_sum != 0. )
        {
            for( uint8_t i = 0; i < NUM_STATES; i++ )
            {
                model->B.expected[j][i] = ZDIV( model->G.cumulative_value[j][i], row_sum );
//                observation_sums[j] += model->B.expected[j][i];
            }
        }
    }
    
    for( uint8_t j = 0; j < NUM_STATES; j++ )
    {
        LOG_HMM(HMM_DEBUG, "%s ", stateString(j));
        for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
        {
            LOG_HMM_BARE(HMM_DEBUG, "|%.4f|", model->B.expected[i][j]);
            observation_sums[i] += model->B.expected[i][j];
        }
        LOG_HMM_BARE(HMM_DEBUG, "\n");
    }
    LOG_HMM(HMM_DEBUG, "    ");
    for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
    {
        LOG_HMM_BARE(HMM_DEBUG, " %.3f  ", observation_sums[i]);
    }
    LOG_HMM_BARE(HMM_DEBUG, "\n");
#endif
}

static double ForwardBackward( hidden_markov_model_t * model, uint8_t k, uint8_t l, uint8_t i, uint8_t j )
{
    double a = model->A.probabilities.map[i][j],
    b = model->B.expected[l][j],
    c = 1.,//model->p[i],
    d = model->B.expected[k][i];
    return a * b * c * d;
}

void inline ReportObservationToHiddenMarkovModel( hidden_markov_model_t * model, observation_symbol_t observation )
{
    model->T = AddToObservationBuffer( &model->O, observation );
}

void BaumWelchGammaSolveHiddenMarkovModel( hidden_markov_model_t * model )
{
    /* Update state expectation matrix */
    uint8_t k = model->O.prev, l = model->O.curr;
    LOG_HMM_BARE(HMM_DEBUG, "\n");
    for( uint8_t i = 0; i < NUM_STATES; i++ )
    {
        LOG_HMM(HMM_DEBUG, "");
        for( uint8_t j = 0; j < NUM_STATES; j++ )
        {
            model->Es[k][l][i][j] = ForwardBackward(model, k, l, i, j);
            LOG_HMM_BARE(HMM_DEBUG, "%.4f ", model->Es[k][l][i][j]);
        }
        LOG_HMM_BARE(HMM_DEBUG, "\n");
    }
    /* Update maximum expectation matarix */
    bool diff = ( k != l );
    double observation_max = 0.;
    for( uint8_t i = 0; i < NUM_STATES; i++ )
    {
        double curr_max = 0., prev_max = 0., curr = 0., diag = ForwardBackward( model, k, l, i, i );
        if( !diff )
            curr_max = diag;
        else
        {
            // Assume l comes from i
            for( uint8_t j = 0; j < NUM_STATES; j++ )
            {
                curr = ForwardBackward( model, k, l, j, i );// model->Es[k][l][j][i];
                if( curr > curr_max )
                    curr_max = curr;
            }
            
            // Assume k comes from i
            for( uint8_t j = 0; j < NUM_STATES; j++ )
            {
                curr = ForwardBackward( model, k, l, i, j );// model->Es[k][l][i][j];
                if( curr > prev_max )
                    prev_max = curr;
            }
        }
        if( diff )
        {
            model->G.cumulative_value[k][i] += prev_max;
        }
        model->G.cumulative_value[l][i] += curr_max;
        
        SET_MAX(observation_max, curr_max);
    }
    
    /* Update gamma expectation maximums */
    if( diff )
        model->G.maximum[k] += observation_max;
    model->G.maximum[l] += observation_max;
    
    LOG_HMM_BARE(HMM_DEBUG, "\n");
}

void PrintObservationMatrixHiddenMarkovModel(  hidden_markov_model_t * model )
{
    LOG_HMM(HMM_DEBUG, "|B|:\n");
    for( uint8_t j = 0; j < NUM_STATES; j++ )
    {
        LOG_HMM(HMM_DEBUG, "");
        double row_s = 0;
        for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
        {
            LOG_HMM_BARE(HMM_DEBUG, "|%.4f|", model->B.expected[i][j]);
            row_s += model->B.expected[i][j];
        }
        LOG_HMM_BARE(HMM_DEBUG, " = %.3f\n", row_s);
    }
}

#endif
