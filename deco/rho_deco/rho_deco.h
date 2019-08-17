/************************************************************************
 *  File: rho_interrupt_model.h
 *  Group: Tau+
 ***********************************************************************/

#ifndef rho_interrupt_model_h
#define rho_interrupt_model_h

#include <stdio.h>
#include <string.h>

#include "rho_types.h"
#include "image_types.h"

#ifdef __cplusplus
extern "C" {
#endif

    void RIM_PERFORM_RHO_C( cimage_t );
    void RIM_PERFORM_RHO_FUNCTION( cimage_t );

    void RIM_INIT_FROM_CORE( rho_core_t * );
    void RIM_FRAME_INIT(  void );
    void RIM_FRAME_START( void );
    void RIM_FRAME_END(   void );
    void RIM_ROW_INT(     void );
    void RIM_PCLK_INT(    void );
    void RIM_LOOP_THREAD( void * );

    typedef struct rho_interrupts rho_interrupts;
    struct rho_interrupts
    {
        void (*INIT_FROM_CORE)( rho_core_t * );
        void (*FRAME_INIT)(  void );
        void (*FRAME_START)( void );
        void (*FRAME_END)(   void );
        void (*ROW_INT)(     void );
        void (*PCLK_INT)(    void );
        void (*LOOP_THREAD)( void * );
        void (*RHO_FUNCTION)( const cimage_t );
    };
    static const rho_interrupts RhoInterrupts =
    {
        .INIT_FROM_CORE = RIM_INIT_FROM_CORE,
        .FRAME_INIT  = RIM_FRAME_INIT,
        .FRAME_START = RIM_FRAME_START,
        .FRAME_END   = RIM_FRAME_END,
        .ROW_INT     = RIM_ROW_INT,
        .PCLK_INT    = RIM_PCLK_INT,
        .LOOP_THREAD = RIM_LOOP_THREAD,
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

        pthread_t       loop_thread;
        pthread_mutex_t rho_int_mutex;
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
        density_t
        THRESH;
        byte_t
        QS,
        PTOG;
    } rho_register_variables;

    typedef struct
    {
        dmap_t
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
        *C_FRAME_END,
        *CAM_PORT;
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
