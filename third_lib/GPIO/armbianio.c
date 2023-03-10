//
// Armbian IO library
//
// Copyright (c) 2017 BitBank Software, Inc.
// written by Larry Bank
// email: bitbank@pobox.com
// Project started 11/12/2017
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include "armbianio.h"
#include <linux/spi/spidev.h>
#include <linux/i2c-dev.h>
#include <pthread.h>
#include <poll.h>

static struct spi_ioc_transfer xfer;
// Maximum header pins for all supported boards
#define MAX_PINS IR_PIN+1
static int iPinHandles[MAX_PINS]; // keep file handles open for GPIO access
static AIOCALLBACK cbList[MAX_PINS];
static AIOIRCALLBACK cbIRList[MAX_PINS];
//
// The following are lists which translate pin numbers into the GPIO numbers
// used by the different boards. The first entry (0) is for the on-board
// button (if present). A -1 indicates that the pin is not available to use
// as a GPIO (e.g. +5V, GND, etc). The numbering normally starts with the 3.3v
// pin as 1 and the 5V pin as 2. The TTY header comes after the last GPIO pin
// and is numbered from the pin closest to the edge of the board. On the
// Orange Pi Zero, this means that GND is pin 27, RX is 28 and TX is 29. The
// reason these are included is because they are multiplexed inside the SoC
// and can be used for GPIOs as well.
//

// Le potato
static int ipotatoPins[] = {-1,-1,-1,5,-1,4,-1,108,101,-1,
			102,-1,6,9,-1,110,103,-1,104,97,
			-1,98,89,100,99,-1,90,85,86,106,
			-1,107,105,95,-1,96,91,94,92,-1,
			93};

// Banana Pi M2 Zero
static int iBPIZPins[] = {355,-1,-1,12,-1,11,-1,6,13,-1,
			14,1,110,0,-1,3,15,-1,68,64,
			-1,65,2,66,67,-1,71,19,18,7,
			-1,8,354,9,-1,10,356,17,21,-1,
			20};

// Raspberry Pi
static int iRPIPins[] = {-1,-1,-1,2,-1,3,-1,4,14,-1,
                        15,17,18,27,-1,22,23,-1,24,10,
                        -1,9,25,11,8,-1,7,0,1,5,
                        -1,6,12,13,-1,19,16,26,20,-1,
                        21};
static int iWiringPiPins[] = {-1,-1,-1,8,-1,9,-1,7,
			15,-1,16,0,1,2,-1,3,
			4,-1,5,12,-1,13,6,14,
			10,-1,11,30,31,21,-1,22,
			26,23,-1,24,27,25,28,-1,
			29};


// Orange Pi Zero Plus
static int iOPIZPPins[] = {-1,-1,-1,12,-1,11,-1,6,198,-1,
                        199,1,7,0,-1,3,19,-1,18,15,
                        -1,16,2,14,13,-1,10,-1,5,4}; // last 3 pins are TTY header

// Orange Pi Zero Plus 2
static int iOPIZP2ins[] = {-1,-1,-1,12,-1,11,-1,6,0,-1,
			1,352,107,353,-1,3,19,-1,18,-1,
			-1,-1,2,14,13,-1,110,-1,5,4}; // last 3 pins are TTY header

// Orange Pi One
static int iOPI1Pins[] = {355,-1,-1,12,-1,11,-1,6,13,-1,
			14,1,110,0,-1,3,68,-1,71,64,
			-1,65,2,66,67,-1,21,19,18,7,
			-1,8,200,9,-1,10,201,20,198,-1
			,199,4,5,-1}; // last 3 pins are TTY header

// Orange Pi Zero
static int iOPIZPins[] = {-1,-1,-1,12,-1,11,-1,6,198,-1,
			199,1,7,0,-1,3,19,-1,18,15,
			-1,16,2,14,13,-1,10,-1,5,4}; // last 3 pins are TTY header

// NanoPi Duo
static int iNPDPins[] = {355,5,-1,4,-1,-1,-1,11,-1,12,
			363,13,203,14,-1,16,-1,15,-1,199,
			-1,198,-1,-1,-1,-1,-1,-1,-1,-1,
			-1,-1,-1};

