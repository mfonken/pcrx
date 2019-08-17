/************************************************************************
 *  File: rho_types.c
 *  Group: Rho Core
 ***********************************************************************/

#ifndef rho_c_types_h
#define rho_c_types_h

/***************************************************************************************/
/*                                    Includes                                         */
/***************************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __PSM__
#include "psm.h"
#else
#include "fsm.h"
#endif

#include "rho_config.h"
#include "rho_kalman.h"
#include "rho_pid.h"

/* Packet Generation Settings */
#define YES 1
#define NO  0

#ifndef MAX_OBSERVATIONS
#define MAX_OBSERVATIONS        (1 << 3) // Length of history
#endif

#define PACKET_HEADER_SIZE (sizeof(packet_header_t));
#define PACKET_HEADER_ID    0xab

#define PACKET_DATA_DECIMAL       floating_t
#define PACKET_DATA_INTEGER       uint8_t
#define PACKET_DATA_DECIMAL_SIZE  ((byte_t)sizeof(PACKET_DATA_DECIMAL))
#define PACKET_DATA_INTEGER_SIZE  ((byte_t)sizeof(PACKET_DATA_INTEGER))

#define PACKET_PX_DATA_OFFSET 0
#define PACKET_PY_DATA_OFFSET 1
#define PACKET_SX_DATA_OFFSET 2
#define PACKET_SY_DATA_OFFSET 3
#define PACKET_PP_DATA_OFFSET 4
#define PACKET_SP_DATA_OFFSET 5
#define PACKET_AP_DATA_OFFSET 6

#define PACKET_PX_DATA_SIZE PACKET_DATA_DECIMAL_SIZE
#define PACKET_PY_DATA_SIZE PACKET_DATA_DECIMAL_SIZE
#define PACKET_SX_DATA_SIZE PACKET_DATA_DECIMAL_SIZE
#define PACKET_SY_DATA_SIZE PACKET_DATA_DECIMAL_SIZE
#define PACKET_PP_DATA_SIZE PACKET_DATA_DECIMAL_SIZE
#define PACKET_SP_DATA_SIZE PACKET_DATA_DECIMAL_SIZE
#define PACKET_AP_DATA_SIZE PACKET_DATA_DECIMAL_SIZE

#define INCLUDE_PACKET_PX_DATA   YES
#define INCLUDE_PACKET_PY_DATA   YES
#define INCLUDE_PACKET_SX_DATA   YES
#define INCLUDE_PACKET_SY_DATA   YES
#define INCLUDE_PACKET_PP_DATA   YES
#define INCLUDE_PACKET_SP_DATA   YES
#define INCLUDE_PACKET_AP_DATA   YES

#define PACKET_INCLUDES  (  INCLUDE_PACKET_PX_DATA << PACKET_PX_DATA_OFFSET \
| INCLUDE_PACKET_PY_DATA << PACKET_PY_DATA_OFFSET \
| INCLUDE_PACKET_SX_DATA << PACKET_SX_DATA_OFFSET \
| INCLUDE_PACKET_SY_DATA << PACKET_SY_DATA_OFFSET \
| INCLUDE_PACKET_PP_DATA << PACKET_PP_DATA_OFFSET \
| INCLUDE_PACKET_SP_DATA << PACKET_SP_DATA_OFFSET \
| INCLUDE_PACKET_AP_DATA << PACKET_AP_DATA_OFFSET \
)
#define PACKET_SIZE    (  INCLUDE_PACKET_PX_DATA * PACKET_PX_DATA_SIZE \
+ INCLUDE_PACKET_PY_DATA * PACKET_PY_DATA_SIZE \
+ INCLUDE_PACKET_SX_DATA * PACKET_SX_DATA_SIZE \
+ INCLUDE_PACKET_SY_DATA * PACKET_SY_DATA_SIZE \
+ INCLUDE_PACKET_PP_DATA * PACKET_PP_DATA_SIZE \
+ INCLUDE_PACKET_SP_DATA * PACKET_SP_DATA_SIZE \
+ INCLUDE_PACKET_AP_DATA * PACKET_AP_DATA_SIZE \
)
#define PACKET_OFFSETS  {                    \
                        PACKET_PX_DATA_SIZE, \
                        PACKET_PY_DATA_SIZE, \
                        PACKET_SX_DATA_SIZE, \
                        PACKET_SY_DATA_SIZE, \
                        PACKET_PP_DATA_SIZE, \
                        PACKET_SP_DATA_SIZE, \
                        PACKET_AP_DATA_SIZE, \
                        }
