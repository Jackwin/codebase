#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <iomanip>
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
#include <math.h>

#include "reader.h"
#include "param.h"
using namespace std;
#define ETH_MLD_TYPE  0x8866
#define MAX_LENGTH 1500
//#define LLR_BUF_SIZE 4096
#define IP_NUM 12
#define DATA_BUF_SIZE (TOTAL_PKT_NUM * LLR_OUTPUT_SIZE * 2)
//#define LLR_OUTPUT_SIZE 8
//#define TOTAL_PKT_NUM (144)
struct frame_header
{
    uint16_t       des_mac[3];
    uint16_t       src_mac[3];
    uint16_t       type;
    uint8_t        total_pck;
    uint8_t        pck_id;
    uint16_t       length;
    uint16_t       reserve[3];
};
bool verify(uint16_t *a_ptr, uint16_t *b_ptr, uint16_t length);
bool bit_verify(uint16_t *a_ptr, uint16_t *b_ptr, uint16_t length);
void log(string file_name, int pkt_cnt, int inter_ip_result) ;
bool inter_ip_verify(uint16_t *p);
uint16_t swap_int16(uint16_t data) {
    return (((data & 0x00ff) << 8 ) + ((data & 0xff00) >> 8));
}

int main(void) {
    uint16_t    src_mac[3], swap_src_mac[3];
    struct      sockaddr_ll device;
    uint16_t     ether_frame[MAX_LENGTH / 2];
   // uint32_t    frame_length, bytes;

    //uint8_t     total_pkt, pkt_id;
   // uint16_t   reserve[4];
    uint16_t    payload_length;
    uint16_t    payload[MAX_LENGTH / 2];
   // uint16_t    crc[2];
   // uint16_t    ip_id = 0;
    //int         recv_pkt_cnt = 0;
    int         pass_pkt_cnt = 0;
    int         fail_pkt_cnt = 0;
    int         pkt_cnt = 0;
    bool        status;
    int         inter_ip_check_fail = 0;
    const char  *interface = "eth0";

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
    //swap

    for (int i = 0; i < 3; i++) {
        swap_src_mac[i] = ((src_mac[i] & 0x00ff) << 8 ) + ((src_mac[i] & 0xff00) >> 8);
    }

    printf("\e[0;32m Local Hardware MAC is %x:%x:%x\n", swap_src_mac[0], swap_src_mac[1], swap_src_mac[2]);

    device.sll_family = AF_PACKET;
    memcpy(device.sll_addr, src_mac, 6);
    device.sll_halen = htons (6);
    socklen_t sll_len = sizeof(device);

    if ((s = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
        perror("Failed to socket");
        exit(EXIT_FAILURE);
    }
    // Read the result data for comparison
    /*
    uint16_t *llr_buf_ptr = (uint16_t *)malloc(sizeof(uint16_t) * LLR_OUTPUT_SIZE * 2 * TOTAL_PKT_NUM);
    if (llr_buf_ptr == NULL) {
        printf("Fail to apply memory for llr_buf_ptr.\n");
        return 0;
    }
    memset(llr_buf_ptr, 0, sizeof(uint16_t) * LLR_OUTPUT_SIZE * 2 * TOTAL_PKT_NUM);
    */
    //string data_file = "16QAM_D____LLR.txt";
    //string data_file = "64QAM_D____LLR.txt";
    //string sim_file = "256QAM_D____LLR.txt";

     // ------------ Get the file list ---------------------------------------
    struct dirent *ptr;
    DIR *dir;
    string PATH ;
    /*
    = "/home/longjia1/work/mld/user_app/output";
    dir = opendir(PATH.c_str());
    vector<string> output_files;
    cout << "Output File list:" << endl;
    while((ptr=readdir(dir))!= NULL) {
        if (ptr->d_name[0] == '.')
            continue;
        output_files.push_back(ptr->d_name);
    }

    for (unsigned i = 0; i < output_files.size(); i++)
        cout << output_files[i] <<endl;

    closedir(dir);
    */
    PATH = "/home/longjia1/work/mld/user_app/input";
    dir = opendir(PATH.c_str());
    vector<string> in_files;
    cout << "File list:" << endl;
    while((ptr=readdir(dir))!= NULL) {
        if (ptr->d_name[0] == '.')
            continue;
        in_files.push_back(ptr->d_name);
    }

    for (unsigned i = 0; i < in_files.size(); i++)
        cout << in_files[i] <<endl;

    closedir(dir);
    // ----------------------------------------------------------------------
    uint16_t llr_buf_ptr[LLR_OUTPUT_SIZE * 3 * TOTAL_PKT_NUM];
    int line_num = LLR_OUTPUT_SIZE * 2 * TOTAL_PKT_NUM;
/*
    uint16_t *llr_buf_ptr = (uint16_t *)malloc(sizeof(uint16_t) * DATA_BUF_SIZE);

    if (llr_buf_ptr == NULL) {
        printf("Fail to apply memory for llr_buf_ptr.\n");
        return 0;
    }
    memset(llr_buf_ptr, 0, sizeof(uint16_t) * DATA_BUF_SIZE);
*/
/*
    int line_num = LLR_OUTPUT_SIZE * 2 * TOTAL_PKT_NUM;
    reader(llr_sim_result_file, line_num, llr_buf_ptr);

    ofstream fout;
    fout.open(llr_onboard_result_file);
    //fout.open("64QAM_R.txt");
    if (!fout.is_open()) {
        cerr<< "Fail to open file"<<llr_onboard_result_file<<"\n";
        exit(0);
    }
*/
   // string data_file = "16QAM_A____Outside_Input.txt";
   // reader(data_file, 88, llr_buf_ptr);
/*
    int ip_pass_times[IP_NUM], ip_fail_times[IP_NUM];
    for (int i = 0; i < IP_NUM; i++) {
        ip_pass_times[i] = 0;
        ip_fail_times[i] = 0;
    }
    */
    //int ip0_pass_times = 0, ip0_fail_times = 0;
    //int ip1_pass_times = 0, ip1_fail_times = 0;

    PATH = "/home/longjia1/work/mld/user_app/onboard_result";
    for (unsigned file_index = 0; file_index < in_files.size(); file_index++) {
        //reader((PATH + "/" + files[file_index]), line_num, llr_buf_ptr);

        string name_s = PATH + "/" + in_files[file_index];
        ofstream fout;
        fout.open(name_s.c_str());

        if (!fout.is_open()) {
            cerr<< "Fail to open file"<<name_s<<"\n";
            exit(0);
        }


        while (1) {

            memset(ether_frame, 0, sizeof(ether_frame));
            memset(payload, 0 , sizeof(payload));

            ssize_t len = recvfrom(s, ether_frame, sizeof(ether_frame), 0, (struct sockaddr *)&device, &sll_len);

            if (len == -1) {
                printf("Recv Error\n");
                break;
            }
            else if (len == 0) {
                continue;
            }

            if (htons(ether_frame[3]) == 0x0102 && htons(ether_frame[4]) == 0x0304 && htons(ether_frame[5]) == 0x0506) {
             //if (src_mac[0] == 0x0102 && src_mac[1] == 0x0304 &&  src_mac[2] == 0x0506) {
                if (pkt_cnt % 1000 == 0)
                    printf("Received packets are %d.\n", pkt_cnt);

                //printf("Source MAC address is %02x:%02x:%02x\n", htons(ether_frame[3]), htons(ether_frame[4]), htons(ether_frame[5]));
               // printf("Source MAC address is %02x:%02x:%02x\n",src_mac[0], src_mac[1], src_mac[2]);
               // printf("frame 6 is %x, frame 7 is %x\n", htons(ether_frame[6]), htons(ether_frame[7]));
                //payload_length = htons(ether_frame[7]) - 2;;
                payload_length = LLR_OUTPUT_SIZE * 4 * 12 + 2 - 2; // 12 IP x 32-byte/IP

                uint16_t *p = (uint16_t *)malloc(payload_length / 2 * sizeof(uint16_t));
                if (p == NULL) return 0;

                for (int i = 0; i < (payload_length)/2; i++) {
                    *(p + i) = htons(ether_frame[i + 6]);

                }

                status = inter_ip_verify(p);
                if (!status){
                    //printf("Inter IP check fails.\n");
                    inter_ip_check_fail = 1;
                    for (int i = 0; i < LLR_OUTPUT_SIZE * 2 * IP_NUM; i++)
                    {
                        printf("P[%d] is %x.\n", i, p[i]);
                    }
                    return 0;
                }
               // printf("Inter-IP check PASSED.\n");
                fout<<"00000004"<<endl;
                for (int i = 0; i < LLR_OUTPUT_SIZE * 2; i++) {
                    fout<<hex<<setw(4)<<setfill('0')<<p[i];
                    fout<<endl;
                }
                // (LLR_OUTPUT_SIZE + 1) because there is a dump data 0x00000004
/*
                bool pass = bit_verify(p, llr_buf_ptr+ pkt_cnt * (LLR_OUTPUT_SIZE + 1) * 2, LLR_OUTPUT_SIZE * 2);
                if (pass) {
                    pass_pkt_cnt++;
                }
                else {
                    fail_pkt_cnt++;
                }
*/
               // printf("Pass packet number is  %d, fail packet number is  %d .\n", pass_pkt_cnt, fail_pkt_cnt);
                 //printf("\n");
                pkt_cnt++;
                free(p);
                p = NULL;
            }
            if (pkt_cnt == TOTAL_PKT_NUM) {
                log(in_files[file_index], pkt_cnt, inter_ip_check_fail);
                inter_ip_check_fail = 0;
                printf("--------------------- Test Finished -----------------------\n");
                cout << "The input data is from " << in_files[file_index] << endl;
                 if(inter_ip_check_fail == 0) cout << "Inter-IP check PASS!" << endl;
                else cout << "Inter-IP check FAIL!" << endl;
                printf("Total test packets are %d.\n", pkt_cnt);
               // printf("Passed packet number is  %d, and failed packet number is  %d .\n", pass_pkt_cnt, fail_pkt_cnt);
                cout<<endl;
                //free(llr_buf_ptr);
                pkt_cnt = 0;
                 fout.close();
                break;
            }
        }

    }
    close(s);
}

void log(string file_name, int pkt_cnt, int inter_ip_result) {
    ofstream fout;
    fout.open("log.txt", ios::app);

    if (!fout.is_open()) {
        cerr<< "Fail to open log.txt"<<"\n";
        exit(0);
    }
    fout << file_name << endl;
    fout << "The received packet is ";
    fout <<pkt_cnt <<endl;
    if (inter_ip_result == 0)
        fout << "Inter-IP check PASS"<<endl;
    else
        fout << "Inter-IP check FAIL"<<endl;
    fout<<endl;
    fout.close();

}

// LLR output size is 8
bool inter_ip_verify(uint16_t *p) {
    for (int i = 1; i < IP_NUM; i++) {
        for (int k = 0; k < LLR_OUTPUT_SIZE * 2; k++) {
            if (*(p + k) != *(p + LLR_OUTPUT_SIZE * i * 2 + k)) {
                printf("Inter IP check fails.\n");
                return false;
            }
        }
    }
    return true;
}

bool verify(uint16_t *a_ptr, uint16_t *b_ptr, uint16_t length) {
    long diff = 0;
    long ref = 0;


    for (int k = 0; k < 1; k++){
        for (int i = 0; i < length; i++) {
            // Cautious: 2 means there is dump data 0x00000004
            uint16_t d = *(a_ptr  + k * LLR_OUTPUT_SIZE * 2 + i ) - *(b_ptr + 2 + i);
            diff += d * d;
            ref += (*(b_ptr + k * LLR_OUTPUT_SIZE * 2  + 2 + i)) * (*(b_ptr + 2 + i));
        }
    }

    const float diff_l2norm = sqrt(diff);
    const float ref_l2norm = sqrt(abs(ref));
    const float err = diff_l2norm / ref_l2norm;
    const bool pass = err < 1e-6;
    //printf("diff_l2norm is %f, and ref_l2norm is %f, and err is %f\n",diff_l2norm, ref_l2norm, err);
    if (!pass) return false;

    printf("Data ckeck between on-board and simulation PASSED.\n");
    return true;
}

bool bit_verify(uint16_t *a_ptr, uint16_t *b_ptr, uint16_t length) {

    for (int i = 0; i < length; i++) {
        // Cautious: 2 means there is dump data 0x00000004
        uint16_t d = *(a_ptr  + i ) - *(b_ptr + 2 + i);
        if (d != 0) return false;
    }
    return true;

}
