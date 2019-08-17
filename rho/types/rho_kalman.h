#ifndef rho_kalman_h
#define rho_kalman_h

#include <stdlib.h>
#include <stdbool.h>
#include "rho_global.h"

#define KALMAN_PUNISH_FACTOR  0.7
#define MIN_KALMAN_GAIN       0.001
#define KALMAN_MATURATION     3 // Seconds

#ifdef __cplusplus
extern "C" {
#endif
    
    /* Standard headers */
//    typedef double floating_t;
    
    /** Kalman Uncertainties */
    typedef struct
    {
        floating_t value;
        floating_t bias;
        floating_t sensor;
    } rho_kalman_uncertainty_c;
    
    /** Kalman structure */
    typedef struct
    {
        floating_t
        K[2],
        P[2][2],
        rate,
        bias,
        value,
        prev,
        velocity,
        variance,
        lifespan,
        timestamp,
        origin,
        score;
        rho_kalman_uncertainty_c
        uncertainty;
        bool
        sorted,
        valid,
        flag;
        index_t
        min_value,
        max_value;
    } rho_kalman_t;
    
//    floating_t TIMESTAMP(void);
    void  InitializeRhoKalman( rho_kalman_t *, floating_t, floating_t, index_t, index_t, rho_kalman_uncertainty_c );
    void       ResetRhoKalman( rho_kalman_t *, floating_t );
    void     PredictRhoKalman( rho_kalman_t *, floating_t );
    void      UpdateRhoKalman( rho_kalman_t *, floating_t );
    floating_t  StepRhoKalman( rho_kalman_t *, floating_t, floating_t );
    bool   IsRhoKalmanExpired( rho_kalman_t * );
    floating_t ScoreRhoKalman( rho_kalman_t * );
    void      PunishRhoKalman( rho_kalman_t * );
    
    struct rho_kalman {
        void (*  Initialize)( rho_kalman_t *, floating_t, floating_t, index_t, index_t, rho_kalman_uncertainty_c );
        void (*      Reset)( rho_kalman_t *, floating_t );
        void (*    Predict)( rho_kalman_t *, floating_t );
        void (*     Update)( rho_kalman_t *, floating_t );
        floating_t (* Step)( rho_kalman_t *, floating_t, floating_t );
        bool (*  IsExpired)( rho_kalman_t * );
        floating_t (*Score)( rho_kalman_t * );
        void (*     Punish)( rho_kalman_t * );
    };
    
    static const struct rho_kalman RhoKalman =
    {
        .Initialize = InitializeRhoKalman,
        .Reset = ResetRhoKalman,
        .Predict = PredictRhoKalman,
        .Update = UpdateRhoKalman,
        .Step = StepRhoKalman,
        .IsExpired = IsRhoKalmanExpired,
        .Score = ScoreRhoKalman,
        .Punish = PunishRhoKalman
    };
    
#ifdef __cplusplus
}
#endif

#endif /* kalman_h */

