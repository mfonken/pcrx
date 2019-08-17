//
//  image_utilities.hpp
//  tau+
//
//  Created by Matthew Fonken on 10/22/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#ifndef image_utilities_hpp
#define image_utilities_hpp

#include "test_setup.h"

#include <stdio.h>
#include <stdint.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <unistd.h>

#include "environment_master.h"
#include "tau_structures.hpp"

#include "unfisheye.hpp"
#include "kinetic.h"

//#define TILT_LINES
//#define GREYSCALE

#define THRESH_IMAGE

#define PATH_NUM_TICKS  200
#define PATH_OFFSET     200
#define TARGET_RADIUS   20
#define DEFAULT_PATH    CIRCLE_DUAL
#define TARGET_COLOR    Scalar(215,215,215)
#define PATH_OFFSET_A   0.2
#define PATH_OFFSET_B   1.1
#define PATH_OFFSET_C   1.5

#define DEFAULT_NOISE   NONE//STATIC_SMALL
#define NOISE_RATE      10  //Hz 
#define NOISE_COLOR     Scalar(200,200,200)
#define NOISE_ORIGIN_X  50
#define NOISE_ORIGIN_Y  200
#define NOISE_WIDTH     150
#define NOISE_HEIGHT    100

typedef enum
{
    CIRCLE_CENTERED = 0,
    CIRCLE_DUAL,
    HORIZONTAL
} path_t;

typedef enum
{
    NONE = 0,
    STATIC_SMALL,
    STATIC_LARGE,
    DYNAMIC_SMALL,
    DYNAMIC_LARGE
} noise_t;

using namespace cv;

class ImageUtility : public TestInterface
{
public:
    int id;
    std::string name;
    virtual void init( void );
    virtual void trigger( void );
    virtual string serialize( void );
    
    void initFile();
    void initCamera();
    void initGenerator();
    
    VideoCapture cam;
    Mat outframe, preoutframe, frame, image;
    cimage_t outimage;
    pthread_mutex_t frame_mtuex,
                    outframe_mutex,
                    outimage_mutex,
                    tau_cross_mutex,
                    self_mutex;
    
    ImageUtility( std::string );
    ImageUtility( std::string, std::string );
    ImageUtility( std::string, std::string, int, int);
    ImageUtility( std::string, std::string, int, int, int );
    
    void RequestBackground();
    
    Mat getNextImage();
    Mat getImage();
    int loop(char c);
    Mat getNextFrame();
    bool isLive();
    void getBeacons();
    void drawOutframe();
    Mat generateImage();
    
    kpoint_t bea[2];
    int thresh;
    bool    generator_active,
            background_request,
            background_ready;
private:
    
    std::string file, subdir;
    int iteration;
    int counter;
    int args;
    bool live;
    bool has_file,
         has_camera,
         has_generator;
    int num_frames;
    
    path_t path;
    int p_x,
        p_y,
        s_x,
        s_y,
        path_center_x,
        path_center_y,
        path_tick,
    path_num_ticks;
    noise_t noise;
    int noise_rate;
    
    cv::Size size;
};

bool thresh(uint8_t t, Vec3d p);

void drawPosition(double x, double y, double z);

#endif /* image_utilities_hpp */

