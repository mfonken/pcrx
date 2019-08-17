/***************************************************************************************************
 * \file   quaternion.h
 * \brief  Quaternion Math Header
 ***************************************************************************************************
 *      Author: Matthew Fonken
 **************************************************************************************************/

#ifndef quaternion_h
#define quaternion_h

#ifdef __cplusplus
extern "C" {
#endif
    
    /* Standard headers */
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
    
/* Included types header */
#include "vector.h"
    
#define SIGN(X)   X>=0?1:-1
#define RASIN(X)  X>-1&&X<1?asin(X):SIGN(X)*M_PI/2
    
#define RAD_TO_DEG (180 / M_PI)
#define DEG_TO_RAD (M_PI / 180)

    /** Tait-Bryan Angles */
    typedef struct _ang3_t
    {
        double    x;                /**< Phi value */
        double    y;                /**< Theta value */
        double    z;                /**< Psi value */
    } ang3_t;
    
    /*! Quaternion hypercomplex type */
    typedef struct
    {
        double x;
        double y;
        double z;
        double w;
    } quaternion_t;
    
    struct quaternion
    {
        void (*fromEuler)( ang3_t       * a, quaternion_t * q );
        void (*toEuler)(   quaternion_t * q, ang3_t       * a );
        void (*toMatrix)(  quaternion_t * q, mat3x3_t     * m );
        void (*rotVec)(    vec3_t       * v, quaternion_t * q, vec3_t       * r );
        void (*combine)(   quaternion_t * a, quaternion_t * b, quaternion_t * c );
        void (*copy)(      quaternion_t * a, quaternion_t * b );
    };
              
    extern const struct quaternion Quaternion;
    
    struct angle
    {
        void  (*degToRad)( ang3_t * a );
        void  (*radToDeg)( ang3_t * a );
    };
    
    extern const struct angle Angle;
    
    
#ifdef __cplusplus
}
#endif

#endif /* matrix_h */
