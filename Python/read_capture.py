import socket

ip = '192.168.1.1'
reg = 0
def Read(ip, addr):
    sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM,0)
    sock.settimeout(3)
    try:
        sock.connect((ip, 4001))
    except:
        print "Connect error"
        return 0
    #sock.send('\x02\x02\x0a%c%c%c\x02\x00'%(chr(addr>>(1+16)&0xff),chr((addr>>1)&0xff), chr(addr>>(1+8)&0xff) ) )
    sock.send('\x02\x02\x0a%c%c%c\x02\x00'%(chr(addr>>(16)&0xff),chr((addr)&0xff), chr(addr>>(8)&0xff) ) )
    ret = sock.recv(3)
    sock.close()
    if(ret[0]!='\x00'):
        print "Error"
        return None
    else:
        reg = ord(ret[1]) + ord(ret[2])*256
        #print ("#0x%x Value is %xh#\t"%(addr&0xfff,reg)),
        return reg

def Write(ip, addr, reg):
    sock = socket.socket(socket.AF_INET,socket.SOCK_STREAM,0)
    sock.settimeout(3)
    sock.connect((ip, 4001))
    #sock.send('\x04\x02\x0a%c%c%c\x02\x00%c%c'%(chr(addr>>(1+16)&0xff),chr((addr>>1)&0xff), chr(addr>>(1+8)&0xff), chr(reg&0xff), chr(reg>>8&0xff) ) )
    sock.send('\x04\x02\x0a%c%c%c\x02\x00%c%c'%(chr(addr>>(16)&0xff),chr((addr)&0xff), chr(addr>>(8)&0xff), chr(reg&0xff), chr(reg>>8&0xff) ) )
    ret = sock.recv(1)
    if(ret[0]!='\x00'):
        print "Error"
    else:
        print "Success"

    sock.close()

#------------------------------------------------------

# Analyze the TLP package according to the header
def TLPAnalyzer(time_stamp, tlp_data):
    log = [0]
    FMT_MASK = 0b111 << 125
    TYPE_MASK = 0b11111 << 120
    TC_MASK = 0b111 << 116
    LEN_MASK = 0x3ff << 96
    REQ_ID_MASK = 0xff << 80
    ADDR_H = 0xffffffff << 32
    ADDR_L = 0xffffffff

    fmt = (tlp_data & FMT_MASK) >> 125
    type1 = (tlp_data & TYPE_MASK) >> 120
    tc = (tlp_data & TC_MASK) >> 116
    length = (tlp_data & LEN_MASK) >> 96
    req_id = (tlp_data & REQ_ID_MASK) >> 80
    addr_h = (tlp_data & ADDR_H) >> 32
    addr_l = tlp_data & ADDR_L

    log[0] = time_stamp

    if (fmt == 0b000  and type1 == 0b00000):
        addr = addr_h
        log.insert(1,'MRD(32)')
    elif (fmt == 0b001 and type1 == 0b00000):
        addr = (addr_h << 32) | addr_l
        log.insert(1,'MRD(64)')
    elif (fmt == 0b000  and type1 == 0b00001):
        addr = addr_h
        log.insert(1,'MRDL(32)')
    elif (fmt == 0b001 and type1 == 0b00001):
        addr = (addr_h << 32) | addr_l
        log.insert(1,'MRDL(64)')
    elif (fmt == 0b010  and type1 == 0b00000):
        addr = addr_h
        log.insert(1, 'MWR(32)')
    elif (fmt == 0b011 and type1 == 0b00000):
        addr = (addr_h << 32) | addr_l
        log.insert(1, 'MWR(64)')
    elif (fmt == 0b000 and type1 == 0b00100):
        addr = 0
        log.insert(1,'CFGRD0')
    elif (fmt == 0b010 and type1 == 0b00100):
        addr = 0
        log.insert(1,'CFGWT0')
    elif (fmt == 0b000 and type1 == 0b00101):
        addr = 0
        log.insert(1,'CFGWRD1')
    elif (fmt == 0b010 and type1 == 0b00101):
        addr = 0
        log.insert(1,'CFGWT1')
    elif (fmt == 0b000 and type1 == 0b01010):
        addr = 0
        log.insert(1,'CPL')
    elif (fmt == 0b010 and type1 == 0b01011):
        addr = 0
        log.insert(1,'CPLD')
    else:
        descript = 'Unkown type'
        log.insert(1,'UKWN')

    log.insert(2, tc)
    log.insert(3, length)
    log.insert(4, req_id)
    log.insert(5,addr)

