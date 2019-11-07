/*
 * Filename: app_test.c
 * Author: viet anh
 * Date: 07/11/2019
 * Desc: ctrinh chay tren user space tuong tac voi vchar_device, thong qua vchar_driver
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BUFFER_SIZE 1024
/*Path to device file*/
#define DEVICE_NODE "/dev/vchar_dev"

/*Ham kien tra entry point open cua vchar driver */

int open_chardev(void)
{
    int fd = open(DEVICE_NODE, O_RDWR);
    if(fd < 0)
    {
        printf("Cannot open the device file \n");
        exit(1);
    }
    return fd;
}

/*Ham kiem tra entry point release cua vchar driver*/
void close_chardev(int fd)
{
    close(fd);
}

/*Ham kiem tra entry point read cua vchar driver */
void read_data_chardev()
{
    int ret = 0;
    char user_buf[BUFFER_SIZE];
    int fd = open_chardev();

    ret = read(fd, user_buf, BUFFER_SIZE);
    close_chardev(fd);
    if(ret < 0)
    {
        printf("Could not read a message from %s \n", DEVICE_NODE);
    }
    else 
    {
        printf("Read message from HW: %s\n", user_buf);
    }
}

/*Ham kiem tra entry point write cua vchar driver*/
void write_data_chardev()
{
    int ret = 0;
    char user_buf[BUFFER_SIZE];
    printf("\nEnter your message: ");
    scanf(" %[^\n]s", user_buf);
    int fd = open_chardev();
    ret = write(fd, user_buf, strlen(user_buf)+ 1); /*Ghi chuoi ki tu, ke ca NULL o cuoi*/
    close_chardev(fd);
    if(ret < 0)
    {
        printf("Could not write the message to %s\n", DEVICE_NODE);
    }
    else
    {
        printf("Wrote the message to HW: %d\n", ret);
    }
}
int main()
{
    int ret = 0;
    char option = 'q';
    int fd = -1;
    printf("Select below options: \n");
    printf("\to (to open a device node)\n");
    printf("\tc (to close the device node)\n");
    printf("\tr (to read data from device node)\n");
    printf("\tw (to write data from device node)\n");
    printf("\tq (to quit the application)\n");
    while(1)
    {
        printf("Enter your options: ");
        scanf(" %c", &option);
        switch (option)
        {
            case 'o':
                {
                    if(fd < 0)
                    {
                        fd = open_chardev();
                    }
                    else
                        printf("%s has already opened\n", DEVICE_NODE);
                    break;
                }
            case 'c':
                if(fd > -1)
                    close_chardev(fd);
                else
                    printf("%s has not opened yet!\n", DEVICE_NODE);
                fd = -1;
                break;
            case 'r':
                read_data_chardev();
                break;
            case 'w':
                write_data_chardev();
                break;
            case 'q':
                if(fd > -1)
                    close_chardev(fd);
                printf("Quit the application. Goodbye!!!\n");
                return 0;
            default:
                printf("Invalid option %c \n", option);
                break;
    
        }
    }
}