#define LOG2_MAX_PACKET_TYPE_SIZE 4
#define PACKET_OFFSET_WIDTH LOG2_MAX_PACKET_TYPE_SIZE
typedef struct
{
    byte_t px:PACKET_OFFSET_WIDTH;
    byte_t py:PACKET_OFFSET_WIDTH;
    byte_t sx:PACKET_OFFSET_WIDTH;
    byte_t sy:PACKET_OFFSET_WIDTH;
    byte_t pp:PACKET_OFFSET_WIDTH;
    byte_t sp:PACKET_OFFSET_WIDTH;
    byte_t ap:PACKET_OFFSET_WIDTH;
} packet_offset_lookup_t;

typedef struct
{
byte_t
    a:PACKET_OFFSET_WIDTH,
    b:PACKET_OFFSET_WIDTH;
}packing_template_t;

typedef struct
{
address_t
    px,
    py,
    sx,
    sy,
    pp,
    sp,
    ap;
} packet_value_lookup_t;

#define NUM_PACKET_ELEMENTS ((byte_t)( sizeof(packet_offset_lookup_t) * ( 8 / PACKET_OFFSET_WIDTH ) ))
#define BEACON_PACKET_ID 0x11
#define BEACON_DEFAULT_PERIOD 20 // cycles

typedef struct
{
byte_t
    ID,
    includes;
timestamp_t
    timestamp;
uint8_t
    padding[6];
} packet_header_t;

typedef struct
{
    packet_header_t header;
    byte_t          data[PACKET_SIZE];
} packet_t;

typedef struct
{
density_t
    max,
    den;
index_t
    loc,
    wth,
    srt;
floating_t
    scr;
byte_t
    tracking_id;
} region_t;

/* Rho Structures* */
typedef struct
{
    dmap_t         *map,
                   *background,
                    max[2];
    index_t         length,
    centroid;
    bool            has_background;
    rho_kalman_t    kalmans[2];
} density_map_t;

typedef struct
{
    density_map_t x,y;
} density_map_pair_t;

typedef struct
{
    byte_t
        background[4][4],
        current[4][4],
        factor[4][4],
        length[4];
} density_redistribution_lookup_config_t;

typedef struct
{
    density_redistribution_lookup_config_t config[4];
} density_redistribution_lookup_t;

typedef struct
{
    floating_t  P[NUM_STATES],
                confidence;
} prediction_probabilities;

typedef struct
{
    rho_kalman_t    TrackingFilters[MAX_TRACKING_FILTERS];
    uint8_t         TrackingFiltersOrder[MAX_TRACKING_FILTERS];
    region_t        Regions[MAX_REGIONS];
    uint8_t         RegionsOrder[MAX_REGIONS],
                    NumRegions;
    floating_t      NuRegions,
                    Primary,
                    Secondary,
                    AverageDensity;
    density_t       PreviousPeak[2],
                    PreviousCentroid;
    density_2d_t    PreviousDensity[2],
                    TotalDensity,
                    FilterDensity[2];
    observation_list_t ObservationList;
    prediction_probabilities Probabilities;
} prediction_t;

typedef struct
{
    prediction_t    x,y;
    prediction_probabilities Probabilities;

    floating_t      NuRegions,
//                    BestConfidence,
                    AverageDensity;
} prediction_pair_t;

typedef struct
{
    density_2d_t left, right;       /* Direction density values */
    bool complete;
} section_process_t;

