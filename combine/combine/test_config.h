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

#define MAIN_FPS        100
#define TAU_FPS         100
#define COMBINE_FPS     100

/* GLOBAL SETUP */
//#define HAS_CAMERA
//#define HAS_IMU
#define HAS_FILE
//#define HAS_GENERATOR

//#define AUTOMATION_RUN
#define AUTOMATION_SIZES        { /*100, 250, 500, 750, 1000, 1250,*/ 1500, 1750, 2000 }
#define AUTOMATION_INSTRUCTIONS { 's' };//, 's', 's' }
#define AUTOMATION_END_AFTER_INSTRUCTIONS

#define IMAGE_SET 0
#define IMAGE_SINGLE 1

#define IMAGE_TICKS                     31

#define TITLE_STRING                    "Combine Alpha v1.0"
#define GROUP_NAME                      "/gradient/"
#define FILE_NAME                       "gradient_centralized"
//#define GROUP_NAME                      "/frames/small/"
//#define FILE_NAME                       "1"
#define FILE_TYPE                       ".png"
#define TYPE                            IMAGE_SINGLE//IMAGE_SET

#if TYPE == IMAGE_SINGLE
#define FRAME_IMAGE_IMAGE_SOURCE_PATH   GROUP_NAME "/" FILE_NAME FILE_TYPE
#define FRAME_IMAGE_SOURCE_NUM_FRAMES   IMAGE_TICKS
#elif TYPE == IMAGE_SET
#define FRAME_IMAGE_IMAGE_SOURCE_PATH   GROUP_NAME
#define FRAME_IMAGE_SOURCE_NUM_FRAMES   31
#endif

//#define PRINT_TUNING_STAGES
#define THRESH_FRAME_PRINT_STEP         30

#ifdef AUTOMATION_RUN
#define FRAME_WIDTH                     50
#define FRAME_HEIGHT                    FRAME_WIDTH
#else
#define FRAME_WIDTH                     700
#define FRAME_HEIGHT                    FRAME_WIDTH
#endif

/* IMAGE PROCESSING */
#define FILE_ROOT                       "/Users/matthew/Desktop/"
#define FRAME_IMAGE_ROOT                FILE_ROOT "PersonalResources/TestImages/"
#define FRAME_SAVE_ROOT                 FILE_ROOT "PCRImages/"

#define sleep(X) usleep(X*1E6)
#define KEY_DELAY 1E3/MAIN_FPS

#define FILENAME                        FILE_ROOT "psm_perf/p_data.csv"
#define X_DIM_FILENAME                  FILE_ROOT "psm_perf/x_data.csv"
#define Y_DIM_FILENAME                  FILE_ROOT "psm_perf/y_data.csv"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define PERF_FILENAME                   FILE_ROOT "psm_perf/perf_data.csv"

#define DASHBOARD_WIDTH                 10000
#define DASHBOARD_HEIGHT                FRAME_HEIGHT

#endif /* test_config_h */
