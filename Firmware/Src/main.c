/*
 * main.c - Polarity Checker (STM32C551KEU6), bare metal / CMSIS only
 *
 * Step 1: system clock 144 MHz, 1 ms tick, GPIO port clocks.
 * Life sign: the display backlight (PB1 -> Q1) blinks at 1 Hz.
 */
#include "stm32c5xx.h"
#include "clock.h"

/* Backlight enable: PB1 (later TIM5_CH4 PWM, AF2) */
#define BL_PORT     GPIOB
#define BL_PIN      1U

static void gpio_clocks_init(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN;
    (void)RCC->AHB2ENR;                         /* read back: clock is active */
}

static void backlight_pin_init(void)
{
    BL_PORT->BSRR  = 1UL << (BL_PIN + 16U);                 /* start low = off */
    BL_PORT->MODER = (BL_PORT->MODER & ~(3UL << (BL_PIN * 2U)))
                   | (1UL << (BL_PIN * 2U));                /* general output */
}

int main(void)
{
    clock_init();
    tick_init();
    gpio_clocks_init();
    backlight_pin_init();

    for (;;) {
        BL_PORT->ODR ^= 1UL << BL_PIN;                       /* toggle */
        delay_ms(500);
    }
}
