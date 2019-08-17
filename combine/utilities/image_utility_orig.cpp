//
//  image_utilities.cpp
//  tau+
//
//  Created by Matthew Fonken on 10/22/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include "image_utility.hpp"

using namespace cv;
using namespace std;

ImageUtility::ImageUtility( std::string n ) : ImageUtility(n, "", 0, FNL_RESIZE_W, FNL_RESIZE_H)
{
    printf("IU: Default\n");
}

ImageUtility::ImageUtility( std::string n, std::string f, int width, int height) : ImageUtility(n, f, 0, width, height)
{
    printf("IU: Still\n");
}

ImageUtility::ImageUtility( std::string n, std::string f, int num, int width, int height ) : frame(Size(FNL_RESIZE_W, FNL_RESIZE_H), CV_8UC3, Scalar(0,0,0)), preoutframe(Size(FNL_RESIZE_W, FNL_RESIZE_H), CV_8UC3, Scalar(0,0,0)), outframe(Size(FNL_RESIZE_W, FNL_RESIZE_H), CV_8UC3, Scalar(0,0,0)), image(Size(FNL_RESIZE_W, FNL_RESIZE_H), CV_8UC3, Scalar(0,0,0))
#ifdef HAS_CAMERA
,cam(CAMERA_ID)
,image(Size(CAM_WIDTH, CAM_HEIGHT), CV_8UC3, Scalar(0,0,0))
#endif
{
    this->name = n;
    this->subdir = f;
    has_file = false;
#ifdef HAS_FILE
    if(f.length() > 1) has_file = true;
#endif
    num_frames = num;
    if(num_frames) printf("IU: Dynamic\n");
    size.width = width;
    size.height = height;
    cimageInit(&outimage, width, height);
    pthread_mutex_init(&outframe_mutex, NULL);
    pthread_mutex_init(&outimage_mutex, NULL);
    pthread_mutex_init(&tau_cross_mutex, NULL);
    pthread_mutex_init(&self_mutex, NULL);
    thresh = THRESHOLD;
    
    path = DEFAULT_PATH;
    path_center_x = size.width/2;
    path_center_y = size.height/2;
    p_x = 0; p_y = 0; s_x = 0; s_y = 0;
    path_tick = 0;
    path_num_ticks = PATH_NUM_TICKS;
    
    noise = DEFAULT_NOISE;
    noise_rate = NOISE_RATE;
    
    background_ready = false;
}

void ImageUtility::init()
{
    has_file
#ifdef HAS_FILE
    = true;
#else
    = false;
#endif
    has_camera
#ifdef HAS_CAMERA
    = true;
#else
    = false;
#endif
    has_generator
#ifdef HAS_GENERATOR
    = true;
#else
    = false;
#endif
    
    printf("Initializing Image Utility: ");
    if(has_file) initFile();
    if(has_camera) initCamera();
    if(has_generator) initGenerator();
}

void ImageUtility::initFile()
{
    Mat temp(size.height, size.width, CV_8UC3, Scalar(0,0,0));
    frame = temp;
    counter = 0;
    file = IMAGE_ROOT;
    
    file.append(subdir);
    file.append("/");
    subdir = file;
    if(num_frames > 0)
    {
        printf("With %d %dx%d frames.\n", num_frames, size.width, size.height);
        counter = 1;
        
        file.append( to_string(counter%num_frames+1) );
        file.append(".png");
    }
    else
    {
        printf("With a single %dx%d frame.\n", size.width, size.height);
        file.append(".bmp");
    }
    printf("\tOpening file: %s\n", file.c_str());
    
    image = imread(file, IMREAD_COLOR );
    if( image.empty() )                      // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return;
    }
    
    resize(image,frame,size);
    preoutframe = frame;
    live = true;
}

void ImageUtility::initCamera()
{
    counter = 0;
    
    cam.set(CV_CAP_PROP_FRAME_WIDTH,  CAM_WIDTH);
    cam.set(CV_CAP_PROP_FRAME_HEIGHT, CAM_HEIGHT);
    cam.set(CV_CAP_PROP_FPS,          CAM_FRAME_RATE);
    
    if (!cam.isOpened()) cout << "cannot open camera" << endl;
    cam.read(image);
    
    printf("Initializing Camera: %dx%d @ %d fps.\n", image.cols, image.rows, (int)cam.get(CV_CAP_PROP_FPS));
    
#ifdef GREYSCALE
    Mat grey;
    cv::cvtColor(image, grey, cv::COLOR_BGR2GRAY);
    image = grey;
#endif
    
    live = true;
    
    resize(image,frame,size);
    preoutframe = frame;
}

