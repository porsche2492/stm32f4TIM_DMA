#include "dma.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_rcc.h"
#include "stdbool.h"

uint16_t timer3_buffer[BUFFER_SIZE];

bool dma_is_ready = false;

void DMA1_Stream5_IRQHandler(void)
{

	//DMA_ClearITPendingBit(DMA1_Stream5, DMA_IT_TC);
	NVIC_DisableIRQ(DMA1_Stream5_IRQn);
	DMA_Cmd(DMA1_Stream5, DISABLE);
	dma_is_ready = true;
}
void DMA1Init()
{
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

	// DMA  Init Timer3 Channel 2, DMA Stream 5 Channel 5. 
	DMA_DeInit(DMA1_Stream5);
	DMA_InitStructure.DMA_Channel = DMA_Channel_5;//  5 ����� DMA ��� ������� 3 ������ 2
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) & (TIM3->CCR2); //����� �������� ������� 3 ������ 2
	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t) &timer3_buffer[0];// ������� ����� ������
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;// ����������� �������� ������ �� ��������� � ������
	DMA_InitStructure.DMA_BufferSize = BUFFER_SIZE;// ������ ������  - 16
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;// ����� ��������� ����� ������ ��������� �� ����������������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;// ����� ������ ����� ������ ��������� ����������������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;// ����������� ������ ���������� - 16 ���
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;// ����������� ������ ������ - 16 ���
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// ��������� ������ DMA � ����������� ������
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;// ��������� ����� �������
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;// ��������� �������
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	DMA_InitStructure.DMA_MemoryBurst = DMA_PeripheralBurst_Single; // ������ ������������ ������ �� ������ 8 ��� �� ����� ����������
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; //������ ������������� ������ �� ���������
	DMA_Init(DMA1_Stream5, &DMA_InitStructure);


	DMA_Cmd(DMA1_Stream5, ENABLE); // ��������� ������  5 ������ DMA
	
	DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE);// ��������� ���������� ����� ��� ������ ��������� � �����
}

void DmaInit(void)
{
	__enable_irq();
	
	DMA1Init();
	
	NVIC_EnableIRQ(DMA1_Stream5_IRQn);
}
