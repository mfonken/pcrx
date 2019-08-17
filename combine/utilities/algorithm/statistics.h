//
//  statistics.h
//  combine
//
//  Created by Matthew Fonken on 8/10/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef statistics_h
#define statistics_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "rho_global.h"

void GenerateCumulativeMomentsStatistics( floating_t, floating_t, floating_t *, floating_t *, floating_t * );
void CumulativeMovingAverageStatistics( floating_t, floating_t *, int );
    
#ifdef __cplusplus
}
#endif

#endif /* statistics_h */
