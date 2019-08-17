#ifndef master_h
#define master_h

/***************************************************************************************/
/*                                    Includes                                         */
/***************************************************************************************/
#include "system.h"

/***************************************************************************************/
/*                              Function Definitions                                   */
/***************************************************************************************/
void Master_Connect( I2C_Handle_t *, TIMER_Handle_t *, USART_Handle_t * );
void Master_Init( void );
void Master_Run( void );

/***************************************************************************************/
/*                              Function Structures                                    */
/***************************************************************************************/
typedef struct
{
  void (*Connect)( I2C_Handle_t *, TIMER_Handle_t *, USART_Handle_t * );
  void (*Init)( void );
  void (*Run)( void );
} master_functions_t;

static master_functions_t MasterFunctions =
{
  .Connect = Master_Connect,
  .Init    = Master_Init,
  .Run     = Master_Run
};

/***************************************************************************************/
/*                                  Routines Definitions                               */
/***************************************************************************************/
void InitializePlatform( void );
void ConnectToHost( void );
void ConfigureApplication( void );
void ExitInitialization( void );
inline void ApplicationCore( void );
void ErrorStateHandler( void );

/***************************************************************************************/
/*                                Core State List                                      */
/***************************************************************************************/
static system_states_list_t global_states_list =
{
  { INITIALIZING,       CONNECTING_TO_HOST, InitializePlatform    },
  { CONNECTING_TO_HOST, CONFIGURING,        ConnectToHost         },
  { CONFIGURING,        READY,              ConfigureApplication  },
  { READY,              ACTIVE,             ExitInitialization    },
  { ACTIVE,             IDLE,               ApplicationCore       },
  { ERROR_STATE,        IDLE,               ErrorStateHandler     }
};

#endif
