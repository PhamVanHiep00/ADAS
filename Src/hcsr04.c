#include "hcsr04.h"
#include "timer.h"
#include "stm32f4xx.h"

static volatile uint32_t t1 = 0;
static volatile uint32_t t2 = 0;
static volatile uint8_t edge = 0;
static volatile uint8_t capture_done = 0;
static volatile float distance = -1;

void hc_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    /* =========================
       PA1 -> TRIG OUTPUT
       ========================= */
    GPIOA->MODER &= ~(3U << (1 * 2));
    GPIOA->MODER |=  (1U << (1 * 2));

    GPIOA->OTYPER &= ~(1U << 1);

    GPIOA->OSPEEDR |= (3U << (1 * 2));

    GPIOA->PUPDR &= ~(3U << (1 * 2));

    /* =========================
       PA0 -> ECHO TIM2_CH1 AF1
       ========================= */
    GPIOA->MODER &= ~(3U << (0 * 2));
    GPIOA->MODER |=  (2U << (0 * 2));

    GPIOA->AFR[0] &= ~(0xFU << (0 * 4));
    GPIOA->AFR[0] |=  (1U << (0 * 4));

    GPIOA->PUPDR &= ~(3U << (0 * 2));
}

void TIM2_IC_Init(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    TIM2->PSC = 16 - 1;
    TIM2->ARR = 0xFFFFFFFF;

    TIM2->EGR |= TIM_EGR_UG;

    TIM2->CCMR1 &= ~TIM_CCMR1_CC1S;
    TIM2->CCMR1 |= TIM_CCMR1_CC1S_0;

    TIM2->CCER &= ~TIM_CCER_CC1P;

    TIM2->CCER |= TIM_CCER_CC1E;

    TIM2->DIER |= TIM_DIER_CC1IE;

    TIM2->SR = 0;

    NVIC_SetPriority(TIM2_IRQn, 0);
    NVIC_EnableIRQ(TIM2_IRQn);

    TIM2->CR1 |= TIM_CR1_CEN;
}

static void TRIG_Pulse(void)
{
    GPIOA->BSRR = (1U << (1 + 16));
    delay_us(2);

    GPIOA->BSRR = (1U << 1);
    delay_us(10);

    GPIOA->BSRR = (1U << (1 + 16));
}

void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_CC1IF)
    {
        TIM2->SR &= ~TIM_SR_CC1IF;

        if(edge == 0)
        {
            t1 = TIM2->CCR1;

            edge = 1;

            /* Switch to falling edge */
            TIM2->CCER |= TIM_CCER_CC1P;
        }
        else
        {
            t2 = TIM2->CCR1;

            edge = 0;

            capture_done = 1;

            /* Switch back to rising edge */
            TIM2->CCER &= ~TIM_CCER_CC1P;

            uint32_t duration;

            if(t2 >= t1)
            {
                duration = t2 - t1;
            }
            else
            {
                duration = (0xFFFFFFFF - t1) + t2;
            }

            distance = duration * 0.0343f / 2.0f;
        }
    }
}

float get_distance(void)
{
    capture_done = 0;

    edge = 0;

    TIM2->CCER &= ~TIM_CCER_CC1P;

    TIM2->CNT = 0;

    TRIG_Pulse();

    uint32_t timeout = 60000;

    while(!capture_done)
    {
        if(TIM2->CNT > timeout)
        {
            return -1;
        }
    }

    return distance;
}
