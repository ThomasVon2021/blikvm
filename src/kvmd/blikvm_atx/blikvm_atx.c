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

#include "GPIO/armbianio.h"     //添加库文件
#include "GPIO/softPwm.h"     //添加库文件

#define TAG "ATX"

#ifdef  RPI
#define PIN_POWER 16  //BCM23 
#define PIN_RESET 13  //BCM27
#define PIN_LED_PWR 18  //BCM24 
#define PIN_LED_HDD 15  //BCM22
#endif

#ifdef  H616
#define PIN_POWER 18   //GPIO 228
#define PIN_RESET 37   //GPIO 272
#define PIN_LED_PWR 36 //GPIO 234   
#define PIN_LED_HDD 41  //GPIO 233
#endif

#define ATX_CYCLE 500 //unit:ms


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
static blikvm_void_t *blikvm_atx_monitor(void *_);
static blikvm_uint8_t blikvm_read_atx_state();
static blikvm_int32_t g_atx_power_on_delay = 500;      //unit:ms
static blikvm_int32_t g_atx_power_off_delay = 3000;    //unit:ms

blikvm_int8_t blikvm_atx_init()
{
    blikvm_int8_t ret = -1;
    do
    {
        AIOAddGPIO(PIN_POWER, GPIO_OUT);
        AIOAddGPIO(PIN_RESET, GPIO_OUT);
        AIOAddGPIO(PIN_LED_PWR, GPIO_IN);
        AIOAddGPIO(PIN_LED_HDD, GPIO_IN);
        AIOWriteGPIO(PIN_POWER, GPIO_LOW);
        AIOWriteGPIO(PIN_RESET, GPIO_LOW);
 
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
            BLILOG_E(TAG,"open /dev/shm/blikvm/atx error\n");
            break;
        }
        else
        {
            blikvm_uint8_t state[1];
            state[0] = 0U;
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
        g_atx.init = 1;
        ret =0;
    } while (0>1);
    return ret;
}


blikvm_int8_t blikvm_atx_start()
{
    blikvm_int8_t ret = -1;
    pthread_t blikvm_atx_thread;
    pthread_t blikvm_monitor_thread;
    do
    {
        if(g_atx.init != 1U)
        {
            BLILOG_E(TAG,"not init\n");
            break;
        }
        blikvm_int8_t thread_ret = pthread_create(&blikvm_atx_thread, NULL, blikvm_atx_loop, NULL);
        if(thread_ret != 0)
        {
            BLILOG_E(TAG,"creat loop thread failed\n");
            break;
        }
        ret =  pthread_create(&blikvm_monitor_thread, NULL, blikvm_atx_monitor, NULL);
        if(thread_ret != 0)
        {
            BLILOG_E(TAG,"creat monitor thread failed\n");
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
            
            if( ret == 1U)
            {
                BLILOG_D(TAG,"atx get:%d\n",g_rev_buff.recvBuf[0]);
                switch(g_rev_buff.recvBuf[0])
                {
                    case ATX_SHORT:
                        AIOWriteGPIO(PIN_POWER, GPIO_HIGH);
                        usleep(g_atx_power_on_delay*1000);
                        AIOWriteGPIO(PIN_POWER, GPIO_LOW);
                        BLILOG_D(TAG,"atx power on\n");
                        break;
                    case ATX_LONG:
                        AIOWriteGPIO(PIN_POWER, GPIO_HIGH);
                        usleep(g_atx_power_off_delay*1000);
		                AIOWriteGPIO(PIN_POWER, GPIO_LOW);
                        BLILOG_D(TAG,"atx power off\n");
                        break;
                    case ATX_RESET:
                        AIOWriteGPIO(PIN_RESET, GPIO_HIGH);
                        usleep(500*1000);
                        AIOWriteGPIO(PIN_RESET, GPIO_LOW);
                        BLILOG_D(TAG,"atx restart\n");
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

static blikvm_void_t *blikvm_atx_monitor(void *_)
{
    blikvm_uint8_t state[1];
    state[0] = 0U;
    blikvm_uint8_t last_state[1];
    last_state[0] = 0U;
    do
    {      
        state[0] = blikvm_read_atx_state();
        if( state[0] != last_state[0])
        {
            g_atx.fp = fopen("/dev/shm/blikvm/atx","wb+");
            if(NULL == g_atx.fp)
            {
                BLILOG_E(TAG,"g_atx.fp is null\n");
                break;
            }
            blikvm_int32_t ret_len = fwrite(state, sizeof(state) , 1, g_atx.fp);
            if(ret_len < 0)
            {
                BLILOG_E(TAG,"write error: %d sizeof state:%d\n",ret_len,sizeof(state));
            }
            fflush(g_atx.fp);
            fclose(g_atx.fp);
            last_state[0] = state[0];
        }

        usleep(ATX_CYCLE*1000);
    }while(1);
    return NULL;
}

static blikvm_uint8_t blikvm_read_atx_state()
{
    blikvm_uint8_t ret = 0;

    do
    {
        if(AIOReadGPIO(PIN_LED_PWR) == 1 )
        {
            ret = 0b01000000;
        }
        if(AIOReadGPIO(PIN_LED_HDD) == 1 )
        {
            ret = ret | 0b00001000;
        }

    } while (0>1);
    //BLILOG_D(TAG,"atx state ret:%02X\n",ret);
    return ret;
}

blikvm_int8_t blikvm_atx_test()
{
    blikvm_uint8_t ret = -1;
    do
    {
        if(g_atx.init != 1U)
        {
            BLILOG_E(TAG,"not init\n");
            break;
        }
        AIOWriteGPIO(PIN_POWER, GPIO_LOW);
        AIOWriteGPIO(PIN_RESET, GPIO_LOW);
        usleep(10*1000);
        blikvm_int32_t value1 = AIOReadGPIO(PIN_LED_PWR);
        blikvm_int32_t value2 =  AIOReadGPIO(PIN_LED_HDD);
        if(value1 != GPIO_LOW ||  value2 != GPIO_LOW)
        {
            BLILOG_E(TAG,"read gpio is not low %d %d\n",value1,value2);
            break;
        }
        AIOWriteGPIO(PIN_POWER, GPIO_HIGH);
        AIOWriteGPIO(PIN_RESET, GPIO_HIGH);
        usleep(10*1000);
        value1 = AIOReadGPIO(PIN_LED_PWR);
        value2 = AIOReadGPIO(PIN_LED_HDD);
        if(value1 != GPIO_HIGH || value2 != GPIO_HIGH)
        {
            BLILOG_E(TAG,"read gpio is not high %d %d\n", value1,value2);
            break;
        }
        ret = 0;
        
    }while(0>1);
    return ret;
}

blikvm_int8_t blikvm_atx_set_power_on_delay(blikvm_int32_t time)
{
    if(time > 0)
    {
        g_atx_power_on_delay = time;
    }
    return 0;
}

blikvm_int8_t blikvm_atx_set_power_off_delay(blikvm_int32_t time)
{
    if(time > 0)
    {
        g_atx_power_off_delay = time;
    }
    return 0;
}