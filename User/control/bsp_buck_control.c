#include "bsp_buck_control.h"
#include "bsp_can.h"
#include "stdio.h"
float ADC_I_SCAP_SampleValue;    //�������������������ADC����ֵ,	ƫ��2.25V			I=(3.3/4096*��ȡ���޷�����-2.25)/50/0.0015��Ҳ����3.3/4096*��ȡ���޷�����/50/0.0015-30
float ADC_I_BAT_SampleValue;     //����������ADC����ֵ,												I=3.3/4096/50/0.005*��ȡ���޷�����
float ADC_V_BAT_SampleValue;     //��������ѹADC����ֵ,˥��16��,							V=3.3/4096*16*��ȡ���޷�����
float ADC_V_SCAP_SampleValue;    //�������ݵ�ѹADC����ֵ,˥��16��,							V=3.3/4096*16*��ȡ���޷�����

float IoRefApp,VoRefApp;//�������趨ֵ
float Input_Voltage,Input_Current,Input_Power,Available_Power,SCAP_Voltage,SCAP_Current;//��������ת��ֵ

uint16_t 	PWM_DUTY	= 0;	//PWMʱ�Ӽ����Ƚ�ֵ(ռ�ձ�)
uint16_t	Vref 		= 1850;	//Ĭ�������ѹ����ֵ,��Ӧ22.36V,����絽22.36V��Ϊ���ݳ���
float Limit_Power;			//���Ƴ��Ĺ���

FilterBuf_STRUCT ADC_I_SCAP_FilterBuf;  //�����������������������,���������˲�
FilterBuf_STRUCT ADC_I_BAT_FilterBuf;   //��������������,���������˲�
FilterBuf_STRUCT ADC_V_BAT_FilterBuf;   //��������ѹ����,���������˲�
FilterBuf_STRUCT ADC_V_SCAP_FilterBuf;  //�������ݵ�ѹ����,���������˲�


PID_STRUCT  gPID_AllIoutLoop;	    //�ŵ繦�ʻ�PID����
PID_STRUCT  gPID_OutPowerCurr;	  //�ŵ������PID����

PID_STRUCT	gPID_VoltOutLoop;     //����ѹ��PID����
PID_STRUCT	gPID_CurrOutLoop;     //��������PID����
PID_STRUCT  gPID_PowerOutLoop;	  //��繦�ʻ�PID����	

/*    �ⲿ��������     */
extern void PWM_Update(uint16_t CompareValue);

