#include "timers.h"
#include "stm32f4xx_gpio.h"             
#include "stm32f4xx_rcc.h"              
#include "stm32f4xx_tim.h"              

const uint16_t _CONST = 1;

//const uint16_t TIMER1_PRESCALER = 84;//Делитель для таймера 1 
//const uint16_t TIMER1_PERIOD = 1000;//Период для таймера 1  (Период 1 мс) Таймер на частоте 1 МГц

const uint16_t TIMER234_PRESCALER = 168;//Делитель для таймеров 2, 3, 4 Таймер на частоте 1 МГц
const uint16_t TIMER234_PERIOD = 1000;//Период для таймеров 2, 3, 4 (Период 1 мс)

uint32_t freq1;//Частота 1
uint32_t freq2;//Частота 2
uint32_t freq3;//Частота 3

volatile uint16_t Tim2Capture;//Количество импульсов 2 таймера
volatile uint16_t Tim3Capture;//Количество импульсов 3 таймера
volatile uint16_t Tim4Capture;//Количество импульсов 4 таймера

void Tim234Off(void);
void Tim234On(void);
void ClearCaptureTim234(void);

/*void TIM1_UP_TIM10_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		Tim234Off();
		
		freq1 = (Tim2Capture / TIMER1_PERIOD) * _CONST; // freq = 1 / T
		freq2 = (Tim3Capture / TIMER1_PERIOD) * _CONST; 
		freq3 = (Tim4Capture / TIMER1_PERIOD) * _CONST; 
		
		ClearCaptureTim234();
		Tim234On();
	}
}*/

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC2);
		Tim2Capture++;
	}	
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);	
		freq1 = (Tim2Capture / TIMER234_PERIOD) * _CONST; // freq = 1 / T 	
		ClearCaptureTim234();
	}
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_CC2) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC2);
		Tim3Capture++;
	}	
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);	
		freq2 = (Tim3Capture / TIMER234_PERIOD) * _CONST; // freq = 1 / T 	
		ClearCaptureTim234();
	}
}

void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_CC3) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_CC3);
		Tim4Capture++;
	}	
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);	
		freq3 = (Tim4Capture / TIMER234_PERIOD) * _CONST; // freq = 1 / T 	
		ClearCaptureTim234();
	}
}

void InitPortTim2()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIO_StructInit(&GPIO_InitStruct);
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM2);
}

void InitPortTim3()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	GPIO_StructInit(&GPIO_InitStruct);
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM3);
}

void InitPortTim4()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	GPIO_StructInit(&GPIO_InitStruct);
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_TIM4);
}

/*void Tim1Init()
{
	TIM_TimeBaseInitTypeDef timer;	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);	
	TIM_TimeBaseStructInit(&timer);
	timer.TIM_Prescaler = TIMER1_PRESCALER;
	timer.TIM_Period = TIMER1_PERIOD;
	TIM_TimeBaseInit(TIM1, &timer);
	
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE);// прерывание таймера 1 по переполнению 
}*/
void Tim2Init()
{
	TIM_TimeBaseInitTypeDef timer;
	TIM_ICInitTypeDef timer_ic;
	
	InitPortTim2();
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	TIM_TimeBaseStructInit(&timer);
	timer.TIM_Prescaler = TIMER234_PRESCALER;
	timer.TIM_Period = TIMER234_PERIOD;
	TIM_TimeBaseInit(TIM2, &timer);
	
	TIM_ICStructInit(&timer_ic);
	timer_ic.TIM_Channel = TIM_Channel_2;
	timer_ic.TIM_ICPolarity = TIM_ICPolarity_Rising;
	timer_ic.TIM_ICSelection = TIM_ICSelection_DirectTI;
	timer_ic.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	timer_ic.TIM_ICFilter = 0;
	TIM_ICInit(TIM2, &timer_ic);
	
	TIM_ITConfig(TIM2, TIM_IT_CC2|TIM_IT_Update, ENABLE); //Прерывание по сигналу от канала 2
}
void Tim3Init()
{
	TIM_TimeBaseInitTypeDef timer;
	TIM_ICInitTypeDef timer_ic;
	
	InitPortTim3();
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	TIM_TimeBaseStructInit(&timer);
	timer.TIM_Prescaler = TIMER234_PRESCALER;
	timer.TIM_Period = TIMER234_PERIOD;
	TIM_TimeBaseInit(TIM3, &timer);
	
	TIM_ICStructInit(&timer_ic);
	timer_ic.TIM_Channel = TIM_Channel_2;
	timer_ic.TIM_ICPolarity = TIM_ICPolarity_Rising;
	timer_ic.TIM_ICSelection = TIM_ICSelection_DirectTI;
	timer_ic.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	timer_ic.TIM_ICFilter = 0;
	TIM_ICInit(TIM3, &timer_ic);
	
	TIM_ITConfig(TIM3, TIM_IT_CC2|TIM_IT_Update, ENABLE);
}
void Tim4Init()
{
	TIM_TimeBaseInitTypeDef timer;
	TIM_ICInitTypeDef timer_ic;
	
	InitPortTim4();
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	TIM_TimeBaseStructInit(&timer);
	timer.TIM_Prescaler = TIMER234_PRESCALER;
	timer.TIM_Period = TIMER234_PERIOD;
	TIM_TimeBaseInit(TIM4, &timer);
	
	TIM_ICStructInit(&timer_ic);
	timer_ic.TIM_Channel = TIM_Channel_3;
	timer_ic.TIM_ICPolarity = TIM_ICPolarity_Rising;
	timer_ic.TIM_ICSelection = TIM_ICSelection_DirectTI;
	timer_ic.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	timer_ic.TIM_ICFilter = 0;
	TIM_ICInit(TIM4, &timer_ic);
	
	TIM_ITConfig(TIM4, TIM_IT_CC3|TIM_IT_Update, ENABLE);
}
void Tim234Off()
{
	TIM_Cmd(TIM2, DISABLE);
	TIM_Cmd(TIM3, DISABLE);
	TIM_Cmd(TIM4, DISABLE);
}
void Tim234On()
{
	TIM_Cmd(TIM2, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
}

void ClearCaptureTim234()
{
	Tim2Capture = 0; Tim3Capture = 0; Tim4Capture = 0;
}

void TimInit()
{
	freq1 = 0; freq2 = 0; freq3 = 0;
	ClearCaptureTim234();
	
	__enable_irq();// включить контроллер прерываний
	
	//Tim1Init();
	Tim2Init();
	Tim3Init();
	Tim4Init();
	
	//TIM_Cmd(TIM1, ENABLE);// Включить таймер 1
	Tim234On();// Включить таймера 2, 3, 4
	
	NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
	NVIC_EnableIRQ(TIM2_IRQn);
	NVIC_EnableIRQ(TIM3_IRQn);
	NVIC_EnableIRQ(TIM4_IRQn);
}
