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
    
    void init( std::string );
    void trigger( std::string data, std::ios_base::openmode mode = std::ofstream::out | std::ofstream::trunc );
};

#endif /* usb_serial_hpp */
