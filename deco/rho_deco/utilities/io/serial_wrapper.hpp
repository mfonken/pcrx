//
//  serial_wrapper.hpp
//  combine_core
//
//  Created by Matthew Fonken on 1/20/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#ifndef serial_wrapper_hpp
#define serial_wrapper_hpp

#include <stdio.h>
#include <string.h>

#include "file_writer.hpp"
#include "sercom.h"

#define DEFAULT_HANDSHAKE_DELAY     1000000
#define DEFAULT_HANDSHAKE_ATTEMPTS  3
#define DEFAULT_HANDSHAKE_ID        "ab\r\n"
#define NO_HANDSHAKE                NULL

#define BUFFER_SIZE                 (1 << 8)

typedef enum
{
    HANDSHAKE_FAILED = -1,
    HANDSHAKE_SUCCEEDED,
    HANDSHAKE_SKIPPED,
    INITIALIZATION_FAILED
} SerialWriter_STATUS;

typedef enum
{
    USB = 0,
    BLUETOOTH,
    SFILE
} SerialWriter_TYPE;

class SerialWriter
{
    SerialWriter_TYPE type;
    SERCOM_Channel channel;
    FileWriter writer;
    
public:
    SerialWriter();
    SerialWriter( SerialWriter_TYPE );
    SerialWriter( SerialWriter_TYPE, const char * );
    SerialWriter_STATUS initUSB(const char *);
    SerialWriter_STATUS initBluetooth(const char *);
    SerialWriter_STATUS initFile(const char *);
    SerialWriter_STATUS init(char *, char *, const char *);
    SerialWriter_STATUS handshake( const char * );
    SerialWriter_STATUS handshake( const char *, int, int );
    
    int isInitialized();
    void write(std::string);
    std::string read(int);
};

#endif /* bluetooth_serial_hpp */
