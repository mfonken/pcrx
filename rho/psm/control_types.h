//
//  control_type.h
//
//  Created by Matthew Fonken on 2/10/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef control_types_h
#define control_types_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "rho_kalman.h"
#include "rho_config.h"

#ifndef ZDIV
#define ZDIV_LNUM 1 << 10
#define ZDIV(X,Y) ((Y==0)?(X==0?0:ZDIV_LNUM):X/Y)
#endif

#ifndef MIN
#define MIN(A,B) (A<B?A:B)
#endif

#ifndef MAX
#define MAX(A,B) (A>B?A:B)
#endif

#define IN_RANGE(A,X,Y) ( ( A > X ) && ( A < Y ) )
#define SET_MAX(A,B)    ( A = MAX ( A, B ) )

#define MAX_THRESH 255
#define MIN_THRESH 0
#define THRESH_RANGE (MAX_THRESH - MIN_THRESH)

#define MAX_SINGLE_CONFIDENCE 0.9

#define TARGET_STATE TARGET_POPULATED

      typedef enum
    {
        UNKNOWN_STATE = -1,
        UNDER_POPULATED,
        TARGET_POPULATED,
        OVER_POPULATED,
        CHAOTIC,
        NUM_STATES
    } state_t;
#define NUM_STATE_GROUPS NUM_STATES

    /* Stability tracking for selec tions */
    typedef struct
    {
        rho_kalman_t
            state,
            system;
    } stability_t;

#ifdef __PSM__
    //#define NUM_STATES              10
#define NUM_OBSERVATION_SYMBOLS 5 // Should be max number of clusters in GMM
#ifndef MAX_OBSERVATIONS
#define MAX_OBSERVATIONS        (1 << 7) // Length of history
#endif
#define MAX_OBSERVATION_MASK    (MAX_OBSERVATIONS-1)

#define MAX_STD_DEVS_TO_BE_INCLUDED_IN_BAND_CALC 3
#define MIN_STD_DEV_SPAN_TO_REJECT_FOR_BAND_CALC 8
#define MIN_VARIANCE_SPAN_TO_REJECT_FOR_BAND_CALC ( MIN_STD_DEV_SPAN_TO_REJECT_FOR_BAND_CALC * MIN_STD_DEV_SPAN_TO_REJECT_FOR_BAND_CALC )

#define MAX_CLUSTER_LIFETIME 15 // sec
#define MAX_DISTANCE 1000.f
#define MIN_TOTAL_MIXTURE_PROBABILITY 1e-15f
#define MAX_CLUSTERS 25
#define MAX_ERROR 0.2
#define INITIAL_VARIANCE 50//150.
#define INV_INITIAL_VARIANCE (1./INITIAL_VARIANCE)
#define MAX_MAHALANOBIS_SQ 9
#define MAX_MAHALANOBIS_SQ_FOR_UPDATE MAX_MAHALANOBIS_SQ
#define SMALL_VALUE_ERROR_OFFSET 1e-4f
#define VALID_CLUSTER_STD_DEV 0.25
#define MIN_CLUSTER_SCORE 0.05
#define FALLBACK_MAX_ERROR 1e-2f
#define ALPHA 0.00001//0.0025
#define BETA 1

//#define MAX_DISTANCE 10000.f
//#define MIN_TOTAL_MIXTURE_PROBABILITY 1e-15f
//#define MAX_CLUSTERS 10
//#define MAX_ERROR 20
//#define INITIAL _VARIANCE 1
//#define INV_INITIAL_VARIANCE (1/INITIAL_VARIANCE)
//#define MIN_MAHALANOBIS_DISTANCE_SQ 1.386f
//#define MAX_MAHALANOBIS_SQ_FOR_UPDATE 1000.f
//#define SMALL_VALUE_ERROR_OFFSET 1e-4f
//#define VALID_CLUSTER_STD_DEV 2.
//#define MIN_CLUSTER_SCORE 1e-3f
//#define FALLBACK_MAX_ERROR 1e-2f

#define MAX_INPUT_COVARIANCE 200/// Change to dynamic



#define MAX_LABELS 10
#define LABEL_MOVING_AVERAGE_MAX_HISTORY 10
#define NULL_LABEL 0xff
#define MIN_LABEL_CONTRIBUTION 0.1

#define BOUNDARY_START(X)   !!(X<0)
#define BOUNDARY_END(X)     !!(X>0)
#endif
    
    typedef struct
    {
        index_t density;
        uint8_t thresh,
        label;
    } observation_t;
    
    typedef struct
    {
        observation_t observations[MAX_OBSERVATIONS];
        uint8_t length;
    } observation_list_t;
    
    typedef struct
    {
        double a,b,c,d;
    } mat2x2;

    typedef struct
    {
        double a,b;
    } vec2;

