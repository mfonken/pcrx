//
//  rho_pid.c
//  rho_client
//
//  Created by Matthew Fonken on 10/28/18.
//  Copyright © 2019 Marbl. All rights reserved.
//

#include "rho_pid.h"
#ifndef TIMESTAMP
#include "timestamp.h"
#endif

void RhoPIDInitialize( rho_pid_t * PID, rho_pid_gain_t K )
{
    /* Zero Entire PID */
    memset( PID, 0, sizeof(rho_pid_t) );
    
    if( !K.Kp && !K.Ki && !K.Kd )
    {
        K.Pu = 1.;
        PID->Gain.Kp = DEFAULT_PROPORTIONAL_FACTOR * K.Ku;
        PID->Gain.Ki = DEFAULT_INTEGRAL_FACTOR * ( PID->Gain.Kp / K.Pu );
        PID->Gain.Kd = DEFAULT_DERIVATIVE_FACTOR * ( PID->Gain.Kd * K.Pu );
    }
    else
    {
        PID->Gain.Kp = K.Kp;
        PID->Gain.Ki = K.Ki;
        PID->Gain.Kd = K.Kd;
    }
    PID->Timestamp = TIMESTAMP();
}

void RhoPIDUpdate( rho_pid_t * PID, floating_t actual, floating_t target )
{
    PID->Error = actual - target;
    
    PID->Pv = PID->Error * PID->Gain.Kp;
    
    PID->Dt = TIMESTAMP() - PID->Timestamp;
    PID->TotalError += PID->Error * PID->Dt;
    PID->Iv = PID->Gain.Ki * PID->TotalError;
    
    PID->DeltaError = PID->PrevError - PID->Error;
    PID->Dv = ZDIV( ( PID->Gain.Kd * PID->DeltaError ), PID->Dt);
    
    PID->Value = PID->Pv + PID->Iv + PID->Dv + PID->Bias;
    
    if( PID->MaxValue > 0 )
        PID->Value = BOUND(PID->Value, PID->MinValue, PID->MaxValue);
    
    PID->PrevError = PID->Error;

    PID->TotalError *= 0.9;
}

void RhoPIDPrint( rho_pid_t * PID )
{
    //printf("\tValue:%3.4f\tBias:%3.4f\tError:%3.4f\tTotalError:%3.4f\t[P%3.2f\tI%3.2f\tD%3.2f]",
           //PID->Value, PID->Bias, PID->Error, PID->TotalError, PID->Pv, PID->Iv, PID->Dv);
}
