
#ifndef SSD1306_H
#define SSD1306_H

#include "hardware/i2c.h"
#include <stdint.h>

typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t address;
    i2c_inst_t *i2c;
    uint8_t buffer[1024];
} ssd1306_t;

void ssd1306_init(ssd1306_t *disp, uint16_t width, uint16_t height, uint8_t address, i2c_inst_t *i2c);
void ssd1306_clear(ssd1306_t *disp);
void ssd1306_show(ssd1306_t *disp);
void ssd1306_draw_string(ssd1306_t *disp, int16_t x, int16_t y, uint8_t scale, const char *text);

#endif
