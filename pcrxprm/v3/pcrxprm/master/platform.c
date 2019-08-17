/************************************************************************
 *  File: platform.c
 *  Group: PCR Core
 ***********************************************************************/

/************************************************************************
 *                             Includes                      *
 ***********************************************************************/
#include "platform.h"

/************************************************************************
 *                           Local Instance                             *
 ***********************************************************************/
platform_t Platform;

/************************************************************************
 *                      Local Function Definitions                      *
 ***********************************************************************/
void InitPlatform( platform_t * platform, protocol_t host_communication_protocol, generic_handle_t host_communication_handle )
{
  *platform = (platform_t){ host_communication_protocol, host_communication_handle };
}

void WritePin( GPIO_t * gpio, uint16_t val )
{
  PLATFORM_SPECIFIC(WritePin)( gpio->port, gpio->pin, val );
}

void SetPortMode(GPIO_t * gpio, uint16_t val )
{
  PLATFORM_SPECIFIC(SetPortMode)( gpio->port, gpio->pin, val );
}

platform_status_enum PerformHostCommand(
  host_command_type_enum command,
  platform_wait_priority_level_enum priority )
{
  platform_status_enum status = NO_STATUS;
  uint16_t return_data = 0;
  host_command_t packet = { HOST_ADDRESS, THIS_ID, priority, command };
  for( uint8_t i = 0;
    i < HOST_COMM_RETRIES
    && status != OK;
    i++ )
  {
    switch( command )
    {
      case PING_HOST:
        return_data = PlatformFunctions.Host.Transmit( (uint8_t *)&packet, sizeof(host_command_t) );
        if( return_data == 0 )
        {
          status = FAILURE;
          continue;
        }
//        return_data = PlatformFunctions.Host.Receive( /* Enter correct buffer */ );
        if( return_data == 0 )
        {
          status = INVALID_OUTPUT;
          continue;
        }
        status = OK;
        break;
      default:
        status = INVALID_INPUT;
        break;
    }
    PlatformFunctions.Wait( HOST_COMMAND_RETRY_TIME );
  }
  return status;
}

uint8_t TransmitToHost( uint8_t * buffer, uint16_t length )
{
#if HOST_COMMUNICATION_PROTOCOL == USART
  return PlatformFunctions.USART.Transmit( (USART_Handle_t *)Platform.HostHandle, buffer, length );
#elif HOST_COMMUNICATION_PROTOCOL == I2C
  PlatformFunctions.I2C.Transmit( (I2C_Handle_t *)Platform.HostHandle, HOST_ADDRESS, buffer, length );
  return 1;
#else
#error "Invalid host communication protocol!"
#endif
}
uint16_t ReceiveFromHost( uint8_t * buffer )
{
  #if HOST_COMMUNICATION_PROTOCOL == USART
    return PlatformFunctions.USART.Receive( (USART_Handle_t *)Platform.HostHandle, buffer );
  #elif HOST_COMMUNICATION_PROTOCOL == I2C
    PlatformFunctions.I2C.Receive( (I2C_Handle_t *)Platform.HostHandle, HOST_ADDRESS, data, len );
    return 1;
  #else
  #error "Invalid host communication protocol!"
  #endif
}

#ifdef __RHO__
void InitRhoInterface( TIMER_Handle_t * timer, USART_Handle_t * usart )
{
}

//inline bool TransmitPacket( packet_t * packet )
//{
//  return (bool)Platform.USART.Transmit( (uint8_t *)packet, sizeof(packet_t));
//}
//
//inline uint16_t ReceivePacket( packet_t * packet )
//{
//  return Platform.USART.Receive( (uint8_t *)packet );
//}

#endif
