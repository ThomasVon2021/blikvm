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

 #include "blikvm_util.h"


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