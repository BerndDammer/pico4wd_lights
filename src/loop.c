#include "loop.h"
#include "blinker.h"
#include "spi_lights.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/watchdog.h"
#include "hardware/clocks.h"

#define CONSOLE_TIMEOUT 333333

void menu(void)
{
    printf("------------------------------------\n");
    printf("r inject red\n");
    printf("press key to select\n");
    printf("------------------------------------\n");
}

void loop(void)
{
    /*
     printf("------------------------------------\n");
     printf("clock_get_hz(clk_sys) %u\n", clock_get_hz(clk_sys));
     printf("------------------------------------\n");
     */
    int counter = 0;
    volatile int c; // make visible in debugger; avoid optimize out

    menu();
    spi_lights_init();
    spi_lights_set_single(1, 70, 0, 0);
    for (;;)
    {
        c = getchar_timeout_us(CONSOLE_TIMEOUT);
        blinker_toggle();

        if (c == PICO_ERROR_TIMEOUT)
        {
            spi_lights_shift_up();
            printf("Loop Counter %i\n", counter);
            counter++;
        }
        else
        {
            switch (c)
            {
            case 'r':
                spi_lights_set_single(0, 255, 0, 0);
                break;
            case ' ':
            case '0':
                break;
            default:
                menu();
                break;
            }
        }
    }
}
