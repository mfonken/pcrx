/************************************************************************
 *  File: rho_core.h
 *  Group: Rho Core
 ***********************************************************************/
 
/************************************************************************
 *                          Includes                                    *
 ***********************************************************************/
#include "rho_core.h"

#ifdef USE_DETECTION_MAP
#include "detection_map.h"
#endif

/************************************************************************
 *                       Local Instance                                 *
 ***********************************************************************/
const rho_core_functions RhoCore =
{
    .Initialize = InitializeRhoCore,
    .Perform = PerformRhoCore,
    .DetectPairs = DetectRhoCorePairs,
    .Detect = DetectRhoCore,
    .UpdatePrediction = UpdateRhoCorePrediction,
    .UpdatePredictions = UpdateRhoCorePredictions,
    .UpdateThreshold = UpdateRhoCoreThreshold,
    .GeneratePacket = GenerateRhoCorePacket
};

/************************************************************************
 *                      Functions Declarations                          *
 ***********************************************************************/
void InitializeRhoCore( rho_core_t * core, index_t width, index_t height )
{
    /* Generic Data */
    RhoUtility.Initialize.Data( core, width, height );

    /* Filters */
    RhoUtility.Initialize.Filters( core );
    
#ifdef USE_DETECTION_MAP
    /* Detection map */
    DetectionMapFunctions.Init( &core->DetectionMap, DETECTION_BUFFER_SIZE );
#endif
    
    /* Density Data */
    RhoUtility.Initialize.DensityMap( &core->DensityMapPair.x, height, core->Cy );
    RhoUtility.Initialize.DensityMap( &core->DensityMapPair.y, width, core->Cx  );

    /* Prediction Structures */
    RhoUtility.Initialize.Prediction( &core->PredictionPair.x, core->Height );
    RhoUtility.Initialize.Prediction( &core->PredictionPair.y, core->Width  );

#ifdef USE_INTERRUPT_MODEL
    /* Frame Conversion Model Connection */
    RhoInterrupts.INIT_FROM_CORE( core );
#endif
}

void PerformRhoCore( rho_core_t * core, bool background_event )
{
    if(background_event)
    {
        density_2d_t xt = RhoUtility.CalculateCentroid( core->DensityMapPair.x.background, core->DensityMapPair.x.length, &core->Bx, BACKGROUND_CENTROID_CALC_THRESH );
        density_2d_t yt = RhoUtility.CalculateCentroid( core->DensityMapPair.y.background, core->DensityMapPair.y.length, &core->By, BACKGROUND_CENTROID_CALC_THRESH );
        core->QbT = MAX(xt, yt);
    }
    else
    {
        LOG_RHO(RHO_DEBUG, "\n");
        LOG_RHO(RHO_DEBUG_2,"Filtering and selecting pairs.\n");
        RhoCore.DetectPairs( core );
        LOG_RHO(RHO_DEBUG_2,"Updating predictions.\n");
        RhoCore.UpdatePredictions( core );
        LOG_RHO(RHO_DEBUG_2,"Updating threshold.\n");
        RhoCore.UpdateThreshold( core );
//       LOG_RHO(RHO_DEBUG_2,"Generating packets.\n");
//       RhoCore.GeneratePacket( core );
    }
}

/* Calculate and process data in variance band from density filter to generate predictions */
void DetectRhoCore( rho_core_t * core, density_map_t * d, prediction_t * r )
{
    rho_detection_variables _;
    RhoUtility.Reset.Detect( &_, d, r );
    core->TotalCoverage = 0;
    core->FilteredCoverage = 0;
    _.target_density = core->TargetFilter.value * (floating_t)TOTAL_RHO_PIXELS;

    /* Perform detect */
    LOG_RHO(RHO_DEBUG_2, "Performing detect:\n");
    RhoUtility.Detect.Perform( &_, d, r );

    /* Update frame statistics */
    LOG_RHO(RHO_DEBUG_2, "Calculating frame statistics:\n");
    RhoUtility.Detect.CalculateFrameStatistics( &_, r );

    /* Update core */
    core->TotalCoverage += _.tden;
    core->FilteredCoverage += _.fden;
}

void DetectRhoCorePairs( rho_core_t * core )
{
    LOG_RHO(RHO_DEBUG_2, "Detecting X Map:\n");
    RhoCore.Detect( core, &core->DensityMapPair.x, &core->PredictionPair.x );
    LOG_RHO(RHO_DEBUG_2, "Detecting Y Map:\n");
    RhoCore.Detect( core, &core->DensityMapPair.y, &core->PredictionPair.y );

    /* Calculate accumulated filtered percentage from both axes */
    core->FilteredPercentage = ZDIV( (floating_t)core->FilteredCoverage, (floating_t)core->TotalCoverage );
    core->PredictionPair.NuRegions = MAX( core->PredictionPair.x.NuRegions, core->PredictionPair.y.NuRegions );
}

/* Correct and factor predictions from variance band filtering into global model */
void UpdateRhoCorePrediction( prediction_t * r )
{
    /* Step predictions of all Kalmans */
    RhoUtility.Predict.TrackingFilters( r );
    RhoUtility.Predict.TrackingProbabilities( r );

    r->Primary   = r->TrackingFilters[0].value;
    r->Secondary = r->TrackingFilters[1].value;
}

void UpdateRhoCorePredictions( rho_core_t * core )
{
    LOG_RHO(RHO_DEBUG_2,"Updating X Map:\n");
    RhoCore.UpdatePrediction( &core->PredictionPair.x );
    LOG_RHO(RHO_DEBUG_2,"Updating Y Map:\n");
    RhoCore.UpdatePrediction( &core->PredictionPair.y );

#ifdef USE_DETECTION_MAP
    DetectionMapFunctions.AddSet( &core->DetectionMap, &core->PredictionPair, core->ThreshByte );
#endif
    
    RhoUtility.Predict.GenerateObservationLists( core );
    
#ifdef __PSM__
     PSMFunctions.Update( &core->PredictiveStateModel, &core->PredictionPair.x.ObservationList, core->PredictionPair.x.NuRegions ); /// TODO: Generalize to be dimensionless
//    PSMFunctions.Update( &core->PredictiveStateModel, &core->PredictionPair.y.ObservationList, core->PredictionPair.y.NuRegions );
#else
    floating_t bands[NUM_STATE_GROUPS] = SPOOF_STATE_BANDS;
    double state_intervals[NUM_STATE_GROUPS];
    KumaraswamyFunctions.GetVector( &core->Kumaraswamy, core->PredictionPair.NuRegions, state_intervals, bands, NUM_STATE_GROUPS );
    FSMFunctions.Sys.Update( &core->StateMachine, state_intervals );
#endif

    prediction_predict_variables _;

    RhoUtility.Reset.Prediction( &_, &core->PredictionPair, core->Cx, core->Cy );
    RhoUtility.Predict.CorrectAmbiguity( &_, core );
    RhoUtility.Predict.CombineProbabilities( &core->PredictionPair );
    RhoUtility.Predict.UpdateCorePredictionData( &_, core );
}

/* Use background and state information to update image threshold */
void UpdateRhoCoreThreshold( rho_core_t * core )
{
    RhoUtility.Calculate.Tune( core );
    core->Thresh = BOUND(core->Thresh + core->Tune.proposed, THRESH_MIN, THRESH_MAX);
    core->ThreshByte = (byte_t)core->Thresh;
}

void GenerateRhoCorePacket( rho_core_t * core )
{
    RhoUtility.GeneratePacket( core );
    RhoUtility.PrintPacket( &core->Packet, 3    );
}
