#include <stdbool.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/spi.h"

#include "spi_lights.h"

#define FSPI 2500000
#define SPI_TX_PIN 19
#define LAMP_COUNT (3*8)

unsigned char rgb[3 * 8 * 3];

unsigned short nibble_table[16];

unsigned char act_line[3 * LAMP_COUNT];
unsigned short act_nibbles[6 * LAMP_COUNT];

void fill_table(void)
{
    for (int i = 0; i < 16; i++)
    {
        unsigned short v = 0;
        if (i & 8)
        {
            v |= 6 << 9;
        }
        else
        {
            v |= 4 << 9;
        }
        if (i & 4)
        {
            v |= 6 << 6;
        }
        else
        {
            v |= 4 << 6;
        }
        if (i & 2)
        {
            v |= 6 << 3;
        }
        else
        {
            v |= 4 << 3;
        }
        if (i & 1)
        {
            v |= 6;
        }
        else
        {
            v |= 4;
        }
        nibble_table[i] = v;
    }
}
void spi_lights_init(void)
{
    fill_table();
    gpio_init(SPI_TX_PIN);
    gpio_set_function(SPI_TX_PIN, GPIO_FUNC_SPI);
    gpio_set_dir(SPI_TX_PIN, true);

    //spi_set_baudrate(spi0, FSPI);
    spi_init(spi0, FSPI);
    spi_set_format(spi0, 12, SPI_CPOL_0, SPI_CPHA_1, SPI_MSB_FIRST);
}

static void light_act_to_nibbles(void)
{
    for (int i = 0; i < 3 * LAMP_COUNT; i++)
    {
        unsigned char b = act_line[i];
        act_nibbles[i << 1] = nibble_table[b >> 4];
        act_nibbles[(i << 1) + 1] = nibble_table[b & 15];
    }
}

static void spi_lights_update(void)
{
    light_act_to_nibbles();
    spi_write16_blocking(spi0, act_nibbles, 6 * LAMP_COUNT);
}

void spi_lights_set_single(int index, int r, int g, int b)
{
    act_line[3 * index] = g;
    act_line[3 * index + 1] = r;
    act_line[3 * index + 2] = b;
    spi_lights_update();
}

void spi_lights_shift_up(void)
{
    for (int i = 3 * LAMP_COUNT - 4; i >= 0; i--)
    {
        act_line[i + 3] = act_line[i];
    }
    for (int i = 0; i < 3; i++)
    {
        act_line[i] = 0;
    }
    spi_lights_update();
}

