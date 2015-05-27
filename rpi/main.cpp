#include "def.h"
#include "serial.h"
#include "dfs.h"

#include <stdio.h>
#include <stdlib.h>

#define RXTX_BUFFER_SIZE 256

int main() {

    byte* tx_data;
    byte* rx_data;

    rx_data = new byte[RXTX_BUFFER_SIZE];
    if(!rx_data) {
        return 1;
    }
    
    int fd = int serial_open();
    /* handshake*/
    // send: TAG_CMD, 1, CMD_WAKEUP
    tx_data = new byte[3];
    tx_data[0] = TAG_CMD;
    tx_data[1] = 1;
    tx_data[3] = CMD_WAKEUP;
    
    serial_write(fd, tx_data, 3);
    
    DepthFirstSearch* dfs = new DepthFirstSearch();
    dfs->init(0,0); // TODO

    /* main loop */
    
    while(true) {
        // read: TAG_DEVICE_STATE, 1, STATE_WAITING_CMD
        //       TAG_SENSOR_DATA, 2, SENSOR_0, distancia
        //       TAG_SENSOR_DATA, 2, SENSOR_1, obstáculo
        int bytes_read = serial_read(fd, rx_data, RXTX_BUFFER_SIZE, 11);
        if(bytes_read != 11) {
            // alguma coisa deu errado
            continue;
        }
        
        packet* pkt_state = packet_create(rx_data[0], rx_data[1], rx_data + 2);
        packet* pkt_distance = packet_create(rx_data[3], rx_data[4], rx_data + 5);
        packet* pkt_obstacle = packet_create(rx_data[7], rx_data[8], rx_data + 9);
        
        //TODO verificar integridade dos pacotes
        
        bool obstacle = pkt_obstacle->value[1] == 1;
        
        
        
    
        // send: TAG_CMD, 1, CMD_*
        //       TAG_DATA, 2, DATA_DISTANCE, distancia
    }
    return 0;
}
