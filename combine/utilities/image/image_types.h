//
//  image_types.h
//  pcr
//
//  Created by Matthew Fonken on 12/15/18.
//  Copyright © 2018 Matthew Fonken. All rights reserved.
//

#ifndef image_types_h
#define image_types_h

typedef unsigned char pixel_base_t;
typedef struct
{
    int width, height;
    pixel_base_t * pixels;
} cimage_t;

typedef struct
{
    double x, y;
} point2d_t;

#endif /* image_types_h */
