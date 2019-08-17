//
//  timestamp.c
//  tau+
//
//  Created by Matthew Fonken on 3/28/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#include "timestamp.h"

double TIMESTAMP(void)
{
    struct timeval stamp;
    gettimeofday(&stamp, NULL);
    return stamp.tv_sec + stamp.tv_usec/1000000.0;
}

bool ISTIMEDOUT( double check, double time_out )
{
    double diff = TIMESTAMP() - check;
    return (diff > time_out);
}
