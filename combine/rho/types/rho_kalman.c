#include "rho_kalman.h"
#ifndef TIMESTAMP
#include "timestamp.h"
#endif
/** SOURCE: http://preview.tinyurl.com/9djhrem */

const struct rho_kalman RhoKalman =
{
    InitializeRhoKalman,
    ResetRhoKalman,
    PredictRhoKalman,
    UpdateRhoKalman,
    StepRhoKalman,
    IsRhoKalmanExpired,
    ScoreRhoKalman,
    PunishRhoKalman
};

void InitializeRhoKalman( rho_kalman_t * k, floating_t v, floating_t ls, index_t minv, index_t maxv, rho_kalman_uncertainty_c uncertainty )
{
    k->lifespan    = ls;
    k->uncertainty.value   = uncertainty.value;
    k->uncertainty.bias    = uncertainty.bias;
    k->uncertainty.sensor  = uncertainty.sensor;
    
    k->origin    = TIMESTAMP();
    k->timestamp = k->origin;
    
    k->min_value = minv;
    k->max_value = maxv;
    
    ResetRhoKalman(k, v);
}

void ResetRhoKalman( rho_kalman_t * k, floating_t v )
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
    k->variance    = 0;
    k->flag        = 0;
    k->score       = 0;
    k->origin      = TIMESTAMP();
}

void PredictRhoKalman( rho_kalman_t * k, floating_t rate_new )
{
    floating_t delta_time = TIMESTAMP() - k->timestamp;
    
    /* Quick expiration check */
    if(delta_time > k->lifespan)
    {
        k->timestamp = TIMESTAMP();
        return;
    }
    k->velocity   = k->value - k->prev;
    k->prev       = k->value;
    k->rate       = rate_new - k->bias;
    k->value     += delta_time * k->rate;
    k->value      = BOUND(k->value, k->min_value, k->max_value);
    
    floating_t dt_P_1_1 = delta_time * k->P[1][1];
    k->P[0][0]   += delta_time * ( dt_P_1_1 -
                                  k->P[0][1] -
                                  k->P[1][0] +
                                  k->uncertainty.value );
    k->P[0][1]   -= dt_P_1_1;
    k->P[1][0]   -= dt_P_1_1;
    k->P[1][1]   += k->uncertainty.bias * delta_time;
    
    LOG_KALMAN("Prediction - Value:%.2f Rate:%.2f Velocity:%.2f\n", k->value, k->rate, k->velocity);
}

void UpdateRhoKalman( rho_kalman_t * k, floating_t value_new )
{
    floating_t S_ = 1. / ( k->P[0][0] + k->uncertainty.sensor );
    k->K[0]       = k->P[0][0] * S_;
    k->K[1]       = k->P[1][0] * S_;
    floating_t delta_value = value_new - k->value;
    k->value     += k->K[0] * delta_value;
    k->bias      += k->K[1] * delta_value;
    
    k->P[0][0]   -= k->K[0] * k->P[0][0];
    k->P[0][1]   -= k->K[0] * k->P[0][1];
    k->P[1][0]   -= k->K[1] * k->P[0][0];
    k->P[1][1]   -= k->K[1] * k->P[0][1];
    
    k->timestamp  = TIMESTAMP();
    
    k->value = BOUND(k->value, k->min_value, k->max_value);
    LOG_KALMAN("Update - Value:%.2f Bias:%.2f K:%.2f|%.2f\n", k->value, k->bias, k->K[0], k->K[1]);
};

floating_t StepRhoKalman( rho_kalman_t * k, floating_t value_new, floating_t rate_new )
{
    LOG_KALMAN("Step - Id:%p NewVal:%.2f NewRate:%.2f\n", k, value_new, rate_new);
    PredictRhoKalman(k, rate_new);
    UpdateRhoKalman(k, value_new);
    return k->value;
}

bool IsRhoKalmanExpired( rho_kalman_t * k )
{
    return ((TIMESTAMP() - k->timestamp) > k->lifespan);
}

inline floating_t ScoreRhoKalman( rho_kalman_t * k )
{
    floating_t score = k->K[0];
    if(k->flag) score = 0.;
    k->score = score;
    return score;
}

void PunishRhoKalman( rho_kalman_t * k )
{
    k->K[0] *= KALMAN_PUNISH_FACTOR;
    if( ScoreRhoKalman(k) < MIN_KALMAN_GAIN )
        ResetRhoKalman(k, 0);
}
