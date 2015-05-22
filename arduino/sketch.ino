#include "packet.h"
#include "def.h"

#define SERIAL_BAUDRATE 57600
#define RXTX_BUFFER_SIZE 256

unsigned char* buffer;

void process(unsigned char cmd) {

}

void setup() {
    Serial.begin(SERIAL_BAUDRATE);
}

void loop() {
    // ler serial
    int len = Serial.read();

    if(len != 3) {
        // alguma coisa deu errado. O arduíno sempre espera o pacote de comando
        // com três bytes
    }

    // ler os próximos len bytes da serial
    buffer = (unsigned char*) malloc(len * sizeof(unsigned char));
    for(int i=0; i< len; i++) {
        buffer[i] = Serial.read();
    }

    // a placa só espera o comando de status, então é apenas um pacote
    unsigned char tag = buffer[0];
    unsigned char length = buffer[1];
    unsigned char* value = buffer + 2;
    packet* p = packet_create(tag, length, value);
    free(buffer);

    if(p->tag == TAG_CMD) {
        process(p->value[0]);
    }
    else {
        // Não chegou um pacote de comando: ou o pacote veio quebrado ou é
        // um pacote não suportado.
    }

    packet_destroy(p);


    delay(200);

    // como precisamos de um ponteiro, criei essa array para facilitar a vida
    unsigned char* data = {
        random(STATE_MOVING_FORWARD, STATE_WAITING_CMD + 1),

        SENSOR_0,
        random(0,256),

        SENSOR_1,
        random(0,256)
    };

    // criar uma array de ponteiros para os pacotes
    packet** packets = (packet**) malloc(sizeof(packet*) * 3);

    // empacotar os dados em cada pacote
    packets[0] = packet_create(TAG_DEVICE_STATE, 1, data + 0);
    packets[1]  = packet_create(TAG_SENSOR_DATA, 2, data + 1);
    packets[2]  = packet_create(TAG_SENSOR_DATA, 2, data + 3);

    // criar um segmento de memória com todos os pacotes
    // o tamanho resultante fica em tx_len
    int tx_len;
    unsigned char* tx_data;
    tx_data = packet_pack(packets, 3, &tx_len);

    // indicar quantos bytes devem ser lidos
    Serial.write(tx_len);

    // escrever os bytes esperados
    Serial.write(tx_data, tx_len);
}
