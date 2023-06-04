#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

#define BUFFER_SIZE 1024

pthread_mutex_t mutex;
pthread_cond_t cond_read_usb;
pthread_cond_t cond_write_uart;
pthread_cond_t cond_read_uart;
pthread_cond_t cond_write_usb;

static int uart_init(char* device)
{
    int serial_fd = open(device, O_RDWR | O_NOCTTY);
    if (serial_fd == -1) {
        perror("Failed to open serial port");
        return -1;
    }

    struct termios serial_opts;
    tcgetattr(serial_fd, &serial_opts);
    cfsetispeed(&serial_opts, B9600);  // 设置串口波特率
    cfsetospeed(&serial_opts, B9600);
    serial_opts.c_cflag |= (CLOCAL | CREAD);  // 忽略调制解调器状态行，允许接收数据
    serial_opts.c_cflag &= ~PARENB;  // 禁用奇偶校验
    serial_opts.c_cflag &= ~CSTOPB;  // 设置停止位为1
    serial_opts.c_cflag &= ~CSIZE;   // 屏蔽数据位设置
    serial_opts.c_cflag |= CS8;      // 设置数据位为8位
    tcsetattr(serial_fd, TCSANOW, &serial_opts);
    return serial_fd;
}

static int usb_init(char* device)
{
    int fd = open(device, O_RDWR | O_NOCTTY);
    if (fd == -1) {
        perror("Failed to open device");
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
    else {
        printf("fd:%d write success len:%ld\n", serial_fd, num_bytes);
    }
    return 1;
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
    printf("fd:%d read:%s\n", fd, result);
    return result;
}

void* uart_read_thread(void* arg)
{
    int uart_fd = *(int*)arg;
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond_read_uart, &mutex);

    char* buff = read_from_fd(uart_fd);

    pthread_cond_signal(&cond_write_usb);
    pthread_mutex_unlock(&mutex);

    if (buff != NULL) {
        free(buff);
    }

    pthread_exit(NULL);
}

void* usb_read_thread(void* arg)
{
    int usb_fd = *(int*)arg;
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond_read_usb, &mutex);

    char* buff = read_from_fd(usb_fd);

    pthread_cond_signal(&cond_write_uart);
    pthread_mutex_unlock(&mutex);

    if (buff != NULL) {
        free(buff);
    }

    pthread_exit(NULL);
}

void* uart_write_thread(void* arg)
{
    int uart_fd = *(int*)arg;
    pthread_mutex_lock(&mutex);

    if (write_to_fd("blikvm", uart_fd) != 1) {
        pthread_cond_signal(&cond_read_uart);
        pthread_mutex_unlock(&mutex);
        printf("write to uart failed\n");
        pthread_exit(NULL);
    }

    pthread_cond_signal(&cond_read_uart);
    pthread_cond_wait(&cond_write_uart, &mutex);
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

void* usb_write_thread(void* arg)
{
    int usb_fd = *(int*)arg;
    pthread_mutex_lock(&mutex);

    if (write_to_fd("blikvm", usb_fd) != 1) {
        pthread_cond_signal(&cond_read_usb);
        pthread_mutex_unlock(&mutex);
        printf("write to usb failed\n");
        pthread_exit(NULL);
    }

    pthread_cond_signal(&cond_read_usb);
    pthread_cond_wait(&cond_write_usb, &mutex);
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

int main()
{
    pthread_t uart_read_tid, uart_write_tid, usb_read_tid, usb_write_tid;

    char uart_device[] = "/dev/ttyS0";
    char usb_device[] = "/dev/ttyUSB0";

    int uart_fd = uart_init(uart_device);
    int usb_fd = usb_init(usb_device);

    printf("uart fd:%d usb fd:%d\n", uart_fd, usb_fd);
    if (uart_fd <= 0 || usb_fd <= 0) {
        printf("open device failed\n");
        return 0;
    }

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_read_uart, NULL);
    pthread_cond_init(&cond_write_uart, NULL);
    pthread_cond_init(&cond_read_usb, NULL);
    pthread_cond_init(&cond_write_usb, NULL);

    pthread_create(&uart_read_tid, NULL, uart_read_thread, &uart_fd);
    pthread_create(&uart_write_tid, NULL, uart_write_thread, &uart_fd);
    pthread_create(&usb_read_tid, NULL, usb_read_thread, &usb_fd);
    pthread_create(&usb_write_tid, NULL, usb_write_thread, &usb_fd);

    pthread_join(uart_read_tid, NULL);
    pthread_join(uart_write_tid, NULL);
    pthread_join(usb_read_tid, NULL);
    pthread_join(usb_write_tid, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_read_uart);
    pthread_cond_destroy(&cond_write_uart);
    pthread_cond_destroy(&cond_read_usb);
    pthread_cond_destroy(&cond_write_usb);

    close(uart_fd);
    close(usb_fd);

    printf("test ok\n");
    return 0;
}
