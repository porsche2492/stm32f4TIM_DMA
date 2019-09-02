#include "delay.h"
#include "stm32f4xx.h"

static uint8_t  s_chFacMicro = 0;
static uint16_t s_hwFacMill = 0;

void delay_init(uint32_t wSystemCoreClock)	 
{
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	 
	s_chFacMicro = wSystemCoreClock / 8000000;	  //SystemCoreClock
	s_hwFacMill = (uint16_t)s_chFacMicro * 1000;//   
}								    

void delay_us(uint32_t wMicro)
{		
	uint32_t wTemp;
	
	SysTick->LOAD = wMicro * s_chFacMicro;   		 
	SysTick->VAL = 0x00;         
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;        
	do {
		wTemp = SysTick->CTRL;
	}
	while(wTemp & 0x01 && !(wTemp & (1 << 16)));   
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;       
	SysTick->VAL = 0X00;       
}

void delay_ms(uint16_t hwMill)
{	 		  	  
	uint32_t wTemp;
	
	SysTick->LOAD = (uint32_t)hwMill * s_hwFacMill;
	SysTick->VAL = 0x00;           
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk ;         
	do {
		wTemp = SysTick->CTRL;
	}
	while(wTemp & 0x01 && !(wTemp & (1 << 16)));
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;       
	SysTick->VAL = 0X00;       
} 

