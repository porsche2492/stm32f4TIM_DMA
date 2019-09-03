#include "RS232.h"
#include "stm32f4xx_gpio.h"             
#include "stm32f4xx_usart.h"            
#include "stm32f4xx_rcc.h"
#include "string.h"
#include "stdbool.h"

//uint16_t array[8];
//uint16_t pos;

uint8_t rs232_value;

bool rs232_is_ready = false;

void LedInit()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12; 		 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;	
	GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void LedOn()
{
	GPIO_SetBits(GPIOC, GPIO_Pin_12);
}
void LedOff()
{
	GPIO_ResetBits(GPIOC, GPIO_Pin_12);
}

void Uart2PortInit()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;	 //UART1 TX	 		 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	//GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;	 //UART1	RX	 		 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	//GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
}

void Uart2Init()
{
	USART_InitTypeDef USART_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
		
	USART_StructInit(&USART_InitStruct);
	USART_InitStruct.USART_BaudRate = 115200;
  USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_OverSampling8Cmd(USART1, ENABLE);
	USART_Init(USART1, &USART_InitStruct);	
	
	NVIC_EnableIRQ(USART1_IRQn);
	USART_SetPrescaler(USART1, 1);
	USART_Cmd(USART1, ENABLE);	
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	
}
void RS232Init()
{
	__enable_irq();// включить контроллер прерываний
	LedInit();
	Uart2PortInit();
	Uart2Init();
	
	//pos = 0;
}

void RS232Send(uint8_t data)
{
	while(!USART_GetFlagStatus(USART1, USART_FLAG_TC));
	USART1->DR = data;
	//USART_SendData(USART1, data);
}

void RS232SendString(char*str)
{
	//uint8_t mass[5]={0xFC, 0xFC, 0xFC, 0xFF, 0x0};
	unsigned int count = strlen(str);
	LedOff();
	for(unsigned int i = 0; i < count;i++)
	//while(1)
	{
		RS232Send(str[i]);
		//RS232Send(0x55);
	}
	LedOn();
}

void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE))
	{
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		//if(pos<8)
		//{
		//	array[pos] = USART_ReceiveData(USART1);
		//	pos++;
		//}
		//else{		pos = 0;}
		rs232_value = USART_ReceiveData(USART1);
		rs232_is_ready = true;
		
	}
}