/**
 **********************************************************
 * @������void dp_cntl_init(void)
 * @��������ʼ��PID����
 * @���룺��
 * @���أ��� 
 **********************************************************
**/
void bsp_PIDCtrlInit(void)
{	
	//�ŵ繦�ʻ�
	pid_func.reset(&gPID_AllIoutLoop);
	gPID_AllIoutLoop.Kp       = 2.0f;//PID�������ڣ���λ100us
	gPID_AllIoutLoop.Ki       = 0.1f;
	gPID_AllIoutLoop.Kd       = 0.0f;
	gPID_AllIoutLoop.max_iout = 2000.0f;	//λ��ʽ�����޷���С
	gPID_AllIoutLoop.min_out  = 0;//��С�������ֵ��������ֵ��.��Ӧ-30A����Ϊ�Ƿŵ������Ǹ�����
	gPID_AllIoutLoop.max_out  = 2730;//���������ֵ,��Ӧ-0.67A
	pid_func.init(&gPID_AllIoutLoop);
	
	//�ŵ������
	pid_func.reset(&gPID_OutPowerCurr);
	gPID_OutPowerCurr.Kp       = 2.0f;//PID�������ڣ���λ100us
	gPID_OutPowerCurr.Ki       = 0.15f;	//1.9
	gPID_OutPowerCurr.Kd       = 0.0f;
	gPID_OutPowerCurr.max_iout = 25000.0f;	//λ��ʽ�����޷���С����PI�����ֵ
	gPID_OutPowerCurr.min_out  = 0.01f * PERIOD;//��Сռ�ձ�
	gPID_OutPowerCurr.max_out  = 1.61f * PERIOD;//���ռ�ձ�
	pid_func.init(&gPID_OutPowerCurr);
	
	//����ѹ��
	pid_func.reset(&gPID_VoltOutLoop);
	gPID_VoltOutLoop.Kp       = 2.00f;	
	gPID_VoltOutLoop.Ki       = 0.1f;
	gPID_VoltOutLoop.Kd       = 0.00f;
	gPID_VoltOutLoop.max_iout = 20000.0f;
	gPID_VoltOutLoop.min_out  = 0.01f * PERIOD;//��Сռ�ձ�
	gPID_VoltOutLoop.max_out  = 1.13f * PERIOD;//���ռ�ձ�
	pid_func.init(&gPID_VoltOutLoop);

	//��������
	pid_func.reset(&gPID_CurrOutLoop);
	gPID_CurrOutLoop.Kp       = 2.0f;//PID�������ڣ���λ100us
	gPID_CurrOutLoop.Ki       = 0.11f;
	gPID_CurrOutLoop.Kd       = 0.0f;
	gPID_CurrOutLoop.max_iout = 19000.0f;	//λ��ʽ�����޷���С����PI�����ֵ
	gPID_CurrOutLoop.min_out  = 0.01f * PERIOD;//��Сռ�ձ�
	gPID_CurrOutLoop.max_out  = 1.13f * PERIOD;//���ռ�ձ�
	pid_func.init(&gPID_CurrOutLoop);
	
	//��繦�ʻ�
	pid_func.reset(&gPID_PowerOutLoop);
	gPID_PowerOutLoop.Kp       = 2.0f;//PID�������ڣ���λ100us
	gPID_PowerOutLoop.Ki       = 0.1f;
	gPID_PowerOutLoop.Kd       = 0.0f;
	gPID_PowerOutLoop.max_iout = 3300.0f;	//λ��ʽ�����޷���С����PI�����ֵ
	gPID_PowerOutLoop.min_out  = 2790;//��С����ֵ,��Ӧ-0.035A
	gPID_PowerOutLoop.max_out  = 3900;//������ֵ,��Ӧ11.88A
	pid_func.init(&gPID_PowerOutLoop);

}

uint8_t	  Direction=0;//�������,1��ʾ���ݹ��磬0��ʾ���ݳ��
uint8_t	  Mode=0;//0��ʾ�ڳ�磬1��ʾ�ڷŵ�
uint8_t	  Last_Direction_Mode=0;
uint8_t	  loop_mode = 0x00;
uint8_t   Transition_Mode=0;//0:����״̬;1:�쳣״̬;2:����״̬
uint32_t  Transition_Time_Out=0; //���״̬������
uint8_t   Mediate_State_Out=0;
uint32_t  Transition_Time_In=0;
uint8_t   Mediate_State_In=0;
uint32_t  Clear_Time=0;
uint8_t   Clear_State=0;
uint16_t	  Turn_VoltOutLoop_Time=0;


