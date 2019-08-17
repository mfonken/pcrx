#include "kalman.h"

/** SOURCE: http://preview.tinyurl.com/9djhrem */

//#include "test_setup.h"
#ifdef KALMAN_IS_TIMELESS
#define TIMELESS
#endif

static double timestamp(void)
{
    struct timeval stamp;
    gettimeofday(&stamp, NULL);
    return stamp.tv_sec + stamp.tv_usec/1000000.0;
}

void init( kalman_t * k, double v, double ls, double v_u, double b_u, double s_u )
{
    k->K[0]        = 0;
    k->K[1]        = 0;
    k->P[0][0]     = 0;
    k->P[0][1]     = 0;
    k->P[1][0]     = 0;
    k->P[1][1]     = 0;
    k->rate        = 0;
    k->bias        = 0;
    k->value       = v;
    k->prev        = 0;
    k->velocity    = 0;
    k->density     = 0;

    k->lifespan    = ls;
    k->uncertainty.value   = v_u;
    k->uncertainty.bias    = b_u;
    k->uncertainty.sensor  = s_u;
}

void update( kalman_t * k, double value_new, double rate_new, update_type_c type )
{
    double delta_time = timestamp() - k->timestamp;

    /* Quick expiration check */
    if(delta_time > k->lifespan)
    {
        k->timestamp = timestamp();
        return;
    }
    k->prev       = k->value;
    switch(type)
    {
        default:
        case VELOCITY:
            k->rate   = rate_new - k->bias;
            break;
        case ACCELERATION:
            k->rate   = delta_time * rate_new - k->bias + k->velocity;
            break;
    }
        
    k->value     += delta_time * k->rate;

    double dt_P_1_1 = delta_time * k->P[1][1];
    k->P[0][0] +=   delta_time * ( dt_P_1_1 -
                                   k->P[0][1] -
                                   k->P[1][0] +
                                   k->uncertainty.value );
    k->P[0][1] -=   dt_P_1_1;
    k->P[1][0] -=   dt_P_1_1;
    k->P[1][1] +=   k->uncertainty.bias * delta_time;

    double S      = k->P[0][0] + k->uncertainty.sensor;
    k->K[0]       = k->P[0][0] / S;
    k->K[1]       = k->P[1][0] / S;
    double delta_value = value_new - k->value;
    k->value     += k->K[0] * delta_value;
    k->bias      += k->K[1] * delta_value;

    k->P[0][0] -= k->K[0] * k->P[0][0];
    k->P[0][1] -= k->K[0] * k->P[0][1];
    k->P[1][0] -= k->K[1] * k->P[0][0];
    k->P[1][1] -= k->K[1] * k->P[0][1];

    k->velocity = k->rate;
    k->timestamp = timestamp();
};

int isExpired( kalman_t * k)
{
    return ((timestamp() - k->timestamp) > k->lifespan);
}

const struct kalman Kalman =
{
    .init = init,
    .update = update,
    .isExpired = isExpired
};

