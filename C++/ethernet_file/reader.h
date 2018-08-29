#pragma once

//#include <sys/types.h> // uint16_t, uint32_t

#include "types.h"
using namespace std;
using std::string;

#ifndef READER_H
#define READER_H
typedef unsigned char           uint8_t;
typedef unsigned short          uint16_t;
typedef unsigned int            uint32_t;

void reader(string data_file, int line_num, uint16_t *data_buf);

#endif