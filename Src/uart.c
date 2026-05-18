#include "uart.h"

void UART1_Init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    GPIOA->MODER &= ~((3 << (9 * 2)) |
                      (3 << (10 * 2)));

    GPIOA->MODER |=  ((2 << (9 * 2)) |
                      (2 << (10 * 2)));

    GPIOA->AFR[1] &= ~((0xF << ((9 - 8) * 4)) |
                       (0xF << ((10 - 8) * 4)));

    GPIOA->AFR[1] |=  ((7 << ((9 - 8) * 4)) |
                       (7 << ((10 - 8) * 4)));

    USART1->BRR = 0x008B;

    USART1->CR1 = USART_CR1_TE |
                  USART_CR1_RE |
                  USART_CR1_UE;
}

void UART1_WriteChar(char c)
{
    while (!(USART1->SR & USART_SR_TXE));

    USART1->DR = c;
}

void UART1_WriteString(char *str)
{
    while (*str)
    {
        UART1_WriteChar(*str++);
    }
}
