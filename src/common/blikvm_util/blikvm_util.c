/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2022-12-29 | 0.1       | Thmoasvon     |                 create
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

#include "blikvm_util.h"

#define MAX_BUFFER_SIZE 1024

static blikvm_int8_t *pi4b_board = "Raspberry Pi 4 Model B";
static blikvm_int8_t *cm4b_board = "Raspberry Pi Compute Module 4";
static blikvm_int8_t *h616_board = "Mango Pi Mcore";

blikvm_int32_t execmd(blikvm_int8_t* cmd, blikvm_int8_t* result) 
{
    blikvm_int8_t buffer[128];                //定义缓冲区                        
    FILE* pipe = popen(cmd, "r");             //打开管道，并执行命令 
    if (!pipe)
    {
        return 0;                             //返回0表示运行失败 
    }  
    while(!feof(pipe)) 
    {
        if(fgets(buffer, 128, pipe))          //将管道输出到result中 
        {             
            strcat(result,buffer);
        }
    }
    pclose(pipe);                             //关闭管道 
    return 1;                                 //返回1表示运行成功 
}

blikvm_void_t split(blikvm_int8_t *src,const blikvm_int8_t *separator,blikvm_int8_t **dest,blikvm_int32_t *num) 
{
	/*
		src 源字符串的首地址(buf的地址)
		separator 指定的分割字符
		dest 接收子字符串的数组
		num 分割后子字符串的个数
	*/
    blikvm_int8_t *pNext;
    blikvm_int32_t count = 0;
    if (src == NULL || strlen(src) == 0) //如果传入的地址为空或长度为0，直接终止
        return;
    if (separator == NULL || strlen(separator) == 0) //如未指定分割的字符串，直接终止
        return;
     pNext = (blikvm_int8_t *)strtok(src,separator); //必须使用(blikvm_int8_t *)进行强制类型转换(虽然不写有的编译器中不会出现指针错误)
     while(pNext != NULL) {
          *dest++ = pNext;
          ++count;
         pNext = (blikvm_int8_t *)strtok(NULL,separator);  //必须使用(blikvm_int8_t *)进行强制类型转换
    }
    *num = count;
} 


blikvm_uint64_t blikvm_get_utc_ms(void)
{
    struct timeval tval;
    (void)gettimeofday(&tval, NULL);
    blikvm_uint64_t t = tval.tv_sec * 1000 + tval.tv_usec/1000;
    return t;
}

blikvm_board_type_e blikvm_get_board_type()
{
    blikvm_board_type_e type = UNKONW_BOARD;
    do
    {
        blikvm_int8_t *cmd1 = "cat /proc/cpuinfo";
        blikvm_int8_t result1[2048]={0};
        execmd(cmd1, result1);

        blikvm_int8_t *cmd2 = "cat /run/machine.id";
        blikvm_int8_t result2[2048]={0};
        execmd(cmd2, result2);

        if( (strstr(result1,pi4b_board) != NULL) || (strstr(result2,pi4b_board) != NULL) )
        {
            type = PI4B_BOARD ;
        }
        else if( (strstr(result1,cm4b_board) != NULL) || (strstr(result2,cm4b_board) != NULL) )
        {
            type = CM4_BOARD;
        }
        else if( (strstr(result1,h616_board) != NULL) || (strstr(result2,h616_board) != NULL))
        {
            type = H616_BOARD;
        }

        //Raspberry Pi 4
        /* code */
    } while (0>1);
    return type;
}

int execterminal(char* cmd, char* result)
{            
    char buffer[128]={0};                     
    FILE* pipe = popen(cmd,"r");             
    if (!pipe)
    {
        return 0;                             
    }  
    while(!feof(pipe)) 
    {
        if(fgets(result, 128, pipe))         
        {             
            strcat(result,buffer);
        }
    }
    pclose(pipe);  

    int len = strlen(result);
    for(int i=0; i<len; i++)
    {
        if(result[i] == '\n')
        {
            result[i] = '\0';
        }
    }
    return 0;
}


float GetCPULoad() 
{
    int FileHandler;
    char FileBuffer[1024];
    float load;

    FileHandler = open("/proc/loadavg", O_RDONLY);
    if(FileHandler < 0) {
        return -1; }
    read(FileHandler, FileBuffer, sizeof(FileBuffer) - 1);
    sscanf(FileBuffer, "%f", &load);
    close(FileHandler);
    return load;
}

