#ifndef __BLIKVM_SOCKET_H__
#define __BLIKVM_SOCKET_H__
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>
/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2022-09-10 | 0.1       | Thomasvon     |                 create
 ******************************************************************************/
#define DEFAULT_BUF_LEN 1024U * 5
#define BLIKVM_DOMAIN_PATH_MAX_LENGTH 30

typedef struct
{
    struct sockaddr_un recv_addr; ///< receive socket struct
    struct sockaddr_un send_addr; ///< send socket struct
    int recv_addr_len;            ///< receive socket struct len
    socklen_t send_addr_len;      ///< send socket struct len
} blikvm_domainsocket_addr_t;

typedef struct 
{
    blikvm_uint8_t recvBuf[DEFAULT_BUF_LEN];         ///< receive data buffer, default length = 1024 * 2
    blikvm_uint8_t leftDataBuf[DEFAULT_BUF_LEN * 2]; ///< transfer data buffer, default length = 1024 * 4
    char mSunPath[BLIKVM_DOMAIN_PATH_MAX_LENGTH];    ///< domain socket adress,default length = 30
}blikvm_domainsocker_rev_t;

 #endif