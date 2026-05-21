#include "warning.h"

/* -----------------------------------------------------------------------
 * Định nghĩa chân (pin)
 *
 * GPIOC Pin Mapping:
 *   PC13 -> LED xanh lá   (Green LED)
 *   PC1  -> LED vàng      (Yellow LED)
 *   PC0  -> LED đỏ        (Red LED)
 *   PC3  -> Buzzer
 * ----------------------------------------------------------------------- */
#define LED_GREEN_PIN       13u   // PC13
#define LED_YELLOW_PIN       1u   // PC1
#define LED_RED_PIN          0u   // PC0
#define BUZZER_PIN           3u   // PC3

/* -----------------------------------------------------------------------
 * Macro tiện ích
 * ----------------------------------------------------------------------- */
#define PIN_SET(port, pin)    ((port)->ODR |=  (1u << (pin)))
#define PIN_CLR(port, pin)    ((port)->ODR &= ~(1u << (pin)))

/* -----------------------------------------------------------------------
 * warning_init
 * ----------------------------------------------------------------------- */
void warning_init(void)
{
    /* 1. Bật clock GPIOC (bit 2 của AHB1ENR) */
    RCC->AHB1ENR |= (1u << 2);

    /* 2. Cấu hình MODER = 01 (Output) cho 4 chân
     *    Mỗi chân chiếm 2 bit trong thanh ghi MODER
     *    Xóa 2 bit cũ → set bit thấp = 1
     */
    const uint8_t pins[] = {
        LED_GREEN_PIN,
        LED_YELLOW_PIN,
        LED_RED_PIN,
        BUZZER_PIN
    };

    for (uint8_t i = 0; i < 4u; i++)
    {
        /* Output mode (01) */
        GPIOC->MODER &= ~(3u << (pins[i] * 2u));
        GPIOC->MODER |=  (1u << (pins[i] * 2u));

        /* Push-pull output */
        GPIOC->OTYPER &= ~(1u << pins[i]);

        /* Không pull-up / pull-down */
        GPIOC->PUPDR &= ~(3u << (pins[i] * 2u));

        /* Tốc độ trung bình (10 MHz) – đủ dùng cho LED & Buzzer */
        GPIOC->OSPEEDR &= ~(3u << (pins[i] * 2u));
        GPIOC->OSPEEDR |=  (1u << (pins[i] * 2u));
    }

    /* 3. Tắt tất cả ngõ ra lúc khởi động */
    warning_all_off();
}

/* -----------------------------------------------------------------------
 * warning_all_off
 * ----------------------------------------------------------------------- */
void warning_all_off(void)
{
    PIN_CLR(GPIOC, LED_GREEN_PIN);   // Tắt LED xanh
    PIN_CLR(GPIOC, LED_YELLOW_PIN);  // Tắt LED vàng
    PIN_CLR(GPIOC, LED_RED_PIN);     // Tắt LED đỏ
    PIN_CLR(GPIOC, BUZZER_PIN);      // Tắt buzzer
}

/* -----------------------------------------------------------------------
 * warning_update
 *
 *   distance > 100 cm          -> Green LED
 *   50 cm < distance <= 100 cm -> Yellow LED
 *   distance <= 50 cm          -> Red LED + Buzzer
 * ----------------------------------------------------------------------- */
void warning_update(float distance)
{
    /* Tắt hết trước, rồi bật đúng trạng thái */
    warning_all_off();

    if (distance > WARNING_DIST_SAFE)
    {
        /* An toàn */
        PIN_SET(GPIOC, LED_GREEN_PIN);   // Bật LED xanh
    }
    else if (distance > WARNING_DIST_MEDIUM)
    {
        /* ⚠Cảnh báo trung bình */
        PIN_SET(GPIOC, LED_YELLOW_PIN);  // Bật LED vàng
    }
    else
    {
        /* Nguy hiểm */
        PIN_SET(GPIOC, LED_RED_PIN);     // Bật LED đỏ
        PIN_SET(GPIOC, BUZZER_PIN);      // Bật buzzer
    }
}
