//
//  FileAccess.h
//  Voltex
//
//  Created by Samuel Dewan on 2015-06-02.
//
//


#ifndef Voltex_FileAccess_h
#define Voltex_FileAccess_h

#include "Voltex.h" //this is just an extention of voltex.cpp since voltec.cpp is where all the params are handled
#include <string>

int writeAllToFile (char filePath[], Voltex* voltex, bool overWrite = false);
int readAllFromFile (char filePath[], Voltex* voltex);

double getStringAsDouble(std::string s);

#endif

/*
 file format:
 
 formatversion:t:a:b:l:e:1:t:a:b:l:e:2:t:a:b:l:e:3:t:a:b:l:e:4:t:a:b:l:e:5:t:a:b:l:e:6:t:a:b:l:e:7:t:a:b:l:e:8:p:a:r:a:m:s:i:n:o:r:d:eEOF
 
 */
