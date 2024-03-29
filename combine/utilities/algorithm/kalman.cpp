#include "kalman.hpp"

/** SOURCE: http://preview.tinyurl.com/9djhrem */

//#define TIMELESS

floating_t TIMESTAMP()
{
    struct timeval stamp;
    gettimeofday(&stamp, NULL);
    return stamp.tv_sec + stamp.tv_usec/1000000.0;
}

KalmanFilter::KalmanFilter() : KalmanFilter(0.)
{
}

KalmanFilter::KalmanFilter( floating_t v ) : KalmanFilter(v, DEFAULT_LS, DEFAULT_VU, DEFAULT_BU, DEFAULT_SU)
{
}

KalmanFilter::KalmanFilter( floating_t v, floating_t ls ) : KalmanFilter(v, ls, DEFAULT_VU, DEFAULT_BU, DEFAULT_SU)
{
}

KalmanFilter::KalmanFilter( floating_t v, floating_t ls, floating_t v_u, floating_t b_u, floating_t s_u )
{
    K[0]        = 0;
    K[1]        = 0;
    P[0][0]     = 0;
    P[0][1]     = 0;
    P[1][0]     = 0;
    P[1][1]     = 0;
    rate        = 0;
    bias        = 0;
    value       = v;
    prev        = 0;
    velocity    = 0;
    
    last_update = 0;
    
    lifespan    = ls;
    uncertainty.value   = v_u;
    uncertainty.bias    = b_u;
    uncertainty.sensor  = s_u;
}

void KalmanFilter::update( floating_t value_new, floating_t rate_new )
{
#ifdef TIMELESS
    //    floating_t delta_time = 0.25;
#else
    floating_t delta_time = TIMESTAMP() - timestamp;
    
    /* Quick expiration check */
    if(delta_time > lifespan)
    {
        timestamp = TIMESTAMP();
        return;
    }
#endif
    last_update = value_new;
    velocity   = value - prev;
    prev       = value;
    rate       = rate_new - bias;
    value     += delta_time * rate;
    
    floating_t dt_P_1_1 = delta_time * P[1][1];
    P[0][0] +=   delta_time * ( dt_P_1_1 - P[0][1] - P[1][0] + uncertainty.value );
    P[0][1] -=   dt_P_1_1;
    P[1][0] -=   dt_P_1_1;
    P[1][1] +=   uncertainty.bias * delta_time;
    
    floating_t S_   = 1 / ( P[0][0] + uncertainty.sensor );
    K[0]       = P[0][0] * S_;
    K[1]       = P[1][0] * S_;
    floating_t delta_value = value_new - value;
    value     += K[0] * delta_value;
    bias      += K[1] * delta_value;
    
    P[0][0] -= K[0] * P[0][0];
    P[0][1] -= K[0] * P[0][1];
    P[1][0] -= K[1] * P[0][0];
    P[1][1] -= K[1] * P[0][1];
    
    timestamp = TIMESTAMP();
};

void KalmanFilter::copyTo(KalmanFilter * k)
{
    if( isnan(this->value) ) return;
    k->K[0]        = this->K[0];
    k->K[1]        = this->K[1];
    k->P[0][0]     = this->P[0][0];
    k->P[0][1]     = this->P[0][1];
    k->P[1][0]     = this->P[1][0];
    k->P[1][1]     = this->P[1][1];
    k->rate        = this->rate;
    k->bias        = this->bias;
    k->value       = this->value;
    k->prev        = this->prev;
    k->velocity    = this->velocity;
    k->last_update = this->last_update;
    k->lifespan    = this->lifespan;
    k->uncertainty = this->uncertainty;
}

std::string KalmanFilter::toString()
{
    char buff[120];
    snprintf(buff, sizeof(buff), "P[%.3f][%.3f][%.3f][%.3f] K[%.3f][%.3f] R[%.3f] B[%.3f] N[%.3f] V[%.3f] E[%.3f]", P[0][0], P[0][1], P[1][0], P[1][1], K[0], K[1], rate, bias, last_update, value, velocity);
    
    std::string ret = buff;
    return ret;
}

int KalmanFilter::isExpired()
{
    return ((TIMESTAMP() - timestamp) > lifespan);
}

