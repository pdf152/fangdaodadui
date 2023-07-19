#include "AS608.h"

#include "UART_user.h"


const uint8_t Get_Device[10] ={0x01,0x00,0x07,0x13,0x00,0x00,0x00,0x00,0x00,0x1b};//������֤
const uint8_t Pack_Head[6] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF};  //Э���ͷ
const uint8_t Get_Img[6] = {0x01,0x00,0x03,0x01,0x00,0x05};    //���ָ��ͼ��
const uint8_t Get_Templete_Count[6] ={0x01,0x00,0x03,0x1D,0x00,0x21 }; //���ģ������
const uint8_t Search[11]={0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x00,0x64,0x00,0x72}; //����ָ��������Χ0 - 100,ʹ��BUFFER1�е�����������
const uint8_t Search_0_9[11]={0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x00,0x13,0x00,0x21}; //����0-9��ָ��
const uint8_t Img_To_Buffer1[7]={0x01,0x00,0x04,0x02,0x01,0x00,0x08}; //��ͼ����뵽BUFFER1
const uint8_t Img_To_Buffer2[7]={0x01,0x00,0x04,0x02,0x02,0x00,0x09}; //��ͼ����뵽BUFFER2
const uint8_t Reg_Model[6]={0x01,0x00,0x03,0x05,0x00,0x09}; //��BUFFER1��BUFFER2�ϳ�����ģ��
const uint8_t Delete_All_Model[6]={0x01,0x00,0x03,0x0d,0x00,0x11};//ɾ��ָ��ģ�������е�ģ��
volatile uint8_t  Save_Finger[9]={0x01,0x00,0x06,0x06,0x01,0x00,0x0B,0x00,0x19};//��BUFFER1�е��������ŵ�ָ����λ��

//�ȴ���Ӧ
uint8_t AS608_Wait_Ack(uint8_t* AS608_uart_buff)
{
    if(AS608_uart_buff[0]==0xEF)
    {
    	AS608_uart_buff[0]=0;
        return AS608_uart_buff[9];
    }
    else
    {
        AS608_uart_buff[0]=0;
        return 0xff;
    }
}

//���ָ��ͼ������
void AS608_Get_Img(void)
{
     uart_send_bytes(USART1,(char*)Pack_Head,6);//��ͷ
     uart_send_bytes(USART1,(char*)Get_Img,6);//ָ��ͼ��
}

//��ͼ��ת��������������Buffer1��
void AS608_Img_To_Buffer1(void)
{
    uart_send_bytes(USART1,(char*)Pack_Head,6);//��ͷ
    uart_send_bytes(USART1,(char*)Img_To_Buffer1,7);//ͼ����뵽BUFFER1
}

//��ͼ��ת��������������Buffer2��
void AS608_Img_To_Buffer2(void)
{
    uart_send_bytes(USART1,(char*)Pack_Head,6);//��ͷ
    uart_send_bytes(USART1,(char*)Img_To_Buffer2,7);//ͼ����뵽BUFFER1
}
//��������������������ϲ�
void AS608_Reg_Model(void)
{
    uart_send_bytes(USART1,(char*)Pack_Head,6);//��ͷ
    uart_send_bytes(USART1,(char*)Reg_Model,7);//�ϲ�������
}
//����ָ��
void AS608_Save_Finger(uint8_t PageID)
{
    uint8_t sum;

    sum = 0x01+0x00+0x06+0x06+0x01+PageID;
    Save_Finger[5]=0x00;
    Save_Finger[6]=PageID;
    Save_Finger[7]=0x00;
    Save_Finger[8]=sum;

    uart_send_bytes(USART1,(char*)Pack_Head,6);//��ͷ
    uart_send_bytes(USART1,(char*)Save_Finger,9);//����ָ��

}
//����ָ��
void AS608_Find_Fingerprint()
{
    uart_send_bytes(USART1,(char*)Pack_Head,6);//��ͷ
    uart_send_bytes(USART1,(char*)Search,11);//����ָ��
}
//�ȴ�����ָ�Ƶ���Ӧ
uint8_t AS608_Wait_Find_Ack(uint8_t* AS608_uart_buff)
{
    if(AS608_uart_buff[0]==0xEF)
    {
        return AS608_uart_buff[11];
    }
    else
    {
        return 0xff;
    }
}














