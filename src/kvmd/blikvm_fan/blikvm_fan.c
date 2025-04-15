/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2022-09-05 | 0.1       | Thomasvon     |                 create
 ******************************************************************************/
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>   
#include "softPwm.h"    

#include "blikvm_fan.h"
#include "common/blikvm_log/blikvm_log.h"
#include "common/blikvm_socket/blikvm_socket.h"
#include "config/blikvm_config.h"

#define TAG "FAN"
#define TEMP_PATH "/sys/class/thermal/thermal_zone0/temp"
#define MAX_SIZE 32

#ifdef  RPI
#define FAN_PIN 32  // BCM
#endif

#ifdef  H616
#define FAN_PIN 15  // BCM269
#endif

#define TEMP_LEFT_LIMIT 60  //C
#define TEMP_RIGHT_LIMIT 65  //C 

typedef struct 
{
    FILE * fp;
    blikvm_int8_t init;
    blikvm_int32_t socket;
    blikvm_domainsocket_addr_t socket_addr;
    blikvm_int32_t left_temp;
    blikvm_int32_t right_temp;
}blikvm_fan_t;

static blikvm_fan_t g_fan = {0};
//static blikvm_domainsocker_rev_t g_rev_buff = {0};
static blikvm_void_t *blikvm_fan_loop(void *_);
static blikvm_void_t *blikvm_fan_monitor(void *_);
static blikvm_int32_t GetDuty(blikvm_int32_t temp);
blikvm_uint32_t Duty = 0;

blikvm_int8_t blikvm_fan_init()
{
    blikvm_int8_t ret = -1;
    do
    {
        if(access("/dev/shm/blikvm/",R_OK) != 0)
        {
            BLILOG_E(TAG,"not exit /dev/shm/blikvm/ will creat this dir\n");
            system("mkdir /dev/shm/blikvm/");
            BLILOG_I(TAG,"creat /dev/shm/blikvm/ ok\n");
        }
        system("chmod 777 /dev/shm/blikvm/");
        g_fan.fp = fopen("/dev/shm/blikvm/fan","wb+");
        system("chmod 777 /dev/shm/blikvm/fan");

        if(access("/var/blikvm/",R_OK) != 0)
        {
            BLILOG_E(TAG,"not exit /var/blikvm/ will creat this dir\n");
            system("mkdir /var/blikvm/");
            BLILOG_I(TAG,"creat /var/blikvm/ ok\n");
        }
        

        if(NULL == g_fan.fp)
        {
            BLILOG_E(TAG,"open /dev/shm/blikvm/fan error\n");
            break;
        }
        else
        {
            blikvm_uint8_t state[] = {0b00000000};
            fwrite(state, sizeof(state) , 1, g_fan.fp );
            fflush(g_fan.fp);
            fclose(g_fan.fp);
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

        //init gpio control
        softPwmCreate(FAN_PIN,0,100); //当前pwmRange为100，频率为100Hz，若pwmRange为50时，频率为200，若pwmRange为2时，频率为5000。
	    softPwmWrite(FAN_PIN,50); //占空比 = value/pwmRange，当前占空比 = 50/100 = 50%

        blikvm_config_t *config = blikvm_get_config();
        if(config->fan.threshold > 0)
        {
            g_fan.left_temp = config->fan.threshold - 5;
            g_fan.right_temp = config->fan.threshold;
        }
        else
        {
            g_fan.left_temp = TEMP_LEFT_LIMIT;
            g_fan.right_temp = TEMP_RIGHT_LIMIT;
        }

        g_fan.init = 1;
        ret =0;
    } while (0>1);

    return ret;
}

blikvm_int8_t blikvm_fan_start()
{
    blikvm_int8_t ret = -1;
    pthread_t blikvm_fan_thread;
    pthread_t blikvm_fan_thread_monitor;
    do
    {
        if(g_fan.init != 1)
        {
            BLILOG_E(TAG,"not init\n");
            break;
        }
        blikvm_int8_t thread_ret = pthread_create(&blikvm_fan_thread, NULL, blikvm_fan_loop, NULL);
        if(thread_ret != 0)
        {
            BLILOG_E(TAG,"creat fan loop thread failed\n");
            break;
        }
        thread_ret = pthread_create(&blikvm_fan_thread_monitor, NULL, blikvm_fan_monitor, NULL);
        if(thread_ret != 0)
        {
            BLILOG_E(TAG,"creat fan loop thread failed\n");
            break;
        }
        ret = 0;
    } while (0>1);
    return ret;
}

static blikvm_void_t *blikvm_fan_monitor(void *_)
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
            
            usleep(1000*1000); // sleep 1s
        }
    } while (0>1);
    return NULL;
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
        blikvm_int8_t buf[MAX_SIZE];
        blikvm_float32_t temp = 0;
        blikvm_int8_t fan_enable = 0;
        blikvm_uint8_t state[1];
        state[0] = 0U;
        blikvm_uint8_t last_state[1];
        last_state[0] = 0U;
        while(1)
        {

            // 打开/sys/class/thermal/thermal_zone0/temp
            int fd = open(TEMP_PATH, O_RDONLY);
            if (fd < 0) {
                BLILOG_E(TAG, "failed to open thermal_zone0/temp\n");
                break;
            }
        
            // 读取内容
            if (read(fd, buf, MAX_SIZE) < 0) {
                BLILOG_E(TAG, "failed to read temp\n");
                break;
            }
        
            // 转换为浮点数打印
            temp = atoi(buf) / 1000.0;
        
            // 关闭文件
            close(fd);

            if(temp > g_fan.right_temp || (temp > g_fan.left_temp && fan_enable == 1))
            {
                fan_enable = 1;
                Duty = GetDuty(temp);
                softPwmWrite(FAN_PIN, Duty);
                BLILOG_D(TAG,"temp: %.2f,Duty: %d\n", temp, Duty);
            }
            else
            {
                fan_enable = 0; 
                softPwmWrite(FAN_PIN,0);
            }
          
            switch (fan_enable)
            {
                case 1U:                 
                    state[0] = 0b10000000;                        
                break;
                case 0U:
                    state[0] = 0b00000000;
                break;
                default:
                    BLILOG_E(TAG,"fan get error open value:%d\n",fan_enable);
                break;
            }
            if(state[0] != last_state[0])
            {
                g_fan.fp = fopen("/dev/shm/blikvm/fan","wb+");
                blikvm_int32_t ret_len;
                ret_len = fwrite(state, sizeof(state) , 1, g_fan.fp);
                if(ret_len > 0)
                {
                    BLILOG_D(TAG,"write ok: %d sizeof state:%d\n",ret_len,sizeof(state));
                }
                fflush(g_fan.fp);
                fclose(g_fan.fp);
                last_state[0] = state[0];
            }
                
            sleep(10);
        }
    } while (0>1);
    return NULL;
}

static blikvm_int32_t GetDuty(blikvm_int32_t temp)
{
    if(temp >= 60)
    {
        return 100;
    }
    else if(temp >= 55)
    {
        return 90;
    }
    else if(temp >= 53)
    {
        return 80;
    }
    else if(temp >= 50)
    {
        return 70;
    }
    else if(temp >= 45)
    {
        return 60;
    }
    else
    {
        return 50;
    }
}



