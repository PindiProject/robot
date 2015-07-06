#ifndef _DEF_H
#define _DEF_H

typedef unsigned char byte;

/****************************************
 *     Identificadores dos pacotes      *
 ****************************************/
#define TAG_DEVICE_STATE    0x10
#define TAG_SENSOR_DATA     0x11
#define TAG_BATTERY_LEVEL   0x12
#define TAG_BIN_STATUS      0x13
#define TAG_CMD             0x14
#define TAG_DATA            0x15

/****************************************
 *        Macros para os pacotes        *
 ****************************************/

/* Identificadores dos sensores: 1 byte */
#define SENSOR_0 0xA0  // distancia
#define SENSOR_1 0xA1  // obstaculo
#define SENSOR_2 0xA2
#define SENSOR_3 0xA3
#define SENSOR_4 0xA4
#define SENSOR_5 0xA5
#define SENSOR_6 0xA6
#define SENSOR_7 0xA7

/* Identificadores de dados */
#define DATA_DISTANCE 0xE0

/* Identificadores de estado: 1 byte */
#define STATE_UNKNOWN           0xB0
#define STATE_MOVING_FORWARD    0xB1
#define STATE_MOVING_BACKWARD   0xB2
#define STATE_TURNING_RIGHT     0xB3
#define STATE_TURNING_LEFT      0xB4
#define STATE_STOPPED           0xB5
#define STATE_WAITING_CMD       0xB6

/* Níveis de bateria */
#define BATTERY_100 0xC0
#define BATTERY_80  0xC1
#define BATTERY_60  0xC2
#define BATTERY_40  0xC3
#define BATTERY_20  0xC4
#define BATTERY_10  0xC5
#define BATTERY_5   0xC6
#define BATTERY_0   0xC7

/* Comandos*/
#define CMD_KEEP_STATE    0x00
#define CMD_MOVE_FORWARD  0x01
#define CMD_MOVE_BACKWARD 0x02
#define CMD_TURN_RIGHT    0x03
#define CMD_TURN_LEFT     0x04
#define CMD_STOP          0x05
#define CMD_WAKEUP        0x06
#define CMD_START         0x07

#endif