// NanoPi 2
static int iNP2Pins[] = {0,-1,-1,99,-1,98,-1,32+28,96+21,-1,
			96+17,32+29,32+26,32+30,-1,32+31,64+14,-1,32+27,64+31,
			-1,96+0,96+1,64+29,64+30,-1,64+13,103,102,64+8,
			-1,64+9,64+28,64+10,-1,64+12,64+7,64+11,162,-1,
			163};

// NanoPi K2
static int iNPK2Pins[] = {3,-1,-1,205,-1,206,-1,211,102,-1,
			225,212,227,213,-1,214,226,-1,215,216,
			-1,218,217,220,219,-1,221,207,208,222,
			-1,127,223,155,-1,252,-1,-1,-1,-1,
			-1};

// NanoPi Neo & NanoPi Air & NanoPi Neo 2
static int iNPNPins[] = {-1,-1,-1,12,-1,11,-1,203,198,-1,
			199,0,6,2,-1,3,200,-1,201,64,
			-1,65,1,66,67,-1,-1,-1,-1,-1,
			363,17,-1,-1,-1,-1,-1,-1,-1,4,
			5};

// NanoPi M4
static int iNPM4Pins[] = {-1,-1,-1,-1,-1,-1,-1,32,145,-1,144,
            33,50,35,-1,36,54,-1,55,-1,-1,
            -1,56,-1,-1,-1,149,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

// Tinkerboard
static int iTinkerPins[] = {-1,-1,-1,252,-1,253,-1,17,161,-1,
			160,164,184,166,-1,167,162,-1,163,257,
			-1,256,171,254,255,-1,251,233,234,165,
			-1,168,239,238,-1,185,223,224,187,-1,
			188};

// Radxa Zero
static int iRadxaZeroPins[] = {-1,-1,-1,490,-1,491,-1,415,412,-1,
                               413,414,501,503,-1,-1,502,-1,500,447,
                               -1,448,475,450,449,-1,-1,415,414,-1,
                               -1,-1,416,-1,-1,420,451,421,422,-1,
                               423};

static int *iPinLists[] = {ipotatoPins, iBPIZPins, iRPIPins, iOPIZPPins, iOPIZP2ins, iOPIZPins, iOPI1Pins, iOPI1Pins,
                           iNPDPins, iNP2Pins, iNPK2Pins, iNPNPins, iNPNPins, iNPNPins, iNPM4Pins, iNPM4Pins,
                           iTinkerPins, iRadxaZeroPins};
static const char *szBoardNames[] = {"Le potato\n","Banana Pi M2 Zero\n","Raspberry Pi\n","Orange Pi Zero Plus\n",
                                     "Orange Pi Zero Plus 2\n","Orange Pi Zero\n","Orange Pi Lite\n","Orange Pi One\n",
                                     "NanoPi Duo\n", "NanoPi 2\n", "Nanopi K2\n", "NanoPi Neo\n", "NanoPi Air\n",
                                     "NanoPi Neo 2\n", "NanoPi M4\n", "NanoPi M4V2\n", "Tinkerboard\n", "Radxa Zero\n",
                                     NULL};
static int iBoardType;
static int iPinCount[] = {40,40,40,29,29,29,43,43,32,40,40,40,40,40,40, 41,41, 40}; // number of pins in the header
// GPIO number of on-board IR receiver
static int iIR_GPIO[] = {7, 0, 0, 363, 363, 363, 363, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//
// Close any open handles to GPIO pins and
// 'unexport' them
//
void AIOShutdown(void)
{
int i;
	for (i=0; i<MAX_PINS; i++) // try to close/release all open GPIO pins
	{
		AIORemoveGPIO(i);
	}
} /* AIOShutdown() */

//
// Initialize the ArmbianIO library
// Convenience function calls AIOInitBoard with NULL
//
int AIOInit(void)
{
	return AIOInitBoard(NULL);
}

//
// Initialize the ArmbianIO library
// Determines the board type (name) and initializes the 'key' if present
//
int AIOInitBoard(const char *pBoardName)
{
FILE *ihandle;
char szTemp[256];
int i;

// Determine what board we're running on to know which GPIO
// pin number table to use

	szTemp[0] = 0;
	if (pBoardName)
	{
		strcpy(szTemp, pBoardName);
	}
	else
	{
		ihandle = fopen("/run/machine.id", "rb");
		if (ihandle != NULL)
		{
			i = fread(szTemp, 1, 255, ihandle);
			fclose(ihandle);
			szTemp[i] = 0; // make sure it's zero terminated
		}
	}
	// see if the board name matches known names
	i = 0;
	iBoardType = -1;
	while (szBoardNames[i] != NULL)
	{
		if (strcmp(szBoardNames[i], szTemp) == 0) // found it!
		{
			iBoardType = i;
			break;
		}
		i++;
	}
	// Pi image not have /run/machine.id
	#ifdef  PRI
	iBoardType = 2;
	#endif
	if (iBoardType == -1) // not found
	{
		fprintf(stderr, "Unrecognized board type, aborting...\n");
		return 0;
	}
	// Initialize all GPIO file system handles to -1 to start
	memset(iPinHandles, -1, sizeof(iPinHandles));
	// Try to activate the GPIO input for the key/button (if there is one)
	AIOAddGPIO(0, GPIO_IN);
	return 1; // success
} /* AIOInit() */

//
// Boolean indicating if the current PCB has an on-board IR receiver module
//
int AIOHasIR(void)
{
	if (iBoardType != -1)
	{
		return (iIR_GPIO[iBoardType] != 0);
	}
	return 0;
} /* AIOHasIR() */

//
// Boolean indicating if the current PCB has a button/key on it
//
int AIOHasButton(void)
{
int *pPins;

	if (iBoardType != -1)
	{
		pPins = iPinLists[iBoardType];
		return (pPins[0] != -1); // if the GPIO number at index 0 is valid
	}
	return 0;
} /* AIOHasButton() */

//
// Read the boolean value of the key/button on the board
// A 0 indicates pressed (pulled to ground) and 1 indicates not pressed
//
int AIOReadButton(void)
{

	if (iBoardType != -1)
	{
		return AIOReadGPIO(0);
	}
	return 0;
} /* AIOReadButton() */

//
// Returns the number of pins for the current board
// This can include the 3 pins for the TTY header since the RX/TX lines can also
// be used as ordinary GPIO inputs/outputs
//
int AIOPinCount(void)
{
	if (iBoardType != -1)
	{
		return iPinCount[iBoardType];
	}
	return 0;
} /* AIOPinCount() */

//
// Simultaneous read/write to SPI bus
//
int AIOReadWriteSPI(int iHandle, unsigned char *pTxBuf, unsigned char *pRxBuf, int iLen)
{
int rc;
	xfer.rx_buf = (unsigned long)pRxBuf;
	xfer.tx_buf = (unsigned long)pTxBuf;
	xfer.len = iLen;
	rc = ioctl(iHandle, SPI_IOC_MESSAGE(1), &xfer);
    	return rc;
} /* AIOReadWriteSPI() */

//
// Read from SPI bus
//
int AIOReadSPI(int iHandle, unsigned char *buf, int iLen)
{
int rc;
	xfer.rx_buf = (unsigned long)buf;
	xfer.tx_buf = 0;
	xfer.len = iLen;
	rc = ioctl(iHandle, SPI_IOC_MESSAGE(1), &xfer);
    	return rc;
} /* AIOReadSPI() */

//
// Write to SPI bus
//
int AIOWriteSPI(int iHandle, unsigned char *pBuf, int iLen)
{
int rc;
	xfer.rx_buf = 0;
	xfer.tx_buf = (unsigned long)pBuf;
	xfer.len = iLen;
	rc = ioctl(iHandle, SPI_IOC_MESSAGE(1), &xfer);
    	return rc;
} /* AIOWriteSPI() */

int AIOOpenI2C(int iChannel, int iAddress)
{
char filename[32];
int file_i2c;

	sprintf(filename, "/dev/i2c-%d", iChannel);
	if ((file_i2c = open(filename, O_RDWR)) < 0)
	{
		fprintf(stderr, "Failed to open the i2c bus\n");
		return -1;
	}

	if (ioctl(file_i2c, I2C_SLAVE, iAddress) < 0)
	{
		fprintf(stderr, "Failed to acquire bus access or talk to slave\n");
		return -1;
	}
	return file_i2c;

} /* AIOOpenI2C() */

void AIOCloseI2C(int iHandle)
{
	close(iHandle);
} /* AIOCloseI2C() */

int AIOReadI2C(int iHandle, unsigned char ucRegister, unsigned char *buf, int iCount)
{
int rc;

	// Reading from an I2C device involves first writing the 8-bit register
	// followed by reading the data
	rc = write(iHandle, &ucRegister, 1); // write the register value
	if (rc == 1)
	{
		rc = read(iHandle, buf, iCount);
	}
	return rc;
} /* AIOReadI2C() */

int AIOWriteI2C(int iHandle, unsigned char ucRegister, unsigned char *buf, int iCount)
{
int rc;
unsigned char ucTemp[2048];

	// Writing to an I2C device involves first writing the 8-bit register
	// followed by writing the data
	ucTemp[0] = ucRegister; // some devices need it written atomically
	memcpy(&ucTemp[1], buf, iCount);
	rc = write(iHandle, ucTemp, iCount+1);
	return rc;
} /* AIOWriteI2C() */

//
// Read from a GPIO pin
//
int AIOReadGPIO(int iPin)
{
int iGPIO = 0;
char szTemp[64];
int rc;
int *pPins;

	if (iBoardType == -1) // library not initialized
		return -1;
	if (iPin < 0 || (iPin != IR_PIN && iPin > iPinCount[iBoardType])) // invalid pin number for this board
		return -1;
	if (iPin == IR_PIN && iIR_GPIO[iBoardType] == 0) // no IR receiver
		return -1;
	if (iPinHandles[iPin] == -1)
	{
		pPins = iPinLists[iBoardType];
		if (iPin == IR_PIN)
			iGPIO = iIR_GPIO[iBoardType];
		else
			iGPIO = pPins[iPin];
		sprintf(szTemp, "/sys/class/gpio/gpio%d/value", iGPIO);
		iPinHandles[iPin] = open(szTemp, O_RDONLY);
	}
	lseek(iPinHandles[iPin], 0, SEEK_SET); // reset file pointer to start
	rc = read(iPinHandles[iPin], szTemp, 1);
	if (rc <= 0) // problem
	{
		fprintf(stderr, "Error reading from GPIO %d\n", iGPIO);
		return -1;
	}
	return (szTemp[0] == '1');
} /* AIOReadGPIO() */

//
// Write a 0 or 1 to a GPIO output line
//
int AIOWriteGPIO(int iPin, int iValue)
{
int rc, iGPIO;
char szTemp[64];
int *pPins;

	if (iBoardType == -1) // not initialized
		return 0;
	if (iPin < 1 || iPin > iPinCount[iBoardType])
		return 0;
	if (iPinHandles[iPin] == -1) // not open yet
	{
		pPins = iPinLists[iBoardType];
		iGPIO = pPins[iPin]; // convert to GPIO number
		sprintf(szTemp, "/sys/class/gpio/gpio%d/value", iGPIO);
		iPinHandles[iPin] = open(szTemp, O_WRONLY);
	}
	if (iValue) rc = write(iPinHandles[iPin], "1", 1);
	else rc = write(iPinHandles[iPin], "0", 1);
	if (rc < 0) // error
	{ // do something
	}
	return 1;
} /* AIOWriteGPIO() */

//
// Set edge value for an open pin
//
int AIOWriteGPIOEdge(int iPin, int iEdge)
{
char szName[64];
int file_gpio, rc, iGPIO;
int *pPins;
char *szEdges[] = {"falling\n","rising\n","both\n","none\n"};

	if (iEdge < EDGE_FALLING || iEdge > EDGE_NONE)
		return 0;
	if (iBoardType == -1) // not initialized
		return 0;
	if (iPin < 0 || (iPin != IR_PIN && iPin > iPinCount[iBoardType]))
		return 0;
	pPins = iPinLists[iBoardType];
	// Set the mapped pin
	if (iPin == IR_PIN)
		iGPIO = iIR_GPIO[iBoardType];
	else
		iGPIO = pPins[iPin];
	sprintf(szName, "/sys/class/gpio/gpio%d/edge", iGPIO);
	file_gpio = open(szName, O_WRONLY);
	// Write edge type
	rc = write(file_gpio, szEdges[iEdge], strlen(szEdges[iEdge]));
	close(file_gpio);
	if (rc < 0) // error
	{ // do something
	}
	return 1;
} /* AIOWriteGPIOEdge() */

//
// GPIO Monitoring thread (one for each pin)
//
void *GPIOThread(void *param)
{
long iPin = (long)param; // pin number is passed in
struct pollfd fdset[1];
char szName[32], szTemp[64];
int gpio_fd;
int *pPins, rc, iGPIO;
int timeout = 3000; // 3 seconds

	pPins = iPinLists[iBoardType];

	if (iPin == IR_PIN)
		iGPIO = iIR_GPIO[iBoardType];
	else
		iGPIO = pPins[iPin];
	sprintf(szName, "/sys/class/gpio/gpio%d/value", iGPIO);
	gpio_fd = open(szName, O_RDONLY);
	if (gpio_fd < 0) // something went wrong
		return NULL;
	lseek(gpio_fd, 0, SEEK_SET);
	rc = read(gpio_fd, szTemp, 64); // initial read to prevent false interrupt

	while (1)
	{
		// If the callback is NULL then exit thread
		if (cbList[iPin] == NULL)
			return NULL;
		memset(fdset, 0, sizeof(fdset));
		fdset[0].fd = gpio_fd;
		fdset[0].events = POLLPRI;
		rc = poll(&fdset[0], 1, timeout);
		if (rc < 0) return NULL;
		// clear the interrupt by reading the data
		lseek(gpio_fd, 0, SEEK_SET);
		rc = read(gpio_fd, szTemp, 64);
		// see if it was a valid interrupt event
		if (fdset[0].revents & POLLPRI)
		{
			if (cbList[iPin])
				(*cbList[iPin])(iPin);
		}
	}
	return NULL;
} /* GPIOThread() */

//
// Set edge to call the given function when the state
// changes. AIOAddGPIO must be called first with direction
// set to GPIO_IN
//
int AIOAddGPIOCallback(long iPin, AIOCALLBACK callback)
{
int *pPins;
pthread_t tinfo;

	if (iBoardType == -1) // not initialize
		return 0;
	pPins = iPinLists[iBoardType];
	if (iPin != IR_PIN && pPins[iPin] == -1) // invalid pin
		return 0;
	cbList[iPin] = callback; // save the callback pointer
	// Start a thread to manage the interrupt/callback
	pthread_create(&tinfo, NULL, GPIOThread, (void *)iPin);
	return 1;
} /* AIOAddGPIOCallback() */

//
// Set pointer in callback list to NULL to cause
// thread to exit
//
int AIORemoveGPIOCallback(int iPin)
{

	if (iBoardType == -1) // not initialize
		return 0;
	if (cbList[iPin] == NULL) // invalid pin
		return 0;
	cbList[iPin] = NULL; // This will force thread to exit
	return 1;
} /* AIORemoveGPIOCallback() */


long getTimeInMicroseconds()
{
	struct timespec time_p;
	clock_gettime(CLOCK_MONOTONIC, &time_p);
	return (time_p.tv_sec % 10) * 1000000 + time_p.tv_nsec / 1000;
}

//
// GPIO Monitoring thread for IR (one for each pin)
// Code length must be less than 50.
//
void *GPIOIRThread(void *param)
{
long iPin = (long)param; // pin number is passed in
int endOfCodeTimeOut = 3; // after 3 ms we think code has ended.
struct pollfd fdset[1];
char szName[32], szTemp[64];
int gpio_fd;
int *pPins, rc, iGPIO;
int timeout = 3000; // 3 seconds
long start = getTimeInMicroseconds();
int currentCode[52];
int i, codePointer = 0;

	pPins = iPinLists[iBoardType];

	if (iPin == IR_PIN)
		iGPIO = iIR_GPIO[iBoardType];
	else
		iGPIO = pPins[iPin];
	sprintf(szName, "/sys/class/gpio/gpio%d/value", iGPIO);
	gpio_fd = open(szName, O_RDONLY);
	if (gpio_fd < 0) // something went wrong
		return NULL;
	lseek(gpio_fd, 0, SEEK_SET);
	rc = read(gpio_fd, szTemp, 64); // initial read to prevent false interrupt

	while (1)
	{
		// If the callback is NULL then exit thread
		if (cbIRList[iPin] == NULL)
			return NULL;
		memset(fdset, 0, sizeof(fdset));
		fdset[0].fd = gpio_fd;
		fdset[0].events = POLLPRI;
		rc = poll(&fdset[0], 1, timeout);
		if (rc < 0) return NULL;
		// clear the interrupt by reading the data
		lseek(gpio_fd, 0, SEEK_SET);
		rc = read(gpio_fd, szTemp, 64);
		// see if it was a valid interrupt event
		if (fdset[0].revents & POLLPRI)
		{
			long now = getTimeInMicroseconds();
			long between = now - start;
//			printf("between: %11d\n", between);
			currentCode[codePointer] = (int) between;
			codePointer++;
			timeout = endOfCodeTimeOut;
			start = now;
//			printf("cp: %d", codePointer);
			if(codePointer > 50) { // max code length reached, send what we have.
			    if (cbIRList[iPin])
                    (*cbIRList[iPin])(currentCode);
				codePointer = 0;
				for(i = 0; i < 50; i++) { currentCode[i] = 0; }
			}
		} else if(timeout == endOfCodeTimeOut) { // are we receiving a code currently?
            //printf("Between: %d\n", getTimeInMicroseconds() - start);
            if (cbIRList[iPin])
                (*cbIRList[iPin])(currentCode);
			timeout = 3000; // wait for next code again, with the default timeout
			codePointer = 0;
			for(i = 0; i < 50; i++) { currentCode[i] = 0; }
		} else {
		    printf("Code timeout\n");
		}
	}
	return NULL;
} /* GPIOIRThread() */


int AIOAddGPIOIRCallback(long iPin, AIOIRCALLBACK callback)
{
int *pPins;
pthread_t tinfo;

	if (iBoardType == -1) // not initialize
		return 0;
	pPins = iPinLists[iBoardType];
	if (iPin != IR_PIN && pPins[iPin] == -1) // invalid pin
		return 0;
	cbIRList[iPin] = callback; // save the callback pointer
	// Start a thread to manage the interrupt/callback
	pthread_create(&tinfo, NULL, GPIOIRThread, (void *)iPin);
	return 1;
} /* AIOAddGPIOCallback() */


int AIORemoveGPIOIRCallback(int iPin)
{

	if (iBoardType == -1) // not initialize
		return 0;
	if (cbIRList[iPin] == NULL) // invalid pin
		return 0;
	cbIRList[iPin] = NULL; // This will force thread to exit
	return 1;
} /* AIORemoveGPIOIRCallback() */

//
// Initialize a GPIO line for input or output
// This will export it to the sysfs driver and
// it will appear in /sys/class/gpio
//
int AIOAddGPIO(int iPin, int iDirection)
{
char szName[64];
int file_gpio, rc, iGPIO;
int *pPins;

	if (iBoardType == -1) // not initialize
		return 0;
	pPins = iPinLists[iBoardType];
	if (iPin != IR_PIN && pPins[iPin] == -1) // invalid pin
		return 0;
	if (iPin == IR_PIN && iIR_GPIO[iBoardType] == 0)
		return 0; // invalid IR pin
	file_gpio = open("/sys/class/gpio/export", O_WRONLY);
	if (file_gpio < 1) {
		fprintf(stderr, "Error opening /sys/class/gpio/export = %d\n", errno);
		return 0;
	}
	if (iPin == IR_PIN)
		iGPIO = iIR_GPIO[iBoardType];
	else
		iGPIO = pPins[iPin];
	sprintf(szName, "%d", iGPIO);
	rc = write(file_gpio, szName, strlen(szName));
	close(file_gpio);
	usleep(200000); // allow time for udev to make the needed changes
	sprintf(szName, "/sys/class/gpio/gpio%d/direction", iGPIO);
	file_gpio = open(szName, O_WRONLY);
	if (file_gpio < 1) {
           fprintf(stderr, "error setting direction on GPIO pin %d\n", iGPIO);
	   return 0;
	}
	if (iDirection == GPIO_OUT)
		rc = write(file_gpio, "out\n", 4);
	else
		rc = write(file_gpio, "in\n", 3);
	close(file_gpio);
	if (iDirection == GPIO_IN_PULLUP) // RPI specific feature, use GPIO library
	{
		sprintf(szName, "gpio mode %d up", iWiringPiPins[iPin]);
		system(szName);
	}
	if (rc < 0)
	{
		fprintf(stderr, "Error setting mode on GPIO pin %d\n", iGPIO);
	}
	return 1;
} /* AIOAddGPIO() */

//
// Remove access to a GPIO pin
// This will 'unexport' it from the sysfs driver
// and remove it from the /sys/class/gpio directory
//
void AIORemoveGPIO(int iPin)
{
int file_gpio, rc;
char szTemp[64];
int *pPins;

	if (iBoardType == -1) // not initialized
		return;
	if (iPin < 1 || iPin > MAX_PINS) // invalid pin
		return;
	if (iPinHandles[iPin] != -1)
	{
		close(iPinHandles[iPin]);
		file_gpio = open("/sys/class/gpio/unexport", O_WRONLY);
		pPins = iPinLists[iBoardType];
		sprintf(szTemp, "%d", pPins[iPin]);
		rc = write(file_gpio, szTemp, strlen(szTemp));
		close(file_gpio);
		if (rc < 0) // suppress compiler warning
		{ // do nothing
		}
		iPinHandles[iPin] = -1;
	}
} /* AIORemoveGPIO() */

//
// Open a handle to the SPI bus
//
int AIOOpenSPI(int iChannel, int iSPIFreq)
{
int rc, iSPIMode = SPI_MODE_0; // | SPI_NO_CS;
char szName[32];
int file_spi;
int i = iSPIFreq;

	sprintf(szName,"/dev/spidev%d.0", iChannel);
	file_spi = open(szName, O_RDWR);
	rc = ioctl(file_spi, SPI_IOC_WR_MODE, &iSPIMode);
	if (rc < 0) fprintf(stderr, "Error setting SPI mode\n");
	rc = ioctl(file_spi, SPI_IOC_WR_MAX_SPEED_HZ, &i);
	if (rc < 0) fprintf(stderr, "Error setting SPI speed\n");
	memset(&xfer, 0, sizeof(xfer));
	xfer.speed_hz = iSPIFreq;
	xfer.cs_change = 0;
	xfer.delay_usecs = 0;
	xfer.bits_per_word = 8;

	if (file_spi < 0)
	{
		fprintf(stderr, "Failed to open the SPI bus\n");
		return -1;
	}
	return file_spi;
} /* AIOOpenSPI() */

void AIOCloseSPI(int iHandle)
{
	close(iHandle);
} /* AIOCloseSPI() */

//
// Returns the name of the board (if recognized)
//
const char * AIOGetBoardName(void)
{
	if (iBoardType == -1)
		return "Unknown";
	else
		return szBoardNames[iBoardType];
} /* AIOGetBoardName() */

