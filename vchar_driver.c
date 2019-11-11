/*
 * Filename: vchar_driver.c
 * Date: 05/11/2019
 * Description: character driver for a virtual device vchar device
 *              vchar device is a device that located in RAM 
 */
#include<linux/module.h> /* Dinh nghia module_init(), module_exit()*/
#include<linux/fs.h> /*chua cac ham cap phat/giai phong device number*/
#include<linux/device.h> /*cac ham phuc vu viec tao device file*/
#include<linux/slab.h> /* chua cac ham kmalloc va kfree*/
#include<linux/cdev.h> /*chua cac ham lam viec voi cdev*/
#include<linux/uaccess.h> /*chua cac ham trao doi du lieu giua user vs kernel*/
#include<linux/ioctl.h> /* chua cac ham phuc vu cho ioctl */
#include"vchar_driver.h" /*thu vien mo ta cac thanh ghi cua thiet bi*/

#define DRIVER_AUTHOR "Bui Viet Anh"
#define DRIVER_VERSION "1.0"
#define DRIVER_DESC "The simple character device"
/*Dinh nghia 4 ma lenh danh cho IOCTL*/
#define MAGICAL_NUMBER 243
#define VCHAR_CLR_DATA_REGS _IO(MAGICAL_NUMBER, 0)
#define VCHAR_GET_STS_REGS  _IOR(MAGICAL_NUMBER, 1, sts_regs_t *)
#define VCHAR_SET_RD_DATA_REGS _IOW(MAGICAL_NUMBER, 2, unsigned char *)
#define VCHAR_SET_WR_DATA_REGS _IOW(MAGICAL_NUMBER, 3, unsigned char *)

typedef struct {
    unsigned char read_count_h_reg;
    unsigned char read_count_l_reg;
    unsigned char write_count_h_reg;
    unsigned char write_count_l_reg;
    unsigned char device_status_reg;
}sts_regs_t;

typedef struct vchar_dev {
    unsigned char * control_regs;
    unsigned char * status_regs;
    unsigned char * data_regs;
}vchar_dev_t;

struct _vchar_drv {
    dev_t dev_num;
    struct class *dev_class;
    struct device *dev;
    vchar_dev_t * vchar_hw;
    struct cdev *vcdev;
    unsigned int open_cnt;
}vchar_drv;

/*************************************Device Specific - START***************************************/

