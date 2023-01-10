/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2022-12-29 | 0.1       | Thmoasvon     |                 create
 ******************************************************************************/

//串口通信
#include <termios.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/select.h>
#include <pthread.h>
#include <string.h>

#include "blikvm_uart.h"

#define TAG "UART"

#define DEFAULT_SERIAL_BAUT (19200)
#define SERIAL_TIME_OUT_MS  (5050)

blikvm_int8_t g_fd = 0;
static blikvm_int32_t set_serial_opt(blikvm_int32_t fd, blikvm_int32_t nSpeed, blikvm_int32_t nBits, blikvm_int8_t nEvent, blikvm_int32_t nStop);

/**
 * @brief open serial device and set baut
 * @param serial_dev serial device in linux host
 * @return 0: success; -1: fail
 */
blikvm_int32_t open_serial_dev(const blikvm_int8_t* serial_dev, blikvm_int32_t baut)
{
    BLILOG_I(TAG,"opening device :%s \n", serial_dev);

    blikvm_int32_t fd =0;

    /* O_RDWR Read/Write access to serial port           */
    /* O_NOCTTY - No terminal will control the process   */
    /* O_NDELAY -Non Blocking Mode,Does not care about-  */
    /* -the status of DCD line,Open() returns immediatly */
    fd = open(serial_dev, O_RDWR | O_NONBLOCK | O_NOCTTY);  // open serial
    if (fd == -1) {
        BLILOG_E(TAG,"open :%s failed \n", serial_dev);
        return -1;
    }
    else {
        BLILOG_I(TAG,"communicate baut is %d \n", baut);
        blikvm_int32_t set_ret = set_serial_opt(fd, baut, 8, 'N', 1);  // CHANG BUAD
        if (set_ret == -1) {
            BLILOG_E(TAG,"failed to change baut:%d \n", baut);
            return -1;
        }
        BLILOG_I(TAG,"set uart ok\n");
    }

    return fd;
}

/**
 *  change baut rate and set flags.
 */
static blikvm_int32_t
set_serial_opt(blikvm_int32_t fd, blikvm_int32_t nSpeed, blikvm_int32_t nBits, blikvm_int8_t nEvent, blikvm_int32_t nStop)
{
    struct termios newtio = { 0 };

    newtio.c_cflag |= CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;
    newtio.c_iflag &= ~(IXON | IXOFF | IXANY | ICRNL | INLCR | IGNCR);  // for linux

    switch (nBits) {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    }

    switch (nEvent) {
    case 'O':
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'E':
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'N':
        newtio.c_cflag &= ~PARENB;
        break;
    }

    switch (nSpeed) {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    case 19200:
        cfsetispeed(&newtio, B19200);
        cfsetospeed(&newtio, B19200);
        break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    case 230400:
        cfsetispeed(&newtio, B230400);
        cfsetospeed(&newtio, B230400);
        break;
    case 460800:
        cfsetispeed(&newtio, B460800);
        cfsetospeed(&newtio, B460800);
        break;

    default:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    }

    if (nStop == 1) {
        newtio.c_cflag &= ~CSTOPB;
    }
    else if (nStop == 2) {
        newtio.c_cflag |= CSTOPB;
    }
    newtio.c_cc[VTIME] = 0;
    newtio.c_cc[VMIN]  = 1;

    newtio.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);  // for linux
    newtio.c_oflag &= ~OPOST;                                     /*No Output Processing*/

    tcflush(fd, TCIOFLUSH);
    if ((tcsetattr(fd, TCSANOW, &newtio)) != 0) {
        BLILOG_E(TAG,"com set error\n");
        return -1;
    }

    return 0;
}