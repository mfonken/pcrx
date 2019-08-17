/************************************************************************
 *  File: platform.h
 *  Group: PCR Core
 ***********************************************************************/

#ifndef platform_h
#define platform_h

/***************************************************************************************/
/*                                    Includes                                         */
/***************************************************************************************/
#include "platform_interface_master.h"

/***************************************************************************************/
/*                                  Macro Definitions                                  */
/***************************************************************************************/
#ifndef _PLATFORM_
#error "No platform specified!"
#else
#define PLATFORM_SPECIFIC_FUNCTION_STRINGIFY(x,y) x ## _ ## y
#define PLATFORM_SPECIFIC_FUNCTION_BUILDER(x,y) PLATFORM_SPECIFIC_FUNCTION_STRINGIFY(x,y)
#define PLATFORM_SPECIFIC(FUNC) PLATFORM_SPECIFIC_FUNCTION_BUILDER(_PLATFORM_, FUNC)
#endif

#ifndef HOST_ADDRESS
#error "HOST_ADDRESS is not defined."
#endif

#define TIMESTAMP PLATFORM_SPECIFIC(Timestamp)

/***************************************************************************************/
/*                                  Type Definitions                                   */
/***************************************************************************************/
typedef struct
{
  GPIO_Port_t * port;
  uint16_t pin;
} GPIO_t;

typedef enum
{
  NO_PROTOCOL_TYPE = 0,
  I2C_PROTOCOL_TYPE,
//  SPI_PROTOCOL_TYPE,
  USART_PROTOCOL_TYPE,
//  USB_PROTOCOL_TYPE
} protocol_t;

typedef struct
{
  /* Generic utilities */
  protocol_t HostProtocol;
  generic_handle_t HostHandle;

  /* Application utilities */
#ifdef __RHO__
  camera_application_flags CameraFlags;
#endif
} platform_t;

typedef enum
{
  PING_HOST = 0x01,
} host_command_type_enum;
/* Keep these to a minimum */

/* Wait type for commands with required response */
typedef enum
{
  NONE = 0,
  PATIENT,
  WANTED,
  NEEDED,
  CRITICAL
} platform_wait_priority_level_enum;

typedef enum
{
  NO_STATUS = 0,
  OK,
  FAILURE,
  INVALID_INPUT,
  INVALID_OUTPUT
} platform_status_enum;

typedef struct
{
  uint8_t
    host_id,
    this_id;
  platform_wait_priority_level_enum
    priority : 8;
  host_command_type_enum
    command : 8;
  // uint8_t
  //   return_data[HOST_COMMAND_RETURN_LENGTH];
} host_command_t;

/***************************************************************************************/
/*                               Function Definitions                                  */
/***************************************************************************************/
void InitPlatform( platform_t *, protocol_t, generic_handle_t );

#ifdef __RHO__
///* Application interfaces */
//static void InitRhoInterface( TIMER_Handle_t * timer, USART_Handle_t * usart );
//
//inline uint8_t   TransmitPacket( packet_t * );
//inline uint16_t  ReceivePacket( packet_t * );
#endif

/*** Custom platform interfaces ***/
/* GPIO */
inline void SetPortMode(GPIO_t *, uint16_t );
inline void WritePin(GPIO_t *, uint16_t );

/* Host */
inline uint8_t TransmitToHost( uint8_t *, uint16_t );
inline uint16_t ReceiveFromHost( uint8_t * );
platform_status_enum PerformHostCommand( host_command_type_enum, platform_wait_priority_level_enum );

/***************************************************************************************/
/*                               Function Structures                                   */
/***************************************************************************************/
#ifdef __RHO__
//typedef struct
//{
//  uint8_t (*Transmit)( packet_t * );
//  uint8_t (*Receive)( packet_t * );
//} rho_interface_packet_functions;
//
//typedef struct
//{
//  void (*Init)( TIMER_Handle_t *, USART_Handle_t * );
//  void (*ActivateFlags)( rho_system_flags variables * );
//  rho_interface_packet_functions Packet;
//} rho_interface_functions;
#endif

