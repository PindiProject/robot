#ifndef _SERIAL_H
#define _SERIAL_H

#define SERIAL_DEVICE_NAME "/dev/ttyACM0"
#define SERIAL_DEVICE_BAUDRATE B57600
#define SERIAL_DEVICE_PARITY 0
#define SERIAL_DEVICE_BLOCKING 0

//int set_interface_attribs (int fd, int speed, int parity);
//void set_blocking (int fd, int should_block);
int serial_open();
int serial_write(int fd, unsigned char* data, int data_size);
int serial_read(int fd, unsigned char* buffer, int buffer_size);

#endif
