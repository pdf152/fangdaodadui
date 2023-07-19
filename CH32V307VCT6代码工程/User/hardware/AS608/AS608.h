#ifndef __AS608_H_
#define __AS608_H_

#include "debug.h"


uint8_t AS608_Wait_Ack(uint8_t* AS608_uart_buff);//等待响应

void AS608_Get_Img(void);//获得指纹图像命令

void AS608_Img_To_Buffer1(void);//将图像转换成特征码存放在Buffer1中

void AS608_Img_To_Buffer2(void);//将图像转换成特征码存放在Buffer2中

void AS608_Reg_Model(void);//将两个缓存区的特征码合并

void AS608_Save_Finger(uint8_t PageID);//保存指纹

void AS608_Find_Fingerprint();//搜索指纹

uint8_t AS608_Wait_Find_Ack(uint8_t* AS608_uart_buff);
#endif
