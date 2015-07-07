#include "def.h"
#include "serial.h"
#include "dfs.h"
#include "packet.h"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <unistd.h>
#include <alsa/asoundlib.h>
#include <math.h>

using namespace std;

#define RXTX_BUFFER_SIZE 256
#define ALSA_PCM_NEW_HW_PARAMS_API

void print_bytes(byte* buff, int len) {

	for (int i = 0; i < len; i++) {
        cout << (int) buff[i] << " ";
	}

    cout << endl;
}

int clap_detect() {
  int rc = 0;
  snd_pcm_t * handle = 0;
  snd_pcm_hw_params_t * params = 0;
  unsigned int val = 0;
  int dir = 0;
  snd_pcm_uframes_t frames;
  float * buffer = 0;
  float seconds = 1;
  int size = seconds * 44100;
  int flag_turn = -1;

  /* Open PCM device for recording (capture). */
  rc = snd_pcm_open(&handle, "plughw:1",
                    SND_PCM_STREAM_CAPTURE, 0);
  if (rc < 0) {
    fprintf(stderr, 
            "unable to open pcm device: %s\n",
            snd_strerror(rc));
    exit(1);
  }
  
  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);

  /* Fill it in with default values. */
  rc = snd_pcm_hw_params_any(handle, params);
if(rc < 0)
{
    fprintf(stderr, 
            "unable to open pcm device: %s\n",
            snd_strerror(rc));
    exit(1);
}

  /* Set the desired hardware parameters. */

  /* Interleaved mode */
  rc = snd_pcm_hw_params_set_access(handle, params,
                      SND_PCM_ACCESS_RW_INTERLEAVED);

   if (rc < 0)
{
	fprintf (stderr, "cannot set access type (%s)\n", snd_strerror (rc));
	exit (1);
	}


  /* Signed 16-bit little-endian format */
  rc = snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_FLOAT);
if (rc < 0)
{
	puts ("endian");
	exit (1);
}

  /* One channels (mono) */
  rc = snd_pcm_hw_params_set_channels(handle, params, 1);
if (rc < 0)
{
	puts ("nono");
	exit (1);
}

  /* 44100 bits/second sampling rate (CD quality) */
  val = 44100;
  rc = snd_pcm_hw_params_set_rate_near(handle, params, 
                                  &val, &dir);
if (rc < 0)
{
	puts ("bits");
	exit (1);
}

  /* Set period size to 32 frames. */
  frames = 32;
  rc = snd_pcm_hw_params_set_period_size_near(handle, 
                              params, &frames, &dir);
if (rc < 0)
{
	puts ("Set");
	exit (1);
}

  /* Use a buffer large enough to hold one period */
  rc = snd_pcm_hw_params_get_period_size(params, &frames, &dir);
if (rc < 0)
{
	puts("buffer");
	exit (1);
}

  buffer = (float *) malloc(size*sizeof(float));

  /* We want to loop for 1 seconds */
  rc = snd_pcm_hw_params_get_period_time(params, &val, &dir);
if (rc < 0)
{
	puts ("buffer");
	exit (1);
}

  snd_pcm_prepare ( handle );

  /* Write the parameters to the driver */
  rc = snd_pcm_hw_params(handle, params);
  if (rc < 0) {
    printf("rc error: %d\n", rc);
    printf("handle %p\n", handle);
    printf("params %p\n", params);

    fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
    exit(1);
  }

  while(1){
    rc = snd_pcm_readi(handle, buffer, size);
    if (rc == -EPIPE) {
      snd_pcm_prepare(handle);
    }

    int window_small_size = (int) (44100*0.0025);

    int slot;
    int slot_small;
    float sum_energy = 0;
    for(slot = 0; slot < size; slot = slot + window_small_size){
      sum_energy = 0;
      for (slot_small = slot; slot_small < slot + window_small_size && 
        slot_small < size; ++slot_small){
        sum_energy = sum_energy + sqrt(buffer[slot_small]*buffer[slot_small]);
      }
      //printf("%f/n", sum_energy);
      if (sum_energy > 15.0){
        flag_turn = flag_turn*(-1);
        if (flag_turn == 1){
            printf("%s\n", "Turn on");
            return 1;
            break;
          }else if (flag_turn == -1){
            printf("%s\n", "Turn off");
            return -1;
            break;
          }
      }
    }
  }

  snd_pcm_drain(handle);
  snd_pcm_close(handle);

  return 0;
}

int main() {    
    cout << "Init" << endl;

    int flag_to_turn_on = -1;
    while(flag_to_turn_on == -1){
        flag_to_turn_on = clap_detect();
    }

    byte* tx_data;
    byte* rx_data;

    rx_data = new byte[RXTX_BUFFER_SIZE];
    if(!rx_data) {
        return 1;
    }
    
    int fd = serial_open();

    if(fd < 0) {
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
    
    while(ack[0] == 0) {
        cout << "Trying..." << endl;
        serial_write(fd, tx_data, 1);
        usleep(1000);
        serial_read(fd, ack, 1);
    }
    
    delete[] ack;
    
    DepthFirstSearch* dfs = new DepthFirstSearch();
    dfs->init(25, 25); // TODO

    /* main loop */
    
    while(true) {
        // read: TAG_DEVICE_STATE, 1, STATE_WAITING_CMD
        //       TAG_SENSOR_DATA, 2, SENSOR_0, distancia
        //       TAG_SENSOR_DATA, 2, SENSOR_1, obstáculo
        cout << "Waiting data..." << endl;
        int bytes_read = serial_read(fd, rx_data, RXTX_BUFFER_SIZE, 9);

        if(bytes_read != 9) {
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
