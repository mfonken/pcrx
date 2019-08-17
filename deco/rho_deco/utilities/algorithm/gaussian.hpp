//
//  gaussian.hpp
//  tau+
//
//  Created by Matthew Fonken on 2/7/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#ifndef gaussian_hpp
#define gaussian_hpp

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

typedef double GAUSS_FLOAT;

class Gaussian
{
    GAUSS_FLOAT   *array;
    uint16_t       length;
    GAUSS_FLOAT    sigma;
    
    static GAUSS_FLOAT gaussianDistribution( int x, int mu, GAUSS_FLOAT sigma)
    {
        int d = x - mu;
        GAUSS_FLOAT n = 1.0 / (sqrt(2 * M_PI) * sigma);
        return exp(-d*d/(2 * sigma * sigma)) * n;
    };
    
public:
    Gaussian(int);
    void blurInt( uint16_t * arr, size_t len, int scale );
    void blur( GAUSS_FLOAT * arr, size_t len );
    void generate( int l, GAUSS_FLOAT s );
};




#endif /* gaussian_hpp */
