#ifndef pico_w_wifi
#define pico_w_wifi

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "lwip/tcp.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"
#include "lwip/init.h"
#include "lwip/pbuf.h"
#include <stdbool.h>

#define WIFI_SSID "INTELBRAS"  // Substitua pelo nome da sua rede Wi-Fi
#define WIFI_PASS "02280419" // Substitua pela senha da sua rede Wi-Fi
#define THINGSPEAK_HOST "api.thingspeak.com"
#define THINGSPEAK_PORT 80

#define API_KEY "I56EISQKQZHVF6VA"  // Chave de escrita do ThingSpeak

void dns_callback(const char *name, const ip_addr_t *ipaddr, void *callback_arg);
int main_wifi(void);

#endif