typedef struct
{
  void (*Handler)( uint16_t );
  void (*Enable)( void );
  void (*Disable)( void );
} platform_interface_interrupt_functions;

typedef struct
{
  void (*Init)( uint32_t, uint32_t, uint16_t, bool );
  void (*Pause)( void );
  void (*Resume)( void );
  void (*Reset)( void );
} platform_interface_dma_functions;

typedef struct
{
  uint8_t (*Transmit)( USART_Handle_t *, uint8_t *, uint16_t );
  uint16_t (*Receive)( USART_Handle_t *, uint8_t * );
  bool (*Completed)( USART_Handle_t * );
} platform_interface_uart_functions;

typedef struct
{
  void (*Transmit)( I2C_Handle_t *, uint16_t, uint8_t *, uint16_t, uint32_t );
} platform_interface_i2c_functions;

typedef struct
{
  void (*SetPortMode)(GPIO_t *, uint16_t );
  void (*Write)( GPIO_t *, uint16_t );
} platform_interface_gpio_functions;

typedef struct
{
  uint32_t (*Now)( void );
  uint32_t (*SysClockFreq)(void);
} platform_interface_clock_functions;

typedef struct
{
  uint8_t (*Transmit)( uint8_t *, uint16_t );
  uint16_t (*Receive)( uint8_t * );
  platform_status_enum (*Command) ( host_command_type_enum, platform_wait_priority_level_enum );
} platform_interface_host_functions;

typedef struct
{
  void (*Init)( platform_t *, protocol_t, generic_handle_t );
  void (*Wait)( uint32_t );
#ifdef __RHO__
//  rho_interface_functions                Rho;
//  platform_interface_packet_functions    Packet;
#endif
  platform_interface_interrupt_functions Interrupt;
  platform_interface_dma_functions       DMA;
  platform_interface_uart_functions      USART;
  platform_interface_i2c_functions       I2C;
  platform_interface_gpio_functions      GPIO;
  platform_interface_clock_functions     Clock;
  platform_interface_host_functions      Host;
} platform_interface_functions;

/***************************************************************************************/
/*                                Global Instances                                     */
/***************************************************************************************/
/* Global platform */
extern platform_t Platform;

static platform_interface_functions PlatformFunctions =
{
  .Init                 = InitPlatform,
  .Wait                 = PLATFORM_SPECIFIC(Wait),

#ifdef __RHO__
//  .Rho.Init             = InitRhoInterface,
//  .Rho.ActivateFlags    = ActivateFlagsRhoInterface,
//
//  .Rho.Packet.Transmit  = TransmitPacket,
//  .Rho.Packet.Receive   = ReceivePacket,
#endif

  .Interrupt.Handler    = PLATFORM_SPECIFIC(InterruptHandler),
  .Interrupt.Enable     = PLATFORM_SPECIFIC(InterruptEnable),
  .Interrupt.Disable    = PLATFORM_SPECIFIC(InterruptDisable),

  .DMA.Init             = PLATFORM_SPECIFIC(InitDMA),
  .DMA.Pause            = PLATFORM_SPECIFIC(PauseDMA),
  .DMA.Resume           = PLATFORM_SPECIFIC(ResumeDMA),
  .DMA.Reset            = PLATFORM_SPECIFIC(ResetDMA),

  .USART.Transmit       = PLATFORM_SPECIFIC(UartTxDMA),
  .USART.Receive        = PLATFORM_SPECIFIC(UartRxDMA),
  .USART.Completed      = PLATFORM_SPECIFIC(UartCompleted),

  .I2C.Transmit         = PLATFORM_SPECIFIC(I2CMasterTx),

  .GPIO.SetPortMode     = SetPortMode,
  .GPIO.Write           = WritePin,

  .Clock.Now            = PLATFORM_SPECIFIC(Timestamp),
  .Clock.SysClockFreq   = PLATFORM_SPECIFIC(SysClockFreq),

  .Host.Transmit        = TransmitToHost,
  .Host.Receive         = ReceiveFromHost,
  .Host.Command         = PerformHostCommand
};



#endif /* platform_h */