void InputPower_Control(void)
{	
	/*  ��ADC����ֵ�����˲�������  */
	FilterSample( &ADC_I_SCAP_FilterBuf, ADC_SampleBuf[0]);//�������������������
	FilterSample( &ADC_I_BAT_FilterBuf , ADC_SampleBuf[1]);//����������
	FilterSample( &ADC_V_BAT_FilterBuf , ADC_SampleBuf[2]);//��������ѹ
	FilterSample( &ADC_V_SCAP_FilterBuf, ADC_SampleBuf[3]);//�������ݵ�ѹ
	
	/*  ��ADCֵ���������˲�  */
	ADC_I_SCAP_SampleValue  = FilterAverage( &ADC_I_SCAP_FilterBuf );//�������������������
	ADC_I_BAT_SampleValue   = FilterAverage( &ADC_I_BAT_FilterBuf  );//����������
	ADC_V_BAT_SampleValue   = FilterAverage( &ADC_V_BAT_FilterBuf  );//��������ѹ
	ADC_V_SCAP_SampleValue  = FilterAverage( &ADC_V_SCAP_FilterBuf );//�������ݵ�ѹ
	
	/*  ����������ʻ�����  */
	//��������ת��
	Input_Voltage=ADC_V_BAT_SampleValue*Voltage_Factor;
	SCAP_Voltage=ADC_V_SCAP_SampleValue*Voltage_Factor;
	Input_Current=ADC_I_BAT_SampleValue*Current_Factor_Pow;
	SCAP_Current=ADC_I_SCAP_SampleValue*Current_Factor_Cap-30;
	
	
	Input_Power=Input_Voltage*Input_Current;
	Limit_Power=MAX_POW-4;   //MAX_POW��CANͨ������ϵͳ����
	
	//�쳣״̬�ӳ�ʱ�� 
	
	//����״̬����(ÿ��һ��ʱ��һ����ѯ����״̬����ֻҪ�����PID�����������������Ҫ��)
	//�ж��Ǵ�ʱ���������ǹ���״̬���ǳ��״̬�������ʾ��ǹ���״̬��ʣ�๦�ʾ��ǳ��״̬
	Transition_Time_Out = Input_Power>=(Limit_Power+5)  ? ++Transition_Time_Out : 0;
	Transition_Time_In = Input_Power<=(Limit_Power-5)  ?  ++Transition_Time_In : 0;
	Clear_Time = Last_Direction_Mode!=Direction ? ++Clear_Time : 0;
	if(Transition_Time_Out>90)	// ����������һ���������жϽ���ĳ��״̬
	{
		Mediate_State_Out=1;			//�ŵ�
		Transition_Time_Out=0;
		Mediate_State_In=0;
	}
	if(Transition_Time_In>90)	
	{
		Mediate_State_In=1;				//���
		Transition_Time_In=0;
		Mediate_State_Out=0;
	}
	if(Clear_Time>25)
	{
		Clear_State=1;
		Clear_Time=0;
	}
	
	if(ADC_SampleBuf[2]>3300 || ADC_SampleBuf[3]>3300)//�쳣״̬PWMɲ��
	{
		Transition_Mode=1;
	}
	
	//��·״̬�����˲ʱֵ����⣬�ͺ��л�	
	if(Mediate_State_Out==1 && ADC_SampleBuf[3]<3000 && ADC_SampleBuf[2]<3000)//��ʱ���ݷŵ�
	{
		Direction=1;//�������ݷ�������� 	
		//�������ݷŵ���Сֵ�жϣ���Ϊ������������жϣ�����Ӧ�������⣬ADCû��ô��ת����ɣ�����Ϊ������������жϣ��������Ĳ���Ҫ���Σ�����������������
		if(ADC_SampleBuf[3]<550) //6.65V
		{
			Transition_Mode=1;
		}
		if(Transition_Mode==1 && ADC_SampleBuf[3]<1200) 
		{
			Transition_Mode=1;
			PWM_Update(PWM_DUTY);	//����PWMռ�ձ�	
			return;
		}
		Transition_Mode=0;
	}
	else if(Mediate_State_In==1 && ADC_SampleBuf[2]<3000 && ADC_SampleBuf[3]<3000)
	{
		Direction=0;//��������������
		Transition_Mode=0;
	}
	
	//ģʽ�л�PID��������,PWMɲ��
	if(Clear_State==1)
	{
		pid_func.clc(&gPID_AllIoutLoop);
		pid_func.clc(&gPID_OutPowerCurr);
		pid_func.clc(&gPID_VoltOutLoop);
		pid_func.clc(&gPID_CurrOutLoop);
		pid_func.clc(&gPID_PowerOutLoop);
		Clear_State=0;
	}
	Last_Direction_Mode=Direction;
	
	
	/**  PID����  **/

	if(Direction==1 && Transition_Mode==0)//�������ݷŵ�ģʽ��˫���������ڻ������⻷����
	{	
		Mode = 1;
		gPID_OutPowerCurr.max_out  = (-6.5002f)*ADC_V_SCAP_SampleValue + 34070;//���ռ�ձ�
		gPID_OutPowerCurr.max_iout = gPID_OutPowerCurr.max_out - 3000 ;
		
		
		/* power_cal�ͺ���void pid_power_calc (PID_STRUCT *pid, float fdb, float ref)һ��
		������� = ref-fdb, ����������������ˣ��;�������ȫ�ɳ������ݹ������Դ�����磬
		�����빦�ʻ���ֵΪ����ϵͳ�����빦��Input_Power��������ԸĽ���������������������*/
		pid_func.power_calc(&gPID_AllIoutLoop,Limit_Power,Input_Power);	// �ú������޸�gPID_AllIoutLoop.out
		
		
		
		//�����ʻ�������뵽��������
		gPID_AllIoutLoop.out = (2770-gPID_AllIoutLoop.out);//��Դ���������������
		
		IoRefApp=gPID_AllIoutLoop.out;
		pid_func.calc(&gPID_OutPowerCurr, IoRefApp, ADC_I_SCAP_SampleValue);//������PID����gAdcIoSampleValue
		gPID_OutPowerCurr.out=gPID_OutPowerCurr.max_out-gPID_OutPowerCurr.out;
		PWM_DUTY=gPID_OutPowerCurr.out;
	}	
	else if(Direction==0 && Transition_Mode==0)	//�������ݳ��ģʽ: ��������
	{
		Mode = 0;
		switch(loop_mode)
		{
			case 0x00: //��ѹ��
				//��̬�޷�
				gPID_VoltOutLoop.min_out  = 6.56f*ADC_V_SCAP_SampleValue - 27;//���ռ�ձ�
			
				pid_func.calc( &gPID_VoltOutLoop, Vref, ADC_V_SCAP_SampleValue);//�������ݳ���ѹ��PID����			
				PWM_DUTY = gPID_VoltOutLoop.out;
				gPID_CurrOutLoop.out = PWM_DUTY;
  			loop_mode = (ADC_I_SCAP_SampleValue > 3100 ) ? 0x01 : 0x00;//����������ݵ�������3.3A��˵����ʱ����ĺ��ͣ��л���������
				break;
			case 0x01:   //���ʻ�+������
				//��̬�޷�
				gPID_CurrOutLoop.min_out  = 6.56f*ADC_V_SCAP_SampleValue - 27;//��Сռ�ձ�
			
				//���ʻ�
				//�������Limit_Power-5��������޶�ֵ��Ϊ��������ֵ
				if(Input_Power>Limit_Power-5) // ������ʱ���ȼ����������ƣ��ټ�����������-5
				{
					pid_func.calc(&gPID_PowerOutLoop,Limit_Power,Input_Power);
				}
				else
				{
					pid_func.calc(&gPID_PowerOutLoop,Limit_Power-5,Input_Power);
				}
				//������
				pid_func.calc( &gPID_CurrOutLoop, gPID_PowerOutLoop.out , ADC_I_SCAP_SampleValue);	//���������PID����	
				PWM_DUTY = gPID_CurrOutLoop.out;
				gPID_VoltOutLoop.out = PWM_DUTY;
				loop_mode = (ADC_V_SCAP_SampleValue > Vref) ? 0x00 : 0x01;//����������ݵ�ѹ����22.36V��˵����ʱ�ӽ��������л�����ѹ��
				break;
			default :
				loop_mode = 0x01;
				break;
		}
	}
	PWM_Update(PWM_DUTY);	//����PWMռ�ձ�	
}


