/*
 * display.h
 *
 *  Created on: May 8, 2026
 *      Author: pvhiep
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_


#include "stm32f4xx.h"

void display_init(void);
void display_clear(void);

void display_print(uint8_t row, uint8_t col, char *str);
void display_print_float(uint8_t row, uint8_t col, float value);

#endif /* INC_DISPLAY_H_ */
