//
//  sercom.cpp
//  combine_core
//
//  Created by Matthew Fonken on 12/7/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include "sercom.h"

int Init_SERCOM_Default( void )
{
    SERCOM_Channel chan = {-1, DEFAULT_SERCOM_PORT, DEFAULT_SERCOM_PORT_ALT, B115200, CS8, 0};
    return Init_SERCOM(&chan);
}
    
int Init_SERCOM( SERCOM_Channel * chan )
{
    chan->initialized = false;
    chan->filestream = open(chan->port, O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
    if (chan->filestream == -1)
    {
#ifdef UTILITY_VERBOSE
        printf("Trying alternate port at %s\n", chan->port_alt);
#endif
        chan->filestream = open(chan->port_alt, O_RDWR | O_NOCTTY | O_NDELAY);
        if (chan->filestream == -1)
        {
#ifdef UTILITY_VERBOSE
            printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
#endif
            return chan->filestream;
        }
        else
        {
#ifdef UTILITY_VERBOSE
            printf("Initializing SERCOM at %s: success on alternate.\n", chan->port);
#endif
        }
    }
#ifdef UTILITY_VERBOSE
    else
    {
        printf("Initializing SERCOM at %s(%d): success on main.\n", chan->port, chan->filestream);
    }
#endif
    
    tcgetattr(chan->filestream, &chan->options);
    chan->options.c_cflag = chan->baud | chan->bits | CLOCAL | CREAD;
    chan->options.c_iflag = IGNPAR;
    chan->options.c_oflag = 0;
    chan->options.c_lflag = 0;
    tcflush(chan->filestream, TCIFLUSH);
    tcsetattr(chan->filestream, TCSANOW, &chan->options);
    chan->initialized = true;
    return chan->filestream;
}

void Write_SERCOM_Byte( int filestream, char b )
{
    int byte_written = -1;
    while(byte_written <= 0) byte_written = (int)write(filestream, &b, 1);
}

void Write_SERCOM_Bytes( int filestream, const char * a, int l )
{
    int byte_written = -1;
    while(byte_written <= 0) byte_written = (int)write(filestream, a, (size_t)l);
}

int Read_SERCOM_Bytes( int filestream, char * a, int l )
{
    return (int)read(filestream, a, (size_t)l);
}

int Read_SERCOM_Byte( int filestream, char * b )
{
    return (int)read(filestream, b, 1);
}

int Test_SERCOM( int filestream )
{
    char t;
    return (int)read(filestream, &t, 1);
}


void tokenifyPacket( char * a, int l, int n, char s, double * d)
{
    char **tokens;
    tokens = str_split(a, ',');
    
    if (tokens != NULL)
    {
        int i;
        if(*(tokens)[0] != s)
        {
            d[0] = 0xffff;
            return;
        }
        for (i = 1; *(tokens + i) && i <= n; i++)
        {
            char * c = *(tokens + i);
#ifdef PACKET_DEBUG
            size_t l = sizeof(c);
            printf("%d - token %s(%lu)\n", i, c, l);
#endif
            if(l) d[i-1] = atof(c);
            else
            {
                d[0] = 0xffff;
#ifdef PACKET_DEBUG
                printf("Ending tokenize\n");
#endif
                return;
            }
            free(*(tokens + i));
        }
        if(i <= n)
        {
            d[0] = 0xffff;
#ifdef PACKET_DEBUG
            printf("Tokenize failed\n");
#endif
        }
        free(tokens);
    }
}

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;
    
    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }
    
    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);
    
    /* Add space for terminating null string so caller
     knows where the list of returned strings ends. */
    count++;
    
    result = malloc(sizeof(char*) * count);
    
    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);
        
        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        //        assert(idx == count - 1);
        *(result + idx) = 0;
    }
    
    return result;
}
