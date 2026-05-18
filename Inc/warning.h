#ifndef WARNING_H
#define WARNING_H

#include "stm32f4xx.h"   /* Thay bằng header phù hợp với dòng MCU của bạn */

/* -----------------------------------------------------------------------
 * Sơ đồ chân phần cứng
 *   Green  LED : PC13
 *   Yellow LED : PC1
 *   Red    LED : PC0
 *   Buzzer     : PC3
 * ----------------------------------------------------------------------- */

/* Ngưỡng khoảng cách (cm) */
#define WARNING_DIST_SAFE       100.0f   /* > 100 cm  : an toàn   */
#define WARNING_DIST_MEDIUM      50.0f   /* 50–100 cm : trung bình */
                                         /* <= 50 cm  : nguy hiểm  */

/* -----------------------------------------------------------------------
 * Prototype
 * ----------------------------------------------------------------------- */

/**
 * @brief  Khởi tạo GPIO cho LED và Buzzer.
 *         Gọi một lần duy nhất trong hàm main() trước vòng lặp chính.
 */
void warning_init(void);

/**
 * @brief  Tắt toàn bộ LED và Buzzer.
 */
void warning_all_off(void);

/**
 * @brief  Cập nhật trạng thái cảnh báo theo khoảng cách đo được.
 * @param  distance  Khoảng cách (cm) từ cảm biến siêu âm.
 *
 *   distance > 100 cm  →  Green  LED sáng
 *   50 cm < distance ≤ 100 cm  →  Yellow LED sáng
 *   distance ≤ 50 cm   →  Red LED sáng + Buzzer kêu
 */
void warning_update(float distance);

#endif /* WARNING_H */
