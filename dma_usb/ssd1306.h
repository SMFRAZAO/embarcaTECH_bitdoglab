#ifndef SSD1306_H
#define SSD1306_H

#include "hardware/i2c.h"
#include <stdint.h>
#include <stdbool.h>

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64
#define SSD1306_I2C_ADDR 0x3C

typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t address;
    i2c_inst_t *i2c;
    uint8_t buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];
} ssd1306_t;

void ssd1306_init(ssd1306_t *disp, uint16_t width, uint16_t height, uint8_t address, i2c_inst_t *i2c);
void ssd1306_clear(ssd1306_t *disp);
void ssd1306_show(ssd1306_t *disp);
void ssd1306_draw_pixel(ssd1306_t *disp, uint8_t x, uint8_t y, bool color);
void ssd1306_draw_char(ssd1306_t *disp, uint8_t x, uint8_t y, char c);
void ssd1306_draw_string(ssd1306_t *disp, uint8_t x, uint8_t y, const char *str);

#endif
