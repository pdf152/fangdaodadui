#ifndef __key_H_
#define __key_H_

#include "debug.h"
#define X1 GPIO_Pin_0
#define X2 GPIO_Pin_1
#define X3 GPIO_Pin_2
#define Y1 GPIO_Pin_3
#define Y2 GPIO_Pin_4
#define Y3 GPIO_Pin_5
#define Y4 GPIO_Pin_6

void GetKey(uint8_t* key);
void GPIOE_Write_Pin(uint16_t GPIO_Pin, uint8_t Val);
uint8_t GPIOE_Read_Pin(uint16_t GPIO_Pin);

#endif
