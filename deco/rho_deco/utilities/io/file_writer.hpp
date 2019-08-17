//
//  usb_serial.hpp
//  combine_core
//
//  Created by Matthew Fonken on 1/21/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#ifndef usb_serial_hpp
#define usb_serial_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>

//#include "environment.hpp"

class FileWriter
{
    const char *file_name;
public:
    FileWriter( const char * name = NULL);
    
    void init( const char * );
    void trigger( std::string data );
};

#endif /* usb_serial_hpp */
