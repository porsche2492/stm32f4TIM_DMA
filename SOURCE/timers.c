#include "timers.h"
#include "stm32f4xx_gpio.h"             
#include "stm32f4xx_rcc.h"              
#include <stdbool.h>

const uint16_t TIMER2_PRESCALER =  11;//Делитель для таймера 6 Таймер работает  на частоте 3.81 МГц
const uint16_t TIMER2_PERIOD = 1;//Период для таймера 2 
const uint16_t TIMER2_PULSE = 1;//Пульсация для таймера 2 

const uint16_t TIMER3_PRESCALER =  1;//Делитель для таймера 3 Таймер работает  на частоте 42 МГц
const uint16_t TIMER3_PERIOD = 0xFFFF;//Период для таймера 3

const uint16_t TIMER4_PRESCALER =  1;//Делитель для таймера 4 Таймер  работает на частоте 42 МГц
const uint16_t TIMER4_PERIOD = 0xFFFF;//Период для таймера 4 

void TimerOn(TIM_TypeDef* TIMx)// Включаем таймер
{
	TIM_Cmd(TIMx, ENABLE);
}
void TimerOff(TIM_TypeDef* TIMx)// Выключаем таймер
{
	TIM_Cmd(TIMx, DISABLE);
}


void InitPortTim2()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_1;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);//вызов функции инициализации
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);
}
void InitPortTim3()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	GPIO_StructInit(&GPIO_InitStruct);
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_TIM3);
}

void InitPortTim4()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	GPIO_StructInit(&GPIO_InitStruct);
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_TIM4);
}

void Tim2Init()
{
	TIM_TimeBaseInitTypeDef TIM2_BaseConfig;
	TIM_OCInitTypeDef TIM_OCConfig;
	
	
	InitPortTim2();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		
		//tim2
  	// Запускаем таймер на тактовой частоте
  TIM2_BaseConfig.TIM_Prescaler = TIMER2_PRESCALER - 1; //Частота таймера 1 Мгц
	TIM2_BaseConfig.TIM_Period = TIMER2_PERIOD;
	TIM2_BaseConfig.TIM_ClockDivision = 0;
		// Отсчет от нуля до TIM_Period
	TIM2_BaseConfig.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM2_BaseConfig);
	
	
		//
		// Конфигурируем второй выход таймера
	TIM_OCConfig.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCConfig.TIM_OutputState = TIM_OutputState_Enable;
		//полярность
	TIM_OCConfig.TIM_OCPolarity = TIM_OCPolarity_High;
		
	
	
	TIM_OCConfig.TIM_Pulse = TIMER2_PULSE;
	
  // Инициализируем второй выход таймера №2 (PA1)
  TIM_OC2Init(TIM2, &TIM_OCConfig);

	TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM2, ENABLE);
	// Включаем таймер
  //TIM_Cmd(TIM2, ENABLE);

}

void Tim3Init()
{
	TIM_TimeBaseInitTypeDef timer;
	TIM_ICInitTypeDef timer_ic;
	
	InitPortTim3();
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIM_TimeBaseStructInit(&timer);
	timer.TIM_Prescaler = TIMER3_PRESCALER - 1;
	timer.TIM_Period = TIMER3_PERIOD;
	timer.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM3, &timer);
	
	TIM_ICStructInit(&timer_ic);
	timer_ic.TIM_Channel = TIM_Channel_2;
	timer_ic.TIM_ICPolarity = TIM_ICPolarity_Rising;
	timer_ic.TIM_ICSelection = TIM_ICSelection_DirectTI;
	timer_ic.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	timer_ic.TIM_ICFilter = 0;
	TIM_ICInit(TIM3, &timer_ic);
	
	//TIM_ITConfig(TIM3, TIM_IT_CC2, ENABLE);// Разрешаем прерывание по захвату
	
	//TIM_DMAConfig(TIM3, TIM_DMABase_CCR2, TIM_DMABurstLength_16Transfers);//Длинна пакета 1
	TIM_DMACmd(TIM3, TIM_DIER_CC2DE, ENABLE);// Разрешаем формировать запрос к DMA от канала 2
	////////////////////////////////////////////////////////////////////////////TIM_DMACmd(TIM3, TIM_DMA_CC2, ENABLE);// Разрешаем работу  DMA для канала 2
	
	 //TIM_Cmd(TIM3, ENABLE);
}
void Tim4Init()
{
	TIM_TimeBaseInitTypeDef timer;
	TIM_ICInitTypeDef timer_ic;
	
	InitPortTim4();
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	TIM_TimeBaseStructInit(&timer);
	timer.TIM_Prescaler = TIMER4_PRESCALER - 1;// TIMER234_PRESCALER;
	timer.TIM_Period = TIMER4_PERIOD;// TIMER234_PERIOD;
	timer.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM4, &timer);
	
	TIM_ICStructInit(&timer_ic);
	timer_ic.TIM_Channel = TIM_Channel_3;
	timer_ic.TIM_ICPolarity = TIM_ICPolarity_Rising;
	timer_ic.TIM_ICSelection = TIM_ICSelection_DirectTI;
	timer_ic.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	timer_ic.TIM_ICFilter = 0;
	TIM_ICInit(TIM4, &timer_ic);
	
	TIM_ITConfig(TIM4, TIM_IT_CC3, ENABLE);;
}

void TimInit()
{	
	__enable_irq();// включить контроллер прерываний
	
	Tim2Init();// Инициализируем таймер 2 для ШИМ
	Tim3Init();// Инициализируем таймер 3 для Захвата
	//Tim4Init();// Инициализируем таймер 4 для Захвата
	
	//TimerOn(TIM4);
	
	NVIC_EnableIRQ(TIM3_IRQn);
	//NVIC_EnableIRQ(TIM4_IRQn);
	
}
