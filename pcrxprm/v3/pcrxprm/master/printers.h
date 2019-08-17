#ifndef printers_h
#define printers_h

#include "platform.h"

#define PRINT_BUFFER_LEN ( 1 << 8 )
static char str_buf[PRINT_BUFFER_LEN];

#ifndef LOG_LEVEL
#define LOG_LEVEL
enum LogLevel
{
    OFF = -1,
    TEST = 0,
    DEBUG_0,
    DEBUG_1,
    DEBUG_2,
    ALWAYS
};
#endif

#define CORE_LOG_LEVEL          ALWAYS

#ifndef LOG
#define LOG(L,...)              if(L >= CORE_LOG_LEVEL) \
                                {  for(uint8_t i=L;i<ALWAYS;i++) \
                                print("\t"); sprintf(str_buf, __VA_ARGS__); print(str_buf); }
#define LOG_BARE(L,...)         if(L >= CORE_LOG_LEVEL) \
                                { sprintf(str_buf, __VA_ARGS__); print(str_buf); }
#endif
                                
#define STATE_DEBUG             DEBUG_2

void print( char * Buf );
//void drawDensityMap( density_t * a, int l );
//void dprintBuffers( void );
//void dprintCapture( void );
//void dprint( uint8_t * scrAddr, uint16_t len );
//void printBuffers( uint32_t s );
//void printAddress( const char * s, uint32_t addr );
//void printAddresses( void );
//void printCapture( void );
//void printBuffer( index_t * a, int l );
//void printPredictionPair( prediction_pair_t * pr );

#endif
