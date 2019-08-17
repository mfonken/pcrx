//
//  control_structures.h
//  hmmtest
//
//  Created by Matthew Fonken on 2/11/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef control_structures_h
#define control_structures_h

#ifdef __cplusplus
extern "C" {
#endif
    
#include "kumaraswamy.h"
    
    /* System self-diagnostic state control type */
    typedef struct
    {
        state_t         state;
        state_t         prev;
        state_t         next;
        uint8_t         selection_index;
        stability_t     stability;
        fsm_map_t       probabilities;
    } fsm_system_t;
#ifdef __PSM__
    typedef struct
    {
        gaussian2d_t
        gaussian_in,
        gaussian_out;
        double
        mahalanobis_sq,
        log_gaussian_norm_factor,
        probability_of_in,
        probability_condition_input,
        score, weight;
        mat2x2
        inv_covariance_in,
        llt_in;
        label_manager_t labels;
        double
        max_y,
        min_y,
        timestamp;
        uint8_t
        primary_id,
        secondary_id;
    } gaussian_mixture_cluster_t;
    
    typedef struct
    {
        gaussian_mixture_cluster_t *
        cluster[MAX_CLUSTERS];
        uint8_t
        num_clusters,
        selected_cluster;
        vec2
        min_in,
        max_in,
        max_out,
        min_out;
        gaussian_mixture_cluster_t
        cluster_mem[MAX_CLUSTERS];
    } gaussian_mixture_model_t;

    typedef struct
    {
        double expected[NUM_OBSERVATION_SYMBOLS][NUM_STATES];
        double best[NUM_OBSERVATION_SYMBOLS];
        double gamma[NUM_STATES];
        uint8_t num_observation_symbols;
    } observation_matrix_t;
    
    typedef double state_expectation_element_t[NUM_STATES][NUM_STATES];
    typedef state_expectation_element_t state_expectation_matrix_t[NUM_OBSERVATION_SYMBOLS][NUM_OBSERVATION_SYMBOLS];
    
#define MAX_VAL_INDEX NUM_STATES
    typedef double expectation_element_t[NUM_STATES+1];
    typedef expectation_element_t expectation_matrix_t[NUM_OBSERVATION_SYMBOLS];
    
#define GAMMA_VALUE_INDEX 1
#define GAMMAA
    typedef double gamma_element_t[NUM_OBSERVATION_SYMBOLS][NUM_STATES];
    typedef struct
    {
        gamma_element_t cumulative_value;
        double maximum[NUM_OBSERVATION_SYMBOLS];
    } gamma_matrix_t;
    
    typedef struct
    {
        double                  p[NUM_STATES];       // Initial probabilities
        fsm_system_t            A;                   // Transition matrix
        observation_matrix_t    B;                   // Observation matrix
        observation_buffer_t    O;                   // Observation sequence
        state_expectation_matrix_t Es;               // State expectation matrix
        gamma_matrix_t          G;                   // Gamma expectation matrix
        
        uint8_t
        T, // Number of observations
        N, // Number of states
        M; // Number of observation symbols
        uint8_t
        best_state;
        double
        best_confidence;
    } hidden_markov_model_t;
#endif
    
    typedef struct
    { /* Predictive State Model */
#ifdef __PSM__
        gaussian_mixture_model_t gmm;
        hidden_markov_model_t hmm;
#endif
        kumaraswamy_t kumaraswamy;
        band_list_t state_bands;
        double
        previous_thresh,
        proposed_thresh,
        proposed_nu,
        proposed_avg_den,
        proposed_primary_id,
        proposed_secondary_id;
        uint8_t
        best_state,
        best_cluster_id,
        observation_state;
        state_t
        current_state;
        observation_symbol_t
        current_observation;
        double
        best_confidence,
        best_cluster_weight,
        state_intervals[NUM_STATE_GROUPS];
    } psm_t;
    
#ifdef __cplusplus
}
#endif

#endif /* control_structures_h */
