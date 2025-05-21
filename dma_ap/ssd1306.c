
#include "ssd1306.h"
#include "pico/stdlib.h"
#include <string.h>

static void ssd1306_send_command(ssd1306_t *disp, uint8_t cmd) {
    uint8_t buffer[2] = {0x00, cmd};
    i2c_write_blocking(disp->i2c, disp->address, buffer, 2, false);
}

static void ssd1306_send_data(ssd1306_t *disp, uint8_t *data, size_t len) {
    uint8_t prefix = 0x40;
    i2c_write_blocking(disp->i2c, disp->address, &prefix, 1, true);
    i2c_write_blocking(disp->i2c, disp->address, data, len, false);
}

void ssd1306_init(ssd1306_t *disp, uint16_t width, uint16_t height, uint8_t address, i2c_inst_t *i2c) {
    disp->width = width;
    disp->height = height;
    disp->address = address;
    disp->i2c = i2c;
    memset(disp->buffer, 0, sizeof(disp->buffer));

    i2c_init(i2c, 400 * 1000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);

    sleep_ms(100);

    ssd1306_send_command(disp, 0xAE); // display off
    ssd1306_send_command(disp, 0x20); // memory addressing mode
    ssd1306_send_command(disp, 0x00); // horizontal addressing mode
    ssd1306_send_command(disp, 0xB0); // page start address
    ssd1306_send_command(disp, 0xC8); // COM output scan direction
    ssd1306_send_command(disp, 0x00); // low column
    ssd1306_send_command(disp, 0x10); // high column
    ssd1306_send_command(disp, 0x40); // start line
    ssd1306_send_command(disp, 0x81); // contrast
    ssd1306_send_command(disp, 0xFF);
    ssd1306_send_command(disp, 0xA1); // segment re-map
    ssd1306_send_command(disp, 0xA6); // normal display
    ssd1306_send_command(disp, 0xA8); // multiplex ratio
    ssd1306_send_command(disp, height - 1);
    ssd1306_send_command(disp, 0xA4); // resume RAM content
    ssd1306_send_command(disp, 0xD3); // display offset
    ssd1306_send_command(disp, 0x00);
    ssd1306_send_command(disp, 0xD5); // display clock divide
    ssd1306_send_command(disp, 0xF0);
    ssd1306_send_command(disp, 0xD9); // pre-charge
    ssd1306_send_command(disp, 0x22);
    ssd1306_send_command(disp, 0xDA); // com pins
    ssd1306_send_command(disp, 0x12);
    ssd1306_send_command(disp, 0xDB); // vcomh
    ssd1306_send_command(disp, 0x20);
    ssd1306_send_command(disp, 0x8D); // charge pump
    ssd1306_send_command(disp, 0x14);
    ssd1306_send_command(disp, 0xAF); // display ON

    ssd1306_clear(disp);
    ssd1306_show(disp);
}

void ssd1306_clear(ssd1306_t *disp) {
    memset(disp->buffer, 0, sizeof(disp->buffer));
}

void ssd1306_show(ssd1306_t *disp) {
    for (uint8_t page = 0; page < 8; page++) {
        ssd1306_send_command(disp, 0xB0 + page);
        ssd1306_send_command(disp, 0x00);
        ssd1306_send_command(disp, 0x10);
        ssd1306_send_data(disp, &disp->buffer[disp->width * page], disp->width);
    }
}

void ssd1306_draw_string(ssd1306_t *disp, int16_t x, int16_t y, uint8_t scale, const char *text) {
    // Simplificado: desenha apenas blocos por caractere
    for (size_t i = 0; text[i] != '\0' && x + 6 * scale * i < disp->width; i++) {
        for (int j = 0; j < 5; j++) {
            uint8_t line = 0xFF; // bloco cheio
            for (uint8_t s = 0; s < scale; s++) {
                disp->buffer[x + (i * 6 * scale) + j * scale + s + (y / 8) * disp->width] = line;
            }
        }
    }
}
