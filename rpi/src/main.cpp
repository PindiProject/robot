#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <unistd.h>

#include "def.hpp"
#include "serial.hpp"
#include "dfs.hpp"
#include "packet.hpp"

using namespace std;

#define RXTX_BUFFER_SIZE 256

void print_bytes(byte* buff, int len)
{

  for (int i = 0; i < len; i++)
  {
    cout << (int) buff[i] << " ";
  }

  cout << endl;
}

int main()
{
  cout << "Init" << endl;

  byte* tx_data;
  byte* rx_data;

  rx_data = new byte[RXTX_BUFFER_SIZE];
  if(!rx_data)
  {
    return 1;
  }

  int fd = serial_open();

  if(fd < 0)
  {
    cout << "Could not open serial" << endl;
    return 1;
  }

  cout << "Serial opened: fd=" << fd << endl;
  /* handshake*/
  // send: TAG_CMD, 1, CMD_WAKEUP
  tx_data = new byte[1];
  tx_data[0] = CMD_WAKEUP; //TAG_CMD;
//    tx_data[1] = 1;
  //  tx_data[2] = CMD_WAKEUP;

  cout << "Sending handshake" << endl;
  byte* ack = new byte[1];
  ack[0] = 0;

  while(ack[0] == 0)
  {
    cout << "Trying..." << endl;
    serial_write(fd, tx_data, 1);
    usleep(1000);
    serial_read(fd, ack, 1);
  }

  delete[] ack;

  DepthFirstSearch* dfs = new DepthFirstSearch();
  dfs->init(25, 25); // TODO

  /* main loop */

  while(true)
  {
    // read: TAG_DEVICE_STATE, 1, STATE_WAITING_CMD
    //       TAG_SENSOR_DATA, 2, SENSOR_0, distancia
    //       TAG_SENSOR_DATA, 2, SENSOR_1, obstáculo
    cout << "Waiting data..." << endl;
    int bytes_read = serial_read(fd, rx_data, RXTX_BUFFER_SIZE, 9);

    if(bytes_read != 9)
    {
      // alguma coisa deu errado
      continue;
    }

    cout << "Bytes lidos: ";
    print_bytes(rx_data, bytes_read);

    packet* pkt_distance = packet_create(rx_data[0], rx_data[1], rx_data + 2);
    packet* pkt_obstacle = packet_create(rx_data[5], rx_data[6], rx_data + 7);

    //TODO verificar integridade dos pacotes
    bool obstacle = pkt_obstacle->value[1] == 49;

    unsigned char b_low, b_high;
    b_high = pkt_distance->value[1];
    b_low = pkt_distance->value[2];
 
    int distance = (b_high << 8) | b_low;
    //cout << "obst: " << obstacle << ": " << (int) pkt_obstacle->value[3] << endl;
    //cout << "dist: " << distance << ": bh=" << (int) b_high << "; bl=" << (int) b_low << endl;

    // processa no algoritmo: tocado pelo pai
    byte cmd_direction = dfs->move(obstacle, distance) & 0xFF;
    byte cmd_distance = dfs->getDistance();// & 0XFF;

    cout << "cmd_d:" << (int) cmd_distance << endl;

    //packet_destroy(pkt_state);
    packet_destroy(pkt_distance);
    packet_destroy(pkt_obstacle);

    tx_data = new byte[6];
    tx_data[0] = TAG_CMD;
    tx_data[1] = 1;
    tx_data[2] = cmd_direction;

    tx_data[3] = TAG_DATA;
    tx_data[4] = 1;
    tx_data[5] = cmd_distance;

    serial_write(fd, tx_data, 6);
    print_bytes(tx_data, 6);
    delete[] tx_data;

  }
  return 0;
}
