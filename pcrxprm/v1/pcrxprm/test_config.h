//
//  test_config.h
//  pcr
//
//  Created by Matthew Fonken on 12/16/18.
//  Copyright © 2018 Matthew Fonken. All rights reserved.
//

#ifndef test_config_h
#define test_config_h

using namespace cv;
using namespace std;

#define MAIN_FPS        15
#define TAU_FPS         10
#define COMBINE_FPS     0

//#define AUTOMATION_RUN
#define AUTOMATION_SIZES        { /*50, 250, 500, 750, 1000, 1250, */ 1500, /*1750, 2000*/ }
#define AUTOMATION_INSTRUCTIONS { 's', 's', 's', 's', 's' }
#define AUTOMATION_END_AFTER_INSTRUCTIONS

#define IMAGE_SET 0
#define IMAGE_SINGLE 1

#define IMAGE_TICKS                     31

#define TITLE_STRING                    "PCR Alpha v0.7 xprm"
#define GROUP_NAME                      "frames"//"gradient"
#define FILE_NAME                       "small"
#define FILE_TYPE                       ".png"
#define TYPE                            IMAGE_SET

#if TYPE == IMAGE_SINGLE
#define FRAME_IMAGE_IMAGE_SOURCE_PATH   GROUP_NAME "/" FILE_NAME FILE_TYPE
#define FRAME_IMAGE_SOURCE_NUM_FRAMES   IMAGE_TICKS
#elif TYPE == IMAGE_SET
#define FRAME_IMAGE_IMAGE_SOURCE_PATH   GROUP_NAME "/" FILE_NAME "/"
#define FRAME_IMAGE_SOURCE_NUM_FRAMES   31
#endif

#define THRESH_FRAME_PRINT_STEP         30

/* GLOBAL SETUP */
//#define HAS_CAMERA
//#define HAS_IMU
#define HAS_FILE
//#define HAS_GENERATOR

#ifdef AUTOMATION_RUN
#define FRAME_WIDTH                     50
#define FRAME_HEIGHT                    FRAME_WIDTH
#else
#define FRAME_WIDTH                     700
#define FRAME_HEIGHT                    FRAME_WIDTH
#endif

/* IMAGE PROCESSING */
#define FRAME_IMAGE_ROOT                "/Users/matthew/Desktop/PersonalResources/TestImages/"
#define FRAME_SAVE_ROOT                 "/Users/matthew/Desktop/PCRImages/"

#define sleep(X) usleep(X*1E6)
#define KEY_DELAY 1E3/MAIN_FPS

#define FILENAME                        "/Users/matthew/Desktop/p_data.csv"
#define X_DIM_FILENAME                  "/Users/matthew/Desktop/x_data.csv"
#define Y_DIM_FILENAME                  "/Users/matthew/Desktop/y_data.csv"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define PERF_FILENAME                   "/Users/matthew/Desktop/psm_perf/perf_data.csv"

#define DASHBOARD_WIDTH                 10000
#define DASHBOARD_HEIGHT                FRAME_HEIGHT

#endif /* test_config_h */
