/*****************************************************************************
* | File        :   dev_hardware_i2c.h
* | Author      :   Waveshare team
* | Function    :   Read and write /dev/i2C,  hardware I2C
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2019-06-26
* | Info        :   Basic version
*
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#ifndef __DEV_HARDWARE_I2C_
#define __DEV_HARDWARE_I2C_

#include <stdint.h>


#define DEV_HARDWARE_I2C_DEBUG 0
#if DEV_HARDWARE_I2C_DEBUG
#define DEV_HARDWARE_I2C_Debug(__info,...) printf("Debug: " __info,##__VA_ARGS__)
#else
#define DEV_HARDWARE_I2C_Debug(__info,...)
#endif

/**
 * Define I2C attribute
**/
typedef struct I2CStruct {
    //GPIO
    uint16_t SCL_PIN;
    uint16_t SDA_PIN;
    
    int fd; //I2C device file descriptor
    uint16_t addr; //I2C device address
} HARDWARE_I2C;

void DEV_HARDWARE_I2C_begin(char *i2c_device);
void DEV_HARDWARE_I2C_end(void);
void DEV_HARDWARE_I2C_setSlaveAddress(uint8_t addr);
uint8_t DEV_HARDWARE_I2C_write(const char * buf, uint32_t len);
uint8_t DEV_HARDWARE_I2C_read(uint8_t reg, char* buf, uint32_t len);
#endif