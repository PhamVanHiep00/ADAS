/* display.c */
#include "display.h"
#include "timer.h"
#include <stdio.h>

/* ===== LCD ADDRESS ===== */
#define LCD_ADDR (0x27 << 1)

/* ===== LCD DEFINE ===== */
#define LCD_BACKLIGHT 0x08
#define ENABLE        0x04
#define RS             0x01

static char buffer[32];

/* ===== I2C INIT ===== */
/* PB8 = SCL */
/* PB9 = SDA */

static void i2c1_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    /* Alternate Function */
    GPIOB->MODER &= ~((3 << (8 * 2)) | (3 << (9 * 2)));
    GPIOB->MODER |=  ((2 << (8 * 2)) | (2 << (9 * 2)));

    /* AF4 = I2C1 */
    GPIOB->AFR[1] &= ~((0xF << 0) | (0xF << 4));
    GPIOB->AFR[1] |=  ((4 << 0) | (4 << 4));

    /* Open Drain */
    GPIOB->OTYPER |= (1 << 8) | (1 << 9);

    /* High Speed */
    GPIOB->OSPEEDR |= ((3 << (8 * 2)) | (3 << (9 * 2)));

    /* Pull Up */
    GPIOB->PUPDR &= ~((3 << (8 * 2)) | (3 << (9 * 2)));
    GPIOB->PUPDR |=  ((1 << (8 * 2)) | (1 << (9 * 2)));

    /* Reset I2C */
    I2C1->CR1 = I2C_CR1_SWRST;
    I2C1->CR1 = 0;

    /* APB1 = 16MHz */
    I2C1->CR2 = 16;

    /* 100kHz */
    I2C1->CCR = 80;

    I2C1->TRISE = 17;

    /* Enable I2C */
    I2C1->CR1 |= I2C_CR1_PE;
}

/* ===== I2C WRITE ===== */

static void i2c1_write(uint8_t data)
{
    while(I2C1->SR2 & I2C_SR2_BUSY);

    I2C1->CR1 |= I2C_CR1_START;
    while(!(I2C1->SR1 & I2C_SR1_SB));

    I2C1->DR = LCD_ADDR;
    while(!(I2C1->SR1 & I2C_SR1_ADDR));

    volatile uint32_t temp;

    temp = I2C1->SR1;
    temp = I2C1->SR2;

    while(!(I2C1->SR1 & I2C_SR1_TXE));

    I2C1->DR = data;

    while(!(I2C1->SR1 & I2C_SR1_BTF));

    I2C1->CR1 |= I2C_CR1_STOP;
}

/* ===== LCD LOW LEVEL ===== */

static void lcd_send_nibble(uint8_t nibble, uint8_t control)
{
    uint8_t data;

    data = nibble | control | LCD_BACKLIGHT;

    i2c1_write(data | ENABLE);
    delay_us(1);

    i2c1_write(data & ~ENABLE);
    delay_us(50);
}

static void lcd_send_cmd(uint8_t cmd)
{
    lcd_send_nibble(cmd & 0xF0, 0);
    lcd_send_nibble((cmd << 4) & 0xF0, 0);
}

static void lcd_send_data(uint8_t data)
{
    lcd_send_nibble(data & 0xF0, RS);
    lcd_send_nibble((data << 4) & 0xF0, RS);
}

/* ===== LCD ===== */

static void lcd_put_cur(uint8_t row, uint8_t col)
{
    uint8_t addr;

    if(row == 0)
        addr = 0x80 + col;
    else
        addr = 0xC0 + col;

    lcd_send_cmd(addr);
}

static void lcd_send_string(char *str)
{
    while(*str)
    {
        lcd_send_data(*str++);
    }
}

/* ===== DISPLAY ===== */

void display_init(void)
{
    i2c1_init();

    delay_ms(50);

    lcd_send_nibble(0x30, 0);
    delay_ms(5);

    lcd_send_nibble(0x30, 0);
    delay_us(200);

    lcd_send_nibble(0x30, 0);
    delay_us(200);

    lcd_send_nibble(0x20, 0);
    delay_us(200);

    lcd_send_cmd(0x28);
    lcd_send_cmd(0x0C);
    lcd_send_cmd(0x06);
    lcd_send_cmd(0x01);

    delay_ms(5);
}

void display_clear(void)
{
    lcd_send_cmd(0x01);
    delay_ms(2);
}

void display_print(uint8_t row, uint8_t col, char *str)
{
    lcd_put_cur(row, col);
    lcd_send_string(str);
}

void display_print_float(uint8_t row, uint8_t col, float value)
{
    int int_part;
    int decimal_part;

    if(value < 0)
    {
        value = -value;
    }

    int_part = (int)value;
    decimal_part = (int)((value - int_part) * 100);

    sprintf(buffer, "%d.%02d", int_part, decimal_part);

    lcd_put_cur(row, col);
    lcd_send_string("                ");

    lcd_put_cur(row, col);
    lcd_send_string(buffer);
}
