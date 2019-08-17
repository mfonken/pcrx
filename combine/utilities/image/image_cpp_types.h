//
//  image_cpp_types.h
//  pcr
//
//  Created by Matthew Fonken on 12/15/18.
//  Copyright © 2018 Matthew Fonken. All rights reserved.
//

#ifndef image_cpp_types_h
#define image_cpp_types_h

#include "image_types.h"

static inline uint8_t pixelDist(cv::Vec3b p) { return p[2]>100?0xff:0; /*sqrt(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]);*/ }
static void cimageInit( cimage_t& img, int width, int height )
{
    img.width = width;
    img.height = height;
    img.pixels = (pixel_base_t *)malloc(sizeof(pixel_base_t)*width*height);
}

static void cimageFromMat( cv::Mat &mat, cimage_t& img )
{
    if(mat.cols != img.width)
        return;
    int w = mat.cols, h = mat.rows, p = 0;
    for(int y = 0; y < h; y++ )
    {
        for(int x = 0; x < w; x++, p++ )
        {
            cv::Vec3b c = mat.at<cv::Vec3b>(y,x);
            img.pixels[p] = pixelDist(c);
        }
    }
}
static void cimageToMat( cimage_t& img, cv::Mat mat )
{
    int w = img.width, h = img.height, p = 0;
    unsigned char d = 0;
    mat.cols = w;
    mat.rows = h;
    for(int y = 0; y < h; y++)
    {
        for(int x = 0; x < w; x++)
        {
            p = x + y*h;
            d = (img.pixels[p]>0)*0xff;
            cv::Vec3b c = {d,d,d};
            mat.at<cv::Vec3b>(y,x) = c;
        }
    }
}

#endif /* image_cpp_types_h */
