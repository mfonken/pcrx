//
//  gaussian.cpp
//  tau+
//
//  Created by Matthew Fonken on 2/7/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#include "gaussian.hpp"

Gaussian::Gaussian( int l )
{
    array = (GAUSS_FLOAT*)malloc(sizeof(GAUSS_FLOAT)*l);
    length = l;
}

void Gaussian::blurInt( uint16_t * arr, size_t len, int scale )
{
    size_t n, k_len = length;
    
    double res[len + k_len - 1];
    
    for (n = 0; n < len + k_len - 1; n++)
    {
        size_t kmin, kmax, k;
        
        res[n] = 0;
        
        kmin = (n >= k_len - 1) ? n - (k_len - 1) : 0;
        kmax = (n < len - 1) ? n : len - 1;
        
        for (k = kmin; k <= kmax; k++)
        {
            res[n] += (GAUSS_FLOAT)arr[k] * array[n - k];
        }
    }
    int offset = (int)(( k_len - 1 ) / 2);
    for (n = 0; n < len; n++)
    {
        arr[n] = (int)(res[n + offset]*scale);
    }
}


void Gaussian::blur( GAUSS_FLOAT * arr, size_t len )
{
    size_t n, k_len = length;
    
    double res[len + k_len - 1];
    
    for (n = 0; n < len + k_len - 1; n++)
    {
        size_t kmin, kmax, k;
        
        res[n] = 0;
        
        kmin = (n >= k_len - 1) ? n - (k_len - 1) : 0;
        kmax = (n < len - 1) ? n : len - 1;
        
        for (k = kmin; k <= kmax; k++)
        {
            res[n] += (GAUSS_FLOAT)arr[k] * array[n - k];
        }
    }
    int offset = (int)(( k_len - 1 ) / 2);
    for (n = 0; n < len; n++)
    {
        arr[n] = res[n + offset];
    }
}

void Gaussian::generate( int l, GAUSS_FLOAT s )
{
    if( l == length && s == sigma) return;
    int m = floor(l/2);
    for(int i = 0, d = l; i <= m; i++, d--)
    {
        array[i] = gaussianDistribution(i,m,s);
        if(i != d) array[d] = array[i];
    }
    length = l;
    sigma = s;
}


