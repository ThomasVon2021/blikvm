#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdarg.h>

#include "blikvm_log.h"

/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2022-09-01 | 0.1       | Thmoasvon     |                 create
 ******************************************************************************/

typedef struct
{
    blikvm_uint8_t inited;           //log module is initialized
    blikvm_uint8_t enable;           //log module enable or disable
    blikvm_log_t config;             //log module config
    pthread_mutex_t log_mutex;         //log record mutex
    blikvm_uint8_t log_mutex_create; //log record mutex create flag
} blikvm_log_manager_t;

typedef struct
{
    blikvm_uint32_t     type;
    blikvm_int8_t       prefix[BLI_LOG_MAX_PREFIX_LEN];           //log prefix like "nmea_fusion.log@@"
    blikvm_int8_t       file_name[BLI_MAX_FILE_NAME];     //file name like "qx_ids"
	blikvm_uint32_t     max_size;                                //file max size unit:KByte
	blikvm_uint32_t     nums;                                    //rotate file number
    blikvm_uint32_t     max_buffer;                              //async write max buffer size unit:Byte
} blikvm_log_type_map_t;

typedef struct
{
	blikvm_uint32_t  type;
	pthread_cond_t  cond;
	pthread_mutex_t mutex;
	pthread_t       thread;
	blikvm_uint8_t   task_run;
}blikvm_file_rec_rotate_task_t;

typedef struct
{
	blikvm_uint8_t   enable;							//current file enable or disable
	blikvm_uint32_t 	index;							//rotate index
	blikvm_uint32_t 	file_cnt;						//rotate per file count
	blikvm_uint32_t  max_size;						//max len of file, unit KB
    blikvm_uint32_t  cur_size;  						//current file len
	blikvm_uint32_t  drop_cnt;						//buffer drop count
	blikvm_int8_t 	file_names[BLI_MAX_ROTATE_CNT][BLI_MAX_FILE_NAME];	//file name(absolute path)
	blikvm_uint32_t  valid_len;						//file write buffer valid data len
	//blikvm_int8_t    buffer[FR_MAX_FILE_BUFFER];	//file write buffer
	blikvm_uint8_t   cache[BLI_MAX_STATIC_CACHE];		//file write static swap cache
	blikvm_uint8_t*  buffer;							//file write buffer pointer
	blikvm_uint32_t  max_buffer;						//max buffer size of rotete write file
    FILE* 			pfile;								//file descriptor
	blikvm_file_rec_rotate_task_t task;
} blikvm_file_rec_rotate_t;

typedef struct
{
	blikvm_uint8_t inited;							//module is initialized
	pthread_mutex_t mutex;							//pthread mutex
	blikvm_int8_t		write_dir[BLI_LOG_FILE_PATH];	//file record dirctory
	blikvm_file_rec_rotate_t   rotate[BLI_LOG_FILE_NUM];	//file roteate config
	blikvm_file_rec_cfg_t      config;						//module config
}blikvm_file_rec_mgr_t;

static blikvm_int8_t blikvm_file_rec_init(blikvm_file_rec_cfg_t* config);
static blikvm_int8_t file_rec_get_date(blikvm_int8_t *date);
blikvm_int8_t blikvm_file_rec_dir_init(const blikvm_int8_t* dir, blikvm_file_rec_dir_type_e dir_type);
blikvm_int8_t blikvm_file_rec_rotate_file_init(blikvm_file_rec_cfg_t* config);
static blikvm_void_t*  blikvm_file_rec_write_task(void* param);
static blikvm_int8_t blikvm_file_rec_write_task_init(void);
static blikvm_int8_t blikvm_file_rec_rotate_file_create(blikvm_file_rec_rotate_t* rotate);
blikvm_int32_t blikvm_get_date_ms(blikvm_int8_t *date);
blikvm_int8_t blikvm_log_output_to_channels(blikvm_log_level_e level,blikvm_log_file_type_e type, blikvm_int8_t *data, blikvm_uint32_t lenth);
blikvm_int8_t blikvm_file_rec_write(blikvm_uint32_t type, const blikvm_int8_t* data, blikvm_uint32_t len);