int GetMemUsage(char* mem) 
{
    //char* cmd = "df -h | awk '$NF==\"/\"{size=$2; used=$3} $NF==\"/mnt\"{size+=$2; used+=$3} END{printf \"%d/%dG %d%%\", used, size, used*100/size}'";
    char* cmd = "df -BG | awk '$NF==\"/\"{size=$2; used=$3} $NF==\"/mnt\"{size+=$2; used+=$3} END{printf \"%d/%dG %d%%\", used, size, used*100/size}'";
    execterminal(cmd,mem);
    return 0; 
}

int GetMemUsageShort(char* mem) 
{
    char* cmd = "df -BG | awk '$NF==\"/\"{size=$2; used=$3} $NF==\"/mnt\"{size+=$2; used+=$3} END{printf \"%d/%dG\", used, size}'";
    
    execterminal(cmd,mem);
    return 0; 
}

int GetCPUTemp() 
{
   int FileHandler;
   char FileBuffer[1024];
   int CPU_temp;
   FileHandler = open("/sys/devices/virtual/thermal/thermal_zone0/temp", O_RDONLY);
   if(FileHandler < 0) {
      return -1; }
   read(FileHandler, FileBuffer, sizeof(FileBuffer) - 1);
   sscanf(FileBuffer, "%d", &CPU_temp);
   close(FileHandler);

   return CPU_temp / 1000;
}


int GetIP(char* ip)
{                                          
    char* cmd = "hostname -I | cut -d\' \' -f1";
    execterminal(cmd,ip);
    //printf("ip:%s\n",ip);
    return 0;
}

char * GetUptime() 
{
    FILE *fp;
    char buffer[256];
    char *uptime_string;
    fp = fopen("/proc/uptime", "r");
    fgets(buffer, 256, fp);
    double uptime_seconds = atof(buffer);
    int uptime_days = uptime_seconds / 86400;
    int uptime_hours = (uptime_seconds / 3600) - (uptime_days * 24);
    int uptime_minutes = (uptime_seconds / 60) - (uptime_days * 1440) - (uptime_hours * 60);
    asprintf(&uptime_string, "%dd %dh %dm", uptime_days, uptime_hours, uptime_minutes);
    fclose(fp);
    return uptime_string;
}

blikvm_int32_t skdy_get_int_uptime() 
{
    FILE *fp;
    char buffer[256];
    fp = fopen("/proc/uptime", "r");
    fgets(buffer, 256, fp);
    blikvm_int32_t uptime_seconds = (blikvm_int32_t)atof(buffer);
    return uptime_seconds;
}

int getWifiSignalStrength(const char* interface, int* signalStrength) {
    char command[100];
    sprintf(command, "iwconfig %s | awk '/Signal level/ {print $4}'", interface);

    FILE* fp = popen(command, "r");
    if (fp == NULL) {
        perror("Failed to execute command");
        return -1;
    }

    char output[32];
    if (fgets(output, sizeof(output), fp) == NULL) {
        perror("Failed to read command output");
        pclose(fp);
        return -1;
    }

    pclose(fp);
    int strength = atoi(&output[6]);
    *signalStrength = strength;
    return 0;
}

blikvm_int8_t isWifiCardAvailable() 
{
    blikvm_int8_t ret = -1;
    FILE* fp;
    char buffer[MAX_BUFFER_SIZE];
    blikvm_int8_t wifiCardAvailable = 0;

    // 执行 iwconfig 命令
    fp = popen("ifconfig wlan0 | grep UP", "r");
    if (fp == NULL) {
        perror("Failed to execute iwconfig");
        return -1;
    }

    // 读取命令输出并解析
    while (fgets(buffer, sizeof(buffer), fp) != NULL) 
    {
        // 检查是否存在无线网卡
        if (strstr(buffer, "UP") != NULL) 
        {
            wifiCardAvailable = 1; // 网卡正常
            break;
        }
    }
    pclose(fp);
    if( wifiCardAvailable ==  1)
    {
        blikvm_int32_t strength;
        if(getWifiSignalStrength("wlan0", &strength) == 0)
        {
            if( strength > 40)
            {
                ret = 0;
            }
        }
        else
        {
            printf("get wifi strength failed\n");
        }
    }
    return ret; // 返回0表示正常，返回-1表示不正常
}