#ifdef __PSM__
    static void vec2SubVec2(vec2 * A, vec2 * B, vec2 * C)
    {
        C->a = A->a - B->a;
        C->b = A->b - B->b;
    }
    static void vec2AddVec2(vec2 * A, vec2 * B, vec2 * C)
    {
        C->a = A->a + B->a;
        C->b = A->b + B->b;
    }
    static void scalarMulVec2(double A, vec2 * B, vec2 * C)
    {
        C->a = A * B->a;
        C->b = A * B->b;
    }
    static double getMat2x2Determinant( mat2x2 * mat )
    {
        return ( ( mat->a * mat->d ) - ( mat->c * mat->b ) );
    }
    static void getMat2x2Inverse( mat2x2 * mat, mat2x2 * res )
    {
        double det = getMat2x2Determinant( mat );
        if( det )
        {
            res->d = mat->a / det;
            res->c = -mat->b / det;
            res->b = -mat->c / det;
            res->a = mat->d / det;
        }
        else
            memset( res, ZDIV_LNUM, sizeof(mat2x2) );
    }
    static double * GetMat2x2ValuePointerByIndex(mat2x2 * mat, uint8_t a, uint8_t b)
    {
        uint8_t offset = a + ( b << 1 );
        return (double *)(mat+offset);
    }

    static void mat2x2DotVec2(mat2x2 * mat, vec2 * vec, vec2 * res)
    {
        res->a = mat->a * vec->a + mat->b * vec->b;
        res->b = mat->c * vec->a + mat->d * vec->b;
    }
    static void mat2x2AddMat2x2(mat2x2 * A, mat2x2 * B, mat2x2 * C)
    {
        C->a = A->a + B->a;
        C->b = A->b + B->b;
        C->c = A->c + B->c;
        C->d = A->d + B->d;
    }
    static void mat2x2SubMat2x2(mat2x2 * A, mat2x2 * B, mat2x2 * C)
    {
        C->a = A->a - B->a;
        C->b = A->b - B->b;
        C->c = A->c - B->c;
        C->d = A->d - B->d;
    }
    static void scalarMulMat2x2( double A, mat2x2 * B, mat2x2 * C )
    {
        C->a = A * B->a;
        C->b = A * B->b;
        C->c = A * B->c;
        C->d = A * B->d;
    }
    static void mat2x2MulMat2x2(mat2x2 * A, mat2x2 * B, mat2x2 * res)
    {
        res->a = A->a * B->a + A->b * B->c;
        res->b = A->a * B->b + A->b * B->d;
        res->c = A->c * B->a + A->d * B->c;
        res->d = A->c * B->b + A->d * B->d;
    }

    static void vec2DotVec2(vec2 * A, vec2 * B, double * res)
    {
        *res = A->a * B->a + A->b * B->b;
    }

    static void GetMat2x2LLT( mat2x2 * mat, mat2x2 * llt )
    {
        mat2x2 L = { 0., 0., 0., 0. };
        L.a = sqrt( mat->a );
        L.b = mat->b;
        L.c = ZDIV( mat->c, L.a );
        L.d = sqrt( mat->d - L.c * L.c );
        if( L.d == 0) L.d = 1;
        *llt = L;
    }

    static double CalculateMahalanobisDistanceSquared(mat2x2 * inv_covariance, vec2 * delta)
    {
        vec2 inv_covariance_delta;
        mat2x2DotVec2(inv_covariance, delta, &inv_covariance_delta);
        double mahalanobis_distance_squared;
        vec2DotVec2(delta, &inv_covariance_delta, &mahalanobis_distance_squared);
        mahalanobis_distance_squared = BOUNDU(mahalanobis_distance_squared, MAX_DISTANCE);
        return mahalanobis_distance_squared;
    }
#endif
    
    typedef enum
    {
        UNKNOWN_SYMBOL = -1,
        ZERO_SYMBOL,
        ONE_SYMBOL,
        TWO_SYMBOL,
        THREE_SYMBOL,
        MANY_SYMBOL
    } observation_symbol_t;
    
    /* FSM state tree with fsm base */
    typedef struct
    {
        double map[NUM_STATES][NUM_STATES];
        uint8_t length;
    } fsm_map_t;

#define UNCALCULABILTY_BOUND_FOR_SAFE_EXPONENT 30.
#define SAFE_EXPONENT_MAX_VALUE 2.35385266837019985408e17f

    static float safe_exp(double x)
    {
        if (x < -UNCALCULABILTY_BOUND_FOR_SAFE_EXPONENT)
            return 0.0f;
        else if (x > UNCALCULABILTY_BOUND_FOR_SAFE_EXPONENT)
            return SAFE_EXPONENT_MAX_VALUE;
        else
            return exp(x);
    }

    typedef struct
    {
        double
        mean,
        std_dev;
    } gaussian1d_t;

    typedef struct
    {
        vec2 mean;
        mat2x2 covariance;
        uint8_t combinations;
    } gaussian2d_t;

    typedef struct
    {
        double
        lower_boundary,
        upper_boundary,
        variance;
        vec2
        true_center;
    } band_t;
    
    typedef struct
    {
        uint8_t length;
        band_t band[NUM_STATES];
    } band_list_t;
    
