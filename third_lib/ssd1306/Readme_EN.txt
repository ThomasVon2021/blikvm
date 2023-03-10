/*****************************************************************************
* | File      	:   Readme_CN.txt
* | Author      :   Waveshare team
* | Function    :   Help with use
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2020-08-28
* | Info        :   Here is an English version of the documentation for your quick use.
******************************************************************************/
This file is to help you use this routine.
Since our OLED screens are getting more and more, it is not convenient for our maintenance, so all the OLED screen programs are made into one project.
A brief description of the use of this project is here:

1. Basic information:
This routine is based on the raspberry PI 4B+ development, the kernel version:
	Linux raspberrypi 5.4.51-v7l+ #1333 SMP Mon Aug 10 16:51:40 BST 2020 armv7l GNU/Linux
This routine can view the corresponding test routines in the examples\ of the project;

2. Pin connection:
Pin connection you can use STM32CubeMX to open the project file oled_demo.ioc for viewing, which is also repeated here:
SPI:
	OLED   =>    RPI(BCM)
	VCC    ->    3.3
	GND    ->    GND
	DIN    ->    10(MOSI)
	CLK    ->    11(SCLK)
	CS     ->    8
	DC     ->    25
	RST    ->    27

IIC:
	OLED   =>    RPI(BCM)
	VCC    ->    3.3
	GND    ->    GND
	DIN    ->    2(SDA)
	CLK    ->    3(SCL)
	CS     ->    8
	DC     ->    25
	RST    ->    27

3. Basic use:
Since this project is a comprehensive project, for use, you may need to read the following:
Please pay attention to which OLED you bought.
eg 1:
	If you bought a 1.3-inch OLED Module (C), enter it in your home directory:
		sudo make clean
		sudo make
		sudo ./main 1.3c

eg 2:
	If you buy a 1.5-inch RGB OLED Module, enter it in your home directory:
		sudo make clean
		sudo make
		sudo ./main 1.5rgb

eg 3:
	If you buy a 0.91inch OLED Module, please note that since the Module only has IIC interface and the routine defaults to SPI, you need to modify it in config.h, that is:
		#define USE_SPI_4W 	1
		#define USE_IIC 	0
	Modified to:
		#define USE_SPI_4W 	0
		#define USE_IIC 	1
	And enter in the home directory:
		sudo make clean
		sudo make
		sudo ./main 0.91

4. Directory structure (selection):
If you use our products frequently, we will be very familiar with our program directory structure. We have a copy of the specific function.
The API manual for the function, you can download it on our WIKI or request it as an after-sales customer service. Here is a brief introduction:
Config\: This directory is a hardware interface layer file. You can see many definitions in DEV_Config.c(.h), including:
    data type：
        #define UBYTE   uint8_t
        #define UWORD   uint16_t
        #define UDOUBLE uint32_t
	SPI or IIC select：
		#define USE_SPI_4W 	1
		#define USE_IIC 	0
	IIC address：
		#define IIC_CMD        0X00
		#define IIC_RAM        0X40
    GPIO read/write：
		#define OLED_CS_0		HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_RESET)
		#define OLED_CS_1		HAL_GPIO_WritePin(OLED_CS_GPIO_Port, OLED_CS_Pin, GPIO_PIN_SET)
		#define OLED_DC_0		HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_RESET)
		#define OLED_DC_1		HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET)
		#define OLED_RST_0		HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_RESET)
		#define OLED_RST_1		HAL_GPIO_WritePin(OLED_RST_GPIO_Port, OLED_RST_Pin, GPIO_PIN_SET)
    SPI transmission：
        void SPI4W_Write_Byte(UBYTE value);
    IIC transmission：
        void I2C_Write_Byte(UBYTE value, UBYTE Cmd);
    delay：
        #define DEV_Delay_ms(__xms) HAL_Delay(__xms);
        Note: This delay function does not use oscilloscope to measure specific values
    The process of module initialization and exit：
        UBYTE	System_Init(void);
        void	System_Exit(void);
        Note: Here is the processing of SOME GPIO before and after using OLED
		
GUI\: This directory is some basic image processing functions, in GUI_Paint.c(.h):
    Common image processing: creating graphics, flipping graphics, mirroring graphics, setting pixels, clearing screens, etc.
    Common drawing processing: drawing points, lines, boxes, circles, Chinese characters, English characters, numbers, etc.;
    Common time display: Provide a common display time function;
    Commonly used display pictures: provide a function to display bitmaps;
    
	Fonts\: for some commonly used fonts:
    Ascii:
        Font8: 5*8
        Font12: 7*12
        Font16: 11*16
        Font20: 14*20
        Font24: 17*24
    Chinese:
        font12CN: 16*21
        font24CN: 32*41
        
OLED\: This screen is the OLED screen driver function;
Examples\: This is the test program for the OLED screen. You can see the specific usage method in it.