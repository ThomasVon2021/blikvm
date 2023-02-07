#ifndef _ARMBIANIO_H
#define _ARMBIANIO_H
//
// SPI_LCD using the SPI interface
// Copyright (c) 2017 Larry Bank
// email: bitbank@pobox.com
// Project started 4/25/2017
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
#define GPIO_OUT 0
#define GPIO_IN 1
#define GPIO_HIGH 1
#define GPIO_LOW 0
// Only available on RPI boards
#define GPIO_IN_PULLUP 2
#define EDGE_FALLING 0
#define EDGE_RISING 1
#define EDGE_BOTH 2
#define EDGE_NONE 3
// Virtual header pin number to access the on-board IR receiver module
// with GPIO functions (since it's just a digital input connected to a GPIO)
#define IR_PIN 50
#ifdef __cplusplus
extern "C" {
#endif
//
// Initialize the library
// 1 = success, 0 = failure
//
// Convenience function to behave like original API. Calls AIOInitBoard with
// NULL
//
int AIOInit(void);

//
// Initialize the library
// 1 = success, 0 = failure
//
// On Armbian 4.1x, pBoardName can be NULL and it will read the name internally
// for non-Armbian (e.g. Raspberry Pi) or earlier builds of Armbian, pass the
// board name to get the correct GPIO pin mappings
//
int AIOInitBoard(const char *pBoardName);


// Free the resources
void AIOShutdown(void);

//
// Returns the name of the board you're running on
// or "Unknown" for an unsupported board
//
const char * AIOGetBoardName(void);

//
// Returns a file handle to the I2C device and address specified
// -1 if it fails to open
//
int AIOOpenI2C(int iChannel, int iAddress);

//
// Returns a file handle to the SPI device specified
// -1 if it fails to open
//
int AIOOpenSPI(int iChannel, int iSpeed);

//
// Close the file handle for the I2C bus
//
void AIOCloseI2C(int iHandle);

//
// Close the file handle for the SPI bus
//
void AIOCloseSPI(int iHandle);

//
// Read bytes from the I2C device
// Pass the "starting" register number
// Returns the number of bytes read or -1 for error
//
int AIOReadI2C(int iHandle, unsigned char ucRegister, unsigned char *buf, int iCount);

//
// Write data to the I2C device starting at the given register
// returns the number of bytes written or -1 for error
//
int AIOWriteI2C(int iHandle, unsigned char ucRegister, unsigned char *buf, int iCount);

//
// Read data from the SPI device
// returns the number of bytes read or -1 for error
//
int AIOReadSPI(int iHandle, unsigned char *buf, int iCount);

//
// Write data to the SPI device
// returns the number of bytes written or -1 for error
//
int AIOWriteSPI(int iHandle, unsigned char *buf, int iCount);

//
// Perform a simultaneous read and write on the SPI device
// returns the number of bytes transferred or -1 for error
//
int AIOReadWriteSPI(int iHandle, unsigned char *inbuf, unsigned char *outbuf, int iCount);

//
// Boolean indicating if the current PCB has a built-in IR receiver
//
int AIOHasIR(void);

//
// Boolean indicating if the current PCB has a button/key on it
//
int AIOHasButton(void);

//
// Read the button on the PCB (if present)
//
int AIOReadButton(void);

//
// Configure a GPIO pin for input or output
// (GPIO_IN or GPIO_OUT)
//
int AIOAddGPIO(int iPin, int iDirection);

typedef void (*AIOCALLBACK)(int iPin);

//
// Set edge to call the given function when the state
// changes. AIOAddGPIO must be called first with direction
// set to GPIO_IN
//
int AIOAddGPIOCallback(int iPin, AIOCALLBACK callback);

//
// Set pointer in callback list to NULL and set edge
// to none
//
int AIORemoveGPIOCallback(int iPin);

// The IR counterparts
typedef void (*AIOIRCALLBACK)(int *codeArray);
int AIOAddGPIOIRCallback(int iPin, AIOIRCALLBACK callback);
int AIORemoveGPIOIRCallback(int iPin);

//
// Release a GPIO pin
//
void AIORemoveGPIO(int iPin);

//
// Read the state of a GPIO input pin
// returns 0 or 1
//
int AIOReadGPIO(int iPin);

//
// Sets the state of a GPIO output pin
// Valid states are 1 (on) or 0 (off)
//
int AIOWriteGPIO(int iPin, int iValue);

//
// Set edge value for an open pin
//
int AIOWriteGPIOEdge(int iPin, int iEdge);

#ifdef __cplusplus
}
#endif

#endif // _ARMBIANIO_H
