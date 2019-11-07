/*
 * Filename: vchar_driver.h
 * Author: vietanh9026@gmail.com
 * Date: 07/11/2019
 * Desc: chua cac dinh nghia mo ta thiet bi gia lap vchar_dev
 *       vchar_device la mot thiet bi gia, va no nam tren RAM
 */
#ifndef _VCHAR_DRIVER_H_
#define _VCHAR_DRIVER_H_

#define REG_SIZE 1 /*kich thuoc 1 thanh ghi la 1 byte*/
#define NUM_CTRL_REGS 1 /*so thanh ghi dieu khien cua thiet bi*/
#define NUM_STS_REGS 5 /*so thanh ghi trang thai cua thiet bi*/
#define NUM_DATA_REGS 256 /*so thanh ghi du lieu cua thiet bi*/

#define NUM_DEV_REGS (NUM_CTRL_REGS + NUM_STS_REGS + NUM_DATA_REGS) /*tong so thanh ghi*/
/********************Mo ta cac thanh ghi trang thai: START*******************/
/*
 * Cap thanh ghi [READ_COUNT_H_REG:READ_COUNT_L_REG]:
 * - gia tri luc khoi tao: 0x0000
 * - moi lan doc thanh cong tu cac thanh ghi du lieu, tang them 1
 */
#define READ_COUNT_H_REG 0
#define READ_COUNT_L_REG 1


/*
 * cap thanh ghi [WRITE_COUNT_H_REG: WRITE_COUNT_L_REG]:
 * - gia tri luc khoi tao: 0x0000
 * - moi lan ghi thanh cong vao cac thanh ghi du lieu, tang them 1
 */
#define WRITE_COUNT_H_REG 2
#define WRITE_COUNT_L_REG 3

/*
 * thanh ghi DEVICE_STATUS_REG:
 * - gia tri khoi tao: 0x03
 * - y nghia cua cac bit:
 *  bit 0:
 *       + 0: cho biet cac thanh ghi du lieu dang khong ready de READ
 *       + 1: cho biet cac thanh ghi du lieu ready de READ
 *  bit 1:
 *       + 0: cho biet cac thanh ghi du lieu dang khong ready de WRITE
 *       + ....
 *  bit 2: 
 *       + 0: khi cac thanh ghi du lieu bi xoa, bit nay se dc set = 0
 *       + 1: khi toan bo cac thanh ghi du lieu da bi WRITE, bit nay set = 1
 *  bit 3~7: chua dung toi
 */
#define DEVICE_STATUS_REG 4

#define STS_READ_ACCESS_BIT (1<<0)
#define STS_WRITE_ACCESS_BIT (1<<1)
#define STS_DATAREGS_OVERFLOW_BIT (1<<2)

#define READY 1
#define NOT_READY 0
#define OVER_FLOW 1
#define NOT_OVER_FLOW 0

/***************************Mo ta cac thanh ghi trang thai: END***************/
/*************************Mo ta cac thanh ghi dieu khien: START*****************/
/*
 * thanh ghi CONTROL_ACCESS_REG:
 * Vai tro: chua cac bi dieu khien kha nang doc/ghi cac thanh ghi du lieu
 * Gia tri luc khoi tao: 0x03
 * bit 0:
 *        0: khong cho phep READ
 *        1: cho phep READ
 * bit 1:
 *        0: khong cho phep WRITE
 *        1:cho phep WRITE
 * bit 2~7: chua dung toi
 */

#define CONTROL_ACCESS_REG 0
#define CTRL_READ_DATA_BIT (1<<0)
#define CTRL_WRITE_DATA_BIT (1<<1)

#define ENABLE 1
#define DISABLE 0
/************************Mo ta cac thanh ghi dieu khien: END********************/
#endif /*_VCHAR_DRIVER_H_*/
