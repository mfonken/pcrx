//
//  point.c
//  combine_core
//
//  Created by Matthew Fonken on 1/13/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#include "point.h"

static void init( kpoint_t * p, double offset_x, double offset_y, double scale, double focal_length )
{
    p->x = ( p->x - offset_x ) * scale;
    p->y = ( p->y - offset_y ) * scale;
    p->z = focal_length;
    p->phi = atan2( p->x, p->z);
    p->the = atan2( p->y, p->z);
}

static double dist( kpoint_t * p )
{
    return sqrt( KPoint.disq(p) );
}
static double disq( kpoint_t * p )
{
    return ( p->x * p->x ) + ( p->y * p->y );
}

/* Linear angle of point relative to origin */
static double angl( kpoint_t * p )
{
    return atan2( p->y, p->x );
}

/* Combined angle of point relative to origin */
///NOTE: acos(cos()cos()) only works for combining frame-perpendicular angles (i.e. theta & phi)
static double anga( kpoint_t * p )
{
    return acos( cos( p->the ) * cos( p->phi ) );
}

static void copy( kpoint_t * a, kpoint_t * b )
{
    b->x = a->x;
    b->y = a->y;
    b->z = a->z;
    
    b->the = a->the;
    b->phi = a->phi;
}

static void toVec3( kpoint_t * p, vec3_t * v )
{
    v->i = p->x;
    v->j = p->y;
    v->k = p->z;
}

const kpoint KPoint =
{
    .init = init,
    .dist = dist,
    .disq = disq,
    .angl = angl,
    .anga = anga,
    .copy = copy,
    .toVec3 = toVec3
};
