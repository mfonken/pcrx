//
//  detection_map.h
//  pcrxprm
//
//  Created by Matthew Fonken on 2/5/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef detection_map_h
#define detection_map_h

#include "rho_types.h"

#define WRITE_DETECTION_MAP_TO_FILE "/Users/matthew/Desktop/gmmtest/data/dm.txt"

#define DETECTION_MAP_NAME "double_circle_fade"
#ifdef __PSM__
#define DETECTION_MAP_DESCRIPTOR DETECTION_MAP_NAME "_psm_on"
#else
#define DETECTION_MAP_DESCRIPTOR DETECTION_MAP_NAME "_psm_off"
#endif

#define SHAKE_INJECTION 0//10

static FILE * detection_map_file;

void InitializeDetectionMap( detection_map_t *, uint16_t );
uint8_t GetDetectionMapFirstIndex( detection_map_t * );
void IncrementDetectionMapIndex( detection_map_t * );
void AddRegionToPredictionMap( detection_map_t *, uint8_t, index_t, uint8_t );
void AddRegionSetToPredictionMap( detection_map_t *, prediction_pair_t * );

typedef struct
{
    void (*Init)( detection_map_t *, uint16_t );
    uint8_t (*FirstIndex)( detection_map_t * );
    void (*Increment)( detection_map_t * );
    void (*Add)( detection_map_t *, uint8_t, index_t, uint8_t );
    void (*AddSet)( detection_map_t *, prediction_pair_t * );
} detection_map_function;

static detection_map_function DetectionMapFunctions =
{
    .Init = InitializeDetectionMap,
    .FirstIndex = GetDetectionMapFirstIndex,
    .Increment = IncrementDetectionMapIndex,
    .Add = AddRegionToPredictionMap,
    .AddSet = AddRegionSetToPredictionMap
};

#endif /* detection_map_h */
