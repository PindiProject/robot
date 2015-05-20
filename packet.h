#ifndef _PACKET_H
#define _PACKET_H

#define MAX_PACKETS 16

struct _packet {
    unsigned char tag;
    unsigned char length;
    unsigned char* value;
};

typedef struct _packet packet;

/* Cria o pacote com o rótulo, tamanho e valoe especificado */
packet* packet_create(unsigned char tag, unsigned char length, unsigned char* data);

/* Destrói um pacote da memória */
void packet_destroy(packet* p);

/* Lê um buffer e cria uma array de pacotes. O número de paootes é salvo em total_read */
packet** packet_unpack(unsigned char* buffer, int len, int* total_read);

/* Copia os pacotes para um buffer */
unsigned char* packet_pack(packet** packets, int packets_len, int* buffer_len);

/* Procura um pacote dentro de uma lista */
packet* packet_find(unsigned char tag, packet** packets, int len);

#endif
