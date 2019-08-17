//
//  serial_wrapper.cpp
//  combine_core
//
//  Created by Matthew Fonken on 1/20/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#include "serial_wrapper.hpp"

SerialWriter::SerialWriter(){}

SerialWriter::SerialWriter( SerialWriter_TYPE type )
{
    SerialWriter(type, NO_HANDSHAKE);
    this->type = type;
}

SerialWriter::SerialWriter( SerialWriter_TYPE type, const char * data )
{
    SerialWriter_STATUS ret;
    this->type = type;
    switch(type)
    {
        default:
        case USB:
            ret = initUSB(data);
            break;
        case BLUETOOTH:
            ret = initBluetooth(data);
            break;
        case SFILE:
            ret = initFile(data);
            break;
    }
#ifdef UTILITY_VERBOSE
    switch(ret)
    {
        case HANDSHAKE_SUCCEEDED:
            printf("Handshake success.\n");
            break;
        case HANDSHAKE_FAILED:
            printf("Handshake failed.\n");
            break;
        case HANDSHAKE_SKIPPED:
            printf("Handshake skipped.\n");
            break;
        case INITIALIZATION_FAILED:
            printf("Initialization failed.\n");
            break;
    }
#endif
}

SerialWriter_STATUS SerialWriter::initUSB(const char * handshake_id)
{
    Init_SERCOM_Default();
    return handshake(handshake_id);
}

SerialWriter_STATUS SerialWriter::initBluetooth(const char * handshake_id)
{
    channel.filestream = -1;
    channel.port = "/dev/tty.Bluetooth-Incoming-Port";
    channel.port_alt = "/dev/cu.Bluetooth-Incoming-Port";
    Init_SERCOM( &channel );
    return handshake(handshake_id);
}

SerialWriter_STATUS SerialWriter::initFile(const char * name)
{
    writer.init(name);
    return HANDSHAKE_SKIPPED;
}

SerialWriter_STATUS SerialWriter::init( char * port, char * port_alt,const  char * handshake_id )
{
    channel.filestream = -1;
    channel.port = port;
    channel.port_alt = port_alt;
    Init_SERCOM( &channel );
    return handshake(handshake_id);
}

SerialWriter_STATUS SerialWriter::handshake(const  char * id )
{
    return handshake(id, DEFAULT_HANDSHAKE_DELAY, DEFAULT_HANDSHAKE_ATTEMPTS);
}
SerialWriter_STATUS SerialWriter::handshake(const  char * id, int delay, int attempts )
{
    if( !channel.initialized ) return INITIALIZATION_FAILED;
    if( id != NULL ) return HANDSHAKE_SKIPPED;
    int counter = 0;
    while( counter < attempts )
    {
        if( Test_SERCOM(channel.filestream) <= 0 )
        {
            counter++;
#ifdef UTILITY_VERBOSE
            printf("SerialWriter attempt #%d.\n", counter );
#endif
            usleep(delay);
        }
        else break;
    }
    if(counter == attempts) return HANDSHAKE_FAILED;
    else
    {
        Write_SERCOM_Bytes(channel.filestream, id, 4);
        return HANDSHAKE_SUCCEEDED;
    }
}

int SerialWriter::isInitialized()
{
    if(channel.initialized) return 1;
    else return 0;
}

void SerialWriter::write( std::string data )
{
    if(type == SFILE) writer.trigger(data);
    else Write_SERCOM_Bytes( channel.filestream, data.c_str(), (int)data.length() );
}

std::string SerialWriter::read( int l )
{
    char data[l];
    Read_SERCOM_Bytes( channel.filestream, data, l );
    return std::string(data);
}
