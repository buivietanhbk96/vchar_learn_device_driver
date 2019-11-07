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
#include<linux/cdev.h>
#include"vchar_driver.h" /*thu vien mo ta cac thanh ghi cua thiet bi*/

#define DRIVER_AUTHOR "Bui Viet Anh"
#define DRIVER_VERSION "1.0"
#define DRIVER_DESC "The simple character device"

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
/*Ham ghi vao cac thanh ghi du lieu cua thiet bi*/
/*Ham doc tu cac thanh ghi trang thai cua thiet bi*/
/*Ham ghi vao cac thanh ghi trang thai cua thiet bi*/
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

static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .open = vchar_driver_open,
    .release = vchar_driver_release,
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
