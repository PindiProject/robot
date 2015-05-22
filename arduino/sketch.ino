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
	buffer = (unsigned char*) malloc(len * sizeof(unsigned char));
	for(int i=0; i< len; i++) {
		buffer[i] = Serial.read();
	}
	
	unsigned char tag = buffer[0];
	unsigned char length = buffer[1];
	unsigned char* value = buffer + 2;
	packet* p = packet_create(tag, length, value);
	free(buffer);
	
	if(p->tag == TAG_CMD) {
		process(p->value[0]);
	}
	else {
		// deu ruim :(
	}

	// escrever serial
}

