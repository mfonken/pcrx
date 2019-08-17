#ifndef kalman_hpp
#define kalman_hpp

//#define TIMELESS

/* Standard headers */
#include <stdio.h>
#include <string>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>
#include <math.h>

#define DEFAULT_LS      5
#define DEFAULT_VU      0.001
#define DEFAULT_BU      0.5
#define DEFAULT_SU      0.7

/** Kalman Uncertainties */
typedef struct
{
    double value;
    double bias;
    double sensor;
} kalman_uncertainty_t;

double now(void);

/** Kalman class */
class KalmanFilter
{
public:
    double lifespan;
    
    double      K[2],
                P[2][2],
                rate,
                bias,
                value,
                last_update,
                prev,
                velocity,
                timestamp;
    kalman_uncertainty_t uncertainty;
    
    KalmanFilter();
    KalmanFilter( double );
    KalmanFilter( double, double );
    KalmanFilter( double, double, double, double, double );
    void update( double, double );
    void copyTo(KalmanFilter *);
    std::string toString();
    int  isExpired();
};




#endif /* kalman_h */

