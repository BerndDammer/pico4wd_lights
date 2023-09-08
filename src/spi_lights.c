#include <stdbool.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/spi.h"

#include "spi_lights.h"

#define FSPI 2500000
#define SPI_TX_PIN 19
#define LAMP_COUNT (3*8)
#define STEP_COUNT 8
#define STEP_COUNT_MASK 7

unsigned short nibble_table[16];

unsigned char step_lines[STEP_COUNT][3 * LAMP_COUNT];
unsigned short act_nibbles[6 * LAMP_COUNT];

/**
 * fills the encoding table
 *
 * converts 4 bit RGB Value int 12 bit for SPI Shift register
 * encoding for WS2812B Lamp
 * 1 -> 110
 * 0 -> 100
 */
static void fill_table(void)
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

/**
 * Every Lamp is controlled by 3 times 8 bit RGB Value
 * The Full Line has 24 Lamps
 *
 * The SPI Clocks with 2,5 MHz
 * A One is transmitted with 110 a Zero wit 100
 *
 * The Spi shift register can be Loaded with max 16 bits
 * The SPI is programmed in 12 Bit Mode
 *
 * The 3*8 = 24 Bits are splitted in 6 Groups of 4 bits
 * 4 bits of RGB Value is converted in 12 bit SPI value
 *
 * For every Lamp the SPI is loaded with 6 Times 12 Bit
 *
 * the
 * act_nibbles
 * has 6 Times short=16 bit per Lamp
 *
 * @param act_line
 * pointer to 3 * 8 bit * 24 Lamps RGB Storage
 */
static void light_act_to_nibbles(unsigned char *act_line)
{
    for (int i = 0; i < 3 * LAMP_COUNT; i++)
    {
        unsigned char b = act_line[i];
        act_nibbles[i << 1] = nibble_table[b >> 4];
        act_nibbles[(i << 1) + 1] = nibble_table[b & 15];
    }
}

////////////////////////////////////////////////////////////////////////////////////////
///
/// callback
///
#define CALLBACK_DELAY_US 125000

int callback_counter = 0;

static repeating_timer_t callback_timer;
unsigned char callback_user_data[256];

static bool spi_lights_timer_callback(repeating_timer_t *pcallback_timer)
{
    callback_counter++;

    light_act_to_nibbles(step_lines[callback_counter & STEP_COUNT_MASK]);
    spi_write16_blocking(spi0, act_nibbles, 6 * LAMP_COUNT);

    return true;
}
///////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////
/// public functions

void spi_lights_init(void)
{
    fill_table();
    gpio_init(SPI_TX_PIN);
    gpio_set_function(SPI_TX_PIN, GPIO_FUNC_SPI);
    gpio_set_dir(SPI_TX_PIN, true);

    //spi_set_baudrate(spi0, FSPI);
    spi_init(spi0, FSPI);
    spi_set_format(spi0, 12, SPI_CPOL_0, SPI_CPHA_1, SPI_MSB_FIRST);

    add_repeating_timer_us( CALLBACK_DELAY_US, spi_lights_timer_callback,
            callback_user_data, &callback_timer);
}

void spi_lights_set_single_blinkmask(int index, int r, int g, int b,
        int blinkmask)
{
    for (int j = 0; j < STEP_COUNT; j++)
    {
        bool on = (blinkmask & 0x80) != 0;
        unsigned char *act_line = step_lines[j];
        act_line[3 * index] = on ? g : 0;
        act_line[3 * index + 1] = on ? r : 0;
        act_line[3 * index + 2] = on ? b : 0;
        blinkmask <<= 1;
    }
}

void spi_lights_set_single_cycler(int index, const int colors[8][3] )
{
    for (int j = 0; j < STEP_COUNT; j++)
    {
        unsigned char *act_line = step_lines[j];
        act_line[3 * index] = colors[j][0];
        act_line[3 * index + 1] = colors[j][1];
        act_line[3 * index + 2] = colors[j][2];
    }
}

void spi_lights_set_single(int index, int r, int g, int b)
{
    for (int j = 0; j < STEP_COUNT; j++)
    {
        unsigned char *act_line = step_lines[j];
        act_line[3 * index] = g;
        act_line[3 * index + 1] = r;
        act_line[3 * index + 2] = b;

    }
}

void spi_lights_off(void)
{
    for (int j = 0; j < STEP_COUNT; j++)
    {
        unsigned char *act_line = step_lines[j];

        for (int i = 0; i < 3 * LAMP_COUNT; i++)
        {
            act_line[i] = 0;
        }
    }
}

void spi_lights_shift_up(void)
{
    for (int j = 0; j < STEP_COUNT; j++)
    {
        unsigned char *act_line = step_lines[j];

        for (int i = 3 * LAMP_COUNT - 4; i >= 0; i--)
        {
            act_line[i + 3] = act_line[i];
        }
        for (int i = 0; i < 3; i++)
        {
            act_line[i] = 0;
        }
    }
}

