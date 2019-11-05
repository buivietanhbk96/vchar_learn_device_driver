/*
 * Filename: vchar_driver.c
 * Date: 05/11/2019
 * Description: character driver for a virtual device vchar device
 *              vchar device is a device that located in RAM 
 */
#include<linux/module.h> /* Thu vien nay dinh nghia cac macro nhu module_init(), module_exit()*/
#include<linux/init.h> 

#define DRIVER_AUTHOR "Bui Viet Anh"
#define DRIVER_VERSION "1.0"
#define DRIVER_DESC "The simple character device"

/*************************************Device Specific - START***************************************/

/*Ham khoi tao thiet bi*/
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
    /*Cap phat device number*/
    /*Tao device file*/
    /*Cap phat bo nho cho cac cau truc du lieu cua driver va khoi tao*/
    /*Khoi tao thiet bi vat ly*/
    /*Dang ky cac entry point voi kernel*/
    /*Dang ky ham xu ly ngat*/
    printk(KERN_INFO "Initialize vchar driver successfully!");
    return 0;
}
static void __exit vchar_driver_exit(void)
{
    /*huy dang ky xu ly ngat */
    /*Huy dang ky cac entry point*/
    /*Giai phong thiet bi vat ly*/
    /*Giai phong bo nho da cap phat cho cac cau truc cua driver*/
    /*Xoa bo device file*/
    /*Xoa bo device number*/

    printk(KERN_INFO "Exit vchar driver");
}

module_init(vchar_driver_init);
module_exit(vchar_driver_exit);
MODULE_LICENSE("GPL"); /* giay phep su dung cua module */
MODULE_AUTHOR(DRIVER_AUTHOR); /* tac gia cua module */
MODULE_DESCRIPTION(DRIVER_DESC); /* mo ta chuc nang cua module */
MODULE_VERSION(DRIVER_VERSION); /* mo ta phien ban cuar module */
MODULE_SUPPORTED_DEVICE("testdevice"); /* kieu device ma module ho tro */
