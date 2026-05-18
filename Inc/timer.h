/*
 * timer.h
 *
 *  Created on: Apr 17, 2026
 *      Author: Phạm Hiệp
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include <stdint.h>

void timer_init(void);
void delay_us(uint32_t us);
void delay_ms(uint32_t ms);

#endif /* INC_TIMER_H_ */
