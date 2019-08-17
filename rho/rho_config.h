/************************************************************************
 *  File: rho_config.h
 *  Group: Rho Core
 ***********************************************************************/

#ifndef rho_config_h
#define rho_config_h

#include "rho_global.h"

//#define SPOOF_STATE_BANDS

/***************************************************************************************/
/*                               CAPTUE PARAMETERS                                     */
/***************************************************************************************/

#define IS_RGGB_ELIMINATE_G     true

/* Capture Config */
#ifdef USE_INTERRUPT_MODEL
#define CAPTURE_WIDTH           700
#define CAPTURE_HEIGHT          700
#else
/* Camera Config */
#define RHO_WIDTH               1280
#define RHO_HEIGHT              800

#define CAPTURE_DIV             4
#define CAPTURE_WIDTH           (RHO_WIDTH>>CAPTURE_DIV)
#define CAPTURE_HEIGHT          (RHO_HEIGHT>>CAPTURE_DIV)
#endif

#define FRAME_SIZE              (CAPTURE_WIDTH*CAPTURE_HEIGHT)
#define CAPTURE_SUB_SAMPLE      SUBSAMPLE_APPLICATION

#if defined __linux || defined __APPLE__
#define DENSITY_MAP_X_SIZE       CAPTURE_WIDTH
#define DENSITY_MAP_Y_SIZE       CAPTURE_HEIGHT
#else
#define DENSITY_MAP_X_SIZE       DENSITY_MAP_X_LENGTH
#define DENSITY_MAP_Y_SIZE       DENSITY_MAP_Y_LENGTH
#endif

#define CAPTURE_BUFFER_WIDTH    (uint32_t)CAPTURE_WIDTH
#define CAPTURE_BUFFER_HEIGHT   (uint32_t)CAPTURE_HEIGHT
#define CWL                     80 // CAPTURE_BUFFER_WIDTH
#define CAPTURE_BUFFER_SIZE     CAPTURE_BUFFER_LENGTH

#define SS                      6 // DEFAULT_SUBSAMPLE

#define COVERAGE_NORMAL_MAX     0.45
#define THRESH_BUFFER_SIZE      THRESH_BUFFER_LENGTH
#define THRESH_BUFFER_MAX       THRESH_BUFFER_LENGTH//((index_t)(sizeof(index_t)*(index_t)THRESH_BUFFER_SIZE))
#define DEFAULT_THRESH          170//250

#define FILTERED_COVERAGE_TARGET   0.007
#define MAX_COVERAGE            1
#define C_FRAME_SIZE            ((int)(MAX_COVERAGE * FRAME_SIZE))
#define Y_DEL                   0xaaaa


/***************************************************************************************/
/*                                RHO PARAMETERS                                       */
/***************************************************************************************/
#define THRESH_STEP_MAX     5
#define THRESH_MIN          1
#define THRESH_MAX          250

#define MIN_VARIANCE        3
#define MAX_VARIANCE        20

#define MAX_REGION_HEIGHT   200
#define RHO_GAP_MAX         10

#define BACKGROUND_CENTROID_CALC_THRESH 10 // pixels

#define BACKGROUNDING_PERIOD   100000 // Frames

#define EXPECTED_NUM_REGIONS  2
#define MAX_REGIONS           4
#define MIN_REGION_DENSITY    2
#define MAX_REGION_SCORE      10
#define REGION_SCORE_FACTOR   0.5
#define MAX_NU_REGIONS        NUM_STATE_GROUPS+1
#define MAX_OBSERVATIONS      1 << 4

#define MAX_RHO_RECALCULATION_LEVEL 3

#define MAX_TRACKING_FILTERS MAX_REGIONS
#define MIN_TRACKING_KALMAN_SCORE 0.02
#define MAX_TRACKING_MATCH_DIFFERNCE 500
#define TRACKING_MATCH_TRUST 0.4

#define TARGET_TUNE_FACTOR  1.0
#define STATE_TUNE_FACTOR   0.2

