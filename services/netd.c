#include "services.h"

#include "../drivers/network.h"
#include "../kernel/console.h"
#include "../kernel/scheduler.h"

#include <stdint.h>

void networking_daemon_entry(void) {
    console_write_line("[netd] networking daemon online");

    uint8_t probe_packet[16];
    for (uint8_t i = 0; i < sizeof(probe_packet); ++i) {
        probe_packet[i] = i;
    }

    for (;;) {
        int sent = network_send_packet(probe_packet, sizeof(probe_packet));
        if (sent < 0) {
            console_write_line("[netd] failed to queue probe packet");
        } else {
            uint8_t response[16];
            int received = network_receive_packet(response, sizeof(response));
            if (received > 0) {
                console_write_line("[netd] received loopback packet");
            }
        }
        scheduler_yield();
    }
}
