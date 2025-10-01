#ifndef JINOS_DRIVERS_NETWORK_H
#define JINOS_DRIVERS_NETWORK_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t mac[6];
    uint32_t ipv4;
} net_interface_t;

void network_init(void);
int network_send_packet(const void *data, size_t length);
int network_receive_packet(void *buffer, size_t buffer_length);

#endif /* JINOS_DRIVERS_NETWORK_H */
