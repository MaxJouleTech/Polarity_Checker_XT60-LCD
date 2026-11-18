/*
 * clock.c - System clock and time base, register level (CMSIS only)
 *
 * After reset the STM32C551 runs from HSIDIV3 = 48 MHz with 0 flash wait states.
 * clock_init() switches SYSCLK to HSIS = 144 MHz (internal, +/-1 %, no crystal):
 *
 *   1. enable HSIS and wait until it is ready
 *   2. AHB/APB prescalers /1 (all buses at 144 MHz, the maximum)
 *   3. flash latency 4 WS  (RM table, range 1: 128 < HCLK <= 144 MHz)
 *      -> latency must be raised BEFORE the frequency goes up
 *   4. switch SYSCLK to HSIS and wait for the switch status
 *   5. flash programming delay for 136-170 MHz (needed for flash writes later)
 *   6. enable the instruction cache (0-wait-state execution from flash)
 *
 * Same sequence as ST's generated mx_rcc_init(), without the HAL.
 */
#include "stm32c5xx.h"
#include "clock.h"

#define FLASH_LATENCY_WS        4U

static volatile uint32_t s_ms;

void clock_init(void)
{
    /* 1. HSIS on */
    RCC->CR1 |= RCC_CR1_HSISON;
    while ((RCC->CR1 & RCC_CR1_HSISRDY) == 0U) { }

    /* 2. All bus prescalers /1 (reset value, written explicitly for clarity) */
    RCC->CFGR2 &= ~(RCC_CFGR2_HPRE | RCC_CFGR2_PPRE1 | RCC_CFGR2_PPRE2 | RCC_CFGR2_PPRE3);

    /* 3. Flash wait states up first, read back to make sure they are applied */
    FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_LATENCY) | (FLASH_LATENCY_WS << FLASH_ACR_LATENCY_Pos);
    while ((FLASH->ACR & FLASH_ACR_LATENCY) != (FLASH_LATENCY_WS << FLASH_ACR_LATENCY_Pos)) { }

    /* 4. SYSCLK = HSIS (SW = 01) */
    RCC->CFGR1 = (RCC->CFGR1 & ~RCC_CFGR1_SW) | RCC_CFGR1_SW_0;
    while ((RCC->CFGR1 & RCC_CFGR1_SWS) != RCC_CFGR1_SWS_0) { }

    /* 5. Flash programming delay: 136-170 MHz range */
    FLASH->ACR = (FLASH->ACR & ~FLASH_ACR_WRHIGHFREQ) | FLASH_ACR_WRHIGHFREQ_1;

    /* 6. Instruction cache on */
    ICACHE->CR |= ICACHE_CR_EN;

    /* Keep the CMSIS variable in sync (reads RCC, gives 144 MHz) */
    SystemCoreClockUpdate();
}

/* ---- 1 ms time base ------------------------------------------------------ */

void SysTick_Handler(void)
{
    s_ms++;
}

void tick_init(void)
{
    SysTick_Config(SystemCoreClock / 1000U);   /* reload, priority, enable */
}

uint32_t tick_ms(void)
{
    return s_ms;
}

void delay_ms(uint32_t ms)
{
    uint32_t start = s_ms;
    while ((s_ms - start) < ms) {
        __WFI();                               /* sleep until the next tick */
    }
}
