//
//  fsm.cpp
//  Finite State Machine
//
//  Created by Matthew Fonken on 2/8/18.
//  Copyright © 2019 Marbl. All rights reserved.
//

#include "fsm.h"

typedef define_loop_variable_template_struct(uint8_t, state_global_t);
state_global_t _;

static inline void reset_loop_variables( state_global_t * _, uint8_t l )
{ _->l = l; _->i = 0; _->j = 0; _->u = 0; _->v = 0.; }

void InitializeFSMMap( fsm_map_t * bm )
{
    LOG_FSM(FSM_DEBUG, "Initializing State Machine.\n");
    reset_loop_variables( &_, NUM_STATES );
    bm->length = NUM_STATES;
    for( ; _.i < _.l; _.i++ )
    {
        for( _.j = 0; _.j < _.l; _.j++ )
            bm->map[_.i][_.j] = 0.0;
        bm->map[_.i][_.i] = 1.0;
    }
}

void NormalizeFSMMap( fsm_map_t * bm )
{
    reset_loop_variables( &_, bm->length );
    for(  _.i = 0; _.i < _.l; _.i++ )
        FSMFunctions.Map.NormalizeState( bm, _.i );
}

void NormalizeFSMState( fsm_map_t * bm, uint8_t i )
{
    reset_loop_variables( &_, bm->length );
    double * total = &_.v;
    for( _.j = 0; _.j < _.l; _.j++ )
    {
        _.u = bm->map[i][_.j];
        if( _.u >= 0. )
            *total += _.u;
    }
    if(*total)
    {
        _.v = ZDIV( 1., (*total) );
        for( _.j = 0; _.j < _.l; _.j++ ) bm->map[i][_.j] *= _.v;
    }
    else bm->map[_.i][i] = 1.0;
}

void ResetFSMState( fsm_map_t * bm, uint8_t i )
{
    reset_loop_variables( &_, bm->length );
    for( _.j = 0; _.j < _.l; _.j++ ) bm->map[i][_.j] = 0.0;
    bm->map[i][i] = 1.0;
}

void InitializeFSMSystem( fsm_system_t * sys, state_t initial_state )
{
    sys->state                = initial_state;
    sys->prev                 = UNKNOWN_STATE;
    sys->next                 = UNKNOWN_STATE;
    sys->selection_index      = 0;
    
    RhoKalman.Initialize( &sys->stability.system, 0., FSM_LIFESPAN, 0., 1., FSM_STABLIITY_UNCERTAINTY );
    RhoKalman.Initialize( &sys->stability.state, 0., FSM_STATE_LIFESPAN, 0., 1., FSM_STATE_UNCERTAINTY );
    
    FSMFunctions.Map.Initialize( &sys->probabilities );
}

void DecayInactiveFSMSystem( fsm_system_t * sys )
{
    reset_loop_variables( &_, NUM_STATES );
    state_t c = sys->state;
    if( c == UNKNOWN_STATE ) return;
    for( _.i = 0; _.i < NUM_STATES; _.i++ )
    {
        uint8_t state_distance = abs( _.i - c );
        for( _.j = 0; _.j < NUM_STATES; _.j++ ) // Punish based on relevance(distance) from current state
        {
            sys->probabilities.map[_.i][_.j] -= STATE_PUNISH * state_distance;
            if( sys->probabilities.map[_.i][_.j] < 0 ) sys->probabilities.map[_.i][_.j] = 0.;
        }
    }
}

void UpdateFSMSystem( fsm_system_t * sys, double p[4] )
{
    reset_loop_variables( &_, NUM_STATES );
    
    FSMFunctions.Sys.UpdateProbabilities( sys, p );
    FSMFunctions.Map.NormalizeState( &sys->probabilities, sys->state );
    
    state_t next = sys->state;
    
    /* Find most probable next state */
    for( ; _.i < _.l; _.i++ )
    {
        _.v = sys->probabilities.map[(uint8_t)sys->state][_.i];
        if( _.v > _.u )
        {
            _.u = _.v;
            next = (state_t)_.i;
        }
    }
    
    sys->next = next;
    
    FSMFunctions.Sys.UpdateState( sys );
//    FSMFunctions.Sys.DecayInactive( sys );
    PrintFSMMap( &sys->probabilities, sys->state );
    
    sys->stability.system.timestamp = TIMESTAMP();
    sys->stability.state.timestamp = TIMESTAMP();
}

void UpdateFSMProbabilities( fsm_system_t * sys, double p[4] )
{
    state_t           c = sys->state;

#ifdef FSM_DEBUG
    LOG_FSM( FSM_DEBUG, "Probabilies are [0]%.2f [1]%.2f [2]%.2f [3]%.2f.\n", p[0], p[1], p[2], p[3]);
    LOG_FSM( FSM_DEBUG, "State %s has stability %.4f\n", stateString(c), sys->stability.state.value );
#endif
    floating_t curr = 0;
    for( _.i = 0; _.i < NUM_STATES; _.i++ )
    {
        LOG_FSM(FSM_DEBUG, "Updating %s by %.2f.\n", stateString(_.i), p[_.i]);
        curr = WeightedAverage( sys->probabilities.map[c][_.i], p[_.i], ( sys->stability.state.value + 1 ) / 2 );
        if( curr <= MAX_SINGLE_CONFIDENCE )
            sys->probabilities.map[c][_.i] = curr;
    }
    floating_t state_change_rate = TIMESTAMP() - sys->stability.state.timestamp;
    RhoKalman.Step( &sys->stability.state, p[sys->state], state_change_rate );
}

void UpdateFSMState( fsm_system_t * sys )
{
    /* State change */
    if(sys->next != UNKNOWN_STATE )
    {
        LOG_FSM(FSM_DEBUG, "Updating state from %s to %s\n", stateString((int)sys->state), stateString((int)sys->next));
        if(sys->next != sys->state) {LOG_FSM(FSM_DEBUG, "~~~ State is %s ~~~\n", stateString(sys->next));}
        sys->prev   = sys->state;
        sys->state  = sys->next;
        sys->next   = UNKNOWN_STATE;
        
        RhoKalman.Reset( &sys->stability.state, 0. );
//        FSMFunctions.Map.ResetState( &sys->probabilities, sys->prev );
        
        floating_t system_change_rate = TIMESTAMP() - sys->stability.system.timestamp;
        RhoKalman.Step( &sys->stability.system, sys->probabilities.map[sys->state][sys->state], system_change_rate );
    }
}

void PrintFSMMap( fsm_map_t * bm, state_t s )
{
#ifdef FSM_DEBUG
    reset_loop_variables( &_, bm->length );
    LOG_FSM(FSM_DEBUG_2, "\t\t\t  ");
    for( _.i = 0; _.i < _.l; _.i++ ) LOG_FSM_BARE(FSM_DEBUG_2, "%s-[%d]\t  ", stateString((uint8_t)_.i), _.i);
    LOG_FSM_BARE(FSM_DEBUG_2, "\n");
    for( _.i = 0; _.i < _.l; _.i++ )
    {
        LOG_FSM(FSM_DEBUG_2, " %s-[%d]", stateString((uint8_t)_.i), _.i);
        for( _.j = 0; _.j < _.l; _.j++ )
        {
            char c = ' ';
            if(_.j == (uint8_t)s) c = '|';
            LOG_FSM_BARE(FSM_DEBUG_2, "\t%c[%.5f]%c",c, bm->map[_.j][_.i],c);
        }
        LOG_FSM_BARE(FSM_DEBUG_2, "\n");
    }
    LOG_FSM_BARE(FSM_DEBUG_2, "\n");
#endif
}
