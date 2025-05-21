#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/time.h"

// Bibliotecas usadas conforme referência:
#include "hardware/adc.h"    // Para controle do ADC (sensor interno temperatura)
#include "hardware/dma.h"    // Para transferência DMA do ADC para RAM
#include "hardware/i2c.h"    // Para comunicação I2C com display SSD1306
#include "ssd1306.h"         // Driver do display OLED SSD1306

#define ADC_DMA_CHANNEL 0
#define NUM_SAMPLES 32
#define I2C_PORT i2c0
#define I2C_SDA_PIN 4
#define I2C_SCL_PIN 5

uint16_t adc_buf[NUM_SAMPLES];

ssd1306_t disp;  // Estrutura do display SSD1306

// Inicializa ADC e DMA para leitura da temperatura interna do chip
void adc_dma_init() {
    adc_init();                           // adc_init(): Inicializa o ADC
    adc_set_temp_sensor_enabled(true);   // adc_set_temp_sensor_enabled(true): Ativa sensor interno no ADC4
    adc_select_input(4);                  // adc_select_input(4): Seleciona canal 4 (sensor temperatura)

    // Configura FIFO do ADC para armazenar dados (usado pelo DMA)
    adc_fifo_setup(
        true,   // enable
        true,   // enable dma data request (DREQ)
        1,      // sample threshold
        false,  // no err bit
        false   // no byte shift
    );

    // Configuração DMA para transferir dados ADC direto para adc_buf na RAM
    int dma_chan = ADC_DMA_CHANNEL;
    dma_channel_config cfg = dma_channel_get_default_config(dma_chan);  // dma_channel_get_default_config()
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);          // Define tamanho dos dados (16 bits)
    channel_config_set_read_increment(&cfg, false);                    // Endereço leitura fixo (ADC FIFO)
    channel_config_set_write_increment(&cfg, true);                    // Endereço escrita incrementa (adc_buf)
    channel_config_set_dreq(&cfg, DREQ_ADC);                           // Fluxo controlado pelo ADC

    dma_channel_configure(
        dma_chan,
        &cfg,
        adc_buf,          // Destino (RAM)
        &adc_hw->fifo,    // Origem (ADC FIFO)
        NUM_SAMPLES,      // Número de amostras a transferir
        false             // Não inicia ainda
    );
}

// Faz leitura da temperatura via ADC e DMA, calcula e retorna a temperatura
float ler_temperatura_dma() {
    dma_channel_set_read_addr(ADC_DMA_CHANNEL, &adc_hw->fifo, true);  // Inicia DMA leitura ADC
    adc_run(true);       // Liga ADC
    sleep_ms(10);        // Aguarda para coletar amostras suficientes
    adc_run(false);      // Desliga ADC

    // Calcula média das amostras ADC
    uint32_t soma = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        soma += adc_buf[i];
    }
    float media = soma / (float)NUM_SAMPLES;

    // Converte valor ADC para tensão (0-3.3V)
    float volt = media * 3.3f / (1 << 12);

    // Fórmula específica do sensor interno do RP2040 para temperatura
    float temp = 27.0f - (volt - 0.706f) / 0.001721f;

    return temp;
}

// Inicializa o display OLED SSD1306 via I2C
void display_init() {
    i2c_init(I2C_PORT, 400 * 1000);          // Inicializa I2C a 400 kHz
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    ssd1306_init(&disp, 128, 64, 0x3C, I2C_PORT);  // ssd1306_init(): Inicializa display SSD1306
}

// Exibe a temperatura formatada no display OLED
void exibir_temperatura(float temperatura) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "Temp: %.2f C", temperatura);

    ssd1306_clear(&disp);               // ssd1306_clear(): limpa tela antes de desenhar
    ssd1306_draw_string(&disp, 0, 0, buffer);  // ssd1306_draw_string(): escreve texto na posição (0,0)
    ssd1306_show(&disp);                // ssd1306_show(): atualiza o display com o buffer
}

int main() {
    stdio_init_all();

    adc_dma_init();    // Inicializa ADC e DMA para leitura temperatura interna
    display_init();    // Inicializa display OLED SSD1306

    const uint intervalo_ms = 500;  // Intervalo entre leituras e atualização do display

    while (true) {
        float temperatura = ler_temperatura_dma();  // Lê temperatura via ADC + DMA
        printf("Temperatura: %.2f C\n", temperatura);

        exibir_temperatura(temperatura);             // Exibe temperatura no display OLED
        sleep_ms(intervalo_ms);                       // Delay para evitar polling excessivo
    }
}