static blikvm_log_manager_t g_log = {0};
static blikvm_file_rec_mgr_t g_filerec = {0};
static char  blikvm_log_loop_thread_name[32];
static blikvm_int8_t g_in_buffer[1024] = {0};
static blikvm_int8_t g_out_buffer[1024 + 32 + 4U] = {0};
static blikvm_log_type_map_t g_type_map[BLI_LOG_TYPE_MAX] = {
    /* type                                          prefix                    filename          maxsize   rotatenum*/
    {(blikvm_uint32_t)BLI_LOG_TYPE_KVMD,          "blikvm_kvmd.log@@",        "blikvm_kvmd",     50*1024,  4, 150*1024},};

#define FR_LOCK \
		if (1 == g_filerec.config.multitask_write) \
		{ \
			(void)pthread_mutex_lock(&g_filerec.mutex); \
		} \

#define FR_UNLOCK \
		if (1 == g_filerec.config.multitask_write) \
		{ \
			(void)pthread_mutex_unlock(&g_filerec.mutex); \
		} \

blikvm_int8_t blikvm_log_init(const blikvm_log_t *log)
{
    blikvm_int8_t ret=-1;
    blikvm_file_rec_cfg_t file_rec_cfg;
    (void)memset(&file_rec_cfg, 0, sizeof(file_rec_cfg));
    if ((log == NULL) || ( 1U == g_log.inited))
    {
        return ret;
    }

    if (0 !=  pthread_mutex_init(&g_log.log_mutex, NULL))
    {
        (void)printf("log mutex create failed\n");
        return ret;
    }
    g_log.log_mutex_create = 1;

    (void)memcpy(&g_log.config, log, sizeof(blikvm_log_t));

    // enable log type accordingly
    for (blikvm_uint8_t i = 0U; i < (blikvm_uint8_t)BLI_LOG_TYPE_MAX; i++)
    {
        if (1U == log->enable[i])
        {
            g_log.enable = 1U;
            break;
        }
    }

    if (1U == g_log.enable)
    {
        if ((g_log.config.channel & (blikvm_uint8_t)BLI_LOG_CHANNEL_FILE) != 0U)
        {
            // set record file variables
            file_rec_cfg.dir_type = DIR_TYPE_NAME;
            file_rec_cfg.multitask_write = 1U;
            (void)strncpy(file_rec_cfg.user_logdir, log->log_out.file.path, BLI_LOG_FILE_PATH);
            printf("path:%s\n", file_rec_cfg.user_logdir);
            // get file numbers
            blikvm_int16_t num = BLI_LOG_MIN(BLI_LOG_FILE_NUM, (blikvm_uint32_t)BLI_LOG_TYPE_MAX);
            for (blikvm_int16_t i = 0; i < num; i++)
            {
                file_rec_cfg.unit_config[i].enable = log->enable[i];
                //user set 0 means use default
                if (0U == log->log_out.file.max_size[i])
                {
                    file_rec_cfg.unit_config[i].max_size = g_type_map[i].max_size;
                }
                else
                {
                    // use default file max size
                    file_rec_cfg.unit_config[i].max_size = log->log_out.file.max_size[i];
                }
                // set file numers & name & buffer for each type
                file_rec_cfg.unit_config[i].nums = g_type_map[i].nums;
                (void)strncpy(file_rec_cfg.unit_config[i].file_name, g_type_map[i].file_name, BLI_MAX_FILE_NAME);
                file_rec_cfg.unit_config[i].max_buffer = g_type_map[i].max_buffer;
            }
            blikvm_int32_t file_rec_ret = blikvm_file_rec_init(&file_rec_cfg);
            if (0 != file_rec_ret)
            {
                printf("file rec init failed ret:%d\n",file_rec_ret);
                return ret;
            }
        }
    }

    g_log.inited = 1U;
    ret = 0;
    return ret;
}