typedef struct
{
  void (*Init)(void);
  void (*Pause)(void);
  void (*Resume)(void);
  void (*Reset)(void);
} platform_dma_interface_functions;

typedef struct
{
  uint8_t (*Transmit)( byte_t *, index_t);
} rho_platform_uart_interace_functions;

#ifndef USE_INTERRUPT_MODEL
typedef struct
{
  void (*Activate)( camera_application_flags * );
} rho_platform_flag_interace_functions;
#endif

typedef struct
{
  uint32_t (*Now)( void );
} rho_platform_time_interace_functions;

typedef struct
{
    index_t
    xl[3],
    yl[3];
    density_2d_t
    area[9];

    density_2d_t a, b, c, d, l, l_, p, q, x, y;
} redistribution_variables;
extern const density_redistribution_lookup_t rlookup;

typedef struct
{
    floating_t
    background,
    state,
    target,
    proposed;
} rho_tune_t;

typedef struct
{
    index_t
      len,
      range[3],
      cyc,
      cyc_,
      gapc,
      width,
      blbf,            /* Region fill */
      x,               /* Generic index */
      start,
      end;
    density_t
      fpeak,
      fpeak_2,
      fbandl,
      c,
      b,
      rcal_c; /* Recalculation counter */
      variance_t
      fvar;
    density_2d_t
      cmax,
      cden,
      tden, /* Target density */
      fden, /* Filtered density */
      fden1, /* Initial filtered density before retries */
      total_density,
      filtered_density;
    bool
      has,
      rcal; /* Recalculate */
    floating_t
      avgc,
      cavg,   /* cumulative average */
      mavg,   /* moment average */
      chaos,
      target_density,
      assumed_regions;
} rho_detection_variables;

typedef struct
{
    index_t
    Ax,
    Ay,
    Bx,
    By,
    Cx,
    Cy;
    int8_t qcheck;
} prediction_predict_variables;

typedef struct
{
    packet_t *packet;
    address_t pdPtr,
    llPtr,
    *alPtr;
    byte_t
    includes,
    i,
    j,
    l,
    t;
} packet_generation_variables;

#define DETECTION_BUFFER_SIZE ( 1 << 8 )
#define DETECTION_BUFFER_MASK ( DETECTION_BUFFER_SIZE - 1 )
#define MAX_DENSITY           ( 1 << 8 )

typedef struct
{
    uint8_t
    thresh;
    index_t
    density;
    uint8_t
    tracking_id;
} detection_element_t;

typedef struct
{
    uint16_t index,
            fill,
            length,
            first;
    detection_element_t buffer[DETECTION_BUFFER_SIZE];
} detection_ring_buffer_t;

typedef detection_ring_buffer_t detection_map_t;

typedef struct
{
    density_map_pair_t  DensityMapPair;
    index_t
        Width,
        Height,
        Subsample,
        RowsLeft,
        Px,
        Py,
        Sx,
        Sy,
        Cx,
        Cy,
        Bx,
        By;
    byte_t
        ThreshByte,
        BackgroundCounter;
    density_2d_t
        Q[4],
        Qb[4],
        Qf[4],
        QbT,
        TotalCoverage,
        FilteredCoverage,
        TargetCoverage,
        BackgroundPeriod;
    floating_t
        FilteredPercentage,
        TargetCoverageFactor,
        CoverageFactor,
        VarianceFactor,
        PreviousThreshFilterValue,
        Thresh;
    rho_tune_t          Tune;
    prediction_pair_t   PredictionPair;
    rho_pid_t           ThreshFilter;
    rho_kalman_t        TargetFilter;
    detection_map_t     DetectionMap;

#ifdef __PSM__
    psm_t               PredictiveStateModel;
#else
    kumaraswamy_t       Kumaraswamy;
    fsm_system_t        StateMachine;
#endif
    packet_t            Packet;

#ifdef USE_INTERRUPT_MODEL
    uint8_t             cframe[C_FRAME_SIZE];
#endif
} rho_core_t;

#endif /* rho_c_types_h */
