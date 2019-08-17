//
//  point.h
//  combine_core
//
//  Created by Matthew Fonken on 1/13/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#ifndef point_h
#define point_h

#ifdef __cplusplus
extern "C" {
#endif
    
#include <stdio.h>
#include <math.h>
    
#include "qmath.h"
    
    typedef struct
    {
        double x, y, z, the, phi;
    } kpoint_t;
    
    typedef struct kpoint kpoint;
    struct kpoint
    {
        void (*init)( kpoint_t *, double, double, double, double );
        double (*dist)( kpoint_t * );
        double (*disq)( kpoint_t * );
        double (*angl)( kpoint_t * );
        double (*anga)( kpoint_t * );
        void   (*copy)( kpoint_t *, kpoint_t * );
        void (*toVec3)( kpoint_t *, vec3_t * );
    };
    extern const kpoint KPoint;
    
#ifdef __cplusplus
}
#endif

#endif /* point_h */

