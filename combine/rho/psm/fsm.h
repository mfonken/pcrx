//
//  fsm.hpp
//  tau+
//
//  Created by Matthew Fonken on 2/8/18.
//  Copyright © 2019 Marbl. All rights reserved.
//

#ifndef fsm_h
#define fsm_h

#include <stdint.h>
#include <stdbool.h>
#include "fsm_support.h"

#ifdef __cplusplus
extern "C" {
#endif

    /***************************************************************************************/
    /*                          DEFINITIONS & MACROS                                       */
    /***************************************************************************************/
#define STATE_DECAY                     0.95
#define STATE_PUNISH                    0.025
    
#define FSM_LIFESPAN                    10.
#define FSM_STATE_LIFESPAN              3.
#define FSM_STABILITY_VALUE_UNCERTAINTY 0.04
#define FSM_STABILTIY_BIAS_UNCERTAINTY  0.1
#define FSM_STABILITY_INPUT_UNCERTAINTY 0.4
#define FSM_STABLIITY_UNCERTAINTY       (rho_kalman_uncertainty_c){ FSM_STABILITY_VALUE_UNCERTAINTY, FSM_STABILTIY_BIAS_UNCERTAINTY, FSM_STABILITY_INPUT_UNCERTAINTY }
#define FSM_STATE_VALUE_UNCERTAINTY     0.04
#define FSM_STATE_BIAS_UNCERTAINTY      0.1
#define FSM_STATE_INPUT_UNCERTAINTY     0.4
#define FSM_STATE_UNCERTAINTY           (rho_kalman_uncertainty_c){ FSM_STATE_VALUE_UNCERTAINTY, FSM_STATE_BIAS_UNCERTAINTY, FSM_STATE_INPUT_UNCERTAINTY }
    
    /***************************************************************************************/
    
    /** Goals **
     *  - Column is current state
     *  - Row is next state
     *  - Diagonal is probability of remaining in state, should tend to be most probable
     *  - System should try to stay in S2
     *  - All probabilities are <= 1.0 (100%) and...
     *  - ...all row probabilities add to <= 1.0 (100%)
     */
    
    /* Tau states */
#define loop_variables_t loop_variables_uint8_t
    
    typedef struct
    {
        void (*Initialize)(     fsm_map_t * );
        void (*Normalize)(      fsm_map_t * );
        void (*NormalizeState)( fsm_map_t *, uint8_t );
        void (*ResetState)(     fsm_map_t *, uint8_t );
        void (*Print)(          fsm_map_t *, state_t s );
    } fsm_map_functions_t;
    
    typedef struct
    {
        void (*Initialize)(          fsm_system_t *, state_t   );
        void (*DecayInactive)(       fsm_system_t *            );
        void (*UpdateProbabilities)( fsm_system_t *, double[4] );
        void (*UpdateState)(         fsm_system_t *            );
        void (*Update)(              fsm_system_t *, double[4] );
    } fsm_system_functions_t;
    
    typedef struct
    {
        fsm_map_functions_t    Map;
        fsm_system_functions_t Sys;
    } fsm_functions_t;
    
    void InitializeFSMMap(        fsm_map_t *                  );
    void NormalizeFSMMap(         fsm_map_t *                  );
    void NormalizeFSMState(       fsm_map_t *,       uint8_t   );
    void ResetFSMState(           fsm_map_t *,       uint8_t   );
    void PrintFSMMap(             fsm_map_t *,       state_t   );
    void InitializeFSMSystem(     fsm_system_t *,    state_t   );
    void DecayInactiveFSMSystem(  fsm_system_t *               );
    void UpdateFSMSystem(         fsm_system_t *,    double[4] );
    void UpdateFSMProbabilities(  fsm_system_t *,    double[4] );
    void UpdateFSMState(          fsm_system_t *               );
    
    static const fsm_functions_t FSMFunctions =
    {
        { /* Map functions */
            .Initialize             = InitializeFSMMap,
            .Normalize              = NormalizeFSMMap,
            .NormalizeState         = NormalizeFSMState,
            .ResetState             = ResetFSMState,
            .Print                  = PrintFSMMap
        },
        { /* System functions */
            .Initialize             = InitializeFSMSystem,
            .DecayInactive          = DecayInactiveFSMSystem,
            .Update                 = UpdateFSMSystem,
            .UpdateProbabilities    = UpdateFSMProbabilities,
            .UpdateState            = UpdateFSMState
        }
    };
    
    static inline void copymax(double * a, double * b) { if(*a>*b)*b=*a;else*a=*b; }
    
#define define_loop_variable_template_struct(T, N)struct { T l, i, j; double u, v;}N;
    
#ifdef __cplusplus
}
#endif

#endif /* fsm_hpp */
