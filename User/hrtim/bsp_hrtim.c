#include "bsp_hrtim.h"

void HRTIM_Config(void)
{
	GPIO_InitTypeDef 			GPIO_InitStructure;
	HRTIM_OutputCfgTypeDef	 	HRTIM_TIM_OutputStructure;
	HRTIM_BaseInitTypeDef 		HRTIM_BaseInitStructure;
	HRTIM_TimerInitTypeDef 		HRTIM_TimerInitStructure;  
	HRTIM_TimerCfgTypeDef 		HRTIM_TimerWaveStructure;
	HRTIM_DeadTimeCfgTypeDef 	HRTIM_DeadtimeStructure;
	HRTIM_ADCTriggerCfgTypeDef	HRTIM_ADCTrigStructure;
	NVIC_InitTypeDef 			NVIC_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;  
	GPIO_InitStructure.GPIO_Speed 	= GPIO_Speed_50MHz;  
	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType 	= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd	= GPIO_PuPd_NOPULL;  
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8 , GPIO_AF_13);//PA8����
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9 , GPIO_AF_13);//PA9����  
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_13);//PA10����
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource11, GPIO_AF_13);//PA11����
	
	RCC_HRTIM1CLKConfig(RCC_HRTIM1CLK_PLLCLK);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_HRTIM1,ENABLE);
 
	HRTIM_DLLCalibrationStart(HRTIM1, HRTIM_CALIBRATIONRATE_14);
	//while(HRTIM_GetCommonFlagStatus(HRTIM1, HRTIM_ISR_DLLRDY) == RESET);

	/* --------------------------------------------------- */
	/* HRTIM_TIMERINDEX_MASTER initialization: timer mode and PWM frequency */
	/* --------------------------------------------------- */
	HRTIM_BaseInitStructure.Period					= PERIOD;// HRTIM���ڼĴ���, Period = fHRCK / fsw
	HRTIM_BaseInitStructure.PrescalerRatio			= HRTIM_PRESCALERRATIO_MUL32;//HRTIMʱ�ӱ�Ƶϵ��
	HRTIM_BaseInitStructure.Mode					= HRTIM_MODE_CONTINOUS;//HRTIM����������ģʽ
	HRTIM_BaseInitStructure.RepetitionCounter		= 7;	// �ж�Ƶ�� = FSW/(7+1)
	
	HRTIM_TimerInitStructure.HalfModeEnable 		= HRTIM_HALFMODE_DISABLED;
	HRTIM_TimerInitStructure.StartOnSync 			= HRTIM_SYNCSTART_DISABLED;
	HRTIM_TimerInitStructure.ResetOnSync 			= HRTIM_SYNCRESET_DISABLED;
	HRTIM_TimerInitStructure.DACSynchro 			= HRTIM_DACSYNC_NONE;
	HRTIM_TimerInitStructure.PreloadEnable 			= HRTIM_PRELOAD_DISABLED;//ֱ�Ӽ���
	HRTIM_TimerInitStructure.UpdateGating 			= HRTIM_UPDATEGATING_INDEPENDENT;
	HRTIM_TimerInitStructure.BurstMode 				= HRTIM_TIMERBURSTMODE_MAINTAINCLOCK;
	HRTIM_TimerInitStructure.RepetitionUpdate 		= HRTIM_UPDATEONREPETITION_ENABLED;

	HRTIM_Waveform_Init(HRTIM1,HRTIM_TIMERINDEX_MASTER,&HRTIM_BaseInitStructure,&HRTIM_TimerInitStructure);
	
	/* --------------------------------------------------- */
	/* HRTIMx initialization: timer mode and PWM frequency */
	/* --------------------------------------------------- */
	HRTIM_TimerInitStructure.RepetitionUpdate 		= HRTIM_UPDATEONREPETITION_DISABLED;	
	
	HRTIM_BaseInitStructure.Mode					= HRTIM_MODE_SINGLESHOT;//HRTIM�����ڷ�����ģʽ��ʹ��MASTER��Ϊ��λʱ��
	
	HRTIM_Waveform_Init(HRTIM1, HRTIM_TIMERINDEX_TIMER_A, &HRTIM_BaseInitStructure, &HRTIM_TimerInitStructure);    
	HRTIM_Waveform_Init(HRTIM1, HRTIM_TIMERINDEX_TIMER_B, &HRTIM_BaseInitStructure, &HRTIM_TimerInitStructure);	
	
	/* ------------------------------------------------ */
	/* TIMERx output and registers update configuration */
	/* ------------------------------------------------ */
	HRTIM_TimerWaveStructure.DeadTimeInsertion		= HRTIM_TIMDEADTIMEINSERTION_ENABLED;//����ʱ�䣺ʹ��(�Գƻ���ģʽ)
	HRTIM_TimerWaveStructure.DelayedProtectionMode 	= HRTIM_TIMDELAYEDPROTECTION_DISABLED;//��ʱ��������
	HRTIM_TimerWaveStructure.FaultEnable 			= HRTIM_TIMFAULTENABLE_NONE;//����ʹ�ܣ���
	HRTIM_TimerWaveStructure.FaultLock 				= HRTIM_TIMFAULTLOCK_READWRITE;//����λ	���ɶ�д
	HRTIM_TimerWaveStructure.PushPull 				= HRTIM_TIMPUSHPULLMODE_DISABLED;
	HRTIM_TimerWaveStructure.ResetTrigger 			= HRTIM_TIMRESETTRIGGER_MASTER_PER;
	HRTIM_TimerWaveStructure.ResetUpdate 			= HRTIM_TIMUPDATEONRESET_DISABLED;
	HRTIM_TimerWaveStructure.UpdateTrigger 			= HRTIM_TIMUPDATETRIGGER_MASTER;
	
	HRTIM_WaveformTimerConfig(HRTIM1,HRTIM_TIMERINDEX_TIMER_A,&HRTIM_TimerWaveStructure);
	HRTIM_WaveformTimerConfig(HRTIM1,HRTIM_TIMERINDEX_TIMER_B,&HRTIM_TimerWaveStructure);
	
	/* -------------------------------- */
	/* -------HRTIM TAx ������� -------*/
	/* -------------------------------- */		
	HRTIM_TIM_OutputStructure.Polarity				= HRTIM_OUTPUTPOLARITY_LOW;//���Կ���λ��������
	HRTIM_TIM_OutputStructure.SetSource				= HRTIM_OUTPUTRESET_MASTERCMP1;
	HRTIM_TIM_OutputStructure.ResetSource			= HRTIM_OUTPUTRESET_MASTERCMP4;
	HRTIM_TIM_OutputStructure.IdleMode 				= HRTIM_OUTPUTIDLEMODE_NONE;//������ܿ���״̬Ӱ��  
	HRTIM_TIM_OutputStructure.IdleState 			= HRTIM_OUTPUTIDLESTATE_INACTIVE;//������ܿ���״̬Ӱ��          
	HRTIM_TIM_OutputStructure.FaultState  			= HRTIM_OUTPUTFAULTSTATE_NONE;//����״̬��PWM������ܹ���Ӱ��          
	HRTIM_TIM_OutputStructure.ChopperModeEnable 	= HRTIM_OUTPUTCHOPPERMODE_DISABLED;//��ʹ��ն����         
	HRTIM_TIM_OutputStructure.BurstModeEntryDelayed = HRTIM_OUTPUTBURSTMODEENTRY_REGULAR;//����ʱ�䲻�������״̬
	
	HRTIM_WaveformOutputConfig(HRTIM1,HRTIM_TIMERINDEX_TIMER_A,HRTIM_OUTPUT_TA2,&HRTIM_TIM_OutputStructure);
	
	HRTIM_TIM_OutputStructure.Polarity				= HRTIM_OUTPUTPOLARITY_LOW;
	HRTIM_WaveformOutputConfig(HRTIM1,HRTIM_TIMERINDEX_TIMER_A,HRTIM_OUTPUT_TA1,&HRTIM_TIM_OutputStructure);

	/* -------------------------------- */
	/* -------HRTIM TBx ������� -------*/
	/* -------------------------------- */		
	HRTIM_TIM_OutputStructure.Polarity				= HRTIM_OUTPUTPOLARITY_LOW;//���Կ���λ��������
	HRTIM_TIM_OutputStructure.SetSource				= HRTIM_OUTPUTRESET_MASTERCMP2;
	HRTIM_TIM_OutputStructure.ResetSource			= HRTIM_OUTPUTRESET_MASTERCMP3;
	HRTIM_TIM_OutputStructure.IdleMode 				= HRTIM_OUTPUTIDLEMODE_NONE;//������ܿ���״̬Ӱ��  
	HRTIM_TIM_OutputStructure.IdleState 			= HRTIM_OUTPUTIDLESTATE_INACTIVE;//������ܿ���״̬Ӱ��          
	HRTIM_TIM_OutputStructure.FaultState  			= HRTIM_OUTPUTFAULTSTATE_NONE;//����״̬��PWM������ܹ���Ӱ��          
	HRTIM_TIM_OutputStructure.ChopperModeEnable 	= HRTIM_OUTPUTCHOPPERMODE_DISABLED;//��ʹ��ն����         
	HRTIM_TIM_OutputStructure.BurstModeEntryDelayed = HRTIM_OUTPUTBURSTMODEENTRY_REGULAR;//����ʱ�䲻�������״̬
	
	HRTIM_WaveformOutputConfig(HRTIM1,HRTIM_TIMERINDEX_TIMER_B,HRTIM_OUTPUT_TB2,&HRTIM_TIM_OutputStructure);
	
	HRTIM_TIM_OutputStructure.Polarity				= HRTIM_OUTPUTPOLARITY_LOW;
	HRTIM_WaveformOutputConfig(HRTIM1,HRTIM_TIMERINDEX_TIMER_B,HRTIM_OUTPUT_TB1,&HRTIM_TIM_OutputStructure);
	
	
	HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_1, PERIOD_Half);
	HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_4, PERIOD_Half);	

	HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_2, PERIOD_Half);
	HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_3, PERIOD_Half);
	
	/* -------------------------------- */
	/* -----HRTIM PWM ����ʱ������ ---- */
	/* -------------------------------- */
	HRTIM_DeadtimeStructure.Prescaler 				= 0x00;//����ʱ��Ԥ��Ƶ��
	HRTIM_DeadtimeStructure.FallingLock				= HRTIM_TIMDEADTIME_FALLINGLOCK_WRITE;
	HRTIM_DeadtimeStructure.FallingSign				= HRTIM_TIMDEADTIME_FALLINGSIGN_NEGATIVE;
	HRTIM_DeadtimeStructure.FallingSignLock			= HRTIM_TIMDEADTIME_FALLINGSIGNLOCK_WRITE;
	HRTIM_DeadtimeStructure.FallingValue			= DeadTime_Value; //����ʱ��
	HRTIM_DeadtimeStructure.RisingLock				= HRTIM_TIMDEADTIME_RISINGLOCK_WRITE;
	HRTIM_DeadtimeStructure.RisingSign				= HRTIM_TIMDEADTIME_RISINGSIGN_NEGATIVE;
	HRTIM_DeadtimeStructure.RisingSignLock			= HRTIM_TIMDEADTIME_RISINGSIGNLOCK_WRITE;
	HRTIM_DeadtimeStructure.RisingValue				= DeadTime_Value;
	
	HRTIM_DeadTimeConfig(HRTIM1,HRTIM_TIMERINDEX_TIMER_A,&HRTIM_DeadtimeStructure);
	HRTIM_DeadTimeConfig(HRTIM1,HRTIM_TIMERINDEX_TIMER_B,&HRTIM_DeadtimeStructure);
	
	/* --------------------------*/
	/* ADC trigger initialization */
	/* --------------------------*/
	HRTIM_ADCTrigStructure.Trigger      			= HRTIM_ADCTRIGGEREVENT13_MASTER_PERIOD;
	HRTIM_ADCTrigStructure.UpdateSource 			= HRTIM_ADCTRIGGERUPDATE_MASTER;
	HRTIM_ADCTriggerConfig(HRTIM1, HRTIM_ADCTRIGGER_1, &HRTIM_ADCTrigStructure);
	
	/* -------------------------*/
	/* Interrupt initialization */
	/* -------------------------*/
	/* Configure and enable HRTIM TIMERA interrupt channel in NVIC */
	NVIC_InitStructure.NVIC_IRQChannel 						= HRTIM1_TIMA_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 0;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	/* TIMER A issues an interrupt on each repetition event */
	HRTIM_ITConfig(HRTIM1, HRTIM_TIMERINDEX_TIMER_A, HRTIM_TIM_IT_REP, ENABLE);//����HRTIM_TIMERA�ж�
	

