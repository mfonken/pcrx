/************************************************************************
 *  File: rho_structure.h
 *  Group: Rho Core
 ***********************************************************************/

#ifndef rho_structure_h
#define rho_structure_h

#define DUAL_FILTER_CYCLE(X) \
for( X = 1, X##_ = 2; X##_ > 0; X--, X##_-- )

#define BOUNDED_CYCLE_DUAL(A,B,C,D,E,F,G) \
for(A = B, D = 0, F = 0; A > C; --A, D = E[A], F = G[A] )

#define PACKET_ADDRESS_INITIALIZER(r)               \
{                                                   \
(address_t)&r.y.TrackingFilters[0].value, /* px */  \
(address_t)&r.x.TrackingFilters[0].value, /* py */  \
(address_t)&r.y.TrackingFilters[1].value, /* sx */  \
(address_t)&r.x.TrackingFilters[1].value, /* sy */  \
(address_t)&r.Probabilities.P[1],         /* pp */  \
(address_t)&r.Probabilities.P[2],         /* ap */  \
(address_t)&r.Probabilities.P[3]          /* ap */  \
}

#endif /* rho_structure_h */
