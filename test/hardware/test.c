#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

#define BUFFER_SIZE 1024

static int uart_init(char* device)
{
    int serial_fd = open(device, O_RDWR | O_NOCTTY  );
    if (serial_fd == -1) {
        perror("Failed to open serial port");
        return -1;
    }

    int set_ret = set_serial_opt(serial_fd, 38400, 8, 'N', 1);  // CHANG BAUD
        if (set_ret == -1) {
            printf("failed to change baut 9600\n");
            return -1;
        }
    return serial_fd;
}

static int usb_init(char* device)
{
    int fd = open(device, O_RDWR| O_NOCTTY  );
    if (fd == -1) {
        perror("Failed to open device");
        return -1;
    }
    int set_ret = set_serial_opt(fd, 38400, 8, 'N', 1);  // CHANG BAUD
        if (set_ret == -1) {
            printf("failed to change baut 9600\n");
            return -1;
        }
    return fd;
}

static int write_to_fd(const char* string, int serial_fd) 
{
    ssize_t num_bytes = write(serial_fd, string, strlen(string));
    if (num_bytes == -1) {
        perror("Failed to write to serial port");
        close(serial_fd);
        return 0;
    }
    else
    {
        printf("fd:%d write success len:%ld\n",serial_fd,num_bytes);
    }
    return 0;
}

static char* read_from_fd(int fd) 
{
    char buffer[256] = {0};
    ssize_t bytesRead = read(fd, buffer, sizeof(buffer) - 1);
    if (bytesRead == -1) {
        perror("Failed to read from device");
        close(fd);
        return NULL;
    }

    buffer[bytesRead] = '\0';  // Null-terminate the string
    char* result = strdup(buffer);  // Allocate memory and copy the string
    if (result == NULL) {
        perror("Failed to allocate memory");
        return NULL;
    }
    printf("fd:%d read:%s\n",fd, result);
    return result;
}

int main()
{
    char uart_device[] = "/dev/ttyS0";
    char usb_device[] = "/dev/ttyUSB0";
    char content[] = "blikvmblikvmblikvmblikvmblikvmblikvmblikvm";
    int uart_fd = uart_init(uart_device);
    int usb_fd = usb_init(usb_device);

    printf("uart fd:%d usb fd:%d\n", uart_fd, usb_fd);
    if(uart_fd<= 0 || usb_fd <= 0 )
    {
        printf(" open device failed\n");
        return 0;
    }

    if (write_to_fd(content,uart_fd) != 0 )
    {
        printf("write to uart failed\n");
        return 0;
    }
    char* buff1 = read_from_fd(usb_fd);
    if(buff1 == NULL )
    {
        printf("read from usb failed\n");
        return 0;
    }
    free(buff1);
    if (write_to_fd(content,usb_fd) != 0 )
    {
        printf("write to usb failed\n");
        return 0;
    }
    char* buff2 = read_from_fd(uart_fd);
    if(buff2 == NULL )
    {
        printf("read from uart failed\n");
        return 0;
    }
    free(buff2);
    printf("test ok\n");
    return 0;
}
