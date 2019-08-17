//
//  stm32_interface.c
//  rho_client
//
//  created by matthew fonken on 9/22/18.
//  copyright © 2018 marbl. all rights reserved.
//

/************************************************************************
 *                             Includes                      *
 ***********************************************************************/
#include "stm32_interface.h"
#include "main.h"
#include "printers.h"

inline void STM_InterruptHandler( uint16_t GPIO_Pin )
{
#ifdef __RHO__
  if(!Platform.CameraFlags.IRQ) return;
  switch(GPIO_Pin)
  {
    case VSYNC_Pin:
        Platform.CameraFlags.Frame = !(flag_t)( VSYNC_GPIO_Port->IDR & VSYNC_Pin );
        break;
    case HREF_Pin:
        Platform.CameraFlags.Row = (flag_t)( HREF_GPIO_Port->IDR & HREF_Pin );
        /* Row capture is callback connected on HREF=HIGH */
        if( Platform.CameraFlags.Row
         && Platform.CameraFlags.Capture.Flag
         && Platform.CameraFlags.Capture.Callback != NULL )
            Platform.CameraFlags.Capture.Callback();
        break;
    default:
        return;
  }

  if(!Platform.CameraFlags.Row || Platform.CameraFlags.Frame ) STM_ResetDMA();
#endif
}
void STM_InterruptEnable( void )
{
  STM_ResumeDMA();
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}
void STM_InterruptDisable( void )
{
  STM_PauseDMA();
  HAL_NVIC_DisableIRQ(EXTI0_IRQn);
  HAL_NVIC_DisableIRQ(EXTI4_IRQn);
}

inline void STM_PauseDMA( void )
{
  __HAL_TIM_DISABLE_IT(Master.Utilities.Timer_Primary, RHO_TIM_IT_CC);
  TIM_CCxChannelCmd(Master.Utilities.Timer_Primary->Instance, RHO_TIM_CHANNEL, TIM_CCx_DISABLE);
}
inline void STM_ResumeDMA( void )
{
  __HAL_TIM_ENABLE_IT(Master.Utilities.Timer_Primary, RHO_TIM_IT_CC);
  TIM_CCxChannelCmd(Master.Utilities.Timer_Primary->Instance, RHO_TIM_CHANNEL, TIM_CCx_ENABLE);
}
inline void STM_ResetDMA( void )
{
    if(_dma_destination != NULL)
        Master.Utilities.Timer_Primary->hdma[RHO_TIM_DMA_ID]->Instance->CMAR = _dma_destination;
}
void STM_InitDMA( uint32_t src, uint32_t dst, uint16_t size, bool init_state )
{
//  if(HAL_DMA_Start_IT(Master.Utilities.Timer_Primary->hdma[RHO_TIM_DMA_ID], src, dst, size) != HAL_OK)
//    Error_Handler();
  __HAL_TIM_ENABLE_DMA(Master.Utilities.Timer_Primary, RHO_TIM_DMA_CC);
  if(init_state) STM_ResumeDMA();
  _dma_destination = dst;
}

inline uint8_t STM_UartTxDMA( USART_Handle_t * huart, uint8_t * buffer, uint16_t length )
{
  return HAL_USART_Transmit_DMA( Master.IOs.USART_Primary, buffer, length );
}

inline uint16_t STM_UartRxDMA( USART_Handle_t * huart, uint8_t * buffer )
{
  ///TODO: Actually implement
  return 1;
}
inline bool STM_UartCompleted( USART_Handle_t * huart )
{
#ifdef __RHO__
  Platform.CameraFlags.UARTBusy = 0;
#endif
  return false;
}

inline void STM_I2CMasterTx( I2C_Handle_t * hi2c, uint16_t addr, uint8_t * buffer, uint16_t length, uint32_t timeout )
{
  HAL_I2C_Master_Transmit( hi2c, addr, buffer, length, timeout);
}

inline void STM_SetPortMode( GPIO_TypeDef * port, uint16_t pin, uint8_t mode )
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = pin;
  GPIO_InitStruct.Mode = mode;
  HAL_GPIO_Init( (GPIO_TypeDef *)port, &GPIO_InitStruct );
}

inline void STM_WritePin( GPIO_TypeDef * port, uint16_t pin, uint8_t state )
{
  HAL_GPIO_WritePin( port, pin, (GPIO_PinState)state);
}

inline uint32_t STM_Timestamp(void)
{
  return HAL_GetTick();
}

inline void STM_Wait( uint32_t nTime )
{
  HAL_Delay( nTime );
}

inline uint32_t STM_SysClockFreq(void)
{
  return HAL_RCC_GetSysClockFreq();
}
