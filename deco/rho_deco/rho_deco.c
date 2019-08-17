/************************************************************************
 *  File: rho_interrupt_model.c
 *  Group: Tau+
 ***********************************************************************/

#include "rho_deco.h"


#define BURN_ROWS 0
#define BURN_COLS 0

#ifdef USE_INTERRUPT_MODEL
/* Universal Port for interrupt model */
pixel_base_t test_port = 0;
#endif

rho_variables RhoVariables = { 0 };

void RIM_INIT_FROM_CORE( rho_core_t * core )
{
    /***** INTERRUPT MODEL CROSS-CONNECTOR VARIABLES START *****/
    /* Connect to Interrupt Model variable structure */
    RhoVariables.ram.Dx      =  core->DensityMapPair.x.map;
    RhoVariables.ram.Dy      =  core->DensityMapPair.y.map;
    RhoVariables.ram.Q       =  core->Q;
    RhoVariables.ram.CX_ADDR = &core->Cx;
    RhoVariables.ram.CY_ADDR = &core->Cy;
    RhoVariables.ram.C_FRAME =  core->cframe;
    RhoVariables.ram.THRESH_ADDR = (density_t *)&core->ThreshByte;
    RhoVariables.ram.CAM_PORT = &test_port;

    RhoVariables.global.C_FRAME_MAX = C_FRAME_SIZE;
    RhoVariables.global.y_delimiter = Y_DEL;
    RhoVariables.global.W    =  core->Width;
    RhoVariables.global.H    =  core->Height;

    /* Frame Initializeializer routine */
    RhoInterrupts.FRAME_INIT();

    /* Interrupt model mutex Initializeializer */
    pthread_mutex_init(&RhoVariables.global.rho_int_mutex, NULL);
    pthread_mutex_lock(&RhoVariables.global.rho_int_mutex);

    RhoVariables.connected = true;
    /*****  INTERRUPT MODEL CROSS-CONNECTOR VARIABLES END  *****/
}

void RIM_PERFORM_RHO_C( cimage_t image )
{
    if(!RhoVariables.connected) return;
    RhoInterrupts.FRAME_START();
    //        pthread_create((pthread_t)&RhoVariables.global.loop_thread, (const pthread_attr_t *)NULL, (void *)RhoInterrupts.LOOP_THREAD, (void *)&RhoVariables.global.rho_int_mutex);
    uint32_t p = BURN_ROWS * image.height;
    for( index_t y = BURN_ROWS, x; y < image.height; y++ )
    {
        RhoInterrupts.ROW_INT();
        p += BURN_COLS;
        for( x = BURN_COLS; x < image.width; x++, p++ )
        {
            *(RhoVariables.ram.CAM_PORT) = image.pixels[p];
            RhoInterrupts.PCLK_INT();
        }
    }
    RhoInterrupts.FRAME_END();
}

void RIM_FRAME_START( void )
{
    RhoVariables.registers.x    = 0;
    RhoVariables.registers.p    = 0;
    RhoVariables.registers.QS   = 0;
    RhoVariables.registers.PTOG = 0;

    RhoVariables.ram.y          = 0;
    RhoVariables.ram.QN         = 0;
    RhoVariables.ram.QN_        = 0;
    RhoVariables.ram.QT         = 0;
    RhoVariables.ram.C_FRAME_END = RhoVariables.ram.C_FRAME + RhoVariables.global.C_FRAME_MAX;

    memset(RhoVariables.ram.C_FRAME, 0, sizeof(char)*RhoVariables.global.C_FRAME_MAX);
    memset(RhoVariables.ram.Dy, 0, sizeof(dmap_t) * RhoVariables.global.W);
    memset(RhoVariables.ram.Dx, 0, sizeof(dmap_t) * RhoVariables.global.H);
    memset(RhoVariables.ram.Q,  0, sizeof(density_2d_t) * 4);

    RhoVariables.registers.Cx   = *RhoVariables.ram.CX_ADDR;
    RhoVariables.registers.Cy   = *RhoVariables.ram.CY_ADDR;
    RhoVariables.registers.wr   = RhoVariables.ram.C_FRAME;
    RhoVariables.registers.rd   = RhoVariables.ram.C_FRAME;
    RhoVariables.registers.THRESH = *RhoVariables.ram.THRESH_ADDR;
}

void RIM_FRAME_END( void )
{
    if(pthread_mutex_trylock(&RhoVariables.global.rho_int_mutex))
        pthread_mutex_unlock(&RhoVariables.global.rho_int_mutex);
    RhoVariables.ram.QT = 0;
    density_2d_t * Qp = RhoVariables.ram.Q;
    for( uint8_t i = 0; i < 4; i++ )
        RhoVariables.ram.QT += Qp[i];
    LOG_RHO(DEBUG_0, ">>>frame density is %d<<<\n", RhoVariables.ram.QT);
}

#define RPC(X) if(X&0xf0)
#define RPCB(X,Y,N) {RPC(X){Q##Y++;N[x]++;}}

void RIM_PERFORM_RHO_FUNCTION( const cimage_t image )
{
    if(!RhoVariables.connected) return;
    index_t w = image.width, h = image.height;

    index_t y, x;
    uint32_t p;

    RhoInterrupts.FRAME_START();

    RhoVariables.ram.QT = 0;
    density_2d_t Q0 = 0, Q1 = 0, Q2 = 0, Q3 = 0, QN = 0, QN_ = 0;
    for( y = 0, p = 0; y < RhoVariables.registers.Cy; y++ )
    {
        for( x = 0; x < RhoVariables.registers.Cx; x++, p++ )
        {
            RPC(image.pixels[p])
            {
                Q0++;
                RhoVariables.ram.Dy[x]++;
            }
        }
        for(      ; x <  w; x++, p++ )
        {
            RPC(image.pixels[p])
            {
                Q1++;
                RhoVariables.ram.Dy[x]++;
            }
        }
        QN = Q0 + Q1;
        RhoVariables.ram.Dx[y] = QN - QN_;
        QN_ = QN;
    }
    QN_ = 0;
    for( ; y < h; y++ )
    {
        for( x = 0; x < RhoVariables.registers.Cx; x++, p++ )
        {
            RPC(image.pixels[p])
            {
                Q2++;
                RhoVariables.ram.Dy[x]++;
            }
        }
        for(      ; x <  w; x++, p++ )
        {
            RPC(image.pixels[p])
            {
                Q3++;
                RhoVariables.ram.Dy[x]++;
            }
        }
        QN = Q2 + Q3;
        RhoVariables.ram.Dx[y] = QN - QN_;
        QN_ = QN;
    }

    RhoVariables.ram.Q[0] = Q0;
    RhoVariables.ram.Q[1] = Q1;
    RhoVariables.ram.Q[2] = Q2;
    RhoVariables.ram.Q[3] = Q3;
    RhoVariables.ram.QT = Q0 + Q1 + Q2 + Q3;

    LOG_RHO(DEBUG_0, "Quadrants are [%d][%d][%d][%d] (%d|%d)\n", Q0, Q1, Q2, Q3, RhoVariables.registers.Cx, RhoVariables.registers.Cy);
    LOG_RHO(DEBUG_0, "# Total coverage is %.3f%%\n", ((double)RhoVariables.ram.QT)/((double)w*h)*100);
}
