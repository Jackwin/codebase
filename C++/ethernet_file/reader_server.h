#pragma once

//#include <sys/types.h> // uint16_t, uint32_t
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "types.h"

using namespace std;
//using std::string;

#ifndef READER_SERVER_H
#define READER_SERVER_H
typedef unsigned char           uint8_t;
typedef unsigned short          uint16_t;
typedef unsigned int            uint32_t;

uint16_t* reader_server(string data_file, int line_num, int buf_size);
//void reader_server(string data_file, int line_num, uint16_t **data_buf_ptr);

#endif