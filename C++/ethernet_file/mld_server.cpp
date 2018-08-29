#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <dirent.h>
#include <vector>

#include <sys/socket.h>
#include <sys/types.h> //u_int8_t, u_int16_t, u_int32_t
#include <errno.h>
#include <netinet/in.h>     // IPPROTO_ICMP, INET_ADDRSTRLEN
#include <netinet/ip.h>     // struct ip and IP_MAXPACKET (which is 65535)
#include <netinet/ip_icmp.h> // struct icmp, ICMP_ECHO
#include <arpa/inet.h>     // inet_pton() and inet_ntop()
#include <sys/ioctl.h>     // macro ioctl is defined
#include <netdb.h>         // struct addrinfo
#include <unistd.h>       // close()
#include <bits/ioctls.h>     // defines values for argument "request" of ioctl.
#include <net/if.h>       // struct ifreq
#include <linux/if_ether.h>   // ETH_P_IP = 0x0800, ETH_P_IPV6 = 0x86DD
#include <linux/if_packet.h> // struct sockaddr_ll (see man 7 packet)
#include <net/ethernet.h>
#include <arpa/inet.h>   // htons

#include "reader_server.h"
#include "crc32.h"
#include "param.h"
using namespace std;

#define ETH_MLD_TYPE 0xaaaa
#define MAX_LENGTH 1500
#define HEAD_LEN_BYTES (12+2+2)
#define TRUE 1
#define FALSE 0

/*
--------------------- Frame format ----------------------------------
    1Gbe ethernet
-----------------------------------------------------------------------------------------
| Des Addr[47:0]| Sour Addr[47:0] | Type[15:0] | |Length[7:0] | Frame_per_Packet[7:0] |

| Payload ...
-----------------------------------------------------------------------------------------

-----------------------------------------------------------------------------------------

For 40G MAC on FPGA, 802.1Q(0x8100-0000) will be inserted after the mac address, but the reason is not clear.
-----------------------------------------------------------------------------------------
Before the FPGA:
| Des Addr[47:0]| Sour Addr[47:0] | Type[15:0] | Packet_length[15:0]|
|Total_packet[7:0] | Packet_Num[7:0] | Payload ............

After the MAC on FPGA
1> | Des Addr[47:0] | Sour Addr[47:0] | Type[15:0] | 802.1Q |
2> |Packet_length[15:0] || Total_packet[7:0] | Packet_Num[7:0] | Payload ............


| Payload ...
-----------------------------------------------------------------------------------------
*/
struct frame_header
{
    uint8_t       des_mac[6];
    uint8_t       src_mac[6];
    uint16_t       type;
    uint8_t        total_pck;
    uint8_t        pck_id;
    uint16_t       length;
};
void delay();
uint16_t* reader_server(string data_file, int line_num, int buf_size);

