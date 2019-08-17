//
//  tau_utility.hpp
//  tau+
//
//  Created by Matthew Fonken on 2/7/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#ifndef tau_utility_hpp
#define tau_utility_hpp

#define MAX_COUNT               1000
#define AVERAGE_COUNT           10

#include <stdio.h>
#include <stdint.h>
#include <string>

#include "rho_drawer_xprm.hpp"
#include "environment_master.h"
#include "image_utility.hpp"
#include "rho_wrapper.hpp"
#include "statistics.h"


class Tau : public TestInterface
{
public:
    void        Init( void );
    void        Trigger( void );
    std::string Serialize( void );
    
    Tau( const char * name, int width, int height, std::string f = "", int num = 0, std::string n = "ImageUtility" );
    virtual ~Tau();
    
    double      Perform( cv::Mat );
    double      Perform( cimage_t & );
    void        UpdateThresh( void );
    void        UpdatePrediction( void );
    void        PrintPacket( GlobalPacket *, int );
    
    int                 count,
                        accuracy_count,
                        stddev_sum;
    double              avg,
                        accuracy,
                        current_accuracy,
                        stddev;
    int                 tick,
                        width,
                        height;
    ImageUtility        utility;
    Rho                 rho;
    GlobalPacket        packet;
    point2d_t           A, B;
    cimage_t            image;
    pthread_mutex_t     predictions_mutex,
                      * utility_mutex;
#ifdef RHO_DRAWER
#ifdef __PSM__
    RhoDrawer           rho_drawer;
#endif
#endif
};

#endif /* tau_utility_hpp */
