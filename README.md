# vchar_learn_device_driver

Tạo và khởi tạo 1 thiết bị ảo nằm trên RAM, và viết driver để điều khiển đọc ghi thiết bị đó

```
1. make
2. sudo dmesg -C
3. sudo insmod vchar_driver.ko
4. dmesg
5. sudo chmod 666 /dev/vchar_dev
6. ./app_test
7. dmes
```
Enjoy :.>


Can check module by command:
```
lsmod
```
Check major number: 
```
cat /proc/devices
```
