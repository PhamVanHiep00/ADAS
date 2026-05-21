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
    RCC->AHB1ENR |= (1 << 0);

    /* =========================
       PA1 -> TRIG OUTPUT
       ========================= */
    GPIOA->MODER |= (1 << 2);
    GPIOA->MODER &= ~(1 << 3);

    GPIOA->OTYPER &= ~(1 << 1);

    GPIOA->OSPEEDR |= (1 << 3);
    GPIOA->OSPEEDR |= (1 << 2);

    GPIOA->PUPDR &= ~(1 << 3);
    GPIOA->PUPDR &= ~(1 << 2);

    /* =========================
       PA0 -> ECHO TIM2_CH1 AF1
       ========================= */
    GPIOA->MODER &= ~(1 << 0);
    GPIOA->MODER |= (1 << 1);

    GPIOA->AFR[0] |= (1 << 0);
    GPIOA->AFR[0] &= ~(1 << 1);
    GPIOA->AFR[0] &= ~(1 << 3);
    GPIOA->AFR[0] &= ~(1 << 2);

    GPIOA->PUPDR &= ~(1 << 0);
    GPIOA->PUPDR &= ~(1 << 1);

}

void TIM2_IC_Init(void){
	RCC->APB1ENR |= (1 << 0);

	TIM2->PSC = 16 - 1;
	TIM2->ARR = 0xFFFFFFFF;

	TIM2->EGR |= (1 << 0);

	TIM2->CCMR1 |= (1 << 0);
	TIM2->CCMR1 &= ~(1 << 1);

	TIM2->CCER &= ~(1 << 1);

	TIM2->CCER |= (1 << 0);

	TIM2->DIER |= (1 << 1);

	TIM2->SR |= (1 << 1);

	NVIC_SetPriority(TIM2_IRQn, 0);
	NVIC_EnableIRQ(TIM2_IRQn);

	TIM2->CR1 |= (1 << 0);


}

static void TRIG_Pulse(void)
{
    GPIOA->BSRR |= (1 << 17);
    delay_us(2);

    GPIOA->BSRR |= (1 << 1);
    delay_us(10);

    GPIOA->BSRR |= (1 << 17);
}

void TIM2_IRQHandler(void){

	if(TIM2->SR & (1 << 1)){
		TIM2->SR &= ~(1 << 1);

		if(edge == 0){

			t1 = TIM2->CCR1;

			edge = 1;

			TIM2->CCER |= (1 << 1);

		}else{

			t2 = TIM2->CCR1;

			edge = 0;

			capture_done = 1;

			TIM2->CCER &= ~(1 << 1);

			uint32_t duration;

			if(t1 <= t2){
				duration = t2-t1;
			}else{

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
