//
//  statistics.c
//  combine
//
//  Created by Matthew Fonken on 8/10/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#include "statistics.h"

void GenerateCumulativeMomentsStatistics( floating_t v, floating_t i, floating_t *m0, floating_t *m1, floating_t *n )
{
#ifdef __USE_RUNNING_AVERAGE__
    floating_t n_=1/(++(*n));
    *m0+=((v-*m0)*n_);
    *m1+=(((v*i)-*m1)*n_);
#else
    ++(*n);
    *m0+=v;
    *m1+=v*i;
#endif
}

void CumulativeMovingAverageStatistics( floating_t new_val, floating_t *avg, int num )
{
    *avg+=(new_val-*avg)/(floating_t)(num+1);
}
