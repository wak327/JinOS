#include "network.h"

#include "../kernel/console.h"

#include <stddef.h>

#define LOOPBACK_MAX 1500

static net_interface_t netif;
static uint8_t loopback_buffer[LOOPBACK_MAX];
static size_t loopback_length = 0;

static void log_mac(const uint8_t mac[6]) {
    console_write("[net] MAC: ");
    for (int i = 0; i < 6; ++i) {
        uint8_t high = (mac[i] >> 4) & 0xF;
        uint8_t low = mac[i] & 0xF;
        char hi = (high < 10) ? (char)('0' + high) : (char)('A' + high - 10);
        char lo = (low < 10) ? (char)('0' + low) : (char)('A' + low - 10);
        char mac_text[3] = {hi, lo, 0};
        console_write(mac_text);
        if (i < 5) {
            console_write(":");
        }
    }
    console_newline();
}

static void log_ipv4(uint32_t addr) {
    console_write("[net] IPv4: ");
    for (int i = 0; i < 4; ++i) {
        uint8_t octet = (uint8_t)((addr >> (8 * (3 - i))) & 0xFF);
        char buf[4];
        int idx = 0;
        if (octet == 0) {
            buf[idx++] = '0';
        } else {
            if (octet >= 100) {
                buf[idx++] = (char)('0' + octet / 100);
            }
            if (octet >= 10) {
                buf[idx++] = (char)('0' + (octet / 10) % 10);
            }
            buf[idx++] = (char)('0' + (octet % 10));
        }
        buf[idx] = '\0';
        console_write(buf);
        if (i < 3) {
            console_write(".");
        }
    }
    console_newline();
}

void network_init(void) {
    netif.mac[0] = 0x02;
    netif.mac[1] = 0x00;
    netif.mac[2] = 0x00;
    netif.mac[3] = 0x00;
    netif.mac[4] = 0x00;
    netif.mac[5] = 0x01;
    netif.ipv4 = 0x0A00020F; /* 10.0.2.15 */
    loopback_length = 0;

    console_write_line("[net] interface up");
    log_mac(netif.mac);
    log_ipv4(netif.ipv4);
}

int network_send_packet(const void *data, size_t length) {
    if (!data || length == 0) {
        return -1;
    }
    if (length > LOOPBACK_MAX) {
        console_write_line("[net] send overflow");
        return -1;
    }

    const uint8_t *bytes = (const uint8_t *)data;
    for (size_t i = 0; i < length; ++i) {
        loopback_buffer[i] = bytes[i];
    }
    loopback_length = length;
    console_write_line("[net] packet queued for loopback");
    return (int)length;
}

int network_receive_packet(void *buffer, size_t buffer_length) {
    if (!buffer || buffer_length == 0) {
        return -1;
    }
    if (loopback_length == 0) {
        return 0;
    }
    if (buffer_length < loopback_length) {
        console_write_line("[net] receive buffer too small");
        return -1;
    }

    uint8_t *bytes = (uint8_t *)buffer;
    for (size_t i = 0; i < loopback_length; ++i) {
        bytes[i] = loopback_buffer[i];
    }
    int received = (int)loopback_length;
    loopback_length = 0;
    console_write_line("[net] loopback packet delivered");
    return received;
}
