/************************************************************************
 *  File: rho_utility.h
 *  Group: Rho Core
 ***********************************************************************/

#ifndef rho_utility_h
#define rho_utility_h

/************************************************************************
 *                             Includes                                 *
 ***********************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "statistics.h"
#include "rho_structure.h"
#include "rho_types.h"

#ifdef USE_DECOUPLING
#include "rho_deco.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
/************************************************************************
 *                          Static Buffers                              *
 ***********************************************************************/
  static density_map_unit_t
#ifdef AUTOMATION_RUN
    FOREGROUND_DENSITY_MAP_Y[2000],
    FOREGROUND_DENSITY_MAP_X[2000],
    BACKGROUND_DENSITY_MAP_Y[2000],
    BACKGROUND_DENSITY_MAP_X[2000];
#else
    FOREGROUND_DENSITY_MAP_Y[DENSITY_MAP_Y_SIZE],
    FOREGROUND_DENSITY_MAP_X[DENSITY_MAP_X_SIZE],
    BACKGROUND_DENSITY_MAP_Y[DENSITY_MAP_Y_SIZE],
    BACKGROUND_DENSITY_MAP_X[DENSITY_MAP_X_SIZE];
#endif

/************************************************************************
 *                       Function Declarations                          *
 ***********************************************************************/
    void InitializeDataRhoUtility( rho_core_t *, index_t, index_t );
    void InitializeFiltersRhoUtility( rho_core_t * );
    void InitializePredictionRhoUtility( prediction_t *, index_t );
    void InitializeDensityMapRhoUtility( density_map_t *, index_t, index_t );

    void ResetForDetectRhoUtility( rho_detection_variables *, density_map_t *, prediction_t * );
    void ResetForPredictionRhoUtility( prediction_predict_variables *, prediction_pair_t *, index_pair_t );
    void ResetDensityMapPairKalmansRhoUtility( rho_core_t * );

    void PredictPeakFilterRhoUtility( rho_detection_variables *, density_map_t *, prediction_t * );
    void PredictTrackingFiltersRhoUtility( prediction_t * );
    index_t CalculateValidTracksRhoUtility( prediction_t * );
    void SortTrackingFiltersRhoUtility( prediction_t * );
    void PredictTrackingProbabilitiesRhoUtility( prediction_t * );

    void PerformDetectRhoUtility( rho_detection_variables *, density_map_t *, prediction_t * );
    bool CalculateBandLowerBoundRhoUtility( rho_detection_variables * );
    void DetectRegionsRhoUtility( rho_detection_variables *, density_map_t *, prediction_t * );
    void DetectRegionRhoUtility( rho_detection_variables *, density_map_t *, prediction_t * );
    void SubtractBackgroundForDetectionRhoUtility( rho_detection_variables * );
    void CalculateChaosRhoUtility( rho_detection_variables *, prediction_t * );
    void ScoreRegionsRhoUtility( rho_detection_variables *, density_map_t *, prediction_t * );
    void SortRegionsRhoUtility( rho_detection_variables *, prediction_t * );
    void CalculatedFrameStatisticsRhoUtility( rho_detection_variables *, prediction_t * );

    void CorrectPredictionAmbiguityRhoUtility( prediction_predict_variables *, rho_core_t * );
    void RedistributeDensitiesRhoUtility( rho_core_t * );
    void CombineAxisProbabilitesRhoUtility( prediction_pair_t * );
    void UpdateCorePredictionDataRhoUtility( prediction_predict_variables *, rho_core_t * );

    void CalculateTuneRhoUtility( rho_core_t * );
    void CalculateBackgroundTuneFactorRhoUtility( rho_core_t * );
    void CalculateStateTuneFactorRhoUtility( rho_core_t * );
    void CalculateTargetTuneFactorRhoUtility( rho_core_t * );
    void CalculateTargetCoverageFactorRhoUtility( rho_core_t * core );

    void GenerateRegionScoreRhoUtility( region_t *, density_t, byte_t );
    density_2d_t GenerateCentroidRhoUtility( density_map_unit_t *, index_t, index_t *, density_t );
    void PrintPacketRhoUtility( packet_t *, index_t );
    void GenerateBackgroundRhoUtility( rho_core_t * );
    void GeneratePacketRhoUtility( rho_core_t * );

    void GenerateObservationListFromPredictionsRhoUtility( prediction_t *, uint8_t );
    void GenerateObservationListsFromPredictionsRhoUtility( rho_core_t * );
    void UpdatePredictiveStateModelPairRhoUtility( rho_core_t * );

    typedef struct
    {
        void (*Data)( rho_core_t *, index_t, index_t );
        void (*Filters)( rho_core_t * );
        void (*Prediction)( prediction_t *, index_t );
        void (*DensityMap)( density_map_t *, index_t, index_t );
    } rho_utility_initializer_functions;

    typedef struct
    {
        void (*Detect)( rho_detection_variables *, density_map_t *, prediction_t * );
        void (*Prediction)( prediction_predict_variables *, prediction_pair_t *, index_pair_t );
        void (*DensityMapPairKalmans)( rho_core_t * );
    } rho_utility_reset_functions;

    typedef struct
    {
        void (*PeakFilter)( rho_detection_variables *, density_map_t *, prediction_t * );
        void (*TrackingFilters)( prediction_t * );
        index_t (*CalculateValidTracks)( prediction_t * );
        void (*SortFilters)( prediction_t * );
        void (*TrackingProbabilities)( prediction_t * );
        void (*CorrectAmbiguity)( prediction_predict_variables *, rho_core_t * );
        void (*CombineProbabilities)( prediction_pair_t * );
        void (*RedistributeDensities)(  rho_core_t * );
        void (*UpdateCorePredictionData)( prediction_predict_variables *, rho_core_t * );
        void (*GenerateObservationList)( prediction_t *, uint8_t );
        void (*GenerateObservationLists)( rho_core_t * );
        void (*UpdatePredictiveStateModelPair)(rho_core_t * );
    } rho_utility_predict_functions;

    typedef struct
    {
        void (*Perform)( rho_detection_variables *, density_map_t *, prediction_t * );
        bool (*LowerBound)( rho_detection_variables * );
        void (*Regions)( rho_detection_variables *, density_map_t *, prediction_t *);
        void (*Region)( rho_detection_variables *, density_map_t *, prediction_t * );
        void (*SubtractBackground)( rho_detection_variables *);
        void (*CalculateChaos)( rho_detection_variables *, prediction_t * );
        void (*ScoreRegions)( rho_detection_variables *, density_map_t *, prediction_t * );
        void (*SortRegions)( rho_detection_variables *, prediction_t * );
        void (*CalculateFrameStatistics)( rho_detection_variables *, prediction_t * );
    } rho_utility_detect_functions;

    typedef struct
    {
        void (*Tune)( rho_core_t * );
        void (*BackgroundTuneFactor)( rho_core_t * );
        void (*StateTuneFactor)( rho_core_t * );
        void (*TargetTuneFactor)( rho_core_t * );
        void (*TargetCoverageFactor)( rho_core_t * );
    } rho_utility_calculate_functions;
    
    typedef struct
    {
        void (*CumulativeMoments)( floating_t, floating_t, floating_t *, floating_t *, floating_t * );
        void (*RegionScore)( region_t *, density_t, byte_t );
        density_2d_t (*Centroid)( density_map_unit_t *, index_t, index_t *, density_t );
        void (*Background)( rho_core_t * );
        void (*Packet)( rho_core_t * );
    } rho_utility_generate_functions;
    
    typedef struct
    {
        void (*Packet)( packet_t *, index_t );
    } rho_utility_print_functions;

    typedef struct
    {
        rho_utility_initializer_functions Initialize;
        rho_utility_reset_functions Reset;
        rho_utility_predict_functions Predict;
        rho_utility_detect_functions Detect;
        rho_utility_calculate_functions Calculate;
        rho_utility_generate_functions Generate;
        rho_utility_print_functions Print;
    } rho_utility_functions;

/************************************************************************
 *                         Local Instance                               *
 ***********************************************************************/
    extern const rho_utility_functions RhoUtility;

#ifdef __cplusplus
}
#endif

#endif