static blikvm_int8_t blikvm_file_rec_write_task_init(void)
{
	blikvm_uint32_t i = 0;
	for (i = 0; i < BLI_LOG_FILE_NUM; i++)
	{
		if (1U == g_filerec.rotate[i].enable)
		{
			(void)pthread_mutex_init(&g_filerec.rotate[i].task.mutex, NULL);
			g_filerec.rotate[i].task.task_run = 1;
			g_filerec.rotate[i].task.type = i;
			pthread_create(&g_filerec.rotate[i].task.thread, NULL, &blikvm_file_rec_write_task, &g_filerec.rotate[i].task.type);
			(void)snprintf(blikvm_log_loop_thread_name, sizeof(blikvm_log_loop_thread_name), "log_loop%d", i);
			pthread_setname_np(g_filerec.rotate[i].task.thread, blikvm_log_loop_thread_name);
			//buffer_init(&g_filerec.rotate[i].record, g_filerec.rotate[i].buffer, FR_MAX_FILE_BUFFER);
		}
	}

	sleep(1);

	return 0;
}

static blikvm_int8_t blikvm_file_rec_init(blikvm_file_rec_cfg_t* config)
{
	blikvm_int8_t ret = -1;
	blikvm_uint32_t i = 0;

    if (1U == g_filerec.inited)
    {
        //repeated initialization returns OK to prevent resource destruction by mistake
        printf("repeat init!!!\n");
        return ret;
    }

	memset(&g_filerec, 0, sizeof(g_filerec));

	if (NULL == config)
	{
		printf("input param invalid\n");
		return ret;
	}

	printf("dir type:%d multask:%d dir:%s\n", config->dir_type, config->multitask_write, config->user_logdir);

    for (i = 0; i < BLI_LOG_FILE_NUM; i++)
    {
        printf("index:%d enable:%d name:%s maxsize:%d cnt:%d\n", i, config->unit_config[i].enable, config->unit_config[i].file_name, config->unit_config[i].max_size, config->unit_config[i].nums);
    }

    g_filerec.config = *config;

    if (1U == g_filerec.config.multitask_write)
    {
        (void)pthread_mutex_init(&g_filerec.mutex, NULL);
    }

	blikvm_int32_t ret_val = blikvm_file_rec_dir_init(config->user_logdir, config->dir_type);
    if (0 != ret_val)
    {
        return ret;
    }

    ret_val = blikvm_file_rec_rotate_file_init(config);
    if (0 != ret_val)
    {
        return ret;
    }

	blikvm_file_rec_write_task_init();

    ret = 0;
	g_filerec.inited = 1;
    
	return ret;
}

