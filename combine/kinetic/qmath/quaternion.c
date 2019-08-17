/***********************************************************************************************//**
* \file   quaternion.c
* \brief  Quaternion Math
***************************************************************************************************
*      Author: Matthew Fonken
**************************************************************************************************/

/* Own header */
#include "quaternion.h"

/* See - http://www.gamasutra.com/view/feature/131686/rotating_objects_using_quaternions.php */
/* Generic intrinsic euler angles to quaternion conversion */
void fromEuler( ang3_t * a, quaternion_t * q )
{
    double half_roll, half_pitch, half_yaw;
    half_roll   = a->x / 2;
    half_pitch  = a->y / 2;
    half_yaw    = a->z / 2;
    double cr, cp, cy, sr, sp, sy, cpcy, spsy, spcy, cpsy;
    cr = cos( half_roll  );
    cp = cos( half_pitch );
    cy = cos( half_yaw   );
    sr = sin( half_roll  );
    sp = sin( half_pitch );
    sy = sin( half_yaw   );
    cpcy = cp * cy;
    spsy = sp * sy;
    spcy = sp * cy;
    cpsy = cp * sy;
    q->w = cr * cpcy + sr * spsy;
    q->x = sr * cpcy - cr * spsy;
    q->y = cr * spcy + sr * cpsy;
    q->z = cr * cpsy - sr * spcy;
}

void toEuler( quaternion_t * q, ang3_t * a )
{
    double w = q->w, x = q->x, y = q->y, z = q->z, m, n;
    
    m = 2*(w*x+y*z);
    n = 1-2*(x*x+y*y);
    a->x = atan2(m, n);
    
    m = 2*(w*y-x*z);
    a->y = RASIN(m);
    
    m = 2*(w*z+x*y);
    n = 1-2*(y*y+z*z);
    a->z = atan2(m, n);
}

void toMatrix(quaternion_t * q, mat3x3_t * m)
{
    double wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
    double w = q->w, x = q->x, y = q->y, z = q->z;
    
    x2 = x + x;
    y2 = y + y;
    z2 = z + z;
    xx = x * x2; xy = x * y2; xz = x * z2;
    yy = y * y2; yz = y * z2; zz = z * z2;
    wx = w * x2; wy = w * y2; wz = w * z2;
    
    m->ii = 1.0 - (yy + zz);
    m->ji = xy - wz;
    m->ki = xz + wy;
    
    m->ij = xy + wz;
    m->jj = 1.0 - (xx + zz);
    m->kj = yz - wx;
    
    m->ik = xz - wy;
    m->jk = yz + wx;
    m->kk = 1.0 - (xx + yy);
}

/* See - https://blog.molecular-matters.com/2013/05/24/a-faster-quaternion-vector-multiplication/ */
void rotVec(vec3_t * v, quaternion_t * q, vec3_t * r)
{
    vec3_t u;
    u.i = q->x;
    u.j = q->y;
    u.k = q->z;
    double s = q->w;
    vec3_t a, b, c, t;
    
    Vector.mul3  (  2, &u, &a );
    Vector.cross3( &a,  v, &t );
    Vector.cross3( &u, &t, &b );
    Vector.mul3  (  s, &t, &c );
    Vector.add33 (  v, &b, &c, r );
}

/* Double quaternion Hamilton multiplication (Generic) */
void combine(quaternion_t * a, quaternion_t * b, quaternion_t * c )
{
    double A, B, C, D, E, F, G, H;
    A = ( a->w + a->x ) * ( b->w + b->x );
    B = ( a->z - a->y ) * ( b->y - b->z );
    C = ( a->w - a->x ) * ( b->y + b->z );
    D = ( a->y + a->z ) * ( b->w - b->x );
    E = ( a->x + a->z ) * ( b->x + b->y );
    F = ( a->x - a->z ) * ( b->x - b->y );
    G = ( a->w + a->y ) * ( b->w - b->z );
    H = ( a->w - a->y ) * ( b->w + b->z );
    c->w = B + ( -E - F + G + H ) / 2;
    c->x = A - (  E + F + G + H ) / 2;
    c->y = C + (  E - F + G - H ) / 2;
    c->z = D + (  E - F - G + H ) / 2;
}

void copy( quaternion_t * a, quaternion_t * b )
{
    b->w = a->w;
    b->x = a->x;
    b->y = a->y;
    b->z = a->z;
}

const struct quaternion Quaternion = {
    .fromEuler = fromEuler,
    .toEuler = toEuler,
    .toMatrix = toMatrix,
    .rotVec = rotVec,
    .combine = combine,
    .copy = copy
};

void radToDeg( ang3_t * a )
{
    a->x *= RAD_TO_DEG;
    a->y *= RAD_TO_DEG;
    a->z *= RAD_TO_DEG;
}

void degToRad( ang3_t * a )
{
    a->x *= DEG_TO_RAD;
    a->y *= DEG_TO_RAD;
    a->z *= DEG_TO_RAD;
}

const struct angle Angle = {
    .radToDeg = radToDeg,
    .degToRad = degToRad
};

/** @} (end addtogroup kinetic) */
/** @} (end addtogroup Application) */
