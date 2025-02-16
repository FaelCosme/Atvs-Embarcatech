#ifndef NP_LED_MATRIX_H
#define NP_LED_MATRIX_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
// #include "ws2818b.pio.h"

#define LED_COUNT 25
#define LED_PIN 7

// Definição de pixel GRB
typedef struct {
    uint8_t G, R, B;
} npLED_t;

extern npLED_t leds[LED_COUNT];
extern PIO np_pio;
extern uint sm;

void npInit(uint pin);
void npSetLED(const uint index, const uint8_t r, const uint8_t g, const uint8_t b);
void npClear();
void npWrite();
int main_matriz();

#endif // NP_LED_MATRIX_H