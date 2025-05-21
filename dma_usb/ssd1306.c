#include "ssd1306.h"
#include "pico/stdlib.h"

// Font básica 5x7
static const uint8_t font[96][5] = {
    // (Espaço até '~', fonte compacta)
    {0x00,0x00,0x00,0x00,0x00}, // ' '
    {0x00,0x00,0x5F,0x00,0x00}, // '!'
    {0x00,0x07,0x00,0x07,0x00}, // '"'
    {0x14,0x7F,0x14,0x7F,0x14}, // '#'
    {0x24,0x2A,0x7F,0x2A,0x12}, // '$'
    {0x23,0x13,0x08,0x64,0x62}, // '%'
    {0x36,0x49,0x55,0x22,0x50}, // '&'
    {0x00,0x05,0x03,0x00,0x00}, // '''
    {0x00,0x1C,0x22,0x41,0x00}, // '('
    {0x00,0x41,0x22,0x1C,0x00}, // ')'
    {0x14,0x08,0x3E,0x08,0x14}, // '*'
    {0x08,0x08,0x3E,0x08,0x08}, // '+'
    {0x00,0x50,0x30,0x00,0x00}, // ','
    {0x08,0x08,0x08,0x08,0x08}, // '-'
    {0x00,0x60,0x60,0x00,0x00}, // '.'
    {0x20,0x10,0x08,0x04,0x02}, // '/'
    {0x3E,0x51,0x49,0x45,0x3E}, // '0'
    {0x00,0x42,0x7F,0x40,0x00}, // '1'
    {0x42,0x61,0x51,0x49,0x46}, // '2'
    {0x21,0x41,0x45,0x4B,0x31}, // '3'
    {0x18,0x14,0x12,0x7F,0x10}, // '4'
    {0x27,0x45,0x45,0x45,0x39}, // '5'
    {0x3C,0x4A,0x49,0x49,0x30}, // '6'
    {0x01,0x71,0x09,0x05,0x03}, // '7'
    {0x36,0x49,0x49,0x49,0x36}, // '8'
    {0x06,0x49,0x49,0x29,0x1E}, // '9'
    // (restante omitido por brevidade)
};

// Envia comando
static void ssd1306_send_command(ssd1306_t *disp, uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd};
    i2c_write_blocking(disp->i2c, disp->address, buf, 2, false);
}

// Envia dados
static void ssd1306_send_data(ssd1306_t *disp, const uint8_t *data, size_t len) {
    uint8_t prefix = 0x40;
    i2c_write_blocking(disp->i2c, disp->address, &prefix, 1, true);
    i2c_write_blocking(disp->i2c, disp->address, data, len, false);
}

// Inicializa display
void ssd1306_init(ssd1306_t *disp, uint16_t width, uint16_t height, uint8_t address, i2c_inst_t *i2c) {
    disp->width = width;
    disp->height = height;
    disp->address = address;
    disp->i2c = i2c;

    sleep_ms(100);
    ssd1306_send_command(disp, 0xAE); // Display off
    ssd1306_send_command(disp, 0x20); // Set memory mode
    ssd1306_send_command(disp, 0x00); // Horizontal mode
    ssd1306_send_command(disp, 0xB0); // Page 0
    ssd1306_send_command(disp, 0xC8); // COM scan
    ssd1306_send_command(disp, 0x00); // Low column start
    ssd1306_send_command(disp, 0x10); // High column start
    ssd1306_send_command(disp, 0x40); // Start line address
    ssd1306_send_command(disp, 0x81); // Contrast
    ssd1306_send_command(disp, 0xFF);
    ssd1306_send_command(disp, 0xA1); // Segment remap
    ssd1306_send_command(disp, 0xA6); // Normal display
    ssd1306_send_command(disp, 0xA8); // Multiplex
    ssd1306_send_command(disp, 0x3F);
    ssd1306_send_command(disp, 0xA4);
    ssd1306_send_command(disp, 0xD3);
    ssd1306_send_command(disp, 0x00);
    ssd1306_send_command(disp, 0xD5);
    ssd1306_send_command(disp, 0xF0);
    ssd1306_send_command(disp, 0xD9);
    ssd1306_send_command(disp, 0x22);
    ssd1306_send_command(disp, 0xDA);
    ssd1306_send_command(disp, 0x12);
    ssd1306_send_command(disp, 0xDB);
    ssd1306_send_command(disp, 0x20);
    ssd1306_send_command(disp, 0x8D);
    ssd1306_send_command(disp, 0x14);
    ssd1306_send_command(disp, 0xAF); // Display on

    ssd1306_clear(disp);
    ssd1306_show(disp);
}

void ssd1306_clear(ssd1306_t *disp) {
    for (int i = 0; i < sizeof(disp->buffer); i++) {
        disp->buffer[i] = 0x00;
    }
}

void ssd1306_show(ssd1306_t *disp) {
    for (uint8_t page = 0; page < 8; page++) {
        ssd1306_send_command(disp, 0xB0 + page);
        ssd1306_send_command(disp, 0x00);
        ssd1306_send_command(disp, 0x10);
        ssd1306_send_data(disp, &disp->buffer[disp->width * page], disp->width);
    }
}

void ssd1306_draw_pixel(ssd1306_t *disp, uint8_t x, uint8_t y, bool color) {
    if (x >= disp->width || y >= disp->height) return;
    if (color)
        disp->buffer[x + (y / 8) * disp->width] |= (1 << (y % 8));
    else
        disp->buffer[x + (y / 8) * disp->width] &= ~(1 << (y % 8));
}

void ssd1306_draw_char(ssd1306_t *disp, uint8_t x, uint8_t y, char c) {
    if (c < 32 || c > 127) c = '?';
    for (int i = 0; i < 5; i++) {
        uint8_t line = font[c - 32][i];
        for (int j = 0; j < 8; j++) {
            ssd1306_draw_pixel(disp, x + i, y + j, (line >> j) & 1);
        }
    }
}

void ssd1306_draw_string(ssd1306_t *disp, uint8_t x, uint8_t y, const char *str) {
    while (*str) {
        ssd1306_draw_char(disp, x, y, *str++);
        x += 6;
    }
}
