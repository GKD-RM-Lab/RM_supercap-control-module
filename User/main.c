#include "stm32f30x.h"
#include "delay.h"
#include "bsp_led.h"
#include "bsp_adc.h"
#include "bsp_hrtim.h"
#include "bsp_buck_control.h"
#include "bsp_pid.h"
#include "bsp_can.h"
#include "bsp_time.h"
#include "bsp_filter.h"
#include "bsp_out_control.h"
#include "stdio.h"
#include "usart.h"

int main(void)
{
	SysTick_Config(SystemCoreClock / 1000);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	Initial_prepheral_();			//��ʼ����ʱ��16�ʹ���1
	
	LED_GPIO_Config();
	Out_Control_GPIO_Config();
	ADC_Config();	
	
	bsp_PIDCtrlInit();//PID������ʼ��0.
	CAN1_Init(CAN_SJW_1tq,CAN_BS2_3tq,CAN_BS1_5tq,4);
	Bsp_TimeInit(50);	//����λ����������
	HRTIM_Config();
	
	HRTIM_WaveformOutputStart(HRTIM1,HRTIM_OUTPUT_TA1 | HRTIM_OUTPUT_TA2);//����PWM���
	HRTIM_WaveformOutputStart(HRTIM1,HRTIM_OUTPUT_TB1 | HRTIM_OUTPUT_TB2);
	while(1)
	{
		LED1(ON);
		Delay_ms(100);
		Print_state();
		LED1(OFF);
		Delay_ms(100);
	}
}

void HRTIM1_TIMA_IRQHandler(void)
{	
	InputPower_Control();
	HRTIM_ClearITPendingBit( HRTIM1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_TIM_FLAG_REP);	//����жϱ�־λ
}



//��λ������������50HZ�ٶ�
void TIM1_UP_TIM16_IRQHandler(void)
{
	float tem=(SCAP_Voltage-6.65f)*6.365f;
	if (tem<0) tem=0;
	else if (tem>99) tem=100;
	else tem=tem;
	int16_t data_to_C_board=float2int16(tem,Input_Power);
	CAN_SEND_DATA(data_to_C_board);
	TIM_ClearFlag( TIM1, TIM1_UP_TIM16_IRQn);
}


int fputc(int ch, FILE *stream)
{
    /* �����жϴ����Ƿ������ */
    while((USART1->ISR & 0X40) == 0);

    /* ���ڷ�����ɣ������ַ����� */
    USART1->TDR = (uint8_t) ch;

    return ch;
}

int16_t float2int16(float V, float W) {
  int8_t high=(int8_t)V; // ��ȡ�����ѹ����������
  int8_t low=(int8_t)W;  // ��ȡ���̹��ʵ���������

    // ȷ������������-128��127֮�䣬��Ϊint8_t�ܱ�ʾ�ķ�Χ��-128��127
  if (high<0)
	{
		high=0;
	} 
  else if (high>120)
	{
		 high=127;
	}
  if (low<0)
	{
		low=0;
	} 
  else if (low>127) 
	{
		low=127;
	}
  int16_t combined=(high<<8)|(uint8_t)low; // ��high����8λ����low��λ��
  return combined;
}

