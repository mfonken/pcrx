//
//  tracking_tuner.h
//  opencv_tracking
//
//  Created by Matthew Fonken on 8/5/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef tracking_tuner_h
#define tracking_tuner_h

#include <stdio.h>
#include "test_setup.h"

#ifndef BOUND
#define BOUNDU(X,MAX)           ( ( X > MAX ) ? MAX : X )         // Bound in upper range
#define BOUND(X,MIN,MAX)        ( ( X < MIN ) ? MIN : BOUNDU( X, MAX ) ) // Bound in upper and lower range
#endif

typedef struct
{
    double
    background,
    state,
    target,
    proposed;
} tune_t;

void CalculateTune(                   tune_t *, int );
void CalculateBackgroundTuneFactor(   tune_t *, int );
void CalculateStateTuneFactor(        tune_t * );
void CalculateTargetTuneFactor(       tune_t * );
void CalculateTargetCoverageFactor(   tune_t * );

#endif /* tracking_tuner_h */
