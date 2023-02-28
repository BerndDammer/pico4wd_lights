#include "timer.h"
#include "loop.h"
#include "blinker.h"

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/watchdog.h"
#include "hardware/clocks.h"

int main()
{
    stdio_init_all();

    timer_init();
    blinker_init();

    loop();

    return 0;
}
