#ifndef __BSP_ADC_H
#define __BSP_ADC_H

#include "stm32f30x.h"
#include "delay.h"

extern  uint16_t ADC_SampleBuf[4];  //ADC����ֵ,ͨ��DMA����
void ADC1_Config(void);
void ADC1_DMA_Config(void);
void ADC1_Config(void); 
void ADC_Config(void);

#endif