# time packet_type TC LENTH ADDR
    width = 80
    time_width = 6
    fmt_width = 4
    type_width = 2
    tc_width = 2
    len_width = 6
    addr_width = 2
    id_width = 2

    descript_width = 30
    header_format = '%-*s%*s%*s%*s%*s%*s'

    log_format = "{:^14x}{:^9}{:^3x}{:^10x}{:^9x}{:^18x}"
    print '='*width
    print header_format %(time_width, '|    Time    |',type_width,'  TYPE  |', tc_width,'TC|', len_width, '   LEN   |', id_width, ' REQ ID |', addr_width, '       ADDR       |')
    print(log_format.format(log[0], log[1], log[2], log[3], log[4], log[5]))
    #print '-' * width
   # print log_format % (time_width, log[0], (type_width+10), log[1], tc_width, log[2], len_width, log[3], (id_width+6), log[4], addr_width+8, log[5])
    print '='*width



# Current index is stored at the address 126 and the data width is 256-bit
# For 32-bit address in backdoor, its address is 126*8=0x3F0
# As the current index ranges from 0 to 125, only read from the address 0x3f0
# Parameters:
#      capture_type: 0 -> tx capture buffer 1-> rx capture buffer
def ReadCurrentIndex(capture_type):
    base_addr = 0x3f0;

    if (capture_type == 0):
        reg = Read('192.168.1.1', (0x2000 + base_addr))
        print 'reg is %d.' %(reg)
        if (reg < 2):
            reg = 0
        else:
            reg = reg - 2
        print 'The current index in tx cpture points to 0x%x.' %(reg)
        return reg
    elif (capture_type == 1):

        reg = Read('192.168.1.1', (0x3000 + base_addr))
        if (reg < 2):
            reg = 0
        else:
            reg = reg - 2
        print 'The current index in rx cpture points to 0x%x.' %(reg)
        return reg
    else:
        print 'Invalid input parameters'
        return 0


# parameter:
#    1. capture_type: 0 -> tx capture buffer 1-> rx capture buffer
#    2. idnex: the index in capture buffer, range from 0 to 125
def ReadCaptureBuffer(capture_type, index):
    reg = 0
    time_stamp = 0
    if (capture_type == 0):
        base_addr = 0x2000
    elif (capture_type == 1):
        base_addr = 0x3000
    else:
        print 'Whoops........!! Invalid input parameters'
        return 0
    for k in range (0, 8):
        addr = base_addr + index * 8 + k
        reg = reg | (Read('192.168.1.1', addr) << (16 * k))

    for k in range (0, 8):
        addr = base_addr + (index + 1) * 8 + k
        time_stamp = time_stamp | (Read('192.168.1.1', addr) << (16 * k))


    if (capture_type == 0):
        print 'The data from TX capture buffer[%x] is 0x%x' %(index, reg)
        #print 'The time stamp is %d.' %(time_stamp)
    else:
        print 'The data from RX capture buffer[%x] is 0x%x' %(index, reg)
        #print 'The time stamp is %d' %(time_stamp)
    TLPAnalyzer(time_stamp,reg)




#tx_current_index = ReadCurrentIndex(0)
#rx_current_index = ReadCurrentIndex(1)

##ReadCaptureBuffer(0, tx_current_index)
#ReadCaptureBuffer(1, rx_current_index)
#print 'Read valid TX capture buffer'
#ReadCaptureBuffer(0, tx_current_index+2)

#print 'Read valid RX capture buffer'
#ReadCaptureBuffer(1, rx_current_index+2)
TLPAnalyzer(128984085,0x20000058000000000000000000000000)


