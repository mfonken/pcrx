//
//  timestamp.h
//  tau+
//
//  Created by Matthew Fonken on 3/28/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#ifndef timestamp_h
#define timestamp_h

#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>

double TIMESTAMP( void );
bool ISTIMEDOUT( double, double );

#endif /* timestamp_h */