#ifdef __PSM__
    static gaussian1d_t getGaussian1dFrom2dY( gaussian2d_t * a )
    {
        return (gaussian1d_t){ a->mean.b, a->covariance.d };
    }

    static void mulGaussian1d( gaussian1d_t * a, gaussian1d_t * b, gaussian1d_t * c )
    {
        double cov_y_a_var = a->std_dev * a->std_dev, cov_y_b_var = b->std_dev * b->std_dev;
        double k = cov_y_a_var / ( cov_y_a_var + cov_y_b_var), mean_diff = b->mean - a->mean;
        c->mean = a->mean + k * mean_diff;
        c->std_dev = a->std_dev * ( 1 - k );
    }
    static void mulGaussian2d( gaussian2d_t * a, gaussian2d_t * b, gaussian2d_t * c )
    { /* See https://math.stackexchange.com/questions/157172/product-of-two-multivariate-gaussians-distributions */
        mat2x2 a_computation, b_computation, c_computation;

        getMat2x2Inverse( &a->covariance, &a_computation );
        getMat2x2Inverse( &b->covariance, &b_computation );
        mat2x2AddMat2x2( &a_computation, &b_computation, &c_computation );
        getMat2x2Inverse( &c_computation, &c->covariance );

        vec2 a_computation_vec, b_computation_vec, c_computation_vec;
        mat2x2DotVec2( &a_computation, &a->mean, &a_computation_vec );
        mat2x2DotVec2( &b_computation, &b->mean, &b_computation_vec );
        vec2AddVec2( &a_computation_vec, &b_computation_vec, &c_computation_vec );
        mat2x2DotVec2( &c->covariance, &c_computation_vec, &c->mean );

        c->combinations = a->combinations + b->combinations + 1;
    }
    static void divGaussian1d( gaussian1d_t * a, gaussian1d_t * b, gaussian1d_t * c )
    {
        double cov_y_a_var = a->std_dev * a->std_dev, cov_y_b_var = b->std_dev * b->std_dev;
        double k = cov_y_a_var / ( cov_y_a_var - cov_y_b_var), mean_diff = a->mean - b->mean;
        c->mean = -a->mean + k * mean_diff;
        c->std_dev = a->std_dev * ( 1 + k );
    }

    static void copyGaussian1d( gaussian1d_t * a, gaussian1d_t * b )
    {
        b->mean = a->mean;
        b->std_dev = a->std_dev;
    }

    typedef struct
    {
        double value;
        int8_t label;
    } cluster_boundary_t;

    typedef struct
    {
        uint8_t length;
        cluster_boundary_t list[MAX_CLUSTERS*2];
    } cluster_boundary_list_t;

    typedef struct
    {
        double average[MAX_LABELS];
        uint8_t count[MAX_LABELS];
        uint8_t num_valid;
    } label_manager_t;

    static double GetMeanXAtGaussianYOffset( gaussian2d_t * gaussian, double y_offset )
    { /* See - https://courses.cs.washington.edu/courses/cse590b/02wi/eig2x2.cpp
       * and http://www.math.harvard.edu/archive/21b_fall_04/exhibits/2dmatrices/index.html
       */
        /* Find maximum eigen value */
        double a = gaussian->covariance.a, b = gaussian->covariance.b, d = gaussian->covariance.d,
        a_minus_d = a - d,
        radius = sqrt( a_minus_d * a_minus_d + 4. * b * b ),
        lambda = a_minus_d + radius;
        return y_offset * ZDIV( -2 * b, a - lambda );
    }

    static vec2 WeightedIncreaseMean( vec2 * new_val, gaussian2d_t * gaussian, double weight )
    {
        vec2 delta_mean, weighted_mean;
        vec2SubVec2( new_val, &gaussian->mean, &delta_mean );
        scalarMulVec2( weight, &delta_mean, &weighted_mean );
        vec2AddVec2( &weighted_mean, &gaussian->mean, &gaussian->mean );
        return delta_mean;
    }

    static bool LimitCovariance( mat2x2 * covariance )
    {
        bool limited = false;
        double max = covariance->a, check;
        for(uint8_t i = 1; i < 4; i++ )
        {
            check = ((double *)covariance)[i];
            if( check > max )
                max = check;
        }
        if( max > MAX_INPUT_COVARIANCE )
        {
            double correction_factor = MAX_INPUT_COVARIANCE / max;
            for(uint8_t i = 0; i < 4; i++ )
                ((double *)covariance)[i] *= correction_factor;
            limited = true;
        }
        return limited;
    }

    static void UpdateGaussianWithWeight( vec2 * A, vec2 * B, gaussian2d_t * gaussian, double weight )
    {
        double delta_mean_a_a = A->a * B->a,
        delta_mean_a_b = A->a * B->b,
        delta_mean_b_b = A->b * B->b;
        if( delta_mean_a_a == 0. ) delta_mean_a_a += SMALL_VALUE_ERROR_OFFSET;
        if( delta_mean_a_b == 0. ) delta_mean_a_b += SMALL_VALUE_ERROR_OFFSET;
        if( delta_mean_b_b == 0. ) delta_mean_b_b += SMALL_VALUE_ERROR_OFFSET;
        mat2x2 covariance_delta_factor, unweighted_covariance_factor =
        { delta_mean_a_a, delta_mean_a_b, delta_mean_a_b, delta_mean_b_b };

//        printf("m_covf: <%.2f %.2f> [%.2f %.2f | %.2f %.2f]\n", A->a, A->b, unweighted_covariance_factor.a, unweighted_covariance_factor.b, unweighted_covariance_factor.c, unweighted_covariance_factor.d);

        mat2x2SubMat2x2( &unweighted_covariance_factor, &gaussian->covariance, &covariance_delta_factor );
        scalarMulMat2x2( weight, &covariance_delta_factor, &covariance_delta_factor );
        mat2x2AddMat2x2( &gaussian->covariance, &covariance_delta_factor, &gaussian->covariance );

        LimitCovariance( &gaussian->covariance );
    }

    static bool ReportLabel( label_manager_t * labels, uint8_t new_label )
    {
        if( new_label > MAX_LABELS ) return false;
        labels->count[new_label]++;
        double sum = 0;
        for( uint8_t i = 0; i < MAX_LABELS; i++ )
            sum += (double)labels->count[i];
        for( uint8_t i = 0; i < MAX_LABELS; i++ )
            labels->average[i] = (double)labels->count[i] / sum;
        return true;
    }

    static void GetTopTwoLabels( label_manager_t * labels, uint8_t * first, uint8_t * second )
    {
        *first = 0; *second = 1;
        double first_value = labels->average[0], second_value = labels->average[1], current = 0.;
        for( uint8_t i = 2; i < MAX_LABELS; i++ )
        {
            current = labels->average[i];
            if( current > first_value )
            {
                second_value = first_value;
                *second = *first;
                first_value = current;
                *first = i;
            }
            else if ( current > second_value )
            {
                second_value = current;
                *second = i;
            }
        }
    }

    static uint32_t GetValidLabels( label_manager_t * labels )
    {
#if MAX_LABELS > 32
#warning "Valid label mask is max 32 bit wide."
#endif
        uint32_t valid_vector = 0;
        for( uint8_t i = 0; i < MAX_LABELS; i++ )
        {
            valid_vector |= ( labels->average[i] > MIN_LABEL_CONTRIBUTION ) << i;
        }
        return valid_vector;
    }
    static uint8_t CountSet( uint32_t v )
    {
        uint8_t c = 0;
        for( uint8_t i = 0, m = 1; i < 32; i++, c += !!(v & m), m <<= 1 );
        return c;
    }
    static uint32_t GetNumberOfValidLabels( label_manager_t * labels )
    {
        return CountSet( GetValidLabels( labels ) );
    }

    typedef struct
    {
        observation_symbol_t
        data[MAX_OBSERVATIONS],
        next, first, last, curr, prev;
    } observation_buffer_t;

    static uint8_t AddToObservationBuffer( observation_buffer_t * buffer, observation_symbol_t v )
    {
        buffer->prev = buffer->curr;
        buffer->curr = v;
        buffer->last = buffer->next;
        uint8_t next_index = (uint8_t)buffer->next + 1;
        buffer->data[next_index] = v;
        buffer->next = (observation_symbol_t)( next_index & MAX_OBSERVATION_MASK );
        if(buffer->next == buffer->first)
            buffer->first = (observation_symbol_t)( ( (uint8_t)buffer->next + 1 ) & MAX_OBSERVATION_MASK );
        return buffer->last;
    }

    static floating_t NumStdDevsFromYMean( gaussian2d_t * gaussian, floating_t check_value )
    {
        floating_t diff = fabs( gaussian->mean.b - check_value );
        floating_t num_std_devs = ZDIV( diff, sqrt( gaussian->covariance.d ) );
        return num_std_devs;
    }
#endif
    
    static floating_t WeightedAverage( floating_t a, floating_t b, floating_t w )
    {
        return ( ( a * w ) + ( b * ( 1 - w ) ) );
    }

#ifdef __cplusplus
}
#endif

#endif /* control_types_h */
