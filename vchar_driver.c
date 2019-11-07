/*
 * Filename: vchar_driver.c
 * Date: 05/11/2019
 * Description: character driver for a virtual device vchar device
 *              vchar device is a device that located in RAM 
 */
#include<linux/module.h> /* Thu vien nay dinh nghia cac macro nhu module_init(), module_exit()*/
#include<linux/init.h> 
#include<linux/fs.h>
#include<linux/device.h>

#define DRIVER_AUTHOR "Bui Viet Anh"
#define DRIVER_VERSION "1.0"
#define DRIVER_DESC "The simple character device"

struct _vchar_drv {
    dev_t dev_num;
    struct class *dev_class;
    struct device *dev;
}vchar_drv;

/*************************************Device Specific - START***************************************/

/*Ham khoi tao thiet bi */
/*Ham giai phong thiet bi*/
/*Ham doc tu cac thanh ghi du lieu cua thiet bi*/
/*Ham ghi vao cac thanh ghi du lieu cua thiet bi*/
/*Ham doc tu cac thanh ghi trang thai cua thiet bi*/
/*Ham ghi vao cac thanh ghi trang thai cua thiet bi*/
/*Ham xu ly tin hieu ngat tu thiet bi*/


/************************************Device Specific -END*******************************************/


/************************************OS Specific - START********************************************/
/*Cac ham entry points*/


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
    /*Khoi tao thiet bi vat ly*/
    /*Dang ky cac entry point voi kernel*/
    /*Dang ky ham xu ly ngat*/
    printk(KERN_INFO "Initialize vchar driver successfully!\nAllocate device number(%d, %d)\n", MAJOR(vchar_drv.dev_num), MINOR(vchar_drv.dev_num));
    return 0;
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
    /*Giai phong thiet bi vat ly*/
    /*Giai phong bo nho da cap phat cho cac cau truc cua driver*/
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
