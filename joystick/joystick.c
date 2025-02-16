#include <stdio.h>          // Biblioteca padrão de entrada e saída
#include "hardware/adc.h"   // Biblioteca para manipulação do ADC no RP2040
#include "hardware/pwm.h"   // Biblioteca para controle de PWM no RP2040
#include "hardware/timer.h" // Biblioteca para temporizar
#include "pico/stdlib.h"    // Biblioteca padrão do Raspberry Pi Pico
#include "ProtejoPratico.h"
#include "matriz/neopixel_pio.h"
#include "conexao/pico_w_wifi.h"

// #define LED_B 12
// #define LED_R 13
// #define LED_G 11
#define SW 22  // Pino do Botão do Joystick
#define VRY 26 // Porta ADC de variação do Y do Joystick
#define VRX 27 // Porta ADC de variação do X do joystick

#define DEAD_ZONE 30
#define JOY_CENTER_X 2170 // Valor central medido do eixo X
#define JOY_CENTER_Y 2040 // Valor central medido do eixo Y

// Flag global (volátil porque é alterada na interrupção)
volatile bool iniciar_resolucao_dns = false;

int rect_y = 128 / 2 - 110 / 2; // variável do retangulo para largura
int rect_x = 37 - 5;            // variável do retangulo para altura
int dx = 0, dy = 0;             // variável para tranformar o valor do joystick em números menores.

const int ADC_CHANNEL_0 = 0;    // Canal ADC para o eixo X do joystick
const int ADC_CHANNEL_1 = 1;    // Canal ADC para o eixo Y do joystick
const float DIVIDER_PWM = 16.0; // Divisor fracional do clock para o PWM
const uint16_t PERIOD = 4096;   // Período do PWM (valor máximo do contador)

// Função para configurar o joystick (pinos de leitura e ADC)
void setup_joystick()
{
  // Inicializa o ADC e os pinos de entrada analógica
  adc_init();         // Inicializa o módulo ADC
  adc_gpio_init(VRX); // Configura o pino VRX (eixo X) para entrada ADC
  adc_gpio_init(VRY); // Configura o pino VRY (eixo Y) para entrada ADC

  // Inicializa o pino do botão do joystick
  gpio_init(SW);             // Inicializa o pino do botão
  gpio_set_dir(SW, GPIO_IN); // Configura o pino do botão como entrada
  gpio_pull_up(SW);          // Ativa o pull-up no pino do botão para evitar flutuações
}

// Função de configuração geral
void setup()
{
  stdio_init_all(); // Inicializa a porta serial para saída de dados
  setup_joystick(); // Chama a função de configuração do joystick
}

// Função para ler os valores dos eixos do joystick (X e Y)
void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value)
{
  // Leitura do valor do eixo X do joystick
  adc_select_input(ADC_CHANNEL_0); // Seleciona o canal ADC para o eixo X
  sleep_us(2);                     // Pequeno delay para estabilidade
  *vrx_value = adc_read();         // Lê o valor do eixo X (0-4095)

  // Leitura do valor do eixo Y do joystick
  adc_select_input(ADC_CHANNEL_1); // Seleciona o canal ADC para o eixo Y
  sleep_us(2);                     // Pequeno delay para estabilidade
  *vry_value = adc_read();         // Lê o valor do eixo Y (0-4095)
}

// Função para acender as duas linhas principais que vão indicar que o movimento tá correto.
void matriz()
{
  // primeira linha
  npSetLED(0, 0, 100, 0); // começando no led linha 0 coluna 4
  npSetLED(1, 0, 100, 0); // 0x3
  npSetLED(2, 0, 100, 0); // 0x2
  npSetLED(3, 0, 100, 0); // 0x1
  npSetLED(4, 0, 100, 0); // 0x0
  // Segunda Linha
  npSetLED(5, 0, 100, 0); // 1x0
  npSetLED(6, 0, 100, 0); // 1x1
  npSetLED(7, 0, 100, 0); // 1x2
  npSetLED(8, 0, 100, 0); // 1x3
  npSetLED(9, 0, 100, 0); // 1x4 //todos os LEDs ligados da 2 linha para baixo
  npWrite();
}
// Função do temporizador
bool repeating_timer_callback(struct repeating_timer *t)
{
  iniciar_resolucao_dns = true;
  return true;
}

