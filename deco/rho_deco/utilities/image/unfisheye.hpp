//
//  unfisheye.h
//  
//
//  Created by Matthew Fonken on 10/30/16.
//
//

#ifndef unfisheye_hpp
#define unfisheye_hpp

#include "opencv_master.h"

#include <stdint.h>
#include <math.h>

//#define PERFORMANCE_TEST
//#define QUICK
//#define ALT_QUICK

#define IN(X,A,B) (X>=A&&X<B)

using namespace cv;

#define LOV_constant 0.414213562
static inline double qdist(double ax, double ay) { return (ax > ay )? ax + ay*LOV_constant:ay + ax*LOV_constant; }
static inline double qatanr_r( double x ) { return 1.0301 - x*(0.0663*x - 0.1784); }

void quickUnfisheye(Mat I, Mat O);
void unfisheye(Mat I, Mat O);
void invfisheye(Mat I, Mat O);

void invfisheye(Point2f * p, int w, int h, double s, double z);
    
#endif /* unfisheye_h */
