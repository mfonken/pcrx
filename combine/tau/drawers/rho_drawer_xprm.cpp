//
//  rho_drawer_xprm.cpp
//  pcrxprm
//
//  Created by Matthew Fonken on 2/5/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//
//#ifdef RHO_DRAWER
#ifdef __PSM__

#include "rho_drawer_xprm.hpp"
#include "timestamp.h"

//#define PRINT_DETECTION_MAP

Vec3b rcolors[] =
{
    {255, 0, 0},
    {100, 150, 0},
    {0, 15, 255},
    {0, 150, 100},
    {0, 150, 100},
    {0, 150, 100},
};

uint8_t NUM_COLORS = sizeof(rcolors)/sizeof(rcolors[0]);
Vec3b rwhite = {255,255,255};
Vec3b rblack = {0,0,0};

RhoDrawer::RhoDrawer(psm_t * model)
: detection_map_frame(Size(DETECTION_MAP_FRAME_OWIDTH, DETECTION_MAP_FRAME_OHEIGHT), CV_8UC3, Scalar(0,0,0)),
psm(model) {}

void RhoDrawer::DrawDetectionMap( detection_map_t * map )
{
    if(++counter == DETECTION_MAP_INTERVAL)
    {
        detection_map_frame = {0};
        rectangle(detection_map_frame, Point(DETECTION_MAP_INSET,DETECTION_MAP_INSET), Point(DETECTION_MAP_FRAME_OWIDTH-DETECTION_MAP_INSET, DETECTION_MAP_FRAME_OHEIGHT-DETECTION_MAP_INSET), rwhite);
        counter = 0;
        if( !map->length ) return;
#ifdef PRINT_DETECTION_MAP
        printf("Detection Map:");
        if( !map->fill ) printf(" Empty\n");
        else printf("\n");
#endif
        int16_t i = map->first, c = 0;
        for( ; c < map->fill; c++, i = c % map->length)
        {
            detection_element_t * e = &map->buffer[i];
#ifdef PRINT_DETECTION_MAP
            printf(" - [%2d](%3d, %3d, %d)\n", c, e->thresh, e->density, e->tracking_id);
#endif
            int x = ((double)e->density / MAX_DETECTION_MAP_DENSITTY_VALUE * DETECTION_MAP_FRAME_IWIDTH) + DETECTION_MAP_INSET*2;
            int y = DETECTION_MAP_FRAME_OHEIGHT - (( 1 - (double)e->thresh / MAX_DETECTION_MAP_THRESH_VALUE) * DETECTION_MAP_FRAME_IHEIGHT) + DETECTION_MAP_INSET;
            
            Vec3b color = rwhite;
            if( e->tracking_id < NUM_COLORS) color = rcolors[e->tracking_id];
            circle(detection_map_frame, Point(x,y), DETECTION_ELEMENT_RADIUS, color, FILLED);
        }
        PostProcess(psm);
    }
}
void RhoDrawer::PostProcess(psm_t * psm)
{
#ifdef __PSM__
    gaussian2d_t gaus;
    double angle;
//    printf("b:%p\n", &psm->gmm);
    for( int j = 0; j < psm->gmm.num_clusters; j++)
    {
        gaus = (*psm->gmm.cluster[j]).gaussian_in;
        angle = GetCovarianceAngle( &gaus.covariance );
        int b = 255-j*5, r =j*5;
        if( b<0) b=0; if( r>255) r=255;
        Point center = Point(gaus.mean.a+DETECTION_MAP_INSET, gaus.mean.b+DETECTION_MAP_INSET);
        center.x = ((double)center.x / MAX_DETECTION_MAP_DENSITTY_VALUE * DETECTION_MAP_FRAME_IWIDTH) + DETECTION_MAP_INSET*2;
        center.y = ((double)center.y / MAX_DETECTION_MAP_THRESH_VALUE * DETECTION_MAP_FRAME_IHEIGHT) + DETECTION_MAP_INSET*2;
        double cs = pow(gaus.combinations+1,sqrt(M_PI)); // Combination scale
        Size size = Size(fabs(gaus.covariance.a*COV_SCALE*cs), fabs(gaus.covariance.d*COV_SCALE*cs));
        if(size.width <= 0 || size.height <= 0 ) continue;
         ellipse(detection_map_frame, center, size, angle, 0, 360, Vec3b{(uint8_t)b,0,(uint8_t)r}, 1, 5, 0);
        label_manager_t &labels = (*psm->gmm.cluster[j]).labels;
        for(int i = 0, offset = -12; i < NUM_LABELS_TO_SHOW; i++, offset+=12)
        {
            Scalar c(rcolors[i][0]*0.7,rcolors[i][1]*0.7,rcolors[i][2]*0.7);
            putText(detection_map_frame, pto_string(labels.average[i],1), Point(center.x, center.y+offset), FONT_HERSHEY_PLAIN, 1, c, 2);
        }
        
        putText(detection_map_frame, pto_string(j,0), Point(center.x-size.width/2, center.y-size.height/2), FONT_HERSHEY_PLAIN, 1, rwhite, 2);
    }
    
    for( uint8_t i = 0; i < psm->state_bands.length; i++ )
    {
        band_t * band = &psm->state_bands.band[i];
        double l = band->lower_boundary, u = band->upper_boundary;
        l = (l / MAX_DETECTION_MAP_THRESH_VALUE * DETECTION_MAP_FRAME_IHEIGHT) + DETECTION_MAP_INSET;
        u = (u / MAX_DETECTION_MAP_THRESH_VALUE * DETECTION_MAP_FRAME_IHEIGHT) + DETECTION_MAP_INSET;
        vec2 tc = band->true_center;
        tc.a = ((double)tc.a / MAX_DETECTION_MAP_DENSITTY_VALUE * DETECTION_MAP_FRAME_IWIDTH) + DETECTION_MAP_INSET*2;
        tc.b = ((double)tc.b / MAX_DETECTION_MAP_THRESH_VALUE * DETECTION_MAP_FRAME_IHEIGHT) + DETECTION_MAP_INSET;
        if(tc.a == 0 && tc.b == 0) tc = (vec2){0, (double)DETECTION_MAP_FRAME_IHEIGHT};
        int tc_line_len = 20;
        
        double f = ((double)i)/5.;
        Vec3b c(255*(1-f),0,255*f);
        line(detection_map_frame, Point(DETECTION_MAP_INSET, l+DETECTION_MAP_INSET), Point(DETECTION_MAP_FRAME_IWIDTH+DETECTION_MAP_INSET, l+DETECTION_MAP_INSET), c);
        
        line(detection_map_frame, Point(tc.a+DETECTION_MAP_INSET-tc_line_len, tc.b+DETECTION_MAP_INSET), Point(tc.a+DETECTION_MAP_INSET+tc_line_len, tc.b+DETECTION_MAP_INSET), c, 3);
        line(detection_map_frame, Point(tc.a+DETECTION_MAP_INSET, tc.b+DETECTION_MAP_INSET-tc_line_len), Point(tc.a+DETECTION_MAP_INSET, tc.b+DETECTION_MAP_INSET+tc_line_len), c, 3);
        
        rectangle(detection_map_frame, Point(0, l+DETECTION_MAP_INSET), Point(DETECTION_MAP_INSET, u+DETECTION_MAP_INSET), c, FILLED);
        rectangle(detection_map_frame, Point(DETECTION_MAP_FRAME_IWIDTH+DETECTION_MAP_INSET, l+DETECTION_MAP_INSET), Point(DETECTION_MAP_FRAME_IWIDTH+DETECTION_MAP_INSET*2, u+DETECTION_MAP_INSET), c, FILLED);
        putText(detection_map_frame, std::to_string(i), Point(DETECTION_MAP_INSET+6, l+DETECTION_MAP_INSET-4), FONT_HERSHEY_PLAIN, 2, c, 2);
        
        /*****************/
        if(band->variance > 0)
        {
            double prev_x = 0, variance_scale = 100;//ZDIV( 1, band->variance * M_PI * 2 );
            for(uint16_t y = 1; y < DETECTION_MAP_FRAME_IHEIGHT && i < NUM_COLORS; y++)
            {
                double tcb = ( band->true_center.b / MAX_DETECTION_MAP_THRESH_VALUE * DETECTION_MAP_FRAME_IHEIGHT) + DETECTION_MAP_INSET;
                double upper = y - tcb;
                double variance = band->variance;
                variance = ( variance / MAX_DETECTION_MAP_THRESH_VALUE * DETECTION_MAP_FRAME_IHEIGHT) + DETECTION_MAP_INSET;
                double inner = ZDIV( upper, variance );
                double exponent = -0.5 * inner * inner;
                double x = variance_scale * safe_exp(exponent);
                x = MAX(0, x);
                x = MIN(DETECTION_MAP_FRAME_IWIDTH, x);
                Point p(DETECTION_MAP_FRAME_IWIDTH + DETECTION_MAP_INSET - x, DETECTION_MAP_INSET+y);
                //                detection_map_frame.at<Vec3b>(DETECTION_MAP_INSET+y,DETECTION_MAP_FRAME_IWIDTH + DETECTION_MAP_INSET - x) = colors[i];
                line( detection_map_frame, Point(prev_x, p.y-1), p, c );
                prev_x = p.x;
            }
        }
        /*****************/
        //        psm->state_bands.band[0].
    }
#endif
}

#endif
//#endif
