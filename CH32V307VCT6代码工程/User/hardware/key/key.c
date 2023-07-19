#include "key.h"

void GPIOE_Init_Output(uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStructure; //定义一个结构体变量，用于存储GPIO的初始化参数

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}
void GPIOE_Init_Input(uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStructure; //定义一个结构体变量，用于存储GPIO的初始化参数

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void GPIOE_Write_Pin(uint16_t GPIO_Pin, uint8_t Val)
{
	GPIOE_Init_Output(GPIO_Pin);
	GPIO_WriteBit(GPIOE, GPIO_Pin, Val);
}

uint8_t GPIOE_Read_Pin(uint16_t GPIO_Pin)
{
	GPIOE_Init_Input(GPIO_Pin);
	return GPIO_ReadInputDataBit( GPIOE, GPIO_Pin);
}



void GetKey(uint8_t* key)
{
	uint8_t KEY=0;
	static uint8_t i=0,KEY1=0;

	GPIOE_Write_Pin(X1, 0);
	GPIOE_Write_Pin(X2, 1);
	GPIOE_Write_Pin(X3, 1);
	GPIOE_Write_Pin(Y1, 1);
	GPIOE_Write_Pin(Y2, 1);
	GPIOE_Write_Pin(Y3, 1);
	GPIOE_Write_Pin(Y4, 1);
	if(GPIOE_Read_Pin(Y1)==0)KEY=1;
	if(GPIOE_Read_Pin(Y2)==0)KEY=4;
	if(GPIOE_Read_Pin(Y3)==0)KEY=7;
	if(GPIOE_Read_Pin(Y4)==0)KEY=10;

	GPIOE_Write_Pin(X1, 1);
	GPIOE_Write_Pin(X2, 0);
	GPIOE_Write_Pin(X3, 1);
	GPIOE_Write_Pin(Y1, 1);
	GPIOE_Write_Pin(Y2, 1);
	GPIOE_Write_Pin(Y3, 1);
	GPIOE_Write_Pin(Y4, 1);
	if(GPIOE_Read_Pin(Y1)==0)KEY=2;
	if(GPIOE_Read_Pin(Y2)==0)KEY=5;
	if(GPIOE_Read_Pin(Y3)==0)KEY=8;
	if(GPIOE_Read_Pin(Y4)==0)KEY=11;

	GPIOE_Write_Pin(X1, 1);
	GPIOE_Write_Pin(X2, 1);
	GPIOE_Write_Pin(X3, 0);
	GPIOE_Write_Pin(Y1, 1);
	GPIOE_Write_Pin(Y2, 1);
	GPIOE_Write_Pin(Y3, 1);
	GPIOE_Write_Pin(Y4, 1);
	if(GPIOE_Read_Pin(Y1)==0)KEY=3;
	if(GPIOE_Read_Pin(Y2)==0)KEY=6;
	if(GPIOE_Read_Pin(Y3)==0)KEY=9;
	if(GPIOE_Read_Pin(Y4)==0)KEY=12;

	if (KEY!=0)
	{
		i++;
		if(i>10)
		{
			KEY1=KEY;
		}
	}
	else
	{
		if(KEY1!=0)
		{
			*key=KEY1;
			KEY1=0;
		}
	}


}





















