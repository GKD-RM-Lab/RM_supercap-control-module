#ifndef __BSP_HRTIM_H
#define __BSP_HRTIM_H

#include "stm32f30x.h"
#include "bsp_buck_control.h"

#define FSW 	         	250E3	//����Ƶ��250K
#define PERIOD 	         	(uint16_t)(4608E6/FSW)  //4608E6����ʱ��HSI(72MHz)��2��Ƶ����32��Ƶ�õ���HRTIMʱ��(4.608GHz)

#define DeadTime_Value		200	//����ʱ��

#define MAX_CompareValue1 	(uint16_t)(1.13f*PERIOD)
#define MAX_CompareValue2 	(uint16_t)(1.65f*PERIOD)
#define MIN_CompareValue 	(uint16_t)(0.01f*PERIOD)
#define PERIOD_Half      	(uint16_t)(0.5f*PERIOD)
#define MAX_Duty         	(0.9f*PERIOD)
#define MIN_Duty         	(uint16_t)(0.1f*PERIOD)


extern uint16_t ADC_SampleBuf[4];//ADC����ֵ,ͨ��DMA����

void HRTIM_Config(void);
void PWM_Update(uint16_t CompareValue);

#endif
