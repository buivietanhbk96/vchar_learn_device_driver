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
#include <sys/ioctl.h>
#define BUFFER_SIZE 1024
/*Path to device file*/
#define DEVICE_NODE "/dev/vchar_dev"
/* Define command for IOCTL*/
#define MAGICAL_NUM 243
#define CLEAR_DATA_CHARDEV  _IO(MAGICAL_NUM, 0)
#define GET_STATUS_CHARDEV  _IOR(MAGICAL_NUM, 1, status_t *)
#define CTRL_READ_CHARDEV   _IOW(MAGICAL_NUM, 2, unsigned char *)
#define CTRL_WRITE_CHARDEV  _IOW(MAGICAL_NUM, 3, unsigned char *)
typedef struct 
{
    unsigned char read_count_h;
    unsigned char read_count_l;
    unsigned char write_count_h;
    unsigned char write_count_l;
    unsigned char device_status;
}status_t;

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
/*Ham clear data cua thiet bi*/
void clear_data_chardev(void)
{
    int ret = 0;
    int fd = open_chardev();
    ret = ioctl(fd, CLEAR_DATA_CHARDEV);
    close_chardev(fd);
    printf("%s data register in char device\n", (ret < 0)?"Couldn't clear":"Cleared");
}
/*Ham get status cua thiet bi*/
void get_status_chardev()
{
    status_t status;
    unsigned int read_cnt, write_cnt;

    int fd = open_chardev();
    ioctl(fd, GET_STATUS_CHARDEV, (status_t *)&status);
    close_chardev(fd);
    read_cnt = status.read_count_h << 8 | status.read_count_l;
    write_cnt = status.write_count_h << 8 | status.write_count_l;
    printf("Statistic: number of reading (%u), number of writing (%u)\n", read_cnt, write_cnt);
}
/*Ham control access read thiet bi*/
void control_read_chardev()
{
    unsigned char isReadable = 0;
    status_t status;
    char choose = 'n';
    printf("\nDo you want to enable read data register of device (y/n): ");
    scanf(" %c", &choose);
    if(choose == 'y')
    {
        isReadable = 1;
    }
    else if(choose == 'n')
    {
        isReadable = 0;
    }
    else 
        return;
    int fd = open_chardev();
    ioctl(fd,CTRL_READ_CHARDEV,&isReadable);
    ioctl(fd, GET_STATUS_CHARDEV, &status);
    close_chardev(fd);
    if(status.device_status & 0x01)
        printf("Enable to read from data registers successful\n");
    else
        printf("Disable to read from data registers successful\n");
}
/*Ham control access write thiet bi*/
void control_write_chardev()
{
    unsigned char isWritable = 0;
    status_t status;
    char choose = 'n';
    printf("\nDo you want to enable read data register of device (y/n): ");
    scanf(" %c", &choose);
    if(choose == 'y')
    {
        isWritable = 1;
    }
    else if(choose == 'n')
    {
        isWritable = 0;
    }
    else 
        return;
    int fd = open_chardev();
    ioctl(fd,CTRL_WRITE_CHARDEV,&isWritable);
    ioctl(fd, GET_STATUS_CHARDEV, &status);
    close_chardev(fd);
    if(status.device_status & 0x02)
        printf("Enable to write from data registers successful\n");
    else
        printf("Disable to write from data registers successful\n");
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
    printf("\tC (to clear data registers)\n");
    printf("\tR (to EN/DIS read data registers)\n");
    printf("\tW (to EN/DIS write data registers)\n");
    printf("\ts (to get status device node)\n");
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
            case 'C':
                clear_data_chardev();
                break;
            case 'R':
                control_read_chardev();
                break;
            case 'W':
                control_write_chardev();
                break;
            case 's':
                get_status_chardev();
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
