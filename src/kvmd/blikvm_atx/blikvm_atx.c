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

#include "wiringPi.h"     //添加库文件
#include "softPwm.h"     //添加库文件

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
static blikvm_int8_t blikvm_read_atx_state();

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
        g_atx.fp = fopen("/dev/shm/blikvm/atx","wb+");
        system("chmod 777 /dev/shm/blikvm/atx");

        if(access("/var/blikvm/",R_OK) != 0)
        {
            BLILOG_E(TAG,"not exit /var/blikvm/ will creat this dir\n");
            system("mkdir /var/blikvm/");
            BLILOG_I(TAG,"creat /var/blikvm/ ok\n");
        }

        if(NULL == g_atx.fp)
        {
            printf("open atx shm failed\n");
            BLILOG_E(TAG,"open /dev/shm/blikvm/atx error\n");
            break;
        }
        else
        {
            blikvm_int8_t state[1];
            state[0] = blikvm_read_atx_state();
            fwrite(state, sizeof(state) , 1, g_atx.fp );
            fflush(g_atx.fp);
            fclose(g_atx.fp);
            BLILOG_D(TAG,"open /dev/shm/blikvm/atx ok state:%c\n",&state[0]);
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

        pinMode(4, OUTPUT); // wpi
        pinMode(2, OUTPUT); //wpi
        g_atx.init = 1;
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
            blikvm_int32_t ret = recvfrom(g_atx.socket,(void *)g_rev_buff.recvBuf,DEFAULT_BUF_LEN,
            0,(struct sockaddr *)&(g_atx.socket_addr.send_addr), &(g_atx.socket_addr.send_addr_len));
            
            blikvm_uint8_t state[1];
            if( ret == 1U)
            {
                g_atx.fp = fopen("/dev/shm/blikvm/atx","wb+");
                blikvm_int32_t ret_len;
                BLILOG_D(TAG,"atx get:%d\n",g_rev_buff.recvBuf[0]);
                switch(g_rev_buff.recvBuf[0])
                {
                    case blikvm_int32_t(ATX_SHORT):
                        digitalWrite(4, HIGH); 
                        usleep(500*1000);
		                digitalWrite(4, LOW);
                        BLILOG_D(TAG,"atx power on\n");
                        state[0] = 0b01000000 | blikvm_read_atx_state(); 
                        break;
                    case blikvm_int32_t(ATX_LONG):
                        digitalWrite(4, HIGH); 
                        usleep(2000*1000);
		                digitalWrite(4, LOW);
                        BLILOG_D(TAG,"atx power off\n");
                        state[0] = 0b00000000 | blikvm_read_atx_state(); 
                        break;
                    case blikvm_int32_t(ATX_RESET):
                        digitalWrite(16, HIGH); 
                        usleep(500*1000);
		                digitalWrite(2, LOW);
                        state[0] = 0b01000000 | blikvm_read_atx_state(); 
                        BLILOG_D(TAG,"atx restart\n");
                        break;
                    default:
                        BLILOG_E(TAG,"atx get error command:%d\n",g_rev_buff.recvBuf[0]);
                        break;
                }

                ret_len = fwrite(state, sizeof(state) , 1, g_atx.fp);
                if(ret_len > 0)
                {
                    BLILOG_E(TAG,"write ok: %d sizeof state:%d\n",ret_len,sizeof(state));
                }
                fflush(g_atx.fp);
                fclose(g_atx.fp);

            }
            else
            {
                BLILOG_E(TAG,"recv error len:%d\n",ret);
            }

        }
    } while (0>1);
    return NULL;
}

static blikvm_int8_t blikvm_read_atx_state()
{
    blikvm_int8_t ret = 0;

    do
    {
    // led_pwr = os.popen('gpio -g read 24').read()
    // led_hdd = os.popen('gpio -g read 22').read()
        FILE  *fp;
        fp = popen("gpio -g read 24","r");
        blikvm_int8_t result_buf[1024];
        if( fp == NULL)
        {
            BLILOG_E(TAG,"read pwr led gpio error\n");
            break;
        }
        blikvm_int32_t len = fread(result_buf,1,sizeof(result_buf),fp);

        if( len > 0)
        {
            blikvm_int32_t gpio = atoi(result_buf);
            if( gpio == 1 )
            {
                ret = 0b10000000;
            }   
        }
        else
        {
            BLILOG_E(TAG,"read pwr led buff error len:%d  conten:%s c0:%d c1:%d\n",len,result_buf,result_buf[0],result_buf[1]);
            break;
        }
        //关闭文件指针
        if(-1 == pclose(fp))
        {
            BLILOG_E(TAG,"close file point failure.\n");
            ret = 0;
            break;
        }

        fp = popen("gpio -g read 22","r");
        if( fp == NULL)
        {
            BLILOG_E(TAG,"read hdd led gpio error\n");
            break;
        }
        if(fread(result_buf,1,sizeof(result_buf),fp) > 0)
        {
            blikvm_int32_t gpio_hdd = atoi(result_buf);
            if( gpio_hdd == 1 )
            {
                ret = ret | 0b00001000;
            }   
        }
        else
        {
            BLILOG_E(TAG,"read hdd led buff error\n");
            break;
        }
        //关闭文件指针
        if(-1 == pclose(fp))
        {
            BLILOG_E(TAG,"close file point failure.\n");
            ret = 0;
            break;
        }

    } while (0>1);
    BLILOG_D(TAG,"atx state ret:%d\n",ret);
    return ret;
}