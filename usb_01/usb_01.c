#include <stdio.h>
#include "pico/stdlib.h"
#include "tusb.h"

void print_binary(uint8_t byte) {
    for (int i = 7; i >= 0; i--) {
        printf("%c", (byte & (1 << i)) ? '1' : '0');
    }
}

int main() {
    stdio_init_all();

    while (!tud_cdc_connected()) {
        sleep_ms(100);
    }

    printf("Conversor USB: Texto para binário\n");

    while (true) {
        if (tud_cdc_available()) {
            uint8_t buf[64];
            uint32_t count = tud_cdc_read(buf, sizeof(buf));
            printf("Recebido: ");

            for (uint32_t i = 0; i < count; i++) {
                printf("%c", buf[i]);
            }
            printf("\n");

            for (uint32_t i = 0; i < count; i++) {
                printf("%c -> ", buf[i]);
                print_binary(buf[i]);
                printf("\n");
            }

            printf("Total de caracteres: %u\n", count);
            printf("Total de bits: %u\n\n", count * 8);

            // Ecoa de volta para o host, se quiser
            tud_cdc_write(buf, count);
            tud_cdc_write_flush();
        }

        tud_task();
    }

    return 0;
}