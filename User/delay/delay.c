#include "delay.h"
uint32_t TimingDelay;
void Delay_ms(__IO uint32_t nCount)
{
  while (nCount != 0)
  {
		nCount--;
	  Delay_us(1000);
  }
}
void Delay_us(uint16_t time)
{
		 TIM16->CNT=0;
	   while((TIM16->CNT)<time);   //�ȴ�?usʱ�䵽 
}

void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}


/*********************************************************************************************************************/
void Init_TIM16(void)                                                         //��ʼ����ʱ��16
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;                              //��ʼ����ʱ���õĽṹ��
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);                       //����TIM16��ʱ��ʱ��
	
	TIM_TimeBaseStructure.TIM_Prescaler=72000000/1000000-1;                     //�趨��ʱ��ʱ��Ƶ��Ϊ1MHz
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;                   //���ϼ���
	TIM_TimeBaseStructure.TIM_Period=0xFFFF;                                    //��65536�ص�0���¼���
	TIM_TimeBaseStructure.TIM_ClockDivision=0;                                  //����Ƶ
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;                              //�ظ�������ٴβŷ����жϣ�0����1,N+1
	TIM_TimeBaseInit(TIM16, &TIM_TimeBaseStructure);                            //��ʼ����ʱ��
	TIM_SetAutoreload(TIM16,0xFFFF);                                            //�Զ�������ֵ��Ӱ�ӼĴ���              
	TIM_ARRPreloadConfig(TIM16,ENABLE);                                         //�Զ�����ֵ����Ϊ����û�п��жϣ�����ʹ�ܻ���ʧ�ܶ��ǿ��Ե�
 	TIM_Cmd(TIM16, ENABLE);                                                     //������ʱ��
}
