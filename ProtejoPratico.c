#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h> // Para sleep()
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pio.h"
#include "hardware/pwm.h"
#include "display/ssd1306.h"
#include "joystick/joystick.h"
#include "matriz/neopixel_pio.h"
#include "conexao/pico_w_wifi.h"
#include "ProtejoPratico.h"

#define I2C_PORT i2c1
#define I2C_SDA 15
#define I2C_SCL 14

ssd1306_t disp;        // Variável display do OLED

// Função para mapear valores de uma faixa para outra
int map_value(int value, int in_min, int in_max, int out_min, int out_max)
{
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Configuração inicial do menu e do display OLED
void setup_menu()
{
    setup_joystick();
    stdio_init_all();
    npInit(LED_PIN);
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    disp.external_vcc = false;
    ssd1306_init(&disp, 128, 64, 0x3C, I2C_PORT);
    ssd1306_clear(&disp);
}

// Função para exibir texto no display OLED
void print_texto(int x, int y, int tam, char *msg)
{
    ssd1306_draw_string(&disp, x, y, tam, msg);
    ssd1306_show(&disp);
}

// Função para desenhar um retângulo móvel no display OLED
void draw_rectangle(int x, int y)
{
    ssd1306_clear(&disp);
    ssd1306_draw_square(&disp, x, y, 110, 5);
    ssd1306_show(&disp);
}

int main()
{
    setup_menu();
    setup_joystick();
    printf("Chamando o servidor Wi-Fi...\n");
    main_wifi();
    // Loop principal
    while (true)
    {
        ssd1306_clear(&disp); // Limpando o display
        sleep_ms(50);         // Delay para suavizar
        cyw43_arch_poll();    // Necessário para manter o Wi-Fi ativo
        joystick_mode();      // Chamando a função para o joystick funcionar OBS. só vai ser chamado depois de conectar no Wi-Fi.
        sleep_ms(100);        // Reduz o uso da CPU
    }
    return 0;
}
