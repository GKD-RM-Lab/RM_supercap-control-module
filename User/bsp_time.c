#include  "bsp_time.h"


//��ʱ��2��ʼ��

void Bsp_TimeInit(uint32_t freq)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //ʱ��ʹ��
	
	TIM_TimeBaseStructure.TIM_Period            = 1e6/freq;            //�Ƚ�ֵ,8e6 = SystemCoreClock/(Prescaler+1)
	TIM_TimeBaseStructure.TIM_Prescaler         = 63;                  //Ԥ��Ƶ,SystemCoreClock/(Prescaler+1)�ļ���Ƶ��  
	TIM_TimeBaseStructure.TIM_ClockDivision     = 0;                   //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);                    //����TIM_TimeBaseInitStruct��ָ���Ĳ���
                                                                     //��ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(  
			TIM1,            //TIMx
			TIM_IT_Update,   //TIM �����ж�Դ 
			ENABLE           //ʹ��
		);                 //ʹ�ܻ���ʧ��ָ����TIM�ж�
	
	NVIC_InitStructure.NVIC_IRQChannel                     = TIM1_UP_TIM16_IRQn; //TIM1�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority   = 1;             //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority          = 3;             //�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd                  = ENABLE;        //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  	//����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd( TIM1, ENABLE ); //ʹ��TIMx����	
}

