//
//  rho_pid.h
//  rho_client
//
//  Created by Matthew Fonken on 10/21/18.
//  Copyright © 2019 Marbl. All rights reserved.
//

#ifndef rho_pid_h
#define rho_pid_h

#include <string.h>
#include "rho_global.h"
#include "rho_config.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct
    {
        floating_t Kp, Ki, Kd, Ku, Pu;
    } rho_pid_gain_t;
    
    typedef struct
    {
      rho_pid_gain_t
        Gain;
      floating_t
        Pv,
        Iv,
        Dv,
        Value,
        Bias,
        Dt,
        Timestamp,
        PrevError,
        Error,
        DeltaError,
        TotalError,
        MinValue,
        MaxValue;
    } rho_pid_t;
    
    typedef struct
    {
        void (*Initialize)( rho_pid_t *, rho_pid_gain_t );
        void (*Update)( rho_pid_t *, floating_t, floating_t );
        void (*Print)( rho_pid_t * );
    } rho_pid_functions;
    
    void RhoPIDInitialize( rho_pid_t *, rho_pid_gain_t );
    void RhoPIDUpdate( rho_pid_t *, floating_t, floating_t );
    void RhoPIDPrint( rho_pid_t * );
    
    static const rho_pid_functions RhoPID =
    {
        .Initialize = RhoPIDInitialize,
        .Update = RhoPIDUpdate,
        .Print = RhoPIDPrint
    };
    
#ifdef __cplusplus
}
#endif

#endif /* rho_pid_h */