int main (void)
{
    uint16_t     src_mac[3], des_mac[3];
    struct      sockaddr_ll device;
    uint16_t     ether_frame[MAX_LENGTH];
    uint16_t    frame_length, bytes;
    const char  *interface = "eth0";
    int    pkt_id;

    // First to creat a socket to acquire the local ethernet inform
    int s = socket(PF_INET, SOCK_RAW, ETH_P_ALL);
    if (s < 0) {
        perror("Failed to create a socket");
        exit(1);
    }

    // Use ioctl() to look up interface name and get its MAC address.
    struct ifreq ifr;
    memset (&ifr, 0, sizeof (ifr));
    snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", interface);

    if (ioctl (s, SIOCGIFHWADDR, &ifr) < 0) {
        perror ("ioctl() failed to get source MAC address ");
        return (EXIT_FAILURE);
    }
    close (s);

    memcpy (src_mac, ifr.ifr_hwaddr.sa_data, 6); // Copy source MAC address.
    memset (&device, 0, sizeof (device));

    if ((device.sll_ifindex = if_nametoindex(interface)) == 0) {
        perror ("if_nametoindex() failed to obtain interface index ");
        exit (EXIT_FAILURE);
    }

    // Set destination MAC
    des_mac[0] = 0x0102;
    des_mac[1] = 0x0304;
    des_mac[2] = 0x0506;

    device.sll_family = AF_PACKET;
    memcpy(device.sll_addr, src_mac, 6);
    device.sll_halen = htons (6);

    for (int i = 0; i < 3; i++) {
                src_mac[i] = htons(src_mac[i]);
    }
    if ((s = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
        perror("Failed to socket");
        exit(EXIT_FAILURE);
    }
    // ------------ Get the file list ---------------------------------------
    struct dirent *ptr;
    DIR *dir;
    string PATH = "/home/longjia1/work/mld/user_app/input";
    dir = opendir(PATH.c_str());
    vector<string> files;
    cout << "File list:" << endl;
    while((ptr=readdir(dir))!= NULL) {
        if (ptr->d_name[0] == '.')
            continue;
        files.push_back(ptr->d_name);
    }

    for (unsigned i = 0; i < files.size(); i++)
        cout << files[i] <<endl;

    closedir(dir);
    // ---------------------------------------------------------------------
    int lines_per_frame = 22 * 2;
    int total_frame = TOTAL_PKT_NUM;
    int frame_per_packet = FRAME_PER_PACKET;
    uint32_t test_times = 1;

    cout << "file size is "<<files.size()<<endl;
    for (unsigned file_index = 0; file_index < files.size(); file_index++) {
         //cout<<"file index is "<<file_index<<endl;
        cout <<"The file is " << files[file_index] <<endl;
         uint16_t *data_buf;
         data_buf = reader_server((PATH + "/" + files[file_index]), lines_per_frame * total_frame, SERVER_DATA_BUF_SIZE);
         //for (int i = 0; i < 32; i++)
        //    printf("data_buf[%d] is %x.\n", i, data_buf[i]);

       // for (int pos = 0; pos < total_frame * lines_per_frame; pos++) {
         //   printf("data_ptr[%d] is %16x.\n", pos, *(data_ptr + pos));
      //  }

        while(test_times > 0) {
           // printf("Time is %d.\n", test_times);
            int offset = 0;

            for (pkt_id = 0; pkt_id < (total_frame / frame_per_packet); pkt_id++) {
                //uint16_t pkt_id = data_ptr[offset] & 0x00ff;
                //uint16_t payload_length = data_ptr[offset+1];
                //uint16_t payload_length = lines_per_frame * frame_num * 4; // Bytes
                cout << "pkt id ."<<pkt_id<<endl;
                uint16_t payload_length = 22 * 4 * frame_per_packet; // Bytes

                //Fill the Ethernet Frame with header and data
                memcpy(ether_frame, des_mac, 6);
                memcpy(ether_frame + 3, src_mac, 6);

                ether_frame[6] = 0x8106;
                ether_frame[7] = (payload_length << 5) | frame_per_packet ;

                //Copy payload
                //memcpy(ether_frame + 12, &data_ptr[offset+2], payload_length);
                memcpy(ether_frame + 8, &data_buf[offset], payload_length);

                frame_length = payload_length + HEAD_LEN_BYTES;
               // printf("Frame length is %d.\n", frame_length);


                for (uint16_t i = 0; i < (frame_length / 2 + 1); i++) {
                    ether_frame[i] = htons(ether_frame[i]);
                }

                if ((bytes = sendto (s, ether_frame, frame_length, 0, (struct sockaddr *) &device, sizeof (device))) <= 0) {
                    perror ("sendto() failed");
                    exit (EXIT_FAILURE);
                }

                //offset = (pkt_num + 1)*(payload_length+4)/2;
                offset = (pkt_id + 1) * lines_per_frame * frame_per_packet;
               // printf("Offset is %d.\n", offset);
                delay();
            }
            //close(s);
            test_times--;

        }

         delay();
        test_times = 1;
        free(data_buf);
    }

    close(s);
    // close(s);
    return (EXIT_SUCCESS);
}
void delay() {
    int delay_cnt = 10000000;
    while(delay_cnt--);
}

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

    uint16_t *data_buf = (uint16_t *)malloc(sizeof(uint16_t) * buf_size * 2);
    if (data_buf == NULL) {
        printf("Fail to apply memory for data_buf.\n");

    }

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

                if (valid_char_cnt  == 4) {
                   data_buf[index] = sum;

                    //printf("data_buf[%d] is %x, sum is %x.\n", index, data_buf[index], sum);
                    index++;
                     valid_char = FALSE;
                     valid_char_cnt = 0;
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
