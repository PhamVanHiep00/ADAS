#include "timer.h"
#include "stm32f4xx.h"

void timer_init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    TIM3->PSC = 16 - 1;   // 1us tick
    TIM3->ARR = 0xFFFFFFFF;
    TIM3->CNT = 0;

    TIM3->CR1 |= TIM_CR1_CEN;
}

void delay_us(uint32_t us)
{
    uint32_t start = TIM3->CNT;

    while ((TIM3->CNT - start) < us);
}

void delay_ms(uint32_t ms)
{
    while (ms--)
    {
        delay_us(1000);
    }
}
