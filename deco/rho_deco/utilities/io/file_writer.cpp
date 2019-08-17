//
//  usb_serial.cpp
//  combine_core
//
//  Created by Matthew Fonken on 1/21/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#include "file_writer.hpp"

using namespace std;

FileWriter::FileWriter( const char * name )
{
    file_name = name;
}

void FileWriter::init( const char * name )
{
    file_name = name;
    std::ofstream outfile;
    outfile.open(file_name, std::ofstream::out | std::ofstream::trunc);
    if (!outfile.is_open())
    {
#ifdef UTILITY_VERBOSE
        printf("Failed to open %s\n", file_name);
#endif
        while(1);
    }
#ifdef UTILITY_VERBOSE
    printf("Opened %s\n", file_name);
#endif
}

void FileWriter::trigger( std::string data )
{
    std::ofstream outfile;
    outfile.open(file_name, std::ofstream::out | std::ofstream::trunc);
    outfile.write(data.c_str(),data.length());
    outfile.close();
}
