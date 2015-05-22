#include "def.h"
#include "serial.h"

#include <stdio.h>
#include <stdlib.h>

#define RXTX_BUFFER_SIZE 256

int main() {

    unsigned char* tx_data;
    unsigned char* rx_data;

    rx_data = (unsigned char*) malloc(sizeof(unsigned char) * RXTX_BUFFER_SIZE);
    if(!rx_data) {
        return 1;
    }

    int serial = serial_open();
    printf("Porta: %s (fd: %d)\n", SERIAL_DEVICE_NAME, serial);

    if(serial < 0) {
        printf("Porta serial não pode ser aberta.\n");
        return 2;
    }


    //printf("Esperando dados...\n");
    int bytes_read = 0;
    do {
        bytes_read = serial_read(serial, rx_data, RXTX_BUFFER_SIZE);
        if(bytes_read > 0) {
            printf("Dados lidos: %d bytes.\n", bytes_read);
            int i;

            printf("=================================\n");
            for(i = 0; i< bytes_read; i++) {
                printf("%#2x ", rx_data[i]);
            }
            printf("=================================\n");
        }
    } while(1);

    return 0;
}
