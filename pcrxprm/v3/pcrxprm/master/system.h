/************************************************************************
 *  File: system.h
 *  Group: PCR Core
 ***********************************************************************/

#ifndef system_h
#define system_h

/***************************************************************************************/
/*                                    Includes                                         */
/***************************************************************************************/
#include <stdint.h>
#ifdef __RHO__
#include "rho_master.h"
#else
#ifdef __OV9712__
#include "OV9712.h"
#else
#endif
#endif

/***************************************************************************************/
/*                              Type Definitions                                       */
/***************************************************************************************/
typedef enum
{
  INITIALIZING = 1,
  CONNECTING_TO_HOST,
  CONFIGURING,
  READY,
  ACTIVE,
  IDLE,
  RECONFIGURING,
  ERROR_STATE,
  NUM_SYSTEM_STATES
} system_state_enum;

static const char * system_state_enum_strings[] =
{
  "INITIALIZING",
  "CONNECTING_TO_HOST",
  "CONFIGURING",
  "READY",
  "ACTIVE",
  "IDLE",
  "RECONFIGURING",
  "ERROR_STATE",
  "UNKNOWN"
};

typedef struct
{
  system_state_enum
    state,
    next;
  void (*routine)(void);
} system_state_t;

typedef system_state_t system_states_list_t[NUM_SYSTEM_STATES];

typedef struct
{
  uint8_t id;
} system_header_t;

typedef struct
{
  /* Header */
  system_header_t header;

  /* State Control */
  system_state_enum state;
  system_states_list_t * state_list;
} system_t;

static system_t System =
{
  { THIS_ID },
  INITIALIZING, // State
};

/***************************************************************************************/
/*                             Function Definitions                                    */
/***************************************************************************************/
void InitSystem(               system_t *, system_states_list_t * );
void NextStateSystem(          system_t *                         );
void SetStateSystem(           system_t *,  system_state_enum     );
void PerformStateSystem(       system_t *                         );
void EnterStateSystem(         system_t *,  system_state_enum     );
bool IsInStateSystem(          system_t *,  system_state_enum     );
system_state_t * GetStateSystem( system_t *                            );
system_state_t * GetStateFromListSystem( system_t *, system_state_enum );

/***************************************************************************************/
/*                             Function Structures                                     */
/***************************************************************************************/
typedef struct
{
  void (*Next)(           system_t *                       );
  void (*Set)(            system_t *, system_state_enum    );
  void (*Perform)(        system_t *                       );
  void (*Enter)(          system_t *, system_state_enum    );
  bool (*IsIn)(           system_t *, system_state_enum    );
  system_state_t * (*Get)(         system_t *                    );
  system_state_t * (*GetFromList)( system_t *, system_state_enum );
} system_state_functions;

typedef struct
{
  void (*Init)( system_t *, system_states_list_t * );
  system_state_functions State;
} system_functions;

/***************************************************************************************/
/*                               Global Instances                                      */
/***************************************************************************************/
static system_functions SystemFunctions =
{
  .Init           = InitSystem,
  .State.Next     = NextStateSystem,    /* Enter next state               */
  .State.Set      = SetStateSystem,     /* Set state, no perform          */
  .State.Perform  = PerformStateSystem, /* Peform current state's routine */
  .State.Enter    = EnterStateSystem,   /* Set and perform state          */
  .State.IsIn     = IsInStateSystem,    /* Check if system is in state    */
  .State.Get      = GetStateSystem,     /* Get current state              */
  .State.GetFromList = GetStateFromListSystem /* Get state data from list */
};

#endif
