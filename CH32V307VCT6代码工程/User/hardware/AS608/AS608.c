#include "AS608.h"

#include "UART_user.h"


const uint8_t Get_Device[10] ={0x01,0x00,0x07,0x13,0x00,0x00,0x00,0x00,0x00,0x1b};//口令验证
const uint8_t Pack_Head[6] = {0xEF,0x01,0xFF,0xFF,0xFF,0xFF};  //协议包头
const uint8_t Get_Img[6] = {0x01,0x00,0x03,0x01,0x00,0x05};    //获得指纹图像
const uint8_t Get_Templete_Count[6] ={0x01,0x00,0x03,0x1D,0x00,0x21 }; //获得模版总数
const uint8_t Search[11]={0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x00,0x64,0x00,0x72}; //搜索指纹搜索范围0 - 100,使用BUFFER1中的特征码搜索
const uint8_t Search_0_9[11]={0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x00,0x13,0x00,0x21}; //搜索0-9号指纹
const uint8_t Img_To_Buffer1[7]={0x01,0x00,0x04,0x02,0x01,0x00,0x08}; //将图像放入到BUFFER1
const uint8_t Img_To_Buffer2[7]={0x01,0x00,0x04,0x02,0x02,0x00,0x09}; //将图像放入到BUFFER2
const uint8_t Reg_Model[6]={0x01,0x00,0x03,0x05,0x00,0x09}; //将BUFFER1跟BUFFER2合成特征模版
const uint8_t Delete_All_Model[6]={0x01,0x00,0x03,0x0d,0x00,0x11};//删除指纹模块里所有的模版
volatile uint8_t  Save_Finger[9]={0x01,0x00,0x06,0x06,0x01,0x00,0x0B,0x00,0x19};//将BUFFER1中的特征码存放到指定的位置

//等待响应
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

//获得指纹图像命令
void AS608_Get_Img(void)
{
     uart_send_bytes(USART1,(char*)Pack_Head,6);//包头
     uart_send_bytes(USART1,(char*)Get_Img,6);//指纹图像
}

//将图像转换成特征码存放在Buffer1中
void AS608_Img_To_Buffer1(void)
{
    uart_send_bytes(USART1,(char*)Pack_Head,6);//包头
    uart_send_bytes(USART1,(char*)Img_To_Buffer1,7);//图像放入到BUFFER1
}

//将图像转换成特征码存放在Buffer2中
void AS608_Img_To_Buffer2(void)
{
    uart_send_bytes(USART1,(char*)Pack_Head,6);//包头
    uart_send_bytes(USART1,(char*)Img_To_Buffer2,7);//图像放入到BUFFER1
}
//将两个缓存区的特征码合并
void AS608_Reg_Model(void)
{
    uart_send_bytes(USART1,(char*)Pack_Head,6);//包头
    uart_send_bytes(USART1,(char*)Reg_Model,7);//合并特征码
}
//保存指纹
void AS608_Save_Finger(uint8_t PageID)
{
    uint8_t sum;

    sum = 0x01+0x00+0x06+0x06+0x01+PageID;
    Save_Finger[5]=0x00;
    Save_Finger[6]=PageID;
    Save_Finger[7]=0x00;
    Save_Finger[8]=sum;

    uart_send_bytes(USART1,(char*)Pack_Head,6);//包头
    uart_send_bytes(USART1,(char*)Save_Finger,9);//保存指纹

}
//搜索指纹
void AS608_Find_Fingerprint()
{
    uart_send_bytes(USART1,(char*)Pack_Head,6);//包头
    uart_send_bytes(USART1,(char*)Search,11);//搜索指纹
}
//等待搜索指纹的响应
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














