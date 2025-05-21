#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include <stdio.h>

#define LED_VERDE 11
#define LED_VERMELHO 13
#define LED_AZUL 12
#define BUZZER 14
#define BOTAO_A 16
#define BOTAO_B 17

volatile bool botao_acionado = false;
struct repeating_timer timer;
int estado = 0;
int contador = 0;

bool temporizador_callback(struct repeating_timer *t);
void acender_leds(bool verde, bool azul, bool vermelho);
void iniciar_ciclo();
void tratar_pedestre();
void callback_interrupcao(uint gpio, uint32_t eventos);

int main() {
    stdio_init_all();

    gpio_init(LED_VERDE); gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(LED_VERMELHO); gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_init(LED_AZUL); gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_init(BUZZER); gpio_set_dir(BUZZER, GPIO_OUT);
    gpio_init(BOTAO_A); gpio_set_dir(BOTAO_A, GPIO_IN); gpio_pull_up(BOTAO_A);
    gpio_init(BOTAO_B); gpio_set_dir(BOTAO_B, GPIO_IN); gpio_pull_up(BOTAO_B);

    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, &callback_interrupcao);
    gpio_set_irq_enabled(BOTAO_B, GPIO_IRQ_EDGE_FALL, true);

    add_repeating_timer_ms(1000, temporizador_callback, NULL, &timer);

    iniciar_ciclo();

    while (true) {
        tight_loop_contents();
    }
}

bool temporizador_callback(struct repeating_timer *t) {
    contador++;

    if (botao_acionado) {
        tratar_pedestre();
        botao_acionado = false;
        return true;
    }

    switch (estado) {
        case 0:
            if (contador == 1) printf("Sinal: Vermelho\n");
            if (contador >= 10) {
                estado = 1;
                contador = 0;
                acender_leds(true, false, false);
                printf("Sinal: Verde\n");
            }
            break;

        case 1:
            if (contador == 1) printf("Sinal: Verde\n");
            if (contador >= 10) {
                estado = 2;
                contador = 0;
                acender_leds(true, false, true);
                printf("Sinal: Amarelo\n");
            }
            break;

        case 2:
            if (contador == 1) printf("Sinal: Amarelo\n");
            if (contador >= 3) {
                estado = 0;
                contador = 0;
                acender_leds(false, false, true);
                printf("Sinal: Vermelho\n");
            }
            break;
    }
    return true;
}

void tratar_pedestre() {
    printf("Pedestre solicitou travessia\n");

    acender_leds(true, false, true);
    sleep_ms(3000);

    acender_leds(false, false, true);
    for (int i = 5; i >= 1; i--) {
        printf("TRAVESSE: %d\n", i);

        gpio_put(BUZZER, 1);
        sleep_ms(200);
        gpio_put(BUZZER, 0);
        sleep_ms(800);
    }

    estado = 1;
    contador = 0;
    acender_leds(true, false, false);
    printf("Sinal: Verde\n");
}

void acender_leds(bool verde, bool azul, bool vermelho) {
    gpio_put(LED_VERDE, verde);
    gpio_put(LED_AZUL, azul);
    gpio_put(LED_VERMELHO, vermelho);
}

void iniciar_ciclo() {
    estado = 0;
    contador = 0;
    acender_leds(false, false, true);
    printf("Sinal: Vermelho\n");
}

void callback_interrupcao(uint gpio, uint32_t eventos) {
    if ((gpio == BOTAO_A || gpio == BOTAO_B) && (eventos & GPIO_IRQ_EDGE_FALL)) {
        printf("Botão pedestres acionado\n");

        // Beep sonoro curto no momento do clique
        gpio_put(BUZZER, 1);
        sleep_ms(100);
        gpio_put(BUZZER, 0);

        botao_acionado = true;
    }
}