// Modo joystick: Move o retângulo na tela e atualiza a matriz de LEDs conforme os movimentos do joystick
void joystick_mode()
{
  uint16_t vrx_value, vry_value;
  const int initial_x = rect_x;
  const int initial_y = rect_y;
  matriz(); // Chamando a função para ligar os LEDs pincipais da matriz.

  struct repeating_timer timer;                                          // Declara uma variável timer do tipo struct repeating_timer.
  add_repeating_timer_ms(15000, repeating_timer_callback, NULL, &timer); // Configura um temporizador que chamará a função repeating_time_callback a cada 15 segundos.

  while (true)
  {

    joystick_read_axis(&vrx_value, &vry_value); // Lendo os valores do Joystick.

    dx = (vrx_value - JOY_CENTER_X) / 1000; // Dividindo a leitura do X por 1000
    dy = (vry_value - JOY_CENTER_Y) / 1000; // Dividindo a leitura do Y por 1000

    // Transformando o valor bruto em um valor menor e verificando o movimento para acender os LEDs da matriz.
    if (vry_value > 3000 + DEAD_ZONE)
    {
      matriz();
      npSetLED(10, 50, 0, 0); // 2x4
      npWrite();
      if (vry_value > 3500 + DEAD_ZONE) // Se caso ele passar mais um pouco do ângulo, vai acender outros LEDs.
      {
        npSetLED(19, 100, 0, 0); // 3x4
        npSetLED(11, 100, 0, 0); // 2x3
        npWrite();               // Função para escrever na matriz.
      }
    }
    if (vrx_value <= 3000 + DEAD_ZONE && vrx_value >= 1000)
    {
      npClear(); // Limpando a matriz.
      matriz();
    }
    else if (vry_value < 1000 - DEAD_ZONE)
    {
      matriz();
      npSetLED(14, 100, 0, 0); // 2x0
      npWrite();
      if (vry_value < 600 - DEAD_ZONE) // Se caso ele passar mais um pouco do ângulo, vai acender outros LEDs.
      {
        npSetLED(13, 100, 0, 0); // 2x1
        npSetLED(15, 100, 0, 0); // 3x0
        npWrite();
      }
    }

    rect_x += dx;
    rect_y += dy;

    // Limitando X
    if (rect_x < 0)
      rect_x = 0;
    if (rect_x > 32)
      rect_x = 32;

    // Limitando Y
    if (rect_y < 0)
      rect_y = 0;
    if (rect_y > 128 - 110)
      rect_y = 128 - 110;

    if (vrx_value >= JOY_CENTER_X - DEAD_ZONE &&
        vry_value >= JOY_CENTER_Y - DEAD_ZONE)
    {
      while (rect_x != initial_x || rect_y != initial_y) // Voltando o retangulo para o ponto inicial.
      {
        if (rect_x < initial_x)
          rect_x += 1;
        else if (rect_x > initial_x)
          rect_x -= 1;

        if (rect_y < initial_y)
          rect_y += 1;
        else if (rect_y > initial_y)
          rect_y -= 1;
        dx = 0;
        draw_rectangle(rect_y, rect_x);
        sleep_ms(20); // Pequeno delay para suavizar a animação
      }
    }

    printf("Joystick X: %d | Y: %d | Retângulo X: %d | Y: %d\n", vrx_value, vry_value, rect_x, rect_y);

    draw_rectangle(rect_y, rect_x);
    if (!gpio_get(1))
    {
      break;
    }
    // Verificando se o timer disparou para resolver o DNS.
    if (iniciar_resolucao_dns)
    {
      iniciar_resolucao_dns = false; // Abaixa a flag para não tentar resolver novamente antes do próximo timer.
      ip_addr_t resolved_ip;
      err_t err = dns_gethostbyname(THINGSPEAK_HOST, &resolved_ip, dns_callback, NULL);
      if (err == ERR_OK)
      {
        printf("DNS em cache, IP: %s\n", ipaddr_ntoa(&resolved_ip));
        dns_callback(THINGSPEAK_HOST, &resolved_ip, NULL);
      }
      else if (err == ERR_INPROGRESS)
      {
        printf("Resolução DNS iniciada...\n");
      }
      else
      {
        printf("Erro ao iniciar resolução de DNS: %d\n", err);
      }
    }
    sleep_ms(100);
  }
}

// Função principal
int main_joystick()
{
  uint16_t vrx_value, vry_value, sw_value; // Variáveis para armazenar os valores do joystick (eixos X e Y) e botão
  setup();                                 // Chama a função de configuração
  printf("Joystick-PWM\n");                // Exibe uma mensagem inicial via porta serial
  // Loop principal
  while (1)
  {
    joystick_read_axis(&vrx_value, &vry_value); // Lê os valores dos eixos do joystick
  }
  return 0;
}
