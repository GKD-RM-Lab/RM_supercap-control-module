/**
  ******************************************************************************
  * @author  �Ρ�СP
  * @version V1.0
  * @date    2017-08-22
  * @brief   ���ְ��ϿƼ����޹�˾ ��Ȩ���У�C��All rights reserved.
  ******************************************************************************
	* 
	*	�ļ�����dp_pid.h
	*	��  �ܣ�PID�㷨ͷ�ļ�
  *
  ******************************************************************************
**/

#ifndef __BSP_PID_H
#define __BSP_PID_H

typedef struct  _PID{ 
	
	  //PID ������
    float Kp;
    float Ki;
    float Kd;

    float max_out;  //������
	  float min_out;	//��С���
    float max_iout; //���������

    float set;			//Ŀ��ֵ
    float fdb;			//��ǰֵ

    float out;
    float Pout;     //����������
    float Iout;			//���ֿ�����
    float Dout;			//΢�ֿ�����
    float Dbuf[3];  //΢���� 0���� 1��һ�� 2���ϴ�
    float error[3]; //����� 0���� 1��һ�� 2���ϴ�
}PID_STRUCT;


typedef struct _PID_FUNC{

	void (*reset) ( PID_STRUCT *pid);
	void (*init ) ( PID_STRUCT *pid);
	void (*clc  ) ( PID_STRUCT *pid);
	void (*calc ) ( PID_STRUCT *pid, float ref, float fdb);
	void (*power_calc)	(PID_STRUCT *pid, float ref, float fdb);

}PID_FUNC;


extern PID_FUNC pid_func;

#endif


