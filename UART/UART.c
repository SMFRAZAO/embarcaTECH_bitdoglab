#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/irq.h"

#define JOY_Y_ADC_CHANNEL 1   // ADC1 = GPIO27
#define JOY_X_ADC_CHANNEL 2
#define LED_VERDE 11
#define LED_AZUL 12
#define LED_VERMELHO 13       // LED ânodo comum
#define LIMITE_ACIONAMENTO 200
#define LIMITE_ACIONAMENTO1 800
#define LIMITE_ACIONAMENTO2 2000
#define LIMITE_ACIONAMENTO3 3000

volatile bool acionado = false;

// Função de tratamento da interrupção do ADC
void adc_irq_handler() {
    // Ler o valor do FIFO do ADC
    uint16_t valor = adc_fifo_get(); 

    if (valor > LIMITE_ACIONAMENTO) {
        if (!acionado) {
            acionado = true;
            gpio_put(LED_VERMELHO, 0); // Acende LED
            printf("Acionado! Valor do ADC: %d\n", valor);
        }

     }else if (valor < LIMITE_ACIONAMENTO1) {
            if (!acionado) {
                acionado = true;
                gpio_put(LED_VERDE, 0); // Acende LED
                printf("Acionado! Valor do ADC: %d\n", valor);
            }
        }else if (valor > LIMITE_ACIONAMENTO2) {
                if (!acionado) {
                    acionado = true;
                    gpio_put(LED_AZUL, 0); // Acende LED
                    printf("Acionado! Valor do ADC: %d\n", valor);
                }
    } else {
        if (acionado) {
            acionado = false;
            gpio_put(LED_VERMELHO, 1); // Apaga LED
            gpio_put(LED_VERDE, 1); // Apaga LED
            gpio_put(LED_AZUL, 1); // Apaga LED
            printf("Desativado. Valor do ADC: %d\n", valor);
        }
    }

    // Limpa a interrupção (automático com FIFO no RP2040)
}

int main() {
    stdio_init_all();
    sleep_ms(500);
    printf("🎮 Sistema iniciado. Aguarde...\n");

    // Inicializa o LED
    gpio_init(LED_VERMELHO);
    gpio_init(LED_AZUL);
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_put(LED_VERMELHO, 1); // LED apagado inicialmente
    gpio_put(LED_AZUL, 1);
    gpio_put(LED_VERDE, 1);
    
    // Inicializa o ADC
    adc_init();
    adc_gpio_init(26);
    adc_gpio_init(27); // GPIO27 = ADC1
    adc_select_input(JOY_Y_ADC_CHANNEL);
    adc_select_input(JOY_X_ADC_CHANNEL);

    // Configura FIFO do ADC
    adc_fifo_setup(
        true,    // Enabling the FIFO
        true,    // Enable DMA data request (não vamos usar aqui, mas o pico SDK recomenda deixar true)
        1,       // DREQ (número de amostras antes de gerar IRQ) = 1
        false,   // Trigger only on threshold (não)
        false    // No error bit
    );

    // Configura a interrupção
    irq_set_exclusive_handler(ADC_IRQ_FIFO, adc_irq_handler);
    irq_set_enabled(ADC_IRQ_FIFO, true);
    adc_irq_set_enabled(true);

    // Começa a primeira conversão
    adc_fifo_drain(); // Limpa qualquer dado antigo
    adc_run(true);


    while (true) {
        
        sleep_ms(50); // Pequeno delay para não gerar muitas leituras (pode ser ajustado)
    }
}