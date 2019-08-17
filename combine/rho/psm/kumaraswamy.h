//
//  kumaraswamy.h
//  hmmtest
//
//  Created by Matthew Fonken on 2/11/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef kumaraswamy_h
#define kumaraswamy_h

#ifdef __cplusplus
extern "C" {
#endif

#include "control_types.h"

#define STATE_KUMARASWAMY_INTERVALS { 0.2, 0.4, 0.65, 1.0 }
    
#define KUMARASWAMY_PDF(X,A,B) ( A * B * pow( x, ( A - 1 ) ) * pow( 1 - pow( x, A ), B - 1 ) )
#define KUMARASWAMY_CDF(X,A,B) ( 1 - pow( 1 - pow( X, A ), B) )

typedef struct
{
    double alpha, beta;
} kumaraswamy_t;

static void InitializeKumaraswamy( kumaraswamy_t * k, double beta )
{
    k->beta = beta;
}

static inline double PerformKumaraswamyCDF( kumaraswamy_t * k, double x )
{
    return KUMARASWAMY_CDF( x, k->alpha, k->beta );
}

static void GetKumaraswamyVector( kumaraswamy_t * k, double alpha, double * interval, double * band_list, uint8_t num_bands ) // boundaries are between 0 to 1
{
    k->alpha = alpha;
    double curr_CDF, prev_CDF = 0.;
    for( uint8_t i = 0; i < num_bands; i++ )
    {
        curr_CDF = PerformKumaraswamyCDF( k, band_list[i] );
        if( curr_CDF < prev_CDF )
        {
            curr_CDF = prev_CDF;
            interval[i] = 0.;
        }
        else
        {
            interval[i] = curr_CDF - prev_CDF;
            prev_CDF = curr_CDF;
        }
    }
}

typedef struct
{
    void (*Initialize)( kumaraswamy_t *, double );
    double(*PerformCDF)( kumaraswamy_t *, double );
    void (*GetVector)( kumaraswamy_t *, double, double *, double *, uint8_t );
} kumaraswamy_functions_t;

static const kumaraswamy_functions_t KumaraswamyFunctions =
{
    .Initialize = InitializeKumaraswamy,
    .PerformCDF = PerformKumaraswamyCDF,
    .GetVector = GetKumaraswamyVector
};
    
#ifdef __cplusplus
}
#endif

#endif /* kumaraswamy_h */
