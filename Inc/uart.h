/*
 * uart.h
 *
 *  Created on: May 17, 2026
 *      Author: Hoàng Hiệp
 */

#ifndef INC_UART_H_
#define INC_UART_H_

#include "stm32f4xx.h"

void UART1_Init(void);

void UART1_WriteChar(char c);

void UART1_WriteString(char *str);


#endif /* INC_UART_H_ */
