#ifndef __AS608_H_
#define __AS608_H_

#include "debug.h"


uint8_t AS608_Wait_Ack(uint8_t* AS608_uart_buff);//�ȴ���Ӧ

void AS608_Get_Img(void);//���ָ��ͼ������

void AS608_Img_To_Buffer1(void);//��ͼ��ת��������������Buffer1��

void AS608_Img_To_Buffer2(void);//��ͼ��ת��������������Buffer2��

void AS608_Reg_Model(void);//��������������������ϲ�

void AS608_Save_Finger(uint8_t PageID);//����ָ��

void AS608_Find_Fingerprint();//����ָ��

uint8_t AS608_Wait_Find_Ack(uint8_t* AS608_uart_buff);
#endif
