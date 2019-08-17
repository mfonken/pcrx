/***********************************************************************************************//**
* \file   matrix.c
* \brief  Matrix Math
***************************************************************************************************
*      Author: Matthew Fonken
**************************************************************************************************/

/* Own header */
#include "vector.h"

double len3( vec3_t * v )
{
    double i_2 = v->i * v->i;
    double j_2 = v->j * v->j;
    double k_2 = v->k * v->k;
    return sqrt( i_2 + j_2 + k_2 );
}

void nrm3( vec3_t * v )
{
    double length = len3( v );
    if(!length) return;
    v->i /= length;
    v->j /= length;
    v->k /= length;
}

void sub3( vec3_t * x, vec3_t * y )
{
    x->i = x->i - y->i;
    x->j = x->j - y->j;
    x->k = x->k - y->k;
}

/* Dot product of two vectors */
double dot3( vec3_t * u, vec3_t * v )
{
    return u->i*v->i + u->j*v->j + u->k*v->k;
}

/* Angle between two vectors */
double ang3( vec3_t * u, vec3_t * v)
{
    double lu, lv;
    lu = len3(u);
    if( lu )
    {
        lv = len3(v);
        if( lv )
        {
            return acos( dot3( u, v ) / ( lu * lv ) );
        }
    }
    return M_PI_2;
}

/* Multiply a vector by a scalar */
void mul3( double m, vec3_t * v, vec3_t * r )
{
    r->i = m * v->i;
    r->j = m * v->j;
    r->k = m * v->k;
}

/* Cross product of two vectors */
void cross3( vec3_t * u, vec3_t * v, vec3_t * r )
{
    r->i = u->j*v->k - u->k*v->j;
    r->j = u->k*v->i - u->i*v->k;
    r->k = u->i*v->j - u->j*v->i;
}

/* Add three vectors */
void add33( vec3_t * u, vec3_t * v, vec3_t * w, vec3_t * r)
{
    r->i = u->i + v->i + w->i;
    r->j = u->j + v->j + w->j;
    r->k = u->k + v->k + w->k;
}

const struct kvector Vector =
{
    .sub3 = sub3,
    .len3 = len3,
    .nrm3 = nrm3,
    .ang3 = ang3,
    .dot3 = dot3,
    .mul3 = mul3,
    .add33 = add33,
    .cross3 = cross3
};

/** @} (end addtogroup kinetic) */
/** @} (end addtogroup Application) */
