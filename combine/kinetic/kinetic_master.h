//
//  kinetic_master.h
//  IMU Math
//
//  Created by Matthew Fonken on 12/27/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#ifndef kinetic_master_h
#define kinetic_master_h

#include "imu_wrapper.h"
#include "kinetic.h"
#include "point.h"

typedef struct
{
    quaternion_t
    data;
    double
    confidence;
    uint32_t
    timestamp;
} orientation_data_t;

typedef struct
{
    kpoint_t
    data[3];
    double
    confidence[3];
    uint32_t
    timestamp;
} rho_data_t;

#endif /* kinetic_master_h */
