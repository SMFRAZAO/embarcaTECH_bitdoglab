#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "tusb.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

#define LED_VERDE 11
#define LED_AZUL 12
#define LED_VERMELHO 13
#define BUZZER 10

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;
uint8_t ssd[ssd1306_buffer_length];
struct render_area frame_area;

// Função para desenhar texto com escala
void ssd1306_draw_string_scaled(uint8_t *buffer, int x, int y, const char *text, int scale) {
    while (*text) {
        for (int dx = 0; dx < scale; dx++) {
            for (int dy = 0; dy < scale; dy++) {
                ssd1306_draw_char(buffer, x + dx, y + dy, *text);
            }
        }
        x += 6 * scale;
        text++;
    }
}

// Função para exibir texto centralizado no OLED
void exibir_texto_oled(const char* texto) {
    memset(ssd, 0, ssd1306_buffer_length);  // Limpa o buffer
    int scale = 2;
    int largura = strlen(texto) * 6 * scale;
    int pos_x = (ssd1306_width - largura) / 2;
    int pos_y = 20;
    ssd1306_draw_string_scaled(ssd, pos_x, pos_y, texto, scale);
    render_on_display(ssd, &frame_area);
}

int main() {
    // Inicializa o USB
    stdio_init_all();

    // Inicializa I2C e display
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init();

    // Define área de renderização
    frame_area.start_column = 0;
    frame_area.end_column = ssd1306_width - 1;
    frame_area.start_page = 0;
    frame_area.end_page = ssd1306_n_pages - 1;
    calculate_render_area_buffer_length(&frame_area);

    memset(ssd, 0, ssd1306_buffer_length); // Limpa tela inicial
    render_on_display(ssd, &frame_area);

    // Inicializa os GPIOs dos LEDs
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_put(LED_VERDE, 0);

    gpio_init(LED_AZUL);
    gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_put(LED_AZUL, 0);

    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_put(LED_VERMELHO, 0);

    // Inicializa o GPIO do Buzzer
    gpio_init(BUZZER);
    gpio_set_dir(BUZZER, GPIO_OUT);
    gpio_put(BUZZER, 0);

    // Aguarda o USB ser montado
    while (!tud_cdc_connected()) {
        sleep_ms(100);
    }

    printf("USB conectado!\n");

    char palavra[64];
    int pos = 0;

    while (true) {
        if (tud_cdc_available()) {
            uint8_t buf[64];
            uint32_t count = tud_cdc_read(buf, sizeof(buf));

            for (uint32_t i = 0; i < count; i++) {
                printf("Recebido: %c\n", buf[i]);

                if (buf[i] == '\n' || buf[i] == '\r') {
                    palavra[pos] = '\0';

                    // Comandos reconhecidos
                    if (strcmp(palavra, "vermelho") == 0) {
                        gpio_put(LED_VERMELHO, 1);
                        exibir_texto_oled("vermelho");
                        sleep_ms(1000);
                        gpio_put(LED_VERMELHO, 0);
                    } else if (strcmp(palavra, "verde") == 0) {
                        gpio_put(LED_VERDE, 1);
                        exibir_texto_oled("verde");
                        sleep_ms(1000);
                        gpio_put(LED_VERDE, 0);
                    } else if (strcmp(palavra, "azul") == 0) {
                        gpio_put(LED_AZUL, 1);
                        exibir_texto_oled("azul");
                        sleep_ms(1000);
                        gpio_put(LED_AZUL, 0);
                    } else if (strcmp(palavra, "som") == 0) {
                        gpio_put(BUZZER, 1);
                        sleep_ms(1000);
                        gpio_put(BUZZER, 0);
                    }

                    pos = 0;
                } else {
                    if (pos < sizeof(palavra) - 1) {
                        palavra[pos++] = buf[i];
                    }
                }
            }

            tud_cdc_write(buf, count);
            tud_cdc_write_flush();
        }

        tud_task();
    }

    return 0;
}
