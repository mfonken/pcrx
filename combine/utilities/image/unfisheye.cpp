//
//  unfisheye.c
//  combine_core
//
//  Created by Matthew Fonken on 12/27/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include "unfisheye.hpp"
#include <highgui.hpp>

using namespace cv;

double strength = 0.1, zoom = 0.5;
int strength_slider = 28, zoom_slider = 11, max_strength = 100, max_zoom = 20;

void quickUnfisheye(Mat I, Mat O)
{
    createTrackbar( "Strength", "Original", &strength_slider, max_strength );
    createTrackbar( "Zoom", "Original", &zoom_slider, max_zoom );
    strength = (double) strength_slider / 10;
    zoom = (double) zoom_slider / 10;
    
    double w = I.cols, h = I.rows;
    double wh = w/2, hh = h/2;
    int x, y = 0, x_, y_, xn, yn;
    double theta_zoom, r;
    
    double correctionRadius_ = (1 / qdist(fabs(wh), fabs(hh))) * strength;
    for(yn = -hh, y = 0; y < h; yn++, y++ )
    {
        for(xn = -wh, x = 0; x < w; xn++, x++ )
        {
            r = qdist(fabs(xn), fabs(yn)) * correctionRadius_;
            theta_zoom = qatanr_r(r) * zoom;
            x_ = wh + theta_zoom * xn;
            y_ = hh + theta_zoom * yn;
            if(IN(x_, 0, w) && IN(y_, 0, h))
                O.at<Vec3b>(y,x) = I.at<Vec3b>(y_,x_);
        }
    }
    putText(O, "Strength: " + std::to_string(strength), Point(30,30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(200,200,250), 1);
    putText(O, "Zoom: " + std::to_string(zoom), Point(30,48), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(200,200,250), 1);
}

void unfisheye(Mat I, Mat O)
{
    createTrackbar( "Strength", "Original", &strength_slider, max_strength );
    createTrackbar( "Zoom", "Original", &zoom_slider, max_zoom );
    
    strength = (double) strength_slider / 10;
    zoom = (double) zoom_slider / 10;
    
    double w = I.cols, h = I.rows;
    double wh = w/2, hh = h/2;
    Vec3b c = {0,0,0};
    int x, y = 0, x_, y_, xn, yn;
    double theta, r;
    
#ifdef QUICK
    double correctionRadius_ = 1 / sqrt((wh*wh)+(hh*hh)) * strength;
    for(yn = -h/2, y = 0; y < h; yn++, y++ )
    {
        for(xn = -w/2, x = 0; x < w; xn++, x++ )
        {
            r = sqrt((xn*xn)+(yn*yn)) * correctionRadius_;
            theta = qatanr_r(r) * zoom;
            x_ = wh + theta * xn;// * zoom;
            y_ = hh + theta * yn;// * zoom;
            O.at<Vec3b>(y,x) = I.at<Vec3b>(y_,x_);
        }
    }
#endif
#ifndef QUICK
#ifdef ALT_QUICK
    double correctionRadius_ = (1 / qdist(fabs(wh), fabs(hh))) * strength, thetaz;
    for(yn = -h/2, y = 0; y < h; yn++, y++ )
    {
        for(xn = -w/2, x = 0; x < h; xn++, x++ )
        {
            r = qdist(fabs(xn), fabs(yn)) * correctionRadius_;
            thetaz = qatanr_r(r) * zoom;
            x_ = wh + thetaz * xn;
            y_ = hh + thetaz * yn;
            O.at<Vec3b>(y,x) = I.at<Vec3b>(y_,x_);
        }
    }
#else
    double correctionRadius = sqrt((wh*wh)+(hh*hh)) / strength;
    for(yn = -h/2, y = 0; y < h; yn++, y++ )
    {
        for(xn = -w/2, x = 0; x < w; xn++, x++ )
        {
            r = sqrt((xn*xn)+(yn*yn)) / correctionRadius;
            if(!r) theta = 1;
            else theta = atan(r)/r;
            x_ = wh + theta * xn * zoom;
            y_ = hh + theta * yn * zoom;
            if(IN(x_, 0,w) && IN(y_, 0,h))
                O.at<Vec3b>(y,x) = I.at<Vec3b>(y_,x_);
        }
    }
#endif
#endif
    putText(O, "Strength: " + std::to_string(strength), Point(30,30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(200,200,250), 1);
    putText(O, "Zoom: " + std::to_string(zoom), Point(30,48), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(200,200,250), 1);
}

void invfisheye(Mat I, Mat O)
{
    createTrackbar( "Strength", "Original", &strength_slider, max_strength );
    createTrackbar( "Zoom", "Original", &zoom_slider, max_zoom );
    
    strength = (double) strength_slider / 10;
    zoom = (double) zoom_slider / 10;
    
    double w = I.cols, h = I.rows;
    double wh = w/2, hh = h/2;
    Vec3b c = {0,0,0};
    int x, y = 0, x_, y_, xn, yn;
    double theta, r;
    
    double correctionRadius = sqrt((wh*wh)+(hh*hh)) / strength;
    for(yn = -h/2, y = 0; y < h; yn++, y++ )
    {
        for(xn = -w/2, x = 0; x < w; xn++, x++ )
        {
            r = sqrt((xn*xn)+(yn*yn)) / correctionRadius;
            if(!r) theta = 1;
            else theta = r / ( atan(r) * zoom );
            x_ = wh + xn * theta;
            y_ = hh + yn * theta;
            if(IN(x_, 0,w) && IN(y_, 0,h))
                O.at<Vec3b>(y_,x_) = I.at<Vec3b>(y,x);
        }
    }
    putText(O, "Strength: " + std::to_string(strength), Point(30,30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(200,200,250), 1);
    putText(O, "Zoom: " + std::to_string(zoom), Point(30,48), FONT_HERSHEY_COMPLEX_SMALL, 0.8, Scalar(200,200,250), 1);
}

void invfisheye(Point2f * p, int w, int h, double s, double z)
{
    double wh = w/2, hh = h/2;
    double correctionRadius = sqrt((wh*wh)+(hh*hh)) / s;

    int xn = p->x - wh, yn = p->y - hh, x_, y_;
    double theta, r;

    r = sqrt((xn*xn)+(yn*yn)) / correctionRadius;
    if(!r) theta = 1;
    else theta = r / ( atan(r) * z );
    
    x_ = wh + xn * theta;
    y_ = hh + yn * theta;
    
    if(x_ > w) x_ = w-1;
    if(y_ > h) y_ = h-1;
    p->x = x_;
    p->y = y_;
}
