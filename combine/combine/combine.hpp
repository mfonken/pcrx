//
//  combine.hpp
//  combine_core
//
//  Created by Matthew Fonken on 1/21/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#ifndef combine_hpp
#define combine_hpp

#include <stdio.h>
#include <string>

#include "kinetic_master.h"
#include "utility_master.h"
#include "environment_master.h"
#include "tau_master.h"

#define MAX_BUFFER 256
#define SCALE 100

class Combine : public TestInterface
{
public:
    void Init( void );
    void Trigger( void );
    string Serialize( void );
    
    Combine(const char *, Tau * );
    
private:
    int         width,
                height;
    Tau       * tau;
    kinetic_t   kin;
    imu_t       imu;
};

#endif /* combine_hpp */
