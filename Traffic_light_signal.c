#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Definição dos pinos dos LEDs
#define LED_RED 11    // GPIO11 para o LED vermelho
#define LED_YELLOW 12 // GPIO12 para o LED amarelo
#define LED_GREEN 13  // GPIO13 para o LED verde

// Estados do semáforo
typedef enum {
    STATE_RED,
    STATE_YELLOW,
    STATE_GREEN
} traffic_light_state_t;

// Variáveis globais
volatile traffic_light_state_t current_state = STATE_RED;
volatile bool timer_callback_called = false;

// Função de callback do temporizador
bool repeating_timer_callback(struct repeating_timer *t) {
    // Altera o estado do semáforo
    switch (current_state) {
        case STATE_RED:
            current_state = STATE_YELLOW;
            break;
        case STATE_YELLOW:
            current_state = STATE_GREEN;
            break;
        case STATE_GREEN:
            current_state = STATE_RED;
            break;
    }

    // Sinaliza que o callback foi chamado
    timer_callback_called = true;

    return true; // Continua o temporizador
}

// Função para configurar os LEDs
void init_leds() {
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_init(LED_YELLOW);
    gpio_set_dir(LED_YELLOW, GPIO_OUT);
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
}

// Função para atualizar os LEDs com base no estado atual
void update_leds() {
    // Desliga todos os LEDs
    gpio_put(LED_RED, 0);
    gpio_put(LED_YELLOW, 0);
    gpio_put(LED_GREEN, 0);

    // Liga o LED correspondente ao estado atual
    switch (current_state) {
        case STATE_RED:
            gpio_put(LED_RED, 1);
            break;
        case STATE_YELLOW:
            gpio_put(LED_YELLOW, 1);
            break;
        case STATE_GREEN:
            gpio_put(LED_GREEN, 1);
            break;
    }
}

int main() {
 // Inicializa a comunicação serial

    // Inicializa os LEDs
    init_leds();

    // Configura o temporizador para chamar a função de callback a cada 3 segundos
    struct repeating_timer timer;
    add_repeating_timer_ms(3000, repeating_timer_callback, NULL, &timer);

    // Variável para controle do tempo de impressão
    absolute_time_t last_print_time = get_absolute_time();

    while (true) {
        // Atualiza os LEDs se o callback foi chamado
        if (timer_callback_called) {
            update_leds();
            timer_callback_called = false;
        }

        // Imprime uma mensagem a cada segundo
        if (absolute_time_diff_us(last_print_time, get_absolute_time()) >= 1000000) {
            printf("Estado atual do semáforo: ");
            switch (current_state) {
                case STATE_RED:
                    printf("Vermelho\n");
                    break;
                case STATE_YELLOW:
                    printf("Amarelo\n");
                    break;
                case STATE_GREEN:
                    printf("Verde\n");
                    break;
            }
            last_print_time = get_absolute_time();
        }
    }

    return 0;
}
