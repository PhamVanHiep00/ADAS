/*
 * control.h
 *
 *  Created on: May 8, 2026
 *      Author: Hoàng Hiệp
 */

#ifndef INC_CONTROL_H_
#define INC_CONTROL_H_

#include "stm32f401xe.h"

#define SPEED_STOP 0
#define SPEED_HALF 500
#define SPEED_MAX  999

void GPIO_Init(void);

void TIM1_PWM_Init(void);

void setSpeed(uint16_t s);

void go_forward(uint8_t level);

#endif /* INC_CONTROL_H_ */
