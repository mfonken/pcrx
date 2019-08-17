//
//  tau_structures.hpp
//  tau+
//
//  Created by Matthew Fonken on 2/8/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#ifndef tau_structures_hpp
#define tau_structures_hpp

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>
#include <string>
#include <sstream>

//#include "rho_kalman.h"
#include "kalman.hpp"

#ifndef LOG
#define LOG(L,...) printf(__VA_ARGS__)
#endif

#ifdef TAU_DEBUG
#define LOG_TAU(L,...) LOG(L,"<Tau> " __VA_ARGS__)
#else
#define LOG_TAU(...)
#endif

#define ABSINV(X)   ((X<1)?1-X:X-1)
#define ABS(X)      ((X>=0)?X:-X)

#define GTTHR(X,Y,T)    X>=(Y+T)
//#define INRANGE(X,Y,T)  (X>(Y-T)&&X<(Y+T))

#define MAX_PERSISTENCE ( 1 << 7 ) - 1 // MAX OF SIGNED BYTE

static inline double timeDiff( struct timeval a, struct timeval b )
{ return ((b.tv_sec  - a.tv_sec) + (b.tv_usec - a.tv_usec)/1000000.0) + 0.0005; }

template <typename T>
static std::string to_stringn(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
}

typedef enum
{
    X_DIMENSION = 0,
    Y_DIMENSION
} dimension_t;

typedef enum
{
    UNWEIGHTED = 0,
    WEIGHTED = 1
} sorting_settings;

/* Tau filter types specifically for kalman matrix structure */
typedef enum
{
    NO_FILTER = 0,
    SOFT_FILTER,
    HARSH_FILTER,
    CHAOS_FILTER
} filter_t;

/* Stability tracking for selec tions */
class Stability
{
public:
    double primary;
    double secondary;
    double alternate;
    double overall;
};

#define PREDICTION_LIFESPAN             1.0
#define PREDICTION_VALUE_UNCERTAINTY    0.5
#define PREDICTION_BIAS_UNCERTAINTY     0.01
#define PREDICTION_SENSOR_UNCERTAINTY   0.001

typedef struct
{
    double  primary,
            secondary,
            alternate;
} prediction_probabilities_t;

typedef enum
{
    SIMILAR = 0,
    OPPOSITE
} selection_pair_t;

class Prediction
{
public:
    KalmanFilter    primary,
                    secondary;
    double          primary_new,
                    secondary_new;
    prediction_probabilities_t probabilities;
    
    Prediction();
};

class PredictionPair
{
public:
    Prediction x;
    Prediction y;
    selection_pair_t        selection_pair;
};

typedef float global_timestamp_t;
typedef struct
{
    uint8_t
    ID,
    includes,
    timestamp[sizeof(global_timestamp_t)];
} global_packet_header;
typedef struct
{
    global_packet_header
    header;
    uint32_t
    padding;
    double
    px,
    py,
    sx,
    sy,
    pp,
    sp,
    ap;
} GlobalPacket;

#endif /* tau_structures_hpp */
