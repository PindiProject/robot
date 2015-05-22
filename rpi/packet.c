#include "packet.h"

#include <stdlib.h>
#include <string.h>

packet* packet_create(unsigned char tag, unsigned char length, unsigned char* data) {
    packet* p = (packet*) malloc(sizeof(packet));
    p->value = (unsigned char*) malloc(sizeof(unsigned char) * length);
    
    p->tag = tag;
    p->length = length;
    memcpy(p->value, data, length);
    
    return p;
}

void packet_destroy(packet* p) {
    free(p->value);
    free(p);
}

unsigned char* packet_pack(packet** packets, int packets_len, int* buffer_len) {
    int packet_index = 0;
    *buffer_len = 0;
    
    while(packet_index < packets_len) {
        packet* p = packets[packet_index];
        unsigned char length;
        length = p->length;
        *buffer_len += length + 2;
        packet_index += 1;
    }
    
    unsigned char* buffer = (unsigned char*) malloc(sizeof(unsigned char) * (*buffer_len));
    int buffer_index = 0;
    packet_index = 0;
    
    while(packet_index < packets_len) {
        packet* p = packets[packet_index];
        buffer[buffer_index] = p->tag;
        buffer[buffer_index + 1] = p->length;
        memcpy(buffer + buffer_index + 2, p->value, p->length);

        buffer_index += p->length + 2;
        packet_index += 1;
    }
    return buffer;
}

packet** packet_unpack(unsigned char* buffer, int buffer_len, int* total_read) {
    *total_read = 0;
    int index = 0;
    packet** list = (packet**) malloc(sizeof(packet*) * MAX_PACKETS);
    
    while(index < buffer_len) {
        unsigned char tag = buffer[index];
        unsigned char length = buffer[index + 1];
        unsigned char* value = buffer + 2;
        
        packet* p = packet_create(tag, length, value);
        
        list[*total_read] = p;
        *total_read += 1;
        index += 2 + length;
    }
    
    return list;
}

packet* packet_find(unsigned char tag, packet** packets, int len) {
    
    int i;
    for(i=0; i< len; i++) {
        if(packets[i]->tag == tag) {
            return packets[i];
        }
    }
    
    return 0;
}