//	HRTIM_WaveformOutputStart(HRTIM1,HRTIM_OUTPUT_TA1 | HRTIM_OUTPUT_TA2);
//	HRTIM_WaveformOutputStart(HRTIM1,HRTIM_OUTPUT_TB1 | HRTIM_OUTPUT_TB2);

	HRTIM_WaveformCounterStart(HRTIM1,HRTIM_TIMERID_TIMER_A);
	HRTIM_WaveformCounterStart(HRTIM1,HRTIM_TIMERID_TIMER_B);
	HRTIM_WaveformCounterStart(HRTIM1,HRTIM_TIMERID_MASTER);
}


void PWM_Update(uint16_t CompareValue)
{
	uint16_t	buck_duty,boost_duty;
	
	//��������ж�
	if(Direction==0)//�����������
	{
		if(CompareValue > MAX_CompareValue1) CompareValue	= MAX_CompareValue1;	//�������ռ�ձ�
		if(CompareValue < MIN_CompareValue ) CompareValue	= MIN_CompareValue;		//������Сռ�ձ�
	}
	else//�����������
	{
		if(CompareValue > MAX_CompareValue2) CompareValue	= MAX_CompareValue2;	//�������ռ�ձ�
		if(CompareValue < MIN_CompareValue ) CompareValue	= MIN_CompareValue;		//������Сռ�ձ�
	}

	if(CompareValue>0.9f*PERIOD)  							//����ռ�ձȴ���90%ʱ������BOOSTģʽ
		boost_duty	= CompareValue/2 - 0.85f*PERIOD_Half;	//����boost��ռ�ձȣ�����ģ�����ĶԳƵ�PWM��ע��Ҫ��ȥbuck_duty��ռ�ձ�: 0.85*PERIOD_Half
	else
		boost_duty	= 0.1f * PERIOD_Half;					//����ռ�ձȲ�����90%ʱ������BUCKģʽ,boost_duty���̶�ռ�ձ�10%

	if(CompareValue>0.9f*PERIOD)
		buck_duty	= 0.9f * PERIOD_Half;					//����ռ�ձȴ���90%ʱ������BOOSTģʽ,buck_duty���̶�ռ�ձ�90%
	else
		buck_duty	= CompareValue/2;						//����buck��ռ�ձȣ�����ģ�����ĶԳƵ�PWM��
	
	
//	if(CompareValue > MAX_Duty)  							//����ռ�ձȴ���90%ʱ������BOOSTģʽ
//	{
//		boost_duty	= CompareValue/2 - 0.85f*PERIOD_Half;	//����boost��ռ�ձȣ�����ģ�����ĶԳƵ�PWM��ע��Ҫ��ȥbuck_duty��ռ�ձ�: 0.85*PERIOD_Half
//	}
//	else
//	{
//		boost_duty	= MIN_Duty;					//����ռ�ձȲ�����90%ʱ������BUCKģʽ,boost_duty���̶�ռ�ձ�10%
//	}
//	
//	if(CompareValue > MAX_Duty)
//	{	
//		buck_duty	= MAX_Duty;					//����ռ�ձȴ���90%ʱ������BOOSTģʽ,buck_duty���̶�ռ�ձ�90%	
//	}
//	else
//	{
//		buck_duty	= CompareValue/2;			//����buck��ռ�ձȣ�����ģ�����ĶԳƵ�PWM��
//	}

	//ģʽ�ж�
	if(Transition_Mode==1)//���ű۹ضϣ����ű۵�ͨ
	{
		HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_1, PERIOD_Half);
		HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_4, PERIOD_Half);	

		HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_2, PERIOD_Half);
		HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_3, PERIOD_Half);	
		return;
	}
	
	if(Direction==1 && Transition_Mode==0)//�������
	{
		HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_1, PERIOD_Half + boost_duty);//PWM��1�Ƚ�����ģ�����ĶԳƵ�PWM����PWM_PER_0_5Ϊ����
		HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_4, PERIOD_Half - boost_duty);//PWM��0�Ƚ��������磨400-50���루400+50�������ĵ�Ϊ400	

		HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_2, PERIOD_Half - buck_duty);
		HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_3, PERIOD_Half + buck_duty);
	}
	if(Direction==0 && Transition_Mode==0)//�������
	{
		HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_1, PERIOD_Half - buck_duty);//PWM��1�Ƚ�����ģ�����ĶԳƵ�PWM����PWM_PER_0_5Ϊ����
		HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_4, PERIOD_Half + buck_duty);//PWM��0�Ƚ��������磨400-50���루400+50�������ĵ�Ϊ400	

		HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_2, PERIOD_Half + boost_duty);
		HRTIM_MasterSetCompare( HRTIM1, HRTIM_COMPAREUNIT_3, PERIOD_Half - boost_duty);	
	}
}



















