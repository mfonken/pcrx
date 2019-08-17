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
#include "rho_global.h"

#define DEFAULT_LS      5
#define DEFAULT_VU      0.001
#define DEFAULT_BU      0.5
#define DEFAULT_SU      0.7

/** Kalman Uncertainties */
typedef struct
{
    floating_t value;
    floating_t bias;
    floating_t sensor;
} kalman_uncertainty_t;

floating_t now(void);

/** Kalman class */
class KalmanFilter
{
public:
    floating_t lifespan;
    
    floating_t      K[2],
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
    KalmanFilter( floating_t );
    KalmanFilter( floating_t, floating_t );
    KalmanFilter( floating_t, floating_t, floating_t, floating_t, floating_t );
    void update( floating_t, floating_t );
    void copyTo(KalmanFilter *);
    std::string toString();
    int  isExpired();
};




#endif /* kalman_h */

