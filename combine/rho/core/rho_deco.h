/************************************************************************
 *  File: rho_deco.h
 *  Group: Tau+
 ***********************************************************************/

#ifndef rho_deco_h
#define rho_deco_h

#include <stdio.h>
#include <string.h>

#include <pthread.h>

#include "rho_types.h"
#include "image_types.h"

#ifdef __cplusplus
extern "C" {
#endif

    void RIM_PERFORM_RHO_FUNCTION( cimage_t );

    void RIM_INIT_FROM_CORE( rho_core_t * );
    void RIM_FRAME_INIT(  void );
    void RIM_FRAME_START( void );
    void RIM_FRAME_END(   void );

    typedef struct rho_interrupts rho_interrupts;
    struct rho_interrupts
    {
        void (*INIT_FROM_CORE)( rho_core_t * );
        void (*FRAME_START)( void );
        void (*FRAME_END)(   void );
        void (*RHO_FUNCTION)( const cimage_t );
    };
    static const rho_interrupts RhoInterrupts =
    {
        .INIT_FROM_CORE = RIM_INIT_FROM_CORE,
        .FRAME_START = RIM_FRAME_START,
        .FRAME_END   = RIM_FRAME_END,
        .RHO_FUNCTION = RIM_PERFORM_RHO_FUNCTION
    };

    typedef struct
    {
        uint32_t
        C_FRAME_MAX;
        index_t
        counter,
        y_delimiter,
        W,
        H;
    } rho_global_variables;

    typedef struct
    {
        index_t
        x,
        p,
        Cx,
        Cy;
        pixel_base_t
        *wr,
        *rd;
        byte_t
        THRESH;
        byte_t
        QS,
        PTOG;
    } rho_register_variables;

    typedef struct
    {
        density_map_unit_t
            *Dx,
            *Dy;
        density_t
            *THRESH_ADDR;
        density_2d_t
            *Q,
            QT,
            QN,
            QN_;
        index_t
            y,
            *CX_ADDR,
            *CY_ADDR;
        pixel_base_t
            *C_FRAME,
            *C_FRAME_END;
    } rho_sram_variables;

    typedef struct
    {
        bool connected;
        rho_global_variables    global;
        rho_register_variables  registers;
        rho_sram_variables      ram;
    } rho_variables;

    extern rho_variables RhoVariables;

#ifdef __cplusplus
}
#endif


#endif /* rho_interrupt_h */
