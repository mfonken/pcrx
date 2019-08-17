#include "printers.h"

/***************************************************************************************/
/*                                    Printers                                         */
/***************************************************************************************/

inline void print( char * Buf )
{
  TransmitToHost( (uint8_t *)Buf, strlen((const char *)Buf) );
}

/*
void drawDensityMap( density_t * a, int l )
{
    for( int i = 0; i < l; i++ )
    {
        density_t curr = a[i];
        sprintf((char *)hex, "%4d[%2d]", curr, i);
        print( hex );
        if( curr > 10 ) curr = 10;
        for( ; curr > 0; curr--) print( " " );
        print( "|\r\n" );
    }
}

void dprintBuffers( void )
{
	HAL_UART_Transmit( this_uart, (uint8_t *)&BUFFER_TX_DEL, 2, UART_TIMEOUT );
	dprint( (uint8_t *)svb.density_y, sizeof(density_t)*(CAPTURE_BUFFER_WIDTH + CAPTURE_BUFFER_HEIGHT ) );
}

void dprintCapture( void )
{
	HAL_UART_Transmit( this_uart, (uint8_t *)&BUFFER_TX_DEL, 2, UART_TIMEOUT );
	dprint( (uint8_t *)svb.capture, sizeof(density_t)*CAPTURE_BUFFER_SIZE );
}

void dprint( uint8_t * scrAddr, uint16_t len )
{
	while(svf.uart_dma_busy);
	svf.uart_dma_busy = 1;
	while(HAL_UART_Transmit_DMA( this_uart, scrAddr, len ) != HAL_OK);// Error_Handler();
}

void printBuffers( uint32_t s )
{
    print( "Printing Thresh Buffer\r\n" );
    printBuffer( svb.thresh, thresh_buffer_size );

    print( "Printing Dx\r\n" );
    drawDensityMap(svb.density_x, CAPTURE_BUFFER_HEIGHT);

    print( "Printing Dy\r\n" );
    drawDensityMap(svb.density_y, CAPTURE_BUFFER_WIDTH);
}

void printAddress( const char * s, uint32_t addr )
{
    sprintf((char *)hex, "%s: %8x\r\n", s, addr);
    print( hex );
}

void printAddresses( void )
{
    printAddress("CamPt", (uint32_t)svd.camera_port);
    printAddress("C bfr", (uint32_t)svb.capture);
    printAddress("C end", (uint32_t)svd.capture_end);
    printAddress("C max", (uint32_t)svd.capture_max);
    printAddress("T bfr", (uint32_t)svb.thresh);
    printAddress("T end", (uint32_t)svd.thresh_end);
    printAddress("T max", (uint32_t)svd.thresh_max);
    printAddress("   Dx", (uint32_t)svb.density_x);
    printAddress("   Dy", (uint32_t)svb.density_y);
    printAddress("    Q", (uint32_t)svb.quadrant);
    printAddress("Q tot", (uint32_t)&svd.quadrant_total);
    printAddress("Q prv", (uint32_t)&svd.quadrant_previous);
    printAddress("   Cx", (uint32_t)&svd.CENTROID_X);
    printAddress("   Cy", (uint32_t)&svd.CENTROID_Y);
    printAddress("   Xm", (uint32_t)&svd.density_x_max);
    printAddress("T val", (uint32_t)&svd.thresh_value);
    printAddress("M end", (uint32_t)&svd.rho_mem_end);
}

void printCapture( void )
{
	for(int y = 0; y < CAPTURE_BUFFER_HEIGHT; y++ )
	{
		for(int x = 0; x < CAPTURE_BUFFER_WIDTH; x++ )
		{
			uint32_t p = x + y * CAPTURE_BUFFER_WIDTH;
			sprintf((char *)hex, " 0x%2x", svb.capture[p]);
			print( hex );
		}
		print( "\r\n" );
	}
}

void printBuffer( index_t * a, int l )
{
    for( int i = 0; i < l; i++ )
    {
        index_t curr = a[i];
        if( curr == 0xaaaa )
            sprintf((char *)hex, "\r\n(%d):", i);
        else
            sprintf((char *)hex, " 0x%x", curr);
        print( hex );
    }
    print("\r\n");
}

void printPredictionPair( prediction_pair_t * pr )
{
	int xp = (int)pr->x.primary.value;
	int yp = (int)pr->y.primary.value;
	int xs = (int)pr->x.secondary.value;
	int ys = (int)pr->y.secondary.value;
	print("Printing predictions ");
	sprintf((char *)hex, "\t\t\tP(%d, %d) | S(%d, %d)\r\n", xp, yp, xs, ys);
	print( hex );
}
*/