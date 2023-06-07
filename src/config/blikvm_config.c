/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2023-06-07 | 0.1       | Thmoasvon     |                 create
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "blikvm_config.h"

#define TAG "CFG"

blikvm_int8_t blikvm_config_init()
{
    const char* directory1 = "/mnt/msd/user";
    const char* directory2 = "/mnt/msd/ventoy";

    // Check if the directories already exist
    struct stat dirStat;
    if (stat(directory1, &dirStat) == 0 && S_ISDIR(dirStat.st_mode)) 
    {
        BLILOG_D(TAG,"%s already exists\n", directory1);
    } else 
    {
        // Create directory1 if it doesn't exist
        if (mkdir(directory1, 0777) != 0) 
        {
            BLILOG_E(TAG,"Failed to create %s\n",directory1 );
            return -1;
        } else {
            BLILOG_D(TAG,"%s created\n", directory1);
        }
    }

    if (stat(directory2, &dirStat) == 0 && S_ISDIR(dirStat.st_mode)) {
        BLILOG_D(TAG,"%s already exists\n", directory2);
    } else {
        // Create directory2 if it doesn't exist
        if (mkdir(directory2, 0777) != 0) {
            BLILOG_E(TAG,"Failed to create %s\n",directory2);
            return -1;
        } else {
            BLILOG_D(TAG,"%s created\n", directory2);
        }
    }

    return 0;
}