#define PIXEL_COUNT_TRUST_FACTOR    0.2
#define PIXEL_COUNT_DROP_FACTOR     0.75

#define FRAME_QUADRANT_TOP_LEFT_INDEX   0
#define FRAME_QUADRANT_TOP_RIGHT_INDEX  1
#define FRAME_QUADRANT_BTM_LEFT_INDEX   2
#define FRAME_QUADRANT_BTM_RIGHT_INDEX  3

#define TOTAL_RHO_PIXELS    ( CAPTURE_WIDTH * CAPTURE_HEIGHT )


/***************************************************************************************/
/*                              FILTER PARAMETERS                                      */
/***************************************************************************************/
/* Kalman Filter Configs */
#define RHO_DEFAULT_LS      5.
#define RHO_DEFAULT_VU      0.001
#define RHO_DEFAULT_BU      0.001
#define RHO_DEFAULT_SU      0.01
#define DEFAULT_KALMAN_UNCERTAINTY \
(rho_kalman_uncertainty_c){ RHO_DEFAULT_VU, RHO_DEFAULT_BU, RHO_DEFAULT_SU }

#define RHO_PREDICTION_LS   1.
#define RHO_PREDICTION_VU   0.05
#define RHO_PREDICTION_BU   0.001
#define RHO_PREDICTION_SU   0.05
#define DEFAULT_PREDICTION_UNCERTAINTY \
(rho_kalman_uncertainty_c){ RHO_PREDICTION_VU, RHO_PREDICTION_BU, RHO_PREDICTION_SU }

#define RHO_TARGET_LS       5.
#define RHO_TARGET_VU       0.001
#define RHO_TARGET_BU       0.05
#define RHO_TARGET_SU       0.1
#define DEFAULT_TARGET_UNCERTAINTY \
(rho_kalman_uncertainty_c){ RHO_TARGET_VU, RHO_TARGET_BU, RHO_TARGET_SU }

//#define RHO_SQRT_HEIGHT     sqrt(CAPTURE_HEIGHT)
//#define RHO_DIM_INFLUENCE   0.1
//#define RHO_K_TARGET_IND    0.3
#define RHO_K_TARGET        10.//0.12//RHO_K_TARGET_IND+(10/RHO_SQRT_HEIGHT*RHO_DIM_INFLUENCE)
#define RHO_VARIANCE_NORMAL 20.//RHO_SQRT_HEIGHT/5.0
#define RHO_VARIANCE_SCALE  10.//RHO_SQRT_HEIGHT/3.0//1.32
#define RHO_VARIANCE(X)     RHO_VARIANCE_NORMAL * ( 1 + RHO_VARIANCE_SCALE * ( RHO_K_TARGET - X ) )

/* PID Filter Configs */
#define PID_SCALE   1//10
#define PID_DRIFT   1.1

#define DEFAULT_PROPORTIONAL_FACTOR 0.5
#define DEFAULT_INTEGRAL_FACTOR     0.002
#define DEFAULT_DERIVATIVE_FACTOR   0.001
#define DEFAULT_PID_GAIN (rho_pid_gain_t){ DEFAULT_PROPORTIONAL_FACTOR, DEFAULT_INTEGRAL_FACTOR, DEFAULT_DERIVATIVE_FACTOR, 0., 0. }

#define MIN_STATE_CONFIDENCE    0.01 //0.5
#define BACKGROUND_PERCENT_MIN  0.02
#define BACKGROUND_COVERAGE_MIN ((int)(BACKGROUND_PERCENT_MIN*FRAME_SIZE))
//#define BACKGROUND_COVERAGE_TOL_PR   0.001
//#define BACKGROUND_COVERAGE_TOL_PX   ((int)(BACKGROUND_COVERAGE_TOL_PR*FRAME_SIZE))

#define SPOOF_STATE_BANDS { 0.2, 0.5, 0.75, 1.0 }

#endif /* rho_config_h */
