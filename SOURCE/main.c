#include "RS232.h"
#include "timers.h"
#include "stm32f4xx_rcc.h"
#include "delay.h"
#include "stdbool.h"
#include "stdio.h"
#include "dma.h"



void MCO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;	 //UART1 TX	 		 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_MCO);
	
	RCC_MCO1Config(RCC_MCO1Source_PLLCLK, RCC_MCO1Div_1);
	
}

void Init(void)
{
	RCC_ClocksTypeDef tRCC;	//Delay
	
	//RCC_DeInit();
	
	RCC_ClockSecuritySystemCmd(ENABLE);
	RCC_HSEConfig(RCC_HSE_ON);
	
	if (RCC_WaitForHSEStartUp() == SUCCESS)
	{
		RCC_PLLCmd(DISABLE);
		while(RESET == RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
		RCC_PLLConfig(RCC_PLLSource_HSE, 8, 336, 2, 7);
		RCC_PLLCmd(ENABLE);
	
		while(RESET == RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
	
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	
		RCC_PCLK1Config(RCC_HCLK_Div4);
		RCC_PCLK2Config(RCC_HCLK_Div2);
	
		RCC_PLLI2SCmd(DISABLE);
		RCC_PLLI2SConfig(240, 2);
		RCC_PLLI2SCmd(ENABLE);
	}
	
	SystemCoreClockUpdate();
	
	RCC_GetClocksFreq(&tRCC);
}
void HardInit(void)
{
	RCC_ClocksTypeDef tRCC;	//Delay
	RCC_GetClocksFreq(&tRCC); 
	delay_init(tRCC.HCLK_Frequency);
	
	MCO_Init();
	
	RS232Init();//RS232
	DmaInit();
	TimInit();
}

uint16_t TimeDiff(uint16_t now, uint16_t before)
{
	return (now >= before) ? (now - before) : (UINT16_MAX - before + now);
}

void SendFreq()// функция для отправки частоты по RS232
{
	extern uint16_t timer3_buffer[BUFFER_SIZE];
	
	uint8_t *pointerFreq;
	
	uint16_t dT = 0;
	float freq = 0;
	
	for (int i = 1; i < BUFFER_SIZE - 1; i++)
	{
		dT = TimeDiff (timer3_buffer[i + 1], timer3_buffer[i]);
		freq = (42.0f * 2 ) / dT; // Частота
    pointerFreq = (uint8_t*) &freq;
		
		for(int j = 0; j < 4; j++)
		{
			RS232Send( *(pointerFreq++));
		}
	}
}

void ConnectRS232()
{
	extern bool rs232_is_ready;// Готовность что данные пришли по RS232;
	extern uint8_t rs232_value; // Значение
	
	bool connect_completed = false;
	while (!connect_completed)
	{
		if(rs232_is_ready)
		{
			if (rs232_value == 0x21)// Подключение выполнено
			{				 
				rs232_is_ready = true;
				connect_completed = true;
				RS232Send (0x99);// Отправляем Запрос на подключение по RS23
			}
		}
		else 
		{
			RS232Send (0x11);// Отправляем Запрос на подключение по RS232
		} 
	}
}

int main(void)
{
  extern bool dma_is_ready; // Готовность данных для передачи по DMA
	extern bool rs232_is_ready;// Готовность что данные пришли по RS232;
	extern uint8_t rs232_value;
	
	bool start = false;

	HardInit(); // Инициализируем периферию 

	ConnectRS232(); // Выполняем подключение
	while(1)
	{	
		if(rs232_is_ready)
		{
			switch (rs232_value)
			{
				case 0x12://Включить
				{
					TimerOn(TIM2);
					TimerOn(TIM3);
					start = true;
					break;
				}
				case 0x13://Выключить
				{
					TimerOff(TIM2);
					TimerOff(TIM3);
					start = false;
					break;
				}
				default:
					break;
			}
			rs232_is_ready = false;
		}
		
		if (dma_is_ready)
		{
			if (start)
				SendFreq();
			dma_is_ready = false;
			NVIC_EnableIRQ(DMA1_Stream5_IRQn);
			DMA_Cmd(DMA1_Stream5, ENABLE);
		}
		
	}
}
