#ifndef joystick_h
#define joystick_h

#include <stdio.h>
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"

// Definições de pinos
#define LED_B 12
#define LED_R 13
#define LED_G 11
#define SW 22  // Pino do botão do joystick
#define VRY 26 // Porta ADC de variação do Y do joystick
#define VRX 27 // Porta ADC de variação do X do joystick

extern const uint16_t PERIOD;      // Período do PWM
extern const float DIVIDER_PWM;    // Divisor de clock do PWM
extern int dy;

// Protótipos das funções
void setup_joystick();
void setup_pwm_led(uint led, uint *slice, uint16_t level);
void setup();
void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value);
void joystick_mode();
int main_joystick();

#endif
