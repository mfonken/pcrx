#ifndef OV9712_h
#define OV9712_h

/************************************************************************
 *                             Includes                      *
 ***********************************************************************/
#include "main.h"
#include "OV9712_calc.h"
#include "OV9712_regs.h"
#include "printers.h"

/************************************************************************
 *                             Local Types                              *
 ***********************************************************************/
typedef struct
{
  /* Pins */
  GPIO_t
    ENABLE,
    LOW_VOLTAGE,
    POWER_DOWN,
    MASTER_CLOCK;
} OV9712_pins_t;

typedef struct
{
  uint8_t         ADDR;
  I2C_Handle_t  * CAM_I2C_PORT;
  OV9712_pins_t * Pins;
} OV9712_t;

/************************************************************************
 *                             Local Functions                              *
 ***********************************************************************/
void OV9712_Init( OV9712_t *, I2C_Handle_t *, OV9712_pins_t * );
void OV9712_Write( OV9712_t *, uint8_t, uint8_t );
void OV9712_Enable( OV9712_t * );
void OV9712_Disable( OV9712_t * );

typedef struct
{
  void (*Init)( OV9712_t *, I2C_Handle_t *, OV9712_pins_t * );
  void (*Write)( OV9712_t *, uint8_t, uint8_t);
  void (*Enable)(  OV9712_t * );
  void (*Disable)(  OV9712_t * );
} OV9712_functions;

static OV9712_functions OV9712_Functions =
{
  .Init = OV9712_Init,
  .Write = OV9712_Write,
  .Enable = OV9712_Enable,
  .Disable = OV9712_Disable
};

/************************************************************************
 *                       Local Instances                                *
 ***********************************************************************/
static OV9712_pins_t Default_OV9712_Pins =
{
  { CAM_EN_GPIO_Port,   CAM_EN_Pin }, // ENABLE
  { EN_1V5_GPIO_Port,   EN_1V5_Pin }, // LOW_VOLTAGE
  { PWDN_GPIO_Port,     PWDN_Pin   }, // POWER_DOWN
  { MCLK_GPIO_Port,     MCLK_Pin   }  // MASTER_CLOCK
};

static OV9712_t OV9712 =
{
  OV9712_ADDR,
  { 0 }
};

#endif
