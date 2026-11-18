/*
 * clock.h - System clock (144 MHz from HSIS) and 1 ms SysTick time base
 */
#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>

#define SYSCLK_HZ   144000000UL     /* HSIS, no crystal */
#define HCLK_HZ     SYSCLK_HZ       /* AHB  /1 */
#define PCLK1_HZ    SYSCLK_HZ       /* APB1 /1 */
#define PCLK2_HZ    SYSCLK_HZ       /* APB2 /1 */
#define PCLK3_HZ    SYSCLK_HZ       /* APB3 /1 */

void     clock_init(void);          /* call first in main() */
void     tick_init(void);           /* 1 ms SysTick interrupt */
uint32_t tick_ms(void);             /* ms since tick_init(), wraps after ~49 days */
void     delay_ms(uint32_t ms);

#endif /* CLOCK_H */
