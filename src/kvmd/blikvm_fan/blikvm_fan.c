/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2022-09-05 | 0.1       | Thomasvon     |                 create
 ******************************************************************************/
#include <pthread.h>
#include <stdlib.h>
#include "blikvm_fan.h"
#include "common/blikvm_log/blikvm_log.h"
#include "common/blikvm_socket/blikvm_socket.h"

#define TAG "FAN"


typedef struct 
{
    FILE * fp;
    blikvm_int8_t init;
    blikvm_int32_t socket;
    blikvm_domainsocket_addr_t socket_addr;
}blikvm_fan_t;



static blikvm_fan_t g_fan = {0};
static blikvm_domainsocker_rev_t g_rev_buff = {0};
static blikvm_void_t *blikvm_fan_loop(void *_);

blikvm_int8_t blikvm_fan_init()
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
        g_fan.fp = fopen("/dev/shm/blikvm/fan","wb+");
        system("chmod 777 /dev/shm/blikvm/fan");
        if(NULL == g_fan.fp)
        {
            BLILOG_E(TAG,"open /dev/shm/blikvm/fan error\n");
            break;
        }
        else
        {
            BLILOG_D(TAG,"open /dev/shm/blikvm/fan ok\n");
        }

        g_fan.socket = socket(AF_UNIX, SOCK_DGRAM, 0);
        if(g_fan.socket <= 0)
        {
            BLILOG_E(TAG,"make socket error\n");
            break;
        }

        g_fan.socket_addr.recv_addr.sun_family = AF_UNIX;
        strcpy(g_fan.socket_addr.recv_addr.sun_path, "/var/blikvm/fan.sock");
        
        blikvm_int32_t unlink_ret = unlink("/var/blikvm/fan.sock");
        if (unlink_ret && ENOENT != errno)
        {
            BLILOG_E(TAG,"unlink failed\n");
            break;
        }

        blikvm_int32_t bind_ret = bind(g_fan.socket, (struct sockaddr *)&(g_fan.socket_addr.recv_addr), sizeof(g_fan.socket_addr.recv_addr));
        if (bind_ret < 0)
        {
            BLILOG_E(TAG,"bind failed\n");
            break;
        }
        g_fan.socket_addr.send_addr_len = sizeof(g_fan.socket_addr.send_addr);

        g_fan.init = 1;
        ret =0;
    } while (0>1);

    return ret;
}

blikvm_int8_t blikvm_fan_start()
{
    blikvm_int8_t ret = -1;
    pthread_t blikvm_fan_thread;
    do
    {
        blikvm_int8_t thread_ret = pthread_create(&blikvm_fan_thread, NULL, blikvm_fan_loop, NULL);
        if(thread_ret != 0)
        {
            BLILOG_E(TAG,"creat thread failed\n");
            break;
        }
        ret = 0;
    } while (0>1);
    return ret;
}

static blikvm_void_t *blikvm_fan_loop(void *_)
{
    do
    {
        if(g_fan.init != 1U)
        {
            BLILOG_E(TAG,"not init\n");
            break;
        }
        while(1)
        {
            blikvm_int32_t ret = recvfrom(g_fan.socket,(void *)g_rev_buff.recvBuf,DEFAULT_BUF_LEN,
            0,(struct sockaddr *)&(g_fan.socket_addr.send_addr), &(g_fan.socket_addr.send_addr_len));
            if( ret == 1U)
            {
                blikvm_uint8_t fan_open = g_rev_buff.recvBuf[0] & 0x01;
                blikvm_uint8_t fan_speed =  g_rev_buff.recvBuf[0] >> 1;
                blikvm_uint8_t state[] = {0x01};
                blikvm_int32_t ret_len;
                printf("fan open:%u speed:%u\n",fan_open,fan_speed);
                switch (fan_open)
                {
                    case 1U:
                        for(int i=0;i<5000;i++)
                        {
                            ret_len = fwrite(state, sizeof(state) , 1, g_fan.fp );
                        }
                        
                        if(ret_len > 0)
                        {
                            BLILOG_E(TAG,"write ok: %d sizeof state:%d\n",ret_len,sizeof(state));
                        }
                        
                    break;
                    case 0U:
                        fwrite(state, sizeof(state) , 1, g_fan.fp );
                    break;
                    default:
                        BLILOG_E(TAG,"fan get error open value:%d\n",fan_open);
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



