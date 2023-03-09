/*
 * spi_lights.h
 *
 *  Created on: 09.03.2023
 *      Author: manni4
 */

#ifndef SPI_LIGHTS_H_
#define SPI_LIGHTS_H_

void spi_lights_init(void);
void spi_lights_strobe(void);
void spi_lights_set_single(int index, int r, int g, int b);
void spi_lights_shift_up(void);
void spi_lights_set_single_blinkmask(int index, int r, int g, int b,
        int blinkmask);
void spi_lights_set_single_cycler(int index, const int colors[8][3]);
void spi_lights_off(void);

#endif /* SPI_LIGHTS_H_ */
