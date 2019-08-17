#ifndef master_interface_h
#define master_interface_h

#include "global_config.h"

/***************************************************************************************/
/*                                  Master Definitions                                   */
/***************************************************************************************/
typedef struct
{
  I2C_Handle_t * I2C_Primary;
  USART_Handle_t * USART_Primary;
} master_ios_t;

typedef struct
{
  TIMER_Handle_t * Timer_Primary;
} master_utilities_t;

typedef struct
{
  master_ios_t IOs;
  master_utilities_t Utilities;
} master_t;

extern master_t Master;

#endif