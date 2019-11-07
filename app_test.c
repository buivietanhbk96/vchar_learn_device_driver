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

int main()
{
    int ret = 0;
    char option = 'q';
    int fd = -1;
    printf("Select below options: \n");
    printf("\to (to open a device node)\n");
    printf("\tc (to close the device node)\n");
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
