/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2022-09-10 | 0.1       | Thomasvon     |                 create
 ******************************************************************************/
#include <pthread.h>
#include <stdlib.h>
#include "blikvm_atx.h" 
#include "common/blikvm_socket/blikvm_socket.h"

#define TAG "ATX"

typedef struct 
{
    FILE * fp;
    blikvm_int8_t init;
    blikvm_int32_t socket;
    blikvm_domainsocket_addr_t socket_addr;
}blikvm_atx_t;

typedef enum
{
    ATX_SHORT = 128,
    ATX_LONG = 192,
    ATX_RESET = 8,
}blikvm_atx_command_t;

static blikvm_atx_t g_atx = {0};
static blikvm_domainsocker_rev_t g_rev_buff = {0};
static blikvm_void_t *blikvm_atx_loop(void *_);

blikvm_int8_t blikvm_atx_init()
{
    blikvm_int8_t ret = -1;
    do
    {
        if(access("/dev/shm/blikvm/",R_OK) != 0)
        {
            BLILOG_E(TAG,"not exit /dev/shm/blikvm/ will creat this dir\n");
            system("mkdir /dev/shm/blikvm/");
            BLILOG_E(TAG,"creat /dev/shm/blikvm/ ok\n");
        }
        system("chmod 777 /dev/shm/blikvm/");
        g_atx.fp = fopen("/dev/shm/blikvm/atx","w+");
        if(NULL == g_atx.fp)
        {
            printf("open atx shm failed\n");
            BLILOG_E(TAG,"open /dev/shm/blikvm/atx error\n");
            break;
        }
        else
        {
            printf("open atx shm success\n");
            BLILOG_D(TAG,"open /dev/shm/blikvm/atx ok\n");
        }

        g_atx.socket = socket(AF_UNIX, SOCK_DGRAM, 0);
        if(g_atx.socket <= 0)
        {
            BLILOG_E(TAG,"make socket error\n");
            break;
        }

        g_atx.socket_addr.recv_addr.sun_family = AF_UNIX;
        strcpy(g_atx.socket_addr.recv_addr.sun_path, "/var/blikvm/atx.sock");
        
        blikvm_int32_t unlink_ret = unlink("/var/blikvm/atx.sock");
        if (unlink_ret && ENOENT != errno)
        {
            BLILOG_E(TAG,"unlink failed\n");
            break;
        }

        blikvm_int32_t bind_ret = bind(g_atx.socket, (struct sockaddr *)&(g_atx.socket_addr.recv_addr), sizeof(g_atx.socket_addr.recv_addr));
        if (bind_ret < 0)
        {
            BLILOG_E(TAG,"bind failed\n");
            break;
        }
        g_atx.socket_addr.send_addr_len = sizeof(g_atx.socket_addr.send_addr);

        g_atx.init = 1;
        ret =0;
        ret =0;
    } while (0>1);
    return ret;
}


blikvm_int8_t blikvm_atx_start()
{
    blikvm_int8_t ret = -1;
    pthread_t blikvm_atx_thread;
    do
    {
        blikvm_int8_t thread_ret = pthread_create(&blikvm_atx_thread, NULL, blikvm_atx_loop, NULL);
        if(thread_ret != 0)
        {
            BLILOG_E(TAG,"creat thread failed\n");
            break;
        }
        ret = 0;
    } while (0>1);
    return ret;
}

static blikvm_void_t *blikvm_atx_loop(void *_)
{
    do
    {
        if(g_atx.init != 1U)
        {
            BLILOG_E(TAG,"not init\n");
            break;
        }
        while(1)
        {
            printf("test atx thread\n");
            blikvm_int32_t ret = recvfrom(g_atx.socket,(void *)g_rev_buff.recvBuf,DEFAULT_BUF_LEN,
            0,(struct sockaddr *)&(g_atx.socket_addr.send_addr), &(g_atx.socket_addr.send_addr_len));
            if( ret == 1U)
            {
                switch(g_rev_buff.recvBuf[0])
                {
                    case int(ATX_SHORT):
                        BLILOG_D(TAG,"atx short\n");
                        break;
                    case int(ATX_LONG):
                        BLILOG_D(TAG,"atx long\n");
                        break;
                    case int(ATX_RESET):
                        BLILOG_D(TAG,"atx reset\n");
                        break;
                    default:
                        BLILOG_E(TAG,"atx get error command:%d\n",g_rev_buff.recvBuf[0]);
                        break;
                }
            }
            else
            {
                BLILOG_E(TAG,"recv error len:%d\n",ret);
            }
        }
    } while (0>1);
    return NULL;
}