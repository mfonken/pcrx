//
//  tracking_tuner.c
//  opencv_tracking
//
//  Created by Matthew Fonken on 8/5/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#include "tracking_tuner.h"

void CalculateTune( tune_t * tune, int total_density )
{
    /* Background-Tune on significant background */
    CalculateBackgroundTuneFactor( tune, total_density );
    
    /* State-Tune by FSM state */
    CalculateStateTuneFactor( tune );
    
    /* Filtered-Tune on target difference */
    CalculateTargetTuneFactor( tune );
    
    tune->proposed = BOUND( tune->background + tune->state + tune->target, -THRESH_STEP_MAX, THRESH_STEP_MAX);
}

void CalculateBackgroundTuneFactor( tune_t * tune, int total_density )
{
    double background_tune_factor = 0.;
    if( total_density > BACKGROUND_COVERAGE_MIN )
    {
        double background_coverage_factor = 1 - ZDIV( BACKGROUND_COVERAGE_MIN, total_density );
//        background_tune_factor = -pow( BOUND(background_coverage_factor, -2, 2), 3);
    }
    tune->background = background_tune_factor;
}

void CalculateStateTuneFactor( tune_t * tune )
{
    core->TargetCoverageFactor = core->TargetFilter.value;
    core->PredictionPair.AverageDensity = MAX( core->PredictionPair.x.AverageDensity, core->PredictionPair.y.AverageDensity );
#ifdef __PSM__
    LOG_RHO(RHO_DEBUG, "Current State: %s\n", stateString(core->PredictiveStateModel.current_state));
    switch(core->PredictiveStateModel.current_state)
#else
    switch(core->StateMachine.state)
#endif
    {
        case CHAOTIC:
            RhoKalman.Reset( &core->DensityMapPair.x.kalmans[0], core->PredictionPair.x.PreviousPeak[0] );
            RhoKalman.Reset( &core->DensityMapPair.x.kalmans[1], core->PredictionPair.x.PreviousPeak[1] );
            RhoKalman.Reset( &core->DensityMapPair.y.kalmans[0], core->PredictionPair.y.PreviousPeak[0] );
            RhoKalman.Reset( &core->DensityMapPair.y.kalmans[1], core->PredictionPair.y.PreviousPeak[1] );
        case TARGET_POPULATED:
            RhoKalman.Step( &core->TargetFilter, core->FilteredPercentage, 0. );
        case OVER_POPULATED:
        case UNDER_POPULATED:
            TargetCoverageFactor( core );
            break;
        default:
            break;
    }
    RhoPID.Update( &core->ThreshFilter, core->TargetCoverageFactor, core->TargetFilter.value );
    LOG_RHO(RHO_DEBUG, "TCF:%.4f | TFV:%.4f\n", core->TargetCoverageFactor, core->ThreshFilter.Value);
    tune->state = core->ThreshFilter.Value;
}

void CalculateTargetTuneFactor( tune_t * tune )
{
    tune->target = TARGET_TUNE_FACTOR * ZDIV( core->ThreshFilter.Value, core->TargetFilter.value );
}

void CalculateTargetCoverageFactor( tune_t * tune )
{
    double TotalPixels = (double)TOTAL_RHO_PIXELS;
#ifdef __PSM__
    if( core->PredictiveStateModel.best_confidence > MIN_STATE_CONFIDENCE )
        core->TargetCoverageFactor = ZDIV( core->PredictiveStateModel.proposed_nu * core->PredictiveStateModel.proposed_avg_den, TotalPixels );
#else
    if( core->PredictionPair.Probabilities.confidence > MIN_STATE_CONFIDENCE )
        core->TargetCoverageFactor = ZDIV( core->PredictionPair.NuRegions * core->PredictionPair.AverageDensity, TotalPixels );
#endif
    else
        core->TargetCoverageFactor = ZDIV( core->TotalCoverage, TotalPixels );
}
