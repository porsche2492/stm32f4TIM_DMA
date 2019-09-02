#ifndef __USE_DELAY_H
#define __USE_DELAY_H 
#include "stdint.h"

extern void delay_init(uint32_t wSystemCoreClock);
extern void delay_us(uint32_t wMicro);
extern void delay_ms(uint16_t hwMill);

#endif /* __USE_DELAY_H */