blikvm_int8_t blikvm_file_rec_dir_init(const blikvm_int8_t* dir, blikvm_file_rec_dir_type_e dir_type)
{
	blikvm_int8_t ret = -1;
	blikvm_int8_t   temp_dir[BLI_LOG_FILE_PATH - 64U] = {0};			//temp log dir

	if (NULL == dir)
	{
        printf("dir NULL\n");
        return ret;
	}

    if (access(dir, R_OK) == 0)
    {
        //the path provided by the user is valid
        strncpy(temp_dir, dir, sizeof(temp_dir));
    }
    else
    {
        //use the current path as the default path
        (void)snprintf(temp_dir, BLI_LOG_FILE_PATH, "./%s", BLI_DEFAULT_DIRNAME);
        if (access(temp_dir, R_OK) != 0)
        {
            mkdir(temp_dir, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        }
    }

        if (DIR_DATE_SUFFIX == dir_type)
        {
        	blikvm_int8_t   date[32] = {0};

        	(void)file_rec_get_date(date);
        	(void)snprintf(g_filerec.write_dir, sizeof(g_filerec.write_dir), "%s/%s", temp_dir, date);

        }
        else
        {
        	strncpy(g_filerec.write_dir, temp_dir, BLI_LOG_FILE_PATH);
        }

		if (access(g_filerec.write_dir, R_OK) != 0)
		{
			//dirctory not exist ,create it
			if(mkdir(g_filerec.write_dir, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0)
			{
				printf("create %s failed:%s \n", g_filerec.write_dir, strerror(errno));
                return ret;
			}
		}

		printf("use dir:%s\n", g_filerec.write_dir);
        ret = 0;
	    return ret;

}

static blikvm_int8_t file_rec_get_date(blikvm_int8_t *date)
{
    time_t timep;
    struct tm *ptm;
    (void)time(&timep);
    ptm = gmtime(&timep);
    sprintf(date,"%d%02d%02d",(ptm->tm_year + 1900), (ptm->tm_mon + 1), (ptm->tm_mday));
    return 0;
}


blikvm_int8_t blikvm_file_rec_rotate_file_init(blikvm_file_rec_cfg_t* config)
{
	blikvm_int8_t ret = -1;
	blikvm_uint32_t i = 0;
	blikvm_uint32_t j = 0;

    if (NULL == config)
    {
        printf("input param invalid\n");
        return ret;
    }

    for (i = 0; i < BLI_LOG_FILE_NUM; i++)
    {
        g_filerec.rotate[i].enable = config->unit_config[i].enable;
        g_filerec.rotate[i].file_cnt = BLI_LOG_MIN(config->unit_config[i].nums, BLI_MAX_ROTATE_CNT);
        if (0 == g_filerec.rotate[i].file_cnt)
        {
            g_filerec.rotate[i].file_cnt = 1;
        }

		g_filerec.rotate[i].max_size = config->unit_config[i].max_size/g_filerec.rotate[i].file_cnt;
        for (j = 0; j < BLI_MAX_ROTATE_CNT; j++)
        {
            (void)sprintf(g_filerec.rotate[i].file_names[j], "%s/%s_%u.log", g_filerec.write_dir, config->unit_config[i].file_name, j+1);
        }
		printf("index:%d name:%s max_size:%d buffer:%d cnt:%d\n", i, g_filerec.rotate[i].file_names[0], g_filerec.rotate[i].max_size, config->unit_config[i].max_buffer, g_filerec.rotate[i].file_cnt);

        if (1U == g_filerec.rotate[i].enable)
        {
            if (0 == config->unit_config[i].max_buffer)
            {
                g_filerec.rotate[i].max_buffer = BLI_MAX_STATIC_CACHE;
            }
            else
            {
                g_filerec.rotate[i].max_buffer = config->unit_config[i].max_buffer;
            }

            g_filerec.rotate[i].buffer = (blikvm_uint8_t*)malloc(g_filerec.rotate[i].max_buffer);
            if (NULL == g_filerec.rotate[i].buffer)
            {
                printf("index:%d buffer malloc failed\n", i);
                break;
            }

			blikvm_int8_t ret_value = blikvm_file_rec_rotate_file_create(&g_filerec.rotate[i]);
            if (0 == ret_value)
            {
                ret = 0;
                printf("index:%d start file name:%s\n", i, g_filerec.rotate[i].file_names[g_filerec.rotate[i].index]);
            }
            else
            {
                printf("index:%d rotate file create failed ret:%d\n", i, ret);
            }
		}
	}
	return ret;
}

static blikvm_int8_t blikvm_file_rec_rotate_file_create(blikvm_file_rec_rotate_t* rotate)
{
    blikvm_int8_t ret = -1;
	blikvm_uint32_t index = 0;

    if (NULL == rotate)
    {
        printf("rotate ptr is NULL\n");
        return ret;
    }

    while (index < rotate->file_cnt)
    {
        if (access(rotate->file_names[index], F_OK) != 0)
        {
            //the file 'XXX_index.log' does not exist, starting from current index
            rotate->pfile = fopen(rotate->file_names[index], "wb");
            rotate->cur_size = 0;
            rotate->index = index;

            if (NULL == rotate->pfile)
            {
                printf("open(wb) file:%s failed:%s\n", rotate->file_names[index], strerror(errno));
                return ret;
            }
            ret = 0;
            return ret;
        }
        else
        {
            //file exists, append to open, determine whether the current index can be written according to the size
            rotate->pfile = fopen(rotate->file_names[index], "ab");
            if (NULL == rotate->pfile)
            {
                printf("open(ab) file:%s failed:%s\n", rotate->file_names[index], strerror(errno));
                return ret;
            }

            (blikvm_void_t) fseek(rotate->pfile, 0L, SEEK_END);
            rotate->cur_size = ftell(rotate->pfile);

            //The file still has space to write
            if (rotate->cur_size < rotate->max_size*1024)
            {
                rotate->index = index;
                printf("rotate->cur_size:%d\n",rotate->cur_size);
                return 0;
            }
            else
            {
                //the file reaches its maximum value, close the file and continue to match the next one
                (blikvm_void_t) fclose(rotate->pfile);
                rotate->pfile = NULL;
                rotate->cur_size = 0;
            }
        }

        ++index;
    }

    //all files are full. by default, the file is written from the first file
    if (index == rotate->file_cnt)
    {
        rotate->pfile = fopen(rotate->file_names[0], "wb");
        rotate->cur_size = 0;
        rotate->index = 0;

        if (NULL == rotate->pfile)
        {
            printf("open(wb) first file:%s failed:%s\n", rotate->file_names[0], strerror(errno));
            return ret;
        }
    }

    ret = 0;
    return ret;
}

blikvm_int64_t file_rec_get_date_us(void)
{
    struct timeval tv;
	blikvm_int64_t timestamp = 0;

    (void)gettimeofday(&tv, NULL);

	timestamp = tv.tv_sec*1000*1000 + tv.tv_usec;

    return timestamp;
}


static blikvm_void_t*  blikvm_file_rec_write_task(void* param)
{
	blikvm_uint32_t type = *(blikvm_uint32_t*)param;
	printf("type:%d task create\n", type);

    //blikvm_int8_t buffer[FR_MAX_FILE_BUFFER] = {0};
	blikvm_uint8_t* write_buffer = NULL;
	blikvm_uint8_t* dynamic_mem = NULL;

    struct timeval cTime;
    struct timespec overTime;

	blikvm_int64_t start_ts = 0;
	blikvm_int64_t end_ts = 0;
	blikvm_int64_t cost_max = 0;
	blikvm_int64_t cost_total = 0;
	blikvm_uint32_t write_cnt = 0;
	blikvm_int32_t pop_size = 0;
	blikvm_int32_t write_ret = 0;

	blikvm_file_rec_rotate_t* rotate_handle = &g_filerec.rotate[type];

    while (rotate_handle->task.task_run)
    {

		pop_size = 0;
        (void)gettimeofday(&cTime, NULL);
        overTime.tv_sec = cTime.tv_sec;
        overTime.tv_nsec = (cTime.tv_usec + 10 * 1000) * 1000;

        (void)pthread_mutex_lock(&rotate_handle->task.mutex);
		(void)pthread_cond_timedwait(&rotate_handle->task.cond, &rotate_handle->task.mutex,&overTime);
		//reentry protect buffer & len

		if (rotate_handle->valid_len > BLI_MAX_STATIC_CACHE)
		{
			dynamic_mem = (blikvm_uint8_t*)malloc(rotate_handle->valid_len);
			if (NULL == dynamic_mem)
			{
				printf("lost type:%d %s malloc len:%d failed\n", type, rotate_handle->file_names[rotate_handle->index], rotate_handle->valid_len);
				rotate_handle->valid_len = 0;
				//rotate_handle->drop_cnt++;
			}
			else
			{
				memcpy(dynamic_mem, rotate_handle->buffer, rotate_handle->valid_len);
			}
			write_buffer = dynamic_mem;
		}
		else
		{
			memset(rotate_handle->cache, 0, sizeof(rotate_handle->cache));
			memcpy(rotate_handle->cache, rotate_handle->buffer, rotate_handle->valid_len);
			write_buffer = rotate_handle->cache;
		}

		pop_size = rotate_handle->valid_len;

		rotate_handle->valid_len = 0;
        (void)pthread_mutex_unlock(&rotate_handle->task.mutex);


		if (pop_size <= 0)
		{
			continue;
		}

		if (NULL != rotate_handle->pfile)
		{
			start_ts = file_rec_get_date_us();
			write_ret = fwrite(write_buffer, 1, pop_size, rotate_handle->pfile);
			end_ts = file_rec_get_date_us();
			if (write_ret != pop_size)
			{
				printf("type:%d %s write len:%d error:%s!!!\n", type, rotate_handle->file_names[rotate_handle->index], pop_size, strerror(errno));
			}

			write_cnt++;
			if (cost_max < (end_ts - start_ts))
			{
				cost_max = end_ts - start_ts;
			}
			cost_total += (end_ts - start_ts);
			if (2000 == write_cnt)
			{
				printf("type:%d write cost time max:%llu ms avg:%llu us drop cnt:%d\n", type, cost_max/1000, cost_total/write_cnt, rotate_handle->drop_cnt);
				cost_total = 0;
				write_cnt = 0;
			}

		}
		else
		{
			printf("type:%d %s write len:%d file not open!!!\n", type, rotate_handle->file_names[rotate_handle->index], pop_size);
		}

		rotate_handle->cur_size += pop_size;

		//determine whether to switch files
		if (rotate_handle->cur_size > rotate_handle->max_size*1024)
		{
			printf("%s:%d ==> %s\n", rotate_handle->file_names[rotate_handle->index], rotate_handle->cur_size, rotate_handle->file_names[(rotate_handle->index + 1) % rotate_handle->file_cnt]);
			(blikvm_void_t) fclose(rotate_handle->pfile);
			rotate_handle->pfile = NULL;
			rotate_handle->cur_size = 0;
			rotate_handle->index = (rotate_handle->index + 1) % rotate_handle->file_cnt;
			rotate_handle->pfile = fopen(rotate_handle->file_names[rotate_handle->index], "wb");
			if (NULL == rotate_handle->pfile)
			{
				printf("open(wb) file:%s failed:%s\n", rotate_handle->file_names[rotate_handle->index], strerror(errno));
			}
		}

		if (dynamic_mem != NULL)
		{
			free(dynamic_mem);
			dynamic_mem = NULL;
		}
    }

	printf("******************** thread type:%d exit !!!********************\n", type);

    return NULL;
}

blikvm_void_t blikvm_log_output(blikvm_log_file_type_e type, blikvm_log_level_e level, const blikvm_int8_t *format, ...)
{
    blikvm_int8_t timeStamp[32] = {0};

    va_list ap;

    do
    {
        //logger not inited or module disbale or type invalid
        if ((1U != g_log.inited) || (1U != g_log.enable) || (type >= BLI_LOG_FILE_NUM))
        {
            break;
        }

        //log type disable or level not allowed
        if ((1U != g_log.config.enable[type]) || ((blikvm_uint8_t)level > g_log.config.level))
        {
            break;
        }

        pthread_mutex_lock(&g_log.log_mutex);

        (void)memset(g_in_buffer, 0, sizeof(g_in_buffer));
        //get all args
        va_start(ap, format);
        (void)vsnprintf(g_in_buffer, 1024U -64U, format, ap);
        va_end(ap);

        //sdk log add timestamp
        if (BLI_LOG_TYPE_KVMD == type)
        {
            (void)memset(g_out_buffer, 0, sizeof(g_out_buffer));

            (void)blikvm_get_date_ms(timeStamp);
            (void)snprintf(g_out_buffer, (1024 + 32 + 4U), "[%s] %s", timeStamp, g_in_buffer);
            // send log to channels
            (void)blikvm_log_output_to_channels(level, type, g_out_buffer, (blikvm_uint32_t)strnlen(g_out_buffer, 1024));
        }
        else
        {
            // send log to channels
            (void)blikvm_log_output_to_channels(level, type, g_in_buffer, (blikvm_uint32_t)strnlen(g_in_buffer, 1024));
        }

        pthread_mutex_unlock(&g_log.log_mutex);
    } while (0 > 1);

    return;
}

blikvm_int8_t blikvm_file_rec_write(blikvm_uint32_t type, const blikvm_int8_t* data, blikvm_uint32_t len)
{
	blikvm_int8_t ret = -1;
	blikvm_file_rec_rotate_t * rotate_handle = NULL;

	FR_LOCK;

    if ((NULL == data) || (1U != g_filerec.inited) || (type >= BLI_LOG_FILE_NUM) || (len > g_filerec.rotate[type].max_buffer)
        || (1U != g_filerec.rotate[type].enable) || (NULL == g_filerec.rotate[type].pfile))
    {
        FR_UNLOCK;
        return ret;
    }

    rotate_handle = &g_filerec.rotate[type];

    //async write file
    (void)pthread_mutex_lock(&rotate_handle->task.mutex);

    if (rotate_handle->valid_len + len <= rotate_handle->max_buffer)
    {
        memcpy(rotate_handle->buffer + rotate_handle->valid_len, data, len);
        rotate_handle->valid_len += len;
        if (rotate_handle->valid_len >= rotate_handle->max_buffer*10/100)
        {
            pthread_cond_signal(&rotate_handle->task.cond);
        }
    }
    else
    {
        rotate_handle->drop_cnt++;
        printf("file lost type:%d %s write len:%d\n", type, rotate_handle->file_names[rotate_handle->index], len);
        pthread_cond_signal(&rotate_handle->task.cond);
        (void)pthread_mutex_unlock(&rotate_handle->task.mutex);
        FR_UNLOCK;
        return ret;
    }

	(void)pthread_mutex_unlock(&rotate_handle->task.mutex);

	FR_UNLOCK;
    ret = 0;
	return ret;
}

blikvm_int8_t out_buffer[1024] = {0};
blikvm_int8_t blikvm_log_output_to_channels(blikvm_log_level_e level,blikvm_log_file_type_e type, blikvm_int8_t *data, blikvm_uint32_t lenth)
{
    do
    {
        // check input variables
        if ((1U != g_log.inited) || (1U != g_log.enable) || (type >= BLI_LOG_FILE_NUM))
        {
            break;
        }
        // if console feature is enabled, then send data to console
        if ((g_log.config.channel & (blikvm_uint32_t)BLI_LOG_CHANNEL_CONSOLE) != 0U)
        {
            (void)printf("%s",data);
        }
        // if file record feature is enabled, then send data to file recorder
        if ((g_log.config.channel & (blikvm_uint32_t)BLI_LOG_CHANNEL_FILE) != 0U)
        {
            (void)blikvm_file_rec_write((blikvm_uint32_t)type, data, lenth);
        }
    } while (0 == 1);

    return 0;
}

blikvm_int32_t blikvm_get_date_ms(blikvm_int8_t *date)
{
    time_t timep;
    struct tm *ptm;
    struct timeval tv;
    //qxids_int8_t date[32U] = {0};

    (void)gettimeofday(&tv, NULL);
    (void)time(&timep);
    ptm = gmtime(&timep);

    sprintf(date,"%d-%02d-%02d_%02d:%02d:%02d-%03ld",(ptm->tm_year + 1900), (ptm->tm_mon + 1), (ptm->tm_mday), (ptm->tm_hour), (ptm->tm_min), (ptm->tm_sec), (tv.tv_usec / 1000));

    return 0;
}