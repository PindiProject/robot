#include "serial.h"

#include <fcntl.h>
#include <cstring>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <iostream>

using namespace std;

typedef unsigned char byte;

int _set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                // TODO adicionar algum erro aqui
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                //error_message ("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

void _set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                //TODO adicionar algum erro aqui
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0) {
            // TODO adicionar algum erro aqui

        }
}


int serial_open(std::string name) {
    const char* port_name = name.c_str();
    int fd = open (port_name, O_RDWR | O_NOCTTY | O_SYNC);

    if (fd < 0) {
        return -1;
    }

    _set_interface_attribs (fd, SERIAL_DEVICE_BAUDRATE, SERIAL_DEVICE_PARITY);
    _set_blocking (fd, SERIAL_DEVICE_BLOCKING);

    return fd;
}
// Nota: usleep
// Cada char demora ~100us para ser entregue. *Idealmente* deveria receber
// um ACK da placa, mas como esse código está longe do ideal, vai ser assim
// mesmo.

int serial_write(int fd, unsigned char* data, int data_size) {
    write(fd, data, data_size);
    usleep(data_size * 100);
    return 1;
}

int serial_read(int fd, unsigned char* buffer, int buffer_size, int max) {
    usleep(buffer_size * 100);
    memset(buffer, 0, buffer_size);

    if(max == -1) {
        int total_read = read(fd, buffer, buffer_size);        
        return total_read;
    }
    else {
        int total_read = 0;
        byte* tmp = new byte[buffer_size];

        while(total_read < max) {
            int bytes_read = read(fd, tmp, buffer_size);
            cout << "Entrou aqui :" << bytes_read << endl;
            memcpy(buffer + total_read, tmp, bytes_read);
            total_read += bytes_read;
        }
        return total_read;
    }

}
