#include "loop.h"
#include "blinker.h"
#include "spi_lights.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/watchdog.h"
#include "hardware/clocks.h"

#include <time.h>
#include <stdlib.h>

#define CONSOLE_TIMEOUT 1000000

const int red_green[8][3] =
{
{ 1, 0, 0 },
{ 1, 0, 0 },
{ 0, 0, 0 },
{ 0, 0, 0 },
{ 0, 1, 0 },
{ 0, 1, 0 },
{ 0, 0, 0 },
{ 0, 0, 0 }, };

const int s3[8][3] =
{
{ 2, 0, 0 },
{ 1, 1, 0 },
{ 0, 2, 0 },
{ 0, 1, 1 },
{ 0, 0, 2 },
{ 1, 0, 1 },
{ 0, 0, 0 },
{ 0, 0, 0 }, };

const int s4[8][3] =
{
{ 0, 0, 0 },
{ 0, 1, 0 },
{ 0, 3, 0 },
{ 0, 1, 1 },
{ 0, 1, 2 },
{ 0, 0, 1 },
{ 0, 0, 0 },
{ 0, 0, 0 }, };

void menu(void)
{
    printf("------------------------------------\n");
    printf("1 red on 1\n");
    printf("2 to 7 get surprised\n");
    printf("8 blue blinky 1\n");
    printf("r randomizing on off\n");
    printf("s scrolling on off\n");
    printf("0 space all off\n");
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
    bool scrolling = false;
    bool randomizing = false;
    srand(time(0));

    menu();
    spi_lights_init();
    for (;;)
    {
        c = getchar_timeout_us(CONSOLE_TIMEOUT);
        blinker_toggle();

        if (c == PICO_ERROR_TIMEOUT)
        {
            if (scrolling)
                spi_lights_shift_up();
            if (randomizing && ((rand() & 7) == 0))
            {
                spi_lights_set_single_blinkmask(0, rand() & 15, rand() & 15,
                        rand() & 15, rand());
            }
            printf("Loop Counter %i\n", counter);
            counter++;
        }
        else
        {
            switch (c)
            {
            case '1':
                spi_lights_set_single(0, 3, 0, 0);
                break;
            case '2':
                spi_lights_set_single_cycler(1, red_green);
                break;
            case '3':
                spi_lights_set_single_cycler(2, s3);
                break;
            case '4':
                spi_lights_set_single_cycler(3, s4);
                break;
            case '5':
                spi_lights_set_single(0, 9, 0, 0);
                break;
            case '6':
                spi_lights_set_single(0, 9, 0, 0);
                break;
            case '7':
                spi_lights_set_single_blinkmask(6, 0, 1, 0, 0x11);
                break;
            case '8':
                spi_lights_set_single_blinkmask(7, 0, 0, 4, 0Xee);
                break;
            case 'r':
                randomizing = !randomizing;
                puts(randomizing ? "Rand on" : "rand off");
                break;
            case 's':
                scrolling = !scrolling;
                break;
            case ' ':
            case '0':
                spi_lights_off();
                break;
            default:
                menu();
                break;
            }
        }
    }
}