/*Ham khoi tao thiet bi */
int vchar_hw_init(vchar_dev_t *hw)
{
    char * buf;
    buf = kzalloc(NUM_DEV_REGS * REG_SIZE, GFP_KERNEL);
    if(!buf)
    {
        return -ENOMEM;
    }
    hw->control_regs = buf;
    hw->status_regs = hw->control_regs + NUM_CTRL_REGS;
    hw->data_regs = hw->status_regs + NUM_STS_REGS;
    /*khoi tao gia tri ban dau cho cac thanh ghi*/
    hw->control_regs[CONTROL_ACCESS_REG] = 0x03;
    hw->status_regs[DEVICE_STATUS_REG] = 0x03;
    return 0;
}
/*Ham giai phong thiet bi*/
void vchar_hw_exit(vchar_dev_t *hw)
{
    kfree(hw->control_regs);
}
/*Ham doc tu cac thanh ghi du lieu cua thiet bi*/
int vchar_hw_read_data(vchar_dev_t *hw, int start_reg, int num_regs, char *kbuf)
{
    int read_bytes = num_regs;
    /* Kiem tra xem co quyen READ khong*/
    if((hw->control_regs[CONTROL_ACCESS_REG] & CTRL_READ_DATA_BIT) == DISABLE)
    {
        return -1;
    }

    /*Kiem tra xem dia chi cua kernel buff co hop le khong*/
    if(NULL == kbuf)
    {
        return -1;
    }
    /*Kiem tra xe vi tri cua cac thanh ghi can doc co hop ly khong*/
    if(start_reg >  NUM_DATA_REGS)
    {
        return -1;
    }
    /*Dieu chinh lai so luong thanh ghi du lieu can doc (neu can thiet)*/
    if(num_regs > (NUM_DATA_REGS - start_reg))
    {
        read_bytes = NUM_DATA_REGS - start_reg;
    }
    /*ghi du lieu tu kernel buffer vao cac thanh ghi du lieu*/
    memcpy(kbuf, hw->data_regs + start_reg, read_bytes);

    /*cap nhat so lan doc tu cac thanh ghi du lieu*/
    hw->status_regs[READ_COUNT_L_REG] += 1;
    if(0 == hw->status_regs[READ_COUNT_L_REG])
    {
        hw->status_regs[READ_COUNT_H_REG] += 1;
    }

    return read_bytes;

}
/*Ham ghi vao cac thanh ghi du lieu cua thiet bi*/
int vchar_hw_write_data(vchar_dev_t *hw, int start_reg, int num_regs, char *kbuf)
{
    int write_bytes = num_regs;
    /*Kiem tra xem co quyen ghi du lieu khong*/
    if((hw->control_regs[CONTROL_ACCESS_REG] & CTRL_WRITE_DATA_BIT) == DISABLE )
        return -1;
    /*kiem tra xem dia chi cua kernel buffer co hop le khong*/
    if(NULL == kbuf)
        return -1;
    /*Kiem tra xem vi tri cua cac thanh ghi co hop le khong */
    if(start_reg > NUM_DATA_REGS)
        return -1;
    /*Dieu chinh lai so luong thanh ghi du lieu can ghi (neu can thiet)*/
    if(num_regs > (NUM_DATA_REGS - start_reg))
    {
        write_bytes = NUM_DATA_REGS - start_reg;
        hw->status_regs[DEVICE_STATUS_REG] |= STS_DATAREGS_OVERFLOW_BIT;
    }
    /*Ghi du lieu vao cac thanh ghi tu kbuf*/
    memcpy(hw->data_regs + start_reg, kbuf, write_bytes);
    /*cap nhat so len ghi vao cac thanh ghi du lieu*/
    hw->status_regs[WRITE_COUNT_L_REG] += 1;
    return write_bytes;
}
int vchar_hw_clear_data(vchar_dev_t *hw)
{
    if((hw->control_regs[CONTROL_ACCESS_REG] & CTRL_WRITE_DATA_BIT) == DISABLE)
    {
        return -1;
    }
    memset(hw->data_regs, 0, NUM_DATA_REGS * REG_SIZE);
    hw->status_regs[DEVICE_STATUS_REG] &= ~STS_DATAREGS_OVERFLOW_BIT;
    return 0;
}
/*Ham doc tu cac thanh ghi trang thai cua thiet bi*/
void vchar_hw_get_status(vchar_dev_t *hw, sts_regs_t *status)
{
    memcpy(status, hw->status_regs, NUM_STS_REGS * REG_SIZE);
}
/*Ham cho phep doc tu cac thanh ghi du lieu tu thiet bi*/
void vchar_hw_enable_read(vchar_dev_t *hw, unsigned char isEnable)
{
    if(isEnable == ENABLE)
    {
        /*Dieu khien cho phep doc tu cac thanh ghi du lieu*/
        hw->control_regs[CONTROL_ACCESS_REG] |= CTRL_READ_DATA_BIT;
        /*Cap nhat trang thai co the doc*/
        hw->status_regs[DEVICE_STATUS_REG] |= STS_READ_ACCESS_BIT;
    }
    else
    {
        /*Khong cho phep doc du lieu tu cac thanh ghi du lieu*/
        hw->control_regs[CONTROL_ACCESS_REG] &= ~CTRL_READ_DATA_BIT;
        /*Cap nhat trang thai "khong the doc"*/
        hw->status_regs[DEVICE_STATUS_REG] &= ~STS_READ_ACCESS_BIT;
    }
}
/*Ham cho phep ghi du lieu*/
void vchar_hw_enable_write(vchar_dev_t *hw, unsigned char isEnable)
{
    if(isEnable == ENABLE)
    {
        hw->control_regs[CONTROL_ACCESS_REG] |= CTRL_WRITE_DATA_BIT;
        hw->status_regs[DEVICE_STATUS_REG] |= STS_WRITE_ACCESS_BIT;
    }
    else 
    {
        hw->control_regs[CONTROL_ACCESS_REG] &= ~CTRL_WRITE_DATA_BIT;
        hw->status_regs[DEVICE_STATUS_REG] &= ~STS_WRITE_ACCESS_BIT;
    }
}
/*Ham ghi vao cac thanh ghi dieu khien cua thiet bi*/
/*Ham xu ly tin hieu ngat tu thiet bi*/


