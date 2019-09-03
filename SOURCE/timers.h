#ifndef __TIMERS_H
#define __TIMERS_H
#include "stm32f4xx_tim.h"
#include "stdint.h"

void TimInit(void);
void TimerOn(TIM_TypeDef* TIMx);
void TimerOff(TIM_TypeDef* TIMx);
void TimerClearCount(TIM_TypeDef* TIMx);

#endif /* __TIMERS_H */
