//
//  rho_drawer_xprm.hpp
//  pcrxprm
//
//  Created by Matthew Fonken on 2/5/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef rho_drawer_xprm_hpp
#define rho_drawer_xprm_hpp

//#ifdef RHO_DRAWER
#ifdef __PSM__

#include "rho_master.h"
#include "opencv_master.h"
#include <string.h>

#define DETECTION_ELEMENT_RADIUS    4

#define DETECTION_MAP_FRAME_OWIDTH  700
#define DETECTION_MAP_FRAME_OHEIGHT DETECTION_MAP_FRAME_OWIDTH
#define DETECTION_MAP_INSET         0//15
#define DETECTION_MAP_FRAME_IWIDTH  DETECTION_MAP_FRAME_OWIDTH-DETECTION_MAP_INSET+DETECTION_ELEMENT_RADIUS
#define DETECTION_MAP_FRAME_IHEIGHT DETECTION_MAP_FRAME_OHEIGHT-DETECTION_MAP_INSET+DETECTION_ELEMENT_RADIUS

#define MAX_DETECTION_MAP_DENSITTY_VALUE 500
#define MAX_DETECTION_MAP_THRESH_VALUE (1 << 8)

#define DETECTION_MAP_INTERVAL  10

#define COV_SCALE 0.25

#define NUM_LABELS_TO_SHOW 5

using namespace cv;

template <typename T>
static std::string pto_string(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
}

#ifdef __PSM__

static double GetCovarianceAngle( mat2x2 * covariance )
{
    double a = covariance->a, b = covariance->b, d = covariance->d,
    a_minus_d = a - d,
    radius = sqrt( a_minus_d * a_minus_d + 4. * b * b ),
    lambda = a_minus_d + radius;
    return atan2( -2 * b, a - lambda ) * 180 / M_PI;
}

class RhoDrawer
{
    uint16_t counter = 0;
    Mat detection_map_frame;
    psm_t * psm;
public:
    RhoDrawer(psm_t *);

    void DrawDetectionMap( detection_map_t * );
    void PostProcess( psm_t * );
    
    Mat& GetDetectionMapFrame() {return detection_map_frame;}
};
//#endif
#endif
#endif
#endif /* rho_drawer_xprm_hpp */
