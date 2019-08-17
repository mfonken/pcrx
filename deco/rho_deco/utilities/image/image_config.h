//
//  image_config.h
//  pcr
//
//  Created by Matthew Fonken on 12/15/18.
//  Copyright © 2018 Matthew Fonken. All rights reserved.
//

#ifndef image_config_h
#define image_config_h

#include "test_config.h"

#define IMAGE_ROOT              FRAME_IMAGE_ROOT

#define CAMERA_ID       1

//#define TILT_LINES
//#define GREYSCALE

#define THRESH_IMAGE
#define ROTATE_IMAGE

#define OBJECT_BRIGHTNESS 220
#define NOISE_BRIGHTNESS 150

#define PATH_NUM_TICKS  200
#define PATH_OFFSET     200
#define TARGET_RADIUS   20
#define DEFAULT_PATH    CIRCLE_CENTERED
#define TARGET_COLOR    Scalar(OBJECT_BRIGHTNESS,OBJECT_BRIGHTNESS,OBJECT_BRIGHTNESS)
#define PATH_OFFSET_A   0.2
#define PATH_OFFSET_B   1.1
#define PATH_OFFSET_C   1.5

#define DEFAULT_NOISE   NONE //STATIC_SMALL
#define NOISE_RATE      10  //Hz
#define NOISE_COLOR     Scalar(NOISE_BRIGHTNESS,NOISE_BRIGHTNESS,NOISE_BRIGHTNESS)
#define NOISE_ORIGIN_X  50
#define NOISE_ORIGIN_Y  200
#define NOISE_WIDTH     150
#define NOISE_HEIGHT    100

//#define IU_WIDTH        FRAME_WIDTH
//#define IU_HEIGHT       FRAME_HEIGHT
#define IU_FRAME_RATE   60

#define IU_THRESHOLD    1//00
#define IU_BRIGHTNESS   230

// ?
#define D_FIXED                 5.0e-2


#endif /* image_config_h */
