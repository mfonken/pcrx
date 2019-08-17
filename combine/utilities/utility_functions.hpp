//
//  utility_functions.hpp
//  tau+
//
//  Created by Matthew Fonken on 10/6/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#ifndef utility_functions_hpp
#define utility_functions_hpp

#include "test_setup.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "global_types.h"

static inline bool thresh(uint8_t t, cv::Vec3d p) { return p[0] > t; }

bool thresh(uint8_t t, cv::Vec3d p);

#endif /* utility_functions_hpp */
