//
//  tau_structures.cpp
//  tau+
//
//  Created by Matthew Fonken on 2/8/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#include "tau_structures.hpp"

Prediction::Prediction()
{
    primary   = KalmanFilter(0., PREDICTION_LIFESPAN, PREDICTION_VALUE_UNCERTAINTY, PREDICTION_BIAS_UNCERTAINTY, PREDICTION_SENSOR_UNCERTAINTY);
    secondary = KalmanFilter(0., PREDICTION_LIFESPAN, PREDICTION_VALUE_UNCERTAINTY, PREDICTION_BIAS_UNCERTAINTY, PREDICTION_SENSOR_UNCERTAINTY);
    primary_new = 0.;
    secondary_new = 0.;
    probabilities = { 0. };
}
