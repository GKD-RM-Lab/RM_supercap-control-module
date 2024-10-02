/**
  ******************************************************************************
  * @author  �Ρ�СP
  * @version V1.0
  * @date    2018-08-22
  * @brief   ���ְ��ϿƼ����޹�˾ ��Ȩ���У�C��All rights reserved.
  ******************************************************************************
	* 
	*	�ļ�����dp_filter.c
	*	��  �ܣ������˲��㷨
    *
  ******************************************************************************
**/ 

#include "bsp_filter.h"


/**
  ******************************************************************************
 * @������ void FilterSample( FilterBuf_STRUCT* p, float32 NewValue)
 * @���������ݲ�������������
 * @���룺NewValue:�µĲ�������ֵ
 * @���أ���
 * @���ߣ��Ρ�СP
 * @�汾��V1.0,2018-08-22
  ******************************************************************************
 */
void FilterSample( FilterBuf_STRUCT* p, float NewValue)
{
	if(p->Head == 0 && p->Rear == 0 && p->Base[0] == 0)
	{
		p->Base[p->Rear]  = NewValue;
	}
	else
	{
		p->Rear = (p->Rear+1)%FilterBuf_MaxLen;
		
		p->Base[p->Rear] = NewValue;
		
		if(p->Head == p->Rear)p->Head = (p->Head+1)%FilterBuf_MaxLen;
	}
}
/**
  ******************************************************************************
 * @������float32 FilterAverage( FilterBuf_STRUCT* p)
 * @����������ƽ����
 * @���룺��������
 * @���أ�������ƽ��ֵ
 * @���ߣ��Ρ�СP
 * @�汾��V1.0,2018-08-22
  ******************************************************************************
 */
float FilterAverage( FilterBuf_STRUCT* p)
{
	float sum	= 0.0f;
	uint16_t  i;
	for( i = 0; i<FilterBuf_MaxLen; i++)
	{
		sum += p->Base[i];
	}
	return (float)(sum/FilterBuf_MaxLen);
}

/*			 [] END OF FILE					*/