void ImageUtility::initGenerator()
{
    
}

void ImageUtility::RequestBackground()
{
    background_request = true;
}

void ImageUtility::trigger()
{
//    pthread_mutex_lock(&self_mutex);
    ++counter;
    if(has_file)   preoutframe = getNextImage();
    if(has_camera) preoutframe = getNextFrame();

    if( background_request )
    {
//        pthread_mutex_lock(&tau_cross_mutex);
        putText(preoutframe, "BACKGROUNDING", Point(10,100), FONT_HERSHEY_PLAIN, 1, Vec3b(0,245,15), 2);
        background_ready = true;
        background_request = false;
//        pthread_mutex_unlock(&tau_cross_mutex);
    }
    else if(has_generator) generateImage();
    
//    pthread_mutex_lock(&outframe_mutex);
#ifdef THRESH_IMAGE
    cv::threshold(preoutframe, outframe, thresh, BRIGHTNESS, 0);
#else
    preoutframe.copyTo(outframe);
#endif
    
//    pthread_mutex_lock(&outimage_mutex);
    if(outframe.cols < 10000)
        cimageFromMat(outframe, &outimage);
//    pthread_mutex_unlock(&outimage_mutex);
//    pthread_mutex_unlock(&outframe_mutex);
//    pthread_mutex_unlock(&self_mutex);
}

std::string ImageUtility::serialize()
{
    return this->name;
}

int ImageUtility::loop(char c)
{
    switch(c)
    {
        case ' ':
            live = !live;
            break;
        case 03:
            break;
        case 02:
            counter-=2;
            if( counter < 1 ) counter += path_num_ticks;
            if( counter < 1 ) counter = 1;
            break;
        default:
            return 0;
    }
    if( has_file ) preoutframe = getImage();
    if( has_camera ) preoutframe = getNextFrame();
    return 1;
}

Mat ImageUtility::getNextImage()
{
    if(!live) return(frame);
    
    if(counter > 0)
    {
        counter %= path_num_ticks;
    }
    return getImage();
}

Mat ImageUtility::getImage()
{
    file = subdir + to_string( counter%num_frames+1) + ".png";
    Mat rdImage = imread( file, IMREAD_COLOR );
    if( rdImage.empty() )
    {
        cout <<  "Could not open or find the image " << file << std::endl ;
        return frame;
    }
    if(frame.cols < 10000)
    {
        resize(rdImage,frame,size);
        return frame;
    }
    return Mat(size, CV_8UC3, Scalar{0,0,0});
}

Mat ImageUtility::getNextFrame()
{
    cam >> image;
    resize(image, frame, size, 1, 1);
    
#ifdef REDSCALE
    Mat bgr[3];
    split(frame,bgr);
    frame = bgr[2];
#endif
    
#ifdef GREYSCALE
    Mat grey;
    cv::cvtColor(frame, grey, cv::COLOR_BGR2GRAY);
    frame = grey;
#endif

    return frame;
}

void ImageUtility::getBeacons()
{
    Mat out = getNextFrame();
}

void ImageUtility::drawOutframe()
{
    putText(outframe, "A", Point(bea[1].x, bea[1].y), FONT_HERSHEY_PLAIN, 2, Vec3b(0,55,255), 3);
    putText(outframe, "B", Point(bea[0].x, bea[0].y), FONT_HERSHEY_PLAIN, 2, Vec3b(0,255,55), 3);
#ifdef TILT_LINES
    double top_x, bot_x, lef_y, rig_y, tr = tan(0/*-bno.roll * DEG_TO_RAD*/);
    top_x = FNL_RESIZE_W/2 - FNL_RESIZE_H/2*tr;
    bot_x = FNL_RESIZE_W/2 + FNL_RESIZE_H/2*tr;
    lef_y = FNL_RESIZE_H/2 + FNL_RESIZE_W/2*tr;
    rig_y = FNL_RESIZE_H/2 - FNL_RESIZE_W/2*tr;
    
    line(outframe, Point(top_x,0), Point(bot_x,FNL_RESIZE_H), Vec3b(255,245,255));
    line(outframe, Point(0,lef_y), Point(FNL_RESIZE_W,rig_y), Vec3b(255,245,255));
#endif
}

bool ImageUtility::isLive()
{
    return live;
}

