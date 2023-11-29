/*******************************************************************************
 *                            CHANGE HISTORY                                   *
 *-----------------------------------------------------------------------------*
 *   <Date>   | <Version> | <Author>      |            <Description>           *
 *-----------------------------------------------------------------------------*
 * 2023-11-19 | 0.1       | Thomasvon     |                 create
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "GPIO/armbianio.h"  
#include "GPIO/softPwm.h"  
 
#define RPI_PIN_POWER 16  //BCM23 
#define RPI_PIN_RESET 13  //BCM27
#define RPI_PIN_LED_PWR 18  //BCM24 
#define RPI_PIN_LED_HDD 15  //BCM22

#define H616_PIN_POWER 18   //GPIO 228
#define H616_PIN_RESET 37   //GPIO 272
#define H616_PIN_LED_PWR 36 //GPIO 234   
#define H616_PIN_LED_HDD 41  //GPIO 233

#define POWER_ON_DELAY 500  //unit:ms
#define POWER_OFF_DELAY 5000  //unit:ms
#define POWER_RESET_DELAY 500  //unit:ms

#define PI4_BOARD "Raspberry Pi\n"
#define H616_BOARD "Mango Pi Mcore\n"

typedef struct 
{
    int power;
    int reset;
    int led_power;
    int led_hdd;
}atx_pin_t;

typedef enum
{
    ATX_UNKNOW = 0,
    ATX_POWER_ON ,
    ATX_POWER_OFF,
    ATX_RESET,
    ATX_LED_PWR,
    ATX_LED_HDD,
}atx_command_e;

static int processArguments(int argc, char *argv[]);

static atx_pin_t g_atx = {0};
static atx_command_e g_atx_cmd_type = ATX_UNKNOW;
static char g_board_type[32] = {0};

int main(int argc, char *argv[]) 
{
    // Check if there are command-line arguments
    if (argc < 2) {
        printf("Usage: %s --v <v1|v2|v3|v4> --c <power_on|power_off|power_reset>|led_power|led_hdd\n", argv[0]);
        return 1; // Exit with an error code
    }

    // Process command-line arguments
    int ret = processArguments(argc, argv);

    if(ret != 0)
    {
        printf("process arguments error\n");
        return -1;
    }

    int rc = AIOInitBoard(g_board_type);
    if (rc == 0)
    {
        printf("Initializing ArmbianIO library error\n");
        return -1;
    }

    AIOAddGPIO(g_atx.power, GPIO_OUT);
    AIOAddGPIO(g_atx.reset , GPIO_OUT);
    AIOAddGPIO(g_atx.led_power, GPIO_IN);
    AIOAddGPIO(g_atx.led_hdd, GPIO_IN);
    AIOWriteGPIO(g_atx.power, GPIO_LOW);
    AIOWriteGPIO(g_atx.reset, GPIO_LOW);

    switch(g_atx_cmd_type)
    {
        case ATX_POWER_ON:
            AIOWriteGPIO(g_atx.power, GPIO_HIGH);
            usleep(POWER_ON_DELAY*1000);
            AIOWriteGPIO(g_atx.power, GPIO_LOW);
            printf("atx power on\n");
            break;
        case ATX_POWER_OFF:
            AIOWriteGPIO(g_atx.power, GPIO_HIGH);
            usleep(POWER_OFF_DELAY*1000);
            AIOWriteGPIO(g_atx.power, GPIO_LOW);
            printf("atx power off\n");
            break;
        case ATX_RESET:
            AIOWriteGPIO(g_atx.reset, GPIO_HIGH);
            usleep(POWER_RESET_DELAY*1000);
            AIOWriteGPIO(g_atx.reset, GPIO_LOW);
            printf("atx restart\n");
            break;
        case ATX_LED_PWR:
            int led_power = AIOReadGPIO(g_atx.led_power);
            printf("atx led_power value:%d\n",led_power);
            break;
        case ATX_LED_HDD:
            int led_hdd = AIOReadGPIO(g_atx.led_hdd);
            printf("atx led_hdd value:%d\n",led_hdd);
            break;
        default:
            printf("atx get error command:%d\n", g_atx_cmd_type);
            break;
    }

    return 0; // Exit successfully
}

// Function to process command line arguments
static int processArguments(int argc, char *argv[]) 
{
    for (int i = 1; i < argc; i++) {
        // Check for "--v" option
        if (strcmp(argv[i], "--v") == 0 && i + 1 < argc) {
            
            if( (strcmp(argv[i + 1], "v1")== 0) || (strcmp(argv[i + 1], "v2")== 0) || (strcmp(argv[i + 1], "v3")== 0) )
            {
                memcpy(g_board_type, PI4_BOARD, strlen(PI4_BOARD));
                g_atx.power = RPI_PIN_POWER;
                g_atx.reset = RPI_PIN_RESET;
                g_atx.led_power = RPI_PIN_LED_PWR;
                g_atx.led_hdd = RPI_PIN_LED_HDD;
            }else if(strcmp(argv[i + 1], "v4")== 0 )
            {
                memcpy(g_board_type, H616_BOARD, strlen(H616_BOARD));
                g_atx.power = H616_PIN_POWER;
                g_atx.reset = H616_PIN_RESET;
                g_atx.led_power = H616_PIN_LED_PWR;
                g_atx.led_hdd = H616_PIN_LED_HDD;
            }else
            {
                printf("Input error version\n");
                return -1;
            }
            printf("board is %s\n", argv[i + 1]);
            i++;
        }
        else if (strcmp(argv[i], "--c") == 0 && i + 1 < argc) {
            if(strcmp(argv[i + 1], "power_on") == 0)
            {
                g_atx_cmd_type = ATX_POWER_ON;
                printf("get atx power on command\n");
            }
            else if( strcmp(argv[i + 1], "power_off")== 0 )
            {
                g_atx_cmd_type = ATX_POWER_OFF;
                printf("get atx power off command\n");
            }
            else if( strcmp(argv[i + 1], "power_reset")== 0 )
            {
                g_atx_cmd_type = ATX_RESET;
                printf("get atx power reset command\n");
            }
            else if( strcmp(argv[i + 1], "led_power") == 0)
            {
                g_atx_cmd_type = ATX_LED_PWR;
                printf("get atx led power command\n");
            }
            else if( strcmp(argv[i + 1], "led_hdd")== 0)
            {
                g_atx_cmd_type = ATX_LED_HDD;
                printf("get atx led hdd command\n");
            }
            else
            {
                printf("get unknow command\n");
            }
            i++;
        }
        // Handle other options or display an error message
        else {
            printf("Invalid option or missing argument: %s\n", argv[i]);
            // Add additional handling for other options if needed
        }
    }
    return 0;
}
