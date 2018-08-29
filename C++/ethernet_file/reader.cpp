#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "reader.h"
using namespace std;
using std::string;


#define TRUE 1

#define FALSE 0
//extern uint16_t data_buf[1024*16];
//uint16_t *reader(string data_file, int line_num)
void reader(string data_file, int line_num, uint16_t *data_buf)
{
    //ifstream in();
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
    //memset (&data_buf, 0, sizeof(data_buf));

    if(in.is_open())
    {
        while (getline (in, line))
        {
            //cout << line << endl;
            int valid_char_cnt = 0;
            bool valid_char = FALSE;
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

                if ((valid_char_cnt % 4 == 0) && (valid_char_cnt != 0) && valid_char) {
                    data_buf[index] = sum;
                    //printf("data_buf[%d] is %x, sum is %x.\n", index, data_buf[index], sum);
                    index++;
                    sum = 0;
                }


            }

            line_cnt = line_cnt + 1;

            if (line_cnt == line_num) {
                in.close();
                break;
            }
        }

    }
    else
    {
        cout <<"no such file" << endl;
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