void drawPosition(double x, double y, double z)
{
    int width = 800, act_width = 600, height = 600, step = 50;
    Mat P(height, width, CV_8UC3, Scalar(0,0,0));
    
    const Vec3b white(255,255,255);
    /* Draw Grid */
    for(int x = 0; x <= act_width; x+=step) for(int y = 0; y < act_width; y++) P.at<Vec3b>(y,x) = white;
    for(int y = 0; y < height; y+=step) for(int x = 0; x < act_width; x++) P.at<Vec3b>(y,x) = white;
    
    int HORZ_INSET = 100;
    
    /* Draw Beacons */
    Point A(HORZ_INSET, 200), B(HORZ_INSET, height - 200);
    int cir_rad = 5;
    const Vec3b cir_col(0,0,255);
    circle(P, A, cir_rad, cir_col, 2, 8, 0);
    circle(P, B, cir_rad, cir_col, 2, 8, 0);
    
    line(P, Point(act_width, 0), Point(act_width, height), white, 3, 8, 0);
    for(int y = 0; y < height; y+=step*2) for(int x = act_width; x < width; x++) P.at<Vec3b>(y,x) = white;
    
    double unit_scale = (B.y-A.y)/D_FIXED;
    
    /* Draw Given (X,Y) */
    Point C( y * unit_scale + HORZ_INSET, height/2 + x * unit_scale - (B.y-A.y) );
    circle(P, C, 10, Vec3b(255,100,0), 2, 8, 0);
    
    /* Draw Given Z */
    double thisZ = height/2 - z * unit_scale;
    line(P, Point(act_width, thisZ), Point(width, thisZ), Vec3b(255,100,0), 3, 10, 0);
    
    //    imshow("Position", P);
}

Mat ImageUtility::generateImage()
{
    path_tick = counter % path_num_ticks;
    if( !has_file && !has_camera ) frame = {0};
    
    switch(noise)
    {
        case STATIC_SMALL:
            rectangle(frame, Point(NOISE_ORIGIN_X,NOISE_ORIGIN_Y), Point(NOISE_ORIGIN_X+NOISE_WIDTH,NOISE_ORIGIN_Y+NOISE_HEIGHT), NOISE_COLOR, -1);
            break;
        case STATIC_LARGE:
            break;
        case DYNAMIC_SMALL:
            if( !( path_tick % noise_rate ) )
            {
                
            }
            break;
        case DYNAMIC_LARGE:
            if( !( path_tick % noise_rate ) )
            {
                
            }
            break;
        default:
            break;
    }
    
    double phase = (double)path_tick/(double)path_num_ticks,
        cphase = 2 * M_PI * phase,
        ocoscp = PATH_OFFSET * cos(cphase),
        osincp = PATH_OFFSET * sin(cphase),
        path_left = path_center_x - size.width/4,
        path_right = path_center_x + size.width/4,
        path_top = path_center_y - size.height/4,
        path_bottom = path_center_y + size.height/4,
        radius_offset = abs(0.5 - phase)*2;
    circle(frame, Point(p_x, p_y), TARGET_RADIUS/**(0.5+radius_offset)*/, TARGET_COLOR, -1);
    circle(frame, Point(s_x, s_y), TARGET_RADIUS/**(1.5-radius_offset)*/, TARGET_COLOR, -1);
//    circle(frame, Point(p_x, p_y), TARGET_RADIUS*(1.0+radius_offset), TARGET_COLOR, -1);
//    circle(frame, Point(s_x, s_y), TARGET_RADIUS*(1.0+radius_offset), TARGET_COLOR, -1);
    switch( path )
    {
        case CIRCLE_CENTERED:
            p_x = path_center_x + PATH_OFFSET_B*ocoscp + TARGET_RADIUS;
            p_y = path_center_y + PATH_OFFSET_C*osincp;
            s_x = path_center_x - PATH_OFFSET_A*ocoscp;
            s_y = path_center_y - PATH_OFFSET_B*osincp - TARGET_RADIUS;
            break;
        case CIRCLE_DUAL:
            p_x = path_left     + PATH_OFFSET_A*ocoscp + TARGET_RADIUS;
            p_y = path_top      + PATH_OFFSET_A*osincp;
            s_x = path_right    - PATH_OFFSET_A*ocoscp;
            s_y = path_bottom   - PATH_OFFSET_A*osincp - TARGET_RADIUS;
            break;
        case HORIZONTAL:
            break;
        default:
            break;
    }
//    circle(frame, Point(size.width/2, size.height/2), TARGET_RADIUS*2, NOISE_COLOR, -1);
    preoutframe = frame;
    return frame;
}
