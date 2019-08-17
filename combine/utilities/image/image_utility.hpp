//
//  image_utilities.hpp
//  tau+
//
//  Created by Matthew Fonken on 10/22/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#ifndef image_utilities_hpp
#define image_utilities_hpp

//#include "test_setup.h"

#include <stdio.h>
#include <stdint.h>
#include <string>
#include <iostream>
#include <unistd.h>

#include "environment_master.h"

#include "opencv_master.h"
#include "image_config.h"
#include "image_cpp_types.h"
#include "unfisheye.hpp"

#ifdef __TAU__
//#include "tau_structures.hpp"
#endif

//#define DEBUG_IU

#ifndef LOG
#define LOG(...) printf(__VA_ARGS__)
#endif

#ifdef DEBUG_IU
#define LOG_IU(...) LOG("<ImageUtility> " __VA_ARGS__)
#else
#define LOG_IU(...)
#endif


//#include "kinetic.h"

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

bool thresh(uint8_t t, Vec3d p);
void drawPosition(double x, double y, double z);

class ImageUtility : public TestInterface
{
public:
    void Init( void );
    void Trigger( void );
    string Serialize( void );
    
    static ImageUtility& Instance();
    
    ImageUtility( std::string, std::string f, int num, int width, int height);
    virtual ~ImageUtility();
    
    void InitFile();
    void InitCamera();
    void InitGenerator();
    
    VideoCapture cam;
    Mat outframe, preoutframe, frame, image, preimage;
    cimage_t outimage;
    pthread_mutex_t frame_mtuex,
                    outframe_mutex,
                    outimage_mutex,
                    tau_cross_mutex,
                    self_mutex;
    
    void RequestBackground();
    Mat GetNextImage();
    Mat GetImage();
    int Loop(char c);
    Mat GetNextFrame();
    bool IsLive();
    void DrawOutframe();
    void GenerateImage(Mat&);
    
    int Cx, Cy, pCx, pCy;
    point2d_t bea[2];
    int thresh;
    bool    generator_active,
            background_request,
            background_ready;
private:
    
    std::string file, subdir;
    int iteration;
    int counter;
    int args;
    bool single_frame;
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

#endif /* image_utilities_hpp */