/************************************Device Specific -END*******************************************/


/************************************OS Specific - START********************************************/
/*Cac ham entry points*/
static int vchar_driver_open(struct inode *inode, struct file *filp)
{
    vchar_drv.open_cnt ++;
    printk(KERN_INFO "Handle opened event [%d]\n", vchar_drv.open_cnt);
    return 0;
}

static int vchar_driver_release(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "Handle closed event\n");
    return 0;
}

static ssize_t vchar_driver_read(struct file *filp, char __user *user_buf, size_t len, loff_t *off)
{
    char *kernel_buf = NULL;
    int num_bytes = 0;
    printk("Handle read event start from %lld, %zu bytes\n", *off, len);
    kernel_buf = kzalloc(len, GFP_KERNEL);
    if(NULL == kernel_buf)
    {
        return 0;
    }
    /*Call device specific function*/
    num_bytes = vchar_hw_read_data(vchar_drv.vchar_hw, *off, len, kernel_buf);
    printk(KERN_INFO "read %d bytes from HW\n", num_bytes);

    if(num_bytes < 0)
    {
        return -EFAULT;
    }
    if(copy_to_user(user_buf, kernel_buf, num_bytes))
    {
        return -EFAULT;
    }
    *off+= num_bytes;
    return num_bytes;
}
static long vchar_driver_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int ret = 0;
    printk("Handle ioctl event (cmd: %u)\n",cmd);

    switch(cmd)
    {
        case VCHAR_CLR_DATA_REGS:
        {
            ret = vchar_hw_clear_data(vchar_drv.vchar_hw);
            if(ret < 0)
            {
                printk(KERN_ALERT "Cannot clear data register\n");
            }
            else 
            {
                printk(KERN_INFO "Data registers has been cleared\n");
            }
            break;
        }
        case VCHAR_SET_RD_DATA_REGS:
        {
            unsigned char isReadEnable;
            copy_from_user(&isReadEnable, (unsigned char *)arg, sizeof(isReadEnable));
            vchar_hw_enable_read(vchar_drv.vchar_hw, isReadEnable);
            printk(KERN_INFO "Data registers have been %s to read\n", (isReadEnable == ENABLE)?"enable":"disable");
            break;
        }
        case VCHAR_SET_WR_DATA_REGS:
        {
            unsigned char isWriteEnable;
            copy_from_user(&isWriteEnable, (unsigned char *)arg, sizeof(isWriteEnable));
            vchar_hw_enable_write(vchar_drv.vchar_hw, isWriteEnable);
            printk(KERN_INFO "Data registers have been %s to write\n", (isWriteEnable == ENABLE)?"enable":"disable");
            break;
        }
        case VCHAR_GET_STS_REGS:
        {
            sts_regs_t status;
            vchar_hw_get_status(vchar_drv.vchar_hw, &status);
            copy_to_user((sts_regs_t *)arg, &status, sizeof(status));
            printk(KERN_INFO "Got infomation from status register\n");
            break;
        }
    }
    return ret;
}

static ssize_t vchar_driver_write(struct file *filp, const char __user *user_buf, size_t len, loff_t *off)
{
    char *kernel_buf = NULL;
    int num_bytes = 0;
    printk(KERN_INFO "Handle write event from %lld, %zu bytes\n", *off, len);

    kernel_buf = kzalloc(len, GFP_KERNEL);
    if(copy_from_user(kernel_buf, user_buf, len))
    {
        return -EFAULT;
    }
    num_bytes = vchar_hw_write_data(vchar_drv.vchar_hw, *off, len, kernel_buf);
    printk("Write %d bytes to HW\n", num_bytes);
    if(num_bytes < 0)
    {
        return -EFAULT;
    }
    *off += num_bytes;
    return num_bytes;
}

static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .open = vchar_driver_open,
    .release = vchar_driver_release,
    .read = vchar_driver_read,
    .write = vchar_driver_write,
    .unlocked_ioctl = vchar_driver_ioctl,
};
/************************************OS Specific - END********************************************/

/*Khoi tao driver*/

