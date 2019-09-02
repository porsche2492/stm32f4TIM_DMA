#include "RS232.h"
#include "timers.h"
#include "stm32f4xx_rcc.h"
#include "delay.h"


void Init(void)
{
	RCC_ClocksTypeDef tRCC;	//Delay
	RCC_ClockSecuritySystemCmd(ENABLE);
	RCC_HSEConfig(RCC_HSE_ON);
	
	while (!RCC_WaitForHSEStartUp());
	
	RCC_PLLCmd(DISABLE);
	while(1!=RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
	RCC_PLLConfig(RCC_PLLSource_HSE, 5, 210, 2, 7);
	RCC_PLLCmd(ENABLE);
	
	while(1!=RCC_GetFlagStatus(RCC_FLAG_PLLRDY));
	
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	
	RCC_PCLK1Config(RCC_HCLK_Div4);
	RCC_PCLK2Config(RCC_HCLK_Div2);
	
	RCC_PLLI2SCmd(DISABLE);
	RCC_PLLI2SConfig(240, 2);
	RCC_PLLI2SCmd(ENABLE);
	RCC_GetClocksFreq(&tRCC);
}
void HardInit(void)
{
	RCC_ClocksTypeDef tRCC;	//Delay
	RCC_GetClocksFreq(&tRCC); 
	delay_init(tRCC.HCLK_Frequency);
	
	RS232Init();//RS232
}

int main(void)
{
	HardInit();
	delay_ms(10000);
	Init();
	SystemCoreClockUpdate();
	HardInit();
	while(1)
	{
		RS232SendString("Старт Программы TimerFreq \r\n");
		delay_ms(1000);
	}
}
