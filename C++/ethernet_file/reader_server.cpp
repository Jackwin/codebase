#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reader_server.h"
//#include "param.h"
using namespace std;
using std::string;

#define SERVER_DATA_BUF_SIZE (144 * 22 * 2)
#define TRUE 1

#define FALSE 0
//extern uint16_t data_buf[1024*16];
//uint16_t *reader(string data_file, int line_num)
uint16_t * reader_server(string data_file, int line_num, int buf_size)
{
    //ifstream in();l
    ifstream in;
    in.open(data_file.c_str());
    if (!in) {
        cerr<< "Fail to open file"<<data_file<<"\n";
        exit(0);
    }

    string filename;
    string line;

    int index = 0;
    int line_cnt = 0;
    uint16_t sum = 0;
    int valid_char_cnt = 0;
            bool valid_char = FALSE;


    uint16_t *data_buf = (uint16_t *)malloc(sizeof(uint16_t) * 144*22*2);
    if (data_buf == NULL) {
        printf("Fail to apply memory for data_buf.\n");

    }

    memset(data_buf, 0, sizeof(uint16_t) * 144*22*1);

    //memset (&data_buf, 0, sizeof(data_buf));
    //for (int i = 0; i < 1024; i++)
    //    *(data_buf + i) = 1;

    if(in.is_open())
    {
        while (getline (in, line))
        {
            //cout << line << endl;

            for (uint32_t i = 0; i < line.size(); i++) {

                sum = sum << 4;
                if (isdigit(line[i])) {
                    sum += line[i] - '0';
                    valid_char_cnt++;
                    valid_char = TRUE;
                }
                else if (tolower(line[i]) && ('a' <= line[i]) && (line[i] <= 'f'))  {
                    sum += line[i] - 'a' + 10;
                    valid_char_cnt++;
                    valid_char = TRUE;
                }
                else if (toupper(line[i]) && ('A' <= line[i] && line[i] <= 'F')) {
                    sum += line[i] - 'A' + 10;
                    valid_char_cnt++;
                    valid_char = TRUE;
                }
                else {
                    valid_char = FALSE;
                }

                if ((valid_char_cnt % 4 == 0) && (valid_char_cnt != 0) && valid_char == TRUE) {
                   data_buf[index] = sum;
                    //printf("data_buf[%d] is %x, sum is %x.\n", index, data_buf[index], sum);
                    index++;
                     valid_char = FALSE;
                }


            }

            line_cnt = line_cnt + 1;

            if (line_cnt == line_num) break;
        }
        in.close();
        return data_buf;
    }
    else
    {
        cout <<"no such file" << endl;
       return NULL;
    }


    //return data_buf;

}

/*
int main(void) {
    uint16_t *data_ptr;
    data_ptr = reader("test.txt", 7);
    for (int i = 0; i < 10; i++) {
        cout<<"hex:"<<hex<<data_ptr[i]<<endl;
    }

}
*/