static int __init vchar_driver_init(void)
{
    int ret = 0;

    /*Cap phat device number*/
    vchar_drv.dev_num = 0;

    ret = alloc_chrdev_region(&vchar_drv.dev_num, 0, 1, "vchar_device");
    if(ret < 0)
    {
        printk(KERN_ERR "Failed to register device number statically\n");
        goto failed_register_devnum;
    }
    /*Tao device file*/
    vchar_drv.dev_class = class_create(THIS_MODULE, "class_vchar_dev");
    if(NULL == vchar_drv.dev_class)
    {
        printk("Failed to create a device class\n");
        goto failed_create_class;
    }
    vchar_drv.dev = device_create(vchar_drv.dev_class, NULL, vchar_drv.dev_num, NULL, "vchar_dev");
    if(IS_ERR(vchar_drv.dev))
    {
        printk("Failed to create a device\n");
        goto failed_create_device;
    }

    /*Cap phat bo nho cho cac cau truc du lieu cua driver va khoi tao*/
    vchar_drv.vchar_hw = kzalloc(sizeof(vchar_dev_t), GFP_KERNEL);
    if(!vchar_drv.vchar_hw)
    {
        printk(KERN_ALERT "Failed to allocate data structute of the driver\n");
        ret = - ENOMEM;
        goto failed_allocate_structure;
    }

    /*Khoi tao thiet bi vat ly*/
    ret = vchar_hw_init(vchar_drv.vchar_hw);
    if(ret < 0)
    {
        printk(KERN_ALERT "Failed to initialize a virtual character device\n");
        goto failed_init_hw;
    }
    /*Dang ky cac entry point voi kernel*/
    vchar_drv.vcdev = cdev_alloc();
    if(NULL == vchar_drv.vcdev)
    {
        printk(KERN_ALERT "Failed to allocate cdev struct\n");
        goto failed_allocate_cdev;
    }
    cdev_init(vchar_drv.vcdev, &fops);
    ret = cdev_add(vchar_drv.vcdev, vchar_drv.dev_num, 1);
    if(ret < 0)
    {
        printk(KERN_ALERT "Failed to add a char device to the system\n");
        goto failed_allocate_cdev;
    }
    /*Dang ky ham xu ly ngat*/
    printk(KERN_INFO "Initialize vchar driver successfully!\nAllocate device number(%d, %d)\n", MAJOR(vchar_drv.dev_num), MINOR(vchar_drv.dev_num));
    return 0;
failed_allocate_cdev:
    vchar_hw_exit(vchar_drv.vchar_hw);
failed_init_hw:
    kfree(vchar_drv.vchar_hw);
failed_allocate_structure:
    device_destroy(vchar_drv.dev_class, vchar_drv.dev_num);
failed_create_device:
    class_destroy(vchar_drv.dev_class);
failed_create_class:
    unregister_chrdev_region(vchar_drv.dev_num,1);
failed_register_devnum:
    return ret;
}
static void __exit vchar_driver_exit(void)
{
    /*huy dang ky xu ly ngat */
    /*Huy dang ky cac entry point*/
    cdev_del(vchar_drv.vcdev);
    /*Giai phong thiet bi vat ly*/
    vchar_hw_exit(vchar_drv.vchar_hw);
    /*Giai phong bo nho da cap phat cho cac cau truc cua driver*/
    kfree(vchar_drv.vchar_hw);
    /*Xoa bo device file*/
    device_destroy(vchar_drv.dev_class, vchar_drv.dev_num);
    class_destroy(vchar_drv.dev_class);
    /*Xoa bo device number*/
    unregister_chrdev_region(vchar_drv.dev_num, 1);

    printk(KERN_INFO "Exit vchar driver\n");
}

module_init(vchar_driver_init);
module_exit(vchar_driver_exit);
MODULE_LICENSE("GPL"); /* giay phep su dung cua module */
MODULE_AUTHOR(DRIVER_AUTHOR); /* tac gia cua module */
MODULE_DESCRIPTION(DRIVER_DESC); /* mo ta chuc nang cua module */
MODULE_VERSION(DRIVER_VERSION); /* mo ta phien ban cuar module */
MODULE_SUPPORTED_DEVICE("testdevice"); /* kieu device ma module ho tro */
