/*! \file spatial.h
 \brief Spatial Orientation Main\r\n
 
 NOTES:
 - IMU is for rotational data ONLY!
 - Positional data comes from camera triangulation module and beacons
 - Right-handed rotation
 
 Created by Matthew Fonken on 10/8/16.
 */

#ifndef kinetic_h
#define kinetic_h

#include <stdio.h>
#include <stdint.h>
#include <math.h>

//#include "test_setup.h"

/* Math headers */
#include "qmath.h"

/* Filters */
#include "kalman.h"

/* Kalman Defaults */
#define MOTION_MAX_KALMAN_LIFE     10.0
#define MOTION_VALUE_UNCERTAINTY   0.01
#define MOTION_BIAS_UNCERTAINTY    0.003
#define MOTION_SENSOR_UNCERTAINTY  0.02

/** Absolute value */
#define     absl(x) x > 0 ? x:-x

/** Initial normal unit vector to beacon plane */
#define     VISION_ZSTATE_I  0
#define     VISION_ZSTATE_J  0
#define     VISION_ZSTATE_K -1

/* Camera Precalculated values */
#define     CAMERA_OFFSET_ANGLE_X   0       // radians
#define     CAMERA_OFFSET_ANGLE_Y   0       // radians
#define     CAMERA_OFFSET_ANGLE_Z   0       // radians

#define     REFERENCE_OFFSET_ANGLE_X   0    // radians
#define     REFERENCE_OFFSET_ANGLE_Y   0    // radians
#define     REFERENCE_OFFSET_ANGLE_Z   -110 * DEG_TO_RAD //M_PI_2 // radians

//#define     CAMERA_ALPHA_W          120     // degrees
//#define     CAMERA_ALPHA_H          75      // degrees

#define     KINETIC_CAMERA_WIDTH    1//CAMERA_WIDTH//1280    // pixels
#define     KINETIC_CAMERA_HEIGHT   1//CAMERA_HEIGHT//800     // pixels
#define     CAMERA_HALF_WIDTH       FNL_RESIZE_W/2
#define     CAMERA_HALF_HEIGHT      FNL_RESIZE_H/2

#define     UNITS                  "cm"
#define     PIXEL_TO_UNIT           3e-6
#define     UNIT_TO_PIXEL         ( 1.0 / PIXEL_TO_UNIT )

#define     CAMERA_HALF_WIDTH_UNIT  CAMERA_HALF_WIDTH*PIXEL_TO_UNIT
#define     CAMERA_HALF_HEIGHT_UNIT CAMERA_HALF_HEIGHT*PIXEL_TO_UNIT

#define     FOCAL_REFRACTION        2.4e-6//1.3e-6//  // units / units
#define     FOCAL_LENGTH          ( 1.39e-3 * FOCAL_REFRACTION ) * UNIT_TO_PIXEL//( 3.5e-3 * FOCAL_REFRACTION ) * UNIT_TO_PIXEL // dimension units

#define     D_FIXED                 5.0e-2

/** Kinetic Type */
typedef struct _kinetic_values_t
{
    double     position[3];             /**< Raw position */
    double     rotation[3];             /**< Raw rotation */
} kinetic_values_t;

typedef struct _kinetic_filters_t
{
    kalman_t position[3];             /**< Raw position */
    kalman_t rotation[3];             /**< Raw rotation */
} kinetic_filters_t;

typedef struct _kinetic_t
{
    kinetic_values_t values;
    kinetic_filters_t filters;
    kpoint_t
    A,
    B;
    double  omega,
    sigmaA,
    sigmaR,
    nu,
    upsilon,
    chi,
    mu,
    gamma;
    int     W,
    H;
    double  f_l,
    d_l,
    d__l,
    r_l;
    ang3_t  raw,
            e;
    vec3_t  r,
    n;
    quaternion_t
    qd,
    qd_,
    qs,
    qs_,
    qc,
    qc_,
    qr,
    qg,
    qa;
} kinetic_t;

typedef struct _serial_kinetic_t
{
    ang3_t  orientation;
    vec3_t  position;
    struct timeval timestamp;
} serial_kinetic_t;

typedef struct
{
    void  (*DefaultInit)(kinetic_t * );
    void  (*Init)( kinetic_t *, int, int, double, double);
    void  (*UpdateRotation)( kinetic_t *, ang3_t *, ang3_t * );
    void  (*UpdatePosition)( kinetic_t *, vec3_t *, kpoint_t, kpoint_t );
    
    void (*MinorAngles)( kinetic_t *, kpoint_t, kpoint_t );
    void (*Quaternions)( kinetic_t * );
    void (*MajorAngles)( kinetic_t * );
    int  (*Chi)( kinetic_t * );
    int  (*Mu)( kinetic_t * );
    void (*Gamma)( kinetic_t * );
    int  (*R_l)( kinetic_t * );
    void (*R)( kinetic_t * );
    void (*Nongrav)( kinetic_t *, vec3_t * );
} kinetic_functions;

extern kinetic_functions KineticFunctions;

void Filters_Init( kinetic_t *);
void Camera_Rotation_Init( kinetic_t *);
void Reference_Rotation_Init( kinetic_t *);

#endif /* kinetic_h */

