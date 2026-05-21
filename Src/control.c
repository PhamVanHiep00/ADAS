#include "control.h"

#define SPEED_STOP    0
#define SPEED_LOW     400
#define SPEED_HIGH    750

void GPIO_Init(void)
{
    RCC->AHB1ENR |= (1 << 0);
    RCC->AHB1ENR |= (1 << 1);
    /*
        PWM

        PA8  -> TIM1_CH1 -> ENA
        PA11 -> TIM1_CH4 -> ENB
    */

    GPIOA->MODER &= ~((3 << (8 * 2))  |
                      (3 << (11 * 2)));

    GPIOA->MODER |=  ((2 << (8 * 2))  |
                      (2 << (11 * 2)));

    GPIOA->AFR[1] &= ~((0xF << ((8  - 8) * 4)) |
                       (0xF << ((11 - 8) * 4)));

    GPIOA->AFR[1] |=  ((1 << ((8  - 8) * 4)) |
                       (1 << ((11 - 8) * 4)));

    /*
        Direction pins

        IN1 -> PB0
        IN2 -> PB1
        IN3 -> PB13
        IN4 -> PB14
    */

    GPIOB->MODER &= ~((3 << (0 * 2))  |
                      (3 << (1 * 2))  |
                      (3 << (13 * 2)) |
                      (3 << (14 * 2)));

    GPIOB->MODER |=  ((1 << (0 * 2))  |
                      (1 << (1 * 2))  |
                      (1 << (13 * 2)) |
                      (1 << (14 * 2)));
}

void TIM1_PWM_Init(void)
{
    RCC->APB2ENR |= (1 << 0);

    TIM1->PSC = 16 - 1;

    TIM1->ARR = 999;

    /* ================= CH1 ================= */

    TIM1->CCMR1 &= ~(7 << 4);

    TIM1->CCMR1 |= (6 << 4);

    TIM1->CCMR1 |= TIM_CCMR1_OC1PE;

    /* ================= CH4 ================= */

    TIM1->CCMR2 &= ~(7 << 12);

    TIM1->CCMR2 |= (6 << 12);

    TIM1->CCMR2 |= TIM_CCMR2_OC4PE;

    TIM1->CCER |= TIM_CCER_CC1E
               |  TIM_CCER_CC4E;

    TIM1->BDTR |= TIM_BDTR_MOE;

    TIM1->CR1 |= TIM_CR1_ARPE;

    TIM1->EGR |= TIM_EGR_UG;

    TIM1->CR1 |= TIM_CR1_CEN;
}

void setSpeed(uint16_t s)
{
    if(s > 999)
    {
        s = 999;
    }

    TIM1->CCR1 = s;
    TIM1->CCR4 = s;
}

void go_forward(uint8_t level)
{
    uint16_t s;

    if(level == 0)
    {
        s = SPEED_STOP;   // 0%
    }
    else if(level == 1)
    {
        s = SPEED_LOW;    // 30%
    }
    else
    {
        s = SPEED_HIGH;   // 50%
    }

    GPIOB->BSRR = (1 << 1)  |
                  (1 << 14);

    GPIOB->BSRR = (1 << (0 + 16)) |
                  (1 << (13 + 16));

    setSpeed(s);
}
