#ifndef __INIT_H__
#define __INIT_H__

#include "stm32f30x.h"
#include "stm32f30x_conf.h"
#include "delay.h"
#include "stm32f30x_tim.h"


#define   Disable   0
#define   Enable    1
void Init_TIM16(void);				//��ʼ����ʱ��

void GPIO_InitConfig(void);       //��ʼ��IO��

void Initial_prepheral_(void);    //��ʼ�����裬����ο������ڲ�

void USART1_Config(void);				//��ʼ������1

void Print_state(void);					//��ӡ��ǰ����ADC״̬			

extern float Input_Voltage,Input_Current,Input_Power,Available_Power,SCAP_Voltage,SCAP_Current,Limit_Power,Input_Power;//��������ת��ֵ
extern uint8_t Mode,loop_mode,Transition_Mode;
extern uint16_t PWM_DUTY;
#endif

