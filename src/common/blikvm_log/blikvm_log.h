#ifndef __BLIKVM_LOG_H__
#define __BLIKVM_LOG_H__

/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2022-09-01 | 0.1       | Thmoasvon     |                 create
 ******************************************************************************/
#include "common/blikvm_type.h"

#define BLI_MAX_STATIC_CACHE 10*1024
#define BLI_LOG_FILE_PATH 256U
#define BLI_LOG_DEVICE_PATH 128U
#define BLI_MAX_FILE_NAME 512U
#define BLI_LOG_MAX_PREFIX_LEN 32U
#define BLI_MAX_ROTATE_CNT 4U
#define BLI_LOG_FILE_NUM 1U
#define BLI_DEFAULT_DIRNAME		"blikvm_log"		//default create directory name

#define BLI_LOG_MIN(A, B) ((A)<(B)? (A):(B))

typedef enum
{
    BLI_LOG_TYPE_KVMD    = 0U,
    BLI_LOG_TYPE_MAX,
}blikvm_log_file_type_e;

typedef enum
{
    BLI_LOG_LEVEL_UNEDF = 0x0U,
    BLI_LOG_LEVEL_ERROR = 0x1U,
    BLI_LOG_LEVEL_WARN = 0x2U,
    BLI_LOG_LEVEL_INFO = 0x3U,
    BLI_LOG_LEVEL_DEBUG = 0x4U,
    BLI_LOG_LEVEL_TRACE = 0x5U,
}blikvm_log_level_e;

typedef enum
{
    BLI_LOG_CHANNEL_UNDEF = 0x0U,   // not defined
    BLI_LOG_CHANNEL_CONSOLE = 0x1U, // use console to output log
    BLI_LOG_CHANNEL_FILE = 0x2U,    // output log to file
} blikvm_log_channel_e;

typedef struct
{
    blikvm_int8_t path[BLI_LOG_FILE_PATH];
    blikvm_uint32_t max_size[BLI_LOG_TYPE_MAX];
}blikvm_log_file_t;

typedef struct
{
    blikvm_int8_t path[BLI_LOG_DEVICE_PATH];
}blikvm_log_console_t;


typedef struct
{
    blikvm_log_file_t file;
    blikvm_log_console_t console;
}blikvm_log_channel_t;

typedef struct
{
    blikvm_uint8_t enable[BLI_LOG_TYPE_MAX];
    blikvm_uint8_t level;
    blikvm_uint32_t channel;
    blikvm_log_channel_t log_out;
}blikvm_log_t;

typedef enum
{
    DIR_TYPE_NAME,      //normal directory
    DIR_DATE_SUFFIX,    //directory with a date suffix
    DIR_TYPE_MAX,
}blikvm_file_rec_dir_type_e;

typedef struct
{
	blikvm_uint8_t      enable;							//file type is enable
	blikvm_int8_t       file_name[BLI_MAX_FILE_NAME];  	//file name like "qx_ids"
	blikvm_uint32_t     max_size;     					//max len of file, unit KB
	blikvm_uint32_t     nums;         					//max number of rotate writes per file
	blikvm_uint32_t     max_buffer;						//max buffer of rotete write file
}blikvm_file_rec_cfg_unit_t;

typedef struct
{
	blikvm_file_rec_dir_type_e  dir_type;							//directory type ref file_rec_dir_type_e
    blikvm_uint8_t        multitask_write;					//support multitask write
	blikvm_int8_t		 user_logdir[BLI_LOG_FILE_PATH];		//user set log directory
	blikvm_file_rec_cfg_unit_t  unit_config[BLI_LOG_FILE_NUM]; 	//configuration for each file type
} blikvm_file_rec_cfg_t;

#define LOG_FORMAT(module,level,format) "[%s] [%s] [%s:%d] " format,module,level,__func__,__LINE__

#define BLILOG_D(module, format, ...)                                                                                            \
    do                                                                                                                  \
    {                                                                                                                   \
        blikvm_log_output(BLI_LOG_TYPE_KVMD, BLI_LOG_LEVEL_DEBUG, LOG_FORMAT(module, "D",format), ##__VA_ARGS__); \
    } while (0 > 1)

#define BLILOG_I(module,format, ...)                                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        blikvm_log_output(BLI_LOG_TYPE_KVMD, BLI_LOG_LEVEL_INFO, LOG_FORMAT(module, "I", format), ##__VA_ARGS__); \
    } while (0 > 1)

#define BLILOG_W(module, format, ...)                                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        blikvm_log_output(BLI_LOG_TYPE_KVMD, BLI_LOG_LEVEL_WARN,  LOG_FORMAT(module, "W", format), ##__VA_ARGS__); \
    } while (0 > 1)

#define BLILOG_E(module,format, ...)                                                                                      \
    do                                                                                                            \
    {                                                                                                             \
        blikvm_log_output(BLI_LOG_TYPE_KVMD, BLI_LOG_LEVEL_ERROR,  LOG_FORMAT(module, "E",format), ##__VA_ARGS__); \
    } while (0 > 1)


blikvm_int8_t blikvm_log_init(const blikvm_log_t *log);
blikvm_int8_t blikvm_log_destory(blikvm_void_t);
blikvm_void_t blikvm_log_output(blikvm_log_file_type_e type, blikvm_log_level_e level, const blikvm_int8_t *format, ...);

#endif