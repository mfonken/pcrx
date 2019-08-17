/************************************************************************
 *  File: rho_deco.c
 *  Group: Tau+
 ***********************************************************************/

#include "rho_deco.h"

#define BURN_ROWS 0
#define BURN_COLS 0

const rho_interrupts RhoInterrupts =
{
    RIM_INIT_FROM_CORE,
    RIM_FRAME_START,
    RIM_FRAME_END,
    RIM_PERFORM_RHO_FUNCTION
};

rho_variables RhoVariables = { 0 };

void RIM_INIT_FROM_CORE( rho_core_t * core )
{
    /* Connect to Interrupt Model variable structure */
    RhoVariables.ram.Dx      =  core->DensityMapPair.x.map;
    RhoVariables.ram.Dy      =  core->DensityMapPair.y.map;
    RhoVariables.ram.Q       =  core->Q;
    RhoVariables.ram.CX_ADDR = &core->Centroid.x;
    RhoVariables.ram.CY_ADDR = &core->Centroid.y;
    RhoVariables.ram.C_FRAME =  core->cframe;
    RhoVariables.ram.THRESH_ADDR = (density_t *)&core->ThreshByte;

    RhoVariables.global.C_FRAME_MAX = C_FRAME_SIZE;
    RhoVariables.global.y_delimiter = Y_DEL;
    RhoVariables.global.W    =  core->Width;
    RhoVariables.global.H    =  core->Height;

    RhoVariables.connected = true;
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

//    memset(RhoVariables.ram.C_FRAME, 0, sizeof(char)*RhoVariables.global.C_FRAME_MAX);
    memset(RhoVariables.ram.Dy, 0, sizeof(density_map_unit_t) * RhoVariables.global.W);
    memset(RhoVariables.ram.Dx, 0, sizeof(density_map_unit_t) * RhoVariables.global.H);
    memset(RhoVariables.ram.Q,  0, sizeof(density_2d_t) * 4);

    RhoVariables.registers.Cx   = *RhoVariables.ram.CX_ADDR;
    RhoVariables.registers.Cy   = *RhoVariables.ram.CY_ADDR;
    RhoVariables.registers.wr   = RhoVariables.ram.C_FRAME;
    RhoVariables.registers.rd   = RhoVariables.ram.C_FRAME;
    RhoVariables.registers.THRESH = (uint8_t)*RhoVariables.ram.THRESH_ADDR;
}

void RIM_FRAME_END( void )
{
}

#define RPC(X,T) if(X&T)
#define RPCB(X,Y,N,T) {RPC(X,T){Q##Y++;N[x]++;}}

void RIM_PERFORM_RHO_FUNCTION( const cimage_t image )
{
    if(!RhoVariables.connected) return;
    index_t w = image.width, h = image.height;

    index_t y = 0, x;
    uint32_t p = 0;

    RhoInterrupts.FRAME_START();
    
    RhoVariables.ram.QT = 0;
    density_2d_t Q0 = 0, Q1 = 0, Q2 = 0, Q3 = 0, QT = 0, QP = 0;
    uint8_t THRESH = RhoVariables.registers.THRESH;
    for(; y < RhoVariables.registers.Cy; y++ )
    {
        for( x = 0; x < RhoVariables.registers.Cx; x++, p++ )
        {
            if(image.pixels[p] > THRESH)
            {
                Q0++;
                RhoVariables.ram.Dy[x]++;
            }
        }
        for( ; x <  w; x++, p++ )
        {
            if(image.pixels[p] > THRESH)
            {
                Q1++;
                RhoVariables.ram.Dy[x]++;
            }
        }
        QT = Q0 + Q1;
        RhoVariables.ram.Dx[y] = QT - QP;
        QP = QT;
    }
    for( QP = 0; y < h; y++ )
    {
        for( x = 0; x < RhoVariables.registers.Cx; x++, p++ )
        {
            if(image.pixels[p] > THRESH)
            {
                Q2++;
                RhoVariables.ram.Dy[x]++;
            }
        }
        for( ; x < w; x++, p++ )
        {
            if(image.pixels[p] > THRESH)
            {
                Q3++;
                RhoVariables.ram.Dy[x]++;
            }
        }
        QT = Q2 + Q3;
        RhoVariables.ram.Dx[y] = QT - QP;
        QP = QT;
    }

    RhoVariables.ram.Q[0] = Q0;
    RhoVariables.ram.Q[1] = Q1;
    RhoVariables.ram.Q[2] = Q2;
    RhoVariables.ram.Q[3] = Q3;
    RhoVariables.ram.QT   = Q0 + Q1 + Q2 + Q3;

    LOG_RHO(DEBUG_0, "Quadrants are [%d][%d][%d][%d] (%d|%d)\n", Q0, Q1, Q2, Q3, RhoVariables.registers.Cx, RhoVariables.registers.Cy);
    LOG_RHO(DEBUG_0, "# Total coverage is %.3f%%\n", ((double)RhoVariables.ram.QT)/((double)w*h)*100);
    LOG_RHO(DEBUG_0, ">>>frame density is %d<<<\n", RhoVariables.ram.QT);
}
