#include <stdio.h>
#include "control.h"
#include "hcsr04.h"
#include "timer.h"
#include "display.h"
#include "warning.h"
#include "uart.h"

float distance;
char buffer[64];

void ctrl_init(void)
{
    GPIO_Init();
    TIM1_PWM_Init();
}

void hcsr04_control_init(void)
{
    timer_init();
    hc_init();
    TIM2_IC_Init();
}

void process(float distance){
	if (distance < 0)
	        {
	            display_print(0, 6, "NO ECHO   ");
	            warning_all_off();
	            go_forward(0);
	        }
	        else
	        {
	            display_print_float(0, 6, distance);
	            display_print(0, 11, "cm ");
	            warning_update(distance);

	            if(distance > 100.0f)
	            {
	                go_forward(2);
	                display_print(1, 0, "Safe Zone");
	            }
	            else if(distance >= 50.0f)
	            {
	                go_forward(1);
	                display_print(1, 0, "Warning Zone");
	            }
	            else
	            {
	                go_forward(0);
	                display_print(1, 0, "Danger Zone");
	            }
	        }


}

void uart_process(float distance){

	int temp = (int)(distance * 100);

	sprintf(buffer, "%d.%02d\r\n", temp / 100, temp % 100);

	UART1_WriteString(buffer);
}

int main(void)
{
    ctrl_init();
    hcsr04_control_init();
    display_init();
    warning_init();
    UART1_Init();

    delay_ms(100);

    display_print(0, 0, "Dist:");

    while (1)
    {
        distance = get_distance();
        process(distance);
        uart_process(distance);
        delay_ms(200);
    }
}
