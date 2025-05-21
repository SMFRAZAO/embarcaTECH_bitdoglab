#include <stdio.h>
#include "pico/stdlib.h"
#include "tusb.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "ssd1306.h" 

ssd1306_t display;

#define LED_VERMELHO 13
#define LED_VERDE 11
#define LED_AZUL 12
#define BUZZER 10

void init_gpio() {
    gpio_init(LED_VERDE);
    gpio_init(LED_AZUL);
    gpio_init(LED_VERMELHO);
    gpio_init(BUZZER);

    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_set_dir(BUZZER, GPIO_OUT);
}

void ativar_led(uint led_gpio) {
    gpio_put(LED_VERMELHO, 0);
    gpio_put(LED_VERDE, 0);
    gpio_put(LED_AZUL, 0);
    gpio_put(BUZZER, 0);

    if (led_gpio == BUZZER) {
        gpio_put(BUZZER, 1);
    } else {
        gpio_put(led_gpio, 1);
    }

    sleep_ms(1000);

    gpio_put(led_gpio, 0);
    gpio_put(BUZZER, 0);
}

void cdc_task() {
    if (tud_cdc_connected() && tud_cdc_available()) {
        char buffer[64];
        int len = tud_cdc_read(buffer, sizeof(buffer));
        buffer[len] = '\0';

        // ECO
        tud_cdc_write_str(buffer);
        tud_cdc_write_flush();

        // Mostrar no display OLED
        ssd1306_clear(&display);
        ssd1306_draw_string(&display, 0, 0, 1, buffer); // x=0, y=0, escala=1
        ssd1306_show(&display);


        // Comandos
        if (strstr(buffer, "vermelho")) {
            ativar_led(LED_VERMELHO);
        } else if (strstr(buffer, "verde")) {
            ativar_led(LED_VERDE);
        } else if (strstr(buffer, "azul")) {
            ativar_led(LED_AZUL);
        } else if (strstr(buffer, "som")) {
            ativar_led(BUZZER);
        }
    }
}

int main() {
    stdio_init_all();
    init_gpio();
    ssd1306_init(&display, 128, 64, 0x3C, i2c_default);
     tusb_init();    // Inicializa a USB CDC

    while (1) {
        tud_task();     // TinyUSB task
        cdc_task();     // Nossa task CDC
    }
}
