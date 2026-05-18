/*
 * hcsr04.h
 *
 *  Created on: Apr 17, 2026
 *      Author: Phạm Hiệp
 */

#ifndef INC_HCSR04_H_
#define INC_HCSR04_H_

#include <stdint.h>

void hc_init(void);
void TIM2_IC_Init(void);
float get_distance(void);

#endif /* INC_HCSR04_H_ */
