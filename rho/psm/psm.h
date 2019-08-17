//
//  psm.h
//  hmmtest
//
//  Created by Matthew Fonken on 2/12/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//
// Description: Predictive State Model

#ifdef __PSM__

#ifndef psm_h
#define psm_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "hmm.h"

//// TEMP LOCATION
//#define HEIGHT 700
//#define WIDTH HEIGHT
    
void InitializePSM(                     psm_t * );
void ReportObservationsPSM(             psm_t *, observation_list_t * );
void UpdateStateIntervalsPSM(           psm_t *, double );
void UpdatePSM(                         psm_t *, observation_list_t *, double );
void UpdateStateBandPSM(                band_list_t *, uint8_t, int8_t, gaussian2d_t * );
void DiscoverStateBandsPSM(             psm_t *, band_list_t * );
uint8_t FindMostLikelyHiddenStatePSM(   psm_t *, uint8_t, double * );
void UpdateBestClusterPSM(              psm_t *, band_list_t * );
uint8_t GetCurrentBandPSM(              psm_t *, band_list_t * );
void GenerateProposalsPSM(              psm_t * );

typedef struct
{
    void (*Initialize)(                     psm_t * );
    void (*ReportObservations)(             psm_t *, observation_list_t * );
    void (*UpdateStateIntervals)(           psm_t *, double );
    void (*Update)(                         psm_t *, observation_list_t *, double nu );
    void (*UpdateStateBand)(                band_list_t *, uint8_t, int8_t, gaussian2d_t * );
    void (*DiscoverStateBands)(             psm_t *, band_list_t * );
    uint8_t (*FindMostLikelyHiddenState)(   psm_t *, uint8_t, double * );
    void (*UpdateBestCluster)(              psm_t *, band_list_t * );
    uint8_t (*GetCurrentBand)(              psm_t *, band_list_t * );
    void (*GenerateProposals)(              psm_t * );
} psm_functions_t;

static const psm_functions_t PSMFunctions =
{
    .Initialize                 = InitializePSM,
    .ReportObservations         = ReportObservationsPSM,
    .UpdateStateIntervals       = UpdateStateIntervalsPSM,
    .Update                     = UpdatePSM,
    .UpdateStateBand            = UpdateStateBandPSM,
    .DiscoverStateBands         = DiscoverStateBandsPSM,
    .FindMostLikelyHiddenState  = FindMostLikelyHiddenStatePSM,
    .UpdateBestCluster          = UpdateBestClusterPSM,
    .GetCurrentBand             = GetCurrentBandPSM,
    .GenerateProposals          = GenerateProposalsPSM,
};
    
#ifdef __cplusplus
}
#endif

#endif /* psm_h */

#endif
