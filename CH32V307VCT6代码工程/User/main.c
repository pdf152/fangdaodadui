/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 USART Print debugging routine:
 USART1_Tx(PA9).
 This example demonstrates using USART1(PA9) as a print debug port output.
 */

#include "debug.h"

#include "oled.h"
#include "bmp.h"
#include "ADC_user.h"
#include "UART_user.h"
#include "AS608.h"
#include "RC522.h"
#include "key.h"

float num1 = 0;

int32_t debug, delay_ms;
uint8_t OLED_Clear_sign;

uint8_t key;

uint8_t AS608_scan_sign;
uint8_t AS608_sign = 2; //1为录指纹,2为识别指纹
uint8_t AS608_step = 1; //指纹的进度

uint8_t RC522_scan_sign = 0;

uint8_t mode;

uint16_t brightness = 0;
uint32_t fps, frame;

uint8_t AS608_uart_buff[20];
uint8_t uart2_buff;
uint8_t uart3_buff[2];
uint8_t uart4_buff[50];

uint8_t ucArray_ID[4];
uint8_t ucStatusReturn;    //返回状态

int32_t keyid[] = { 0x43AC91A5, 0x93D94796 };
uint8_t code[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 0, 0, 0, 0, 0, 10 };
uint8_t CDK[][16] = {
		{ 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 8 },
		{ 3, 3, 3, 3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 8 },
		{ 4, 4, 4, 4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 8 },
};

/********************************************************************
 * 函 数 名        : TIM1_Init
 * 函数功能        : 初始化 定时器 TIM1
 * 输    入        : arr：自动重装值，psc 预分频系数
 * 输    出        : 无
 ********************************************************************/
void TIM1_Init(u16 arr, u16 psc)
{

	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = { 0 };

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_TIM1, ENABLE);

	/* 配置定时器 */
	TIM_TimeBaseInitStructure.TIM_Period = arr; // 设置自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc; // 设置预分频值
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; // 设置时钟分割
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // 设置计数器为向上计数
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure); // 初始化TIM1
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE); // 使能定时器更新中断

	//    TIM_OC1PreloadConfig( TIM1, TIM_OCPreload_Disable );
	TIM_ARRPreloadConfig(TIM1, ENABLE);
	TIM_Cmd(TIM1, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure = { 0 };
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn; //定时器1的更新中断号
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;        //子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM2_PWMOut_Init(u16 arr, u16 psc, u16 ccp)
{
	GPIO_InitTypeDef GPIO_InitStructure = { 0 };
	TIM_OCInitTypeDef TIM_OCInitStructure = { 0 };
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure = { 0 };

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); // 使能TIM2时钟
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE); // 使能GPIOA时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; // 配置PA0为复用推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init( GPIOA, &GPIO_InitStructure);

	TIM_TimeBaseInitStructure.TIM_Period = arr; // 设置周期值
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc; // 设置分频器值
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; // 设置计数器为向上计数
	TIM_TimeBaseInit( TIM2, &TIM_TimeBaseInitStructure);

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; // 设置PWM输出模式1

	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = ccp; // 设置占空比
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; // 设置输出极性
	TIM_OC1Init( TIM2, &TIM_OCInitStructure); // 配置TIM2输出比较通道1

	TIM_CtrlPWMOutputs(TIM2, ENABLE); // 使能TIM2输出PWM信号
	TIM_OC1PreloadConfig( TIM2, TIM_OCPreload_Disable); // 禁止TIM2输出比较通道1预装载
	TIM_ARRPreloadConfig( TIM2, ENABLE); // 使能TIM2重载寄存器ARR预装载
	TIM_Cmd( TIM2, ENABLE); // 使能TIM2
}

/*******************************************************************************
 * Function Name  : GPIO_INIT
 * Description    : Initializes GPIOA.0
 * Input          : None
 * Return         : None
 *******************************************************************************/
void GPIO_INIT(void) //定义一个函数，用于初始化GPIO
{
	GPIO_InitTypeDef GPIO_InitStructure; //定义一个结构体变量，用于存储GPIO的初始化参数

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //设置GPIOE的11和12引脚为推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //设置GPIOE的11和12引脚的输出速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure); //根据结构体变量的参数，初始化GPIOE的10和11引脚

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //设置GPIOE的11和12引脚为推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //设置GPIOE的11和12引脚的输出速度为50MHz
	GPIO_Init(GPIOD, &GPIO_InitStructure); //根据结构体变量的参数，初始化GPIOE的10和11引脚

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12; // 配置GPIOE Pin11和Pin12
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 配置为复用推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 配置GPIO速度为50MHz
	GPIO_Init(GPIOE, &GPIO_InitStructure); //根据结构体变量的参数，初始化GPIOE的10和11引脚

}
uint16_t Steer_Control(float angle)
{
	uint16_t Control_Num = 0;

	Control_Num = (uint16_t) ((((0.5 + angle / 135) / 20) * 10000));    //除以135是因为它是270度舵机,180度舵机则除以90，period为脉冲数

	return Control_Num;
}

void OpenDoor()
{
	TIM_SetCompare1(TIM2, Steer_Control(95));
}

void CloseDoor()
{
	TIM_SetCompare1(TIM2, Steer_Control(5));
}


void ESP8266_SendData(char *c)
{
	char buff[50],k=0;

	for(uint8_t i=0;;i++)
	{
		if(c[i]=='\n')
		{
			k=i-1;
			break;
		}
	}
	sprintf(buff,"AT+CIPSEND=%d\r\n",k);
	for(uint8_t i=0;;i++)
	{
		if(buff[i]=='\n')
		{
			k=i+1;
			break;
		}
	}
	uart_send_bytes(UART4,buff,k);

	Delay_Ms(10);
	for(uint8_t i=0;;i++)
	{
		if(c[i]=='\n')
		{
			k=i+1;
			break;
		}
	}
	uart_send_bytes(UART4,c,k);

}



/*******************************************************************************
 * Function Name  : main
 * Description    : Main program.
 * Input          : None
 * Return         : None
 *******************************************************************************/
int main(void)
{
	uint16_t arr = 10000 - 1;

	Delay_Init();
	GPIO_INIT();
	USARTx_Init(USART1, 57600);
	USARTx_Init(USART2, 115200);
	USARTx_Init(USART3, 115200);
	USARTx_Init(UART4, 115200);
	OLED_Init();

	RC522_GPIO_Init();
	RC522_Rese();
	RC522_Antenna_On();

	TIM1_Init(arr, (SystemCoreClock / 1000 / arr) - 1);
	TIM2_PWMOut_Init(arr, (SystemCoreClock / 50 / arr) - 1, 250);

	CloseDoor();


	uart_send_bytes(UART4,"AT+CIPSTART=\"TCP\",\"192.168.11.120\",1234\r\n",sizeof("AT+CIPSTART=\"TCP\",\"192.168.11.120\",1234\r\n"));
	Delay_Ms(10);
	ESP8266_SendData(" 初始化成功\r\n");
//	printf("初始化成功!\r\n");
	Delay_Ms(50);
	while (1)
	{
		switch(mode)
		{
			case 0: //扫描
			{
				static uint8_t CODE[15],CODE_i=0;

				//密码
				if(key!=0)
				{
					if(key>=1&&key<=10)
					{
						if(CODE_i>=15)
						{
							CODE_i=14;
						}
						CODE[CODE_i]=key;
						CODE_i++;
						key=0;
					}
					if(key==11)
					{
						if(CODE_i>0)
						{

							CODE_i--;
							OLED_Clear();
						}
						key=0;
					}
					if(key==12)
					{
						if(CODE_i!=0)
						{
							uint8_t CODE_bool=1;
							if(CODE_i==code[15])
							{
								for(uint8_t i=0;i<CODE_i;i++)
								{
									if(CODE[i]!=code[i])
									{
										CODE_bool=0;
										break;
									}
								}

							}
							else
							{
								CODE_bool=0;
							}

							OLED_Clear();
							if(CODE_bool==1)
							{
								OLED_ShowCHinese(16, 24, "密码解锁成功");
								printf(" 通过密码解锁\r\n");
								ESP8266_SendData(" 通过密码解锁\r\n");
								delay_ms=2000;
								OLED_Clear_sign=1;
								mode=9;
							}
							else
							{

								OLED_ShowCHinese(32, 24, "密码错误");
								delay_ms=2000;
								OLED_Clear_sign=1;
							}
							CODE_i=0;

						}
						key=0;
					}
				}
				for (uint8_t i = 0; i < CODE_i; i++)
				{
//					OLED_ShowNum((128-8*CODE_i)/2+i*8, 32, CODE[i], 1);
					OLED_ShowString((128-8*CODE_i)/2+i*8, 32, "*");
				}

				//NFC
				if(RC522_scan_sign==1)
				{
					RC522_scan_sign=0;
					ucStatusReturn = PcdRequest ( PICC_REQALL, ucArray_ID );
					if ( ucStatusReturn == MI_OK )
					{
						/* 防冲突操作，被选中的卡片序列传入数组ucArray_ID中 */
						if ( PcdAnticoll ( ucArray_ID ) == MI_OK )
						{
							uint32_t KEYID=0;
							KEYID=(ucArray_ID [ 0 ]<<24)|(ucArray_ID [ 1 ]<<16)|(ucArray_ID [ 2 ]<<8)|(ucArray_ID [ 3 ]);
							printf (" 通过门禁卡解锁 ID: %02X%02X%02X%02X\r\n", ucArray_ID [ 0 ], ucArray_ID [ 1 ], ucArray_ID [ 2 ], ucArray_ID [ 3 ] );
							sprintf(uart4_buff," 通过门禁卡解锁 ID: %02X%02X%02X%02X\r\n", ucArray_ID [ 0 ], ucArray_ID [ 1 ], ucArray_ID [ 2 ], ucArray_ID [ 3 ] );
							ESP8266_SendData(uart4_buff);
//							printf ("%08X\r\n",KEYID );
							if(KEYID==keyid[0]||KEYID==keyid[1])
							{
								mode=9;
							}
						}
					}
				}


				//指纹
				if (AS608_scan_sign == 1)
				{
					AS608_scan_sign = 0;
					switch (AS608_step)
					{
						case 1:
						{
							AS608_Get_Img();    //获得指纹图像命令
							AS608_step = 2;
							break;
						}
						case 2:
						{
							switch (AS608_Wait_Ack(AS608_uart_buff))    //等待响应
							{
								case 0x00:
								{
									AS608_step = 3;
									break;
								}
								case 0xFF:
								{
									break;
								}
								default:
								{
									AS608_step = 1;
									break;
								}
							}
							break;
						}
						case 3:
						{
							AS608_Img_To_Buffer1();    //将图像转换成特征码存放在Buffer1中
							AS608_step = 4;
							break;
						}
						case 4:
						{
							switch (AS608_Wait_Ack(AS608_uart_buff))    //等待响应
							{
								case 0x00:
								{
									AS608_step = 5;
									break;
								}
								case 0xFF:
								{
									break;
								}
								default:
								{
									AS608_step = 1;
									break;
								}
							}
							break;
						}
						case 5:
						{
							AS608_Find_Fingerprint();    //搜索指纹
							AS608_step = 6;
							break;
						}
						case 6:
						{
							uint8_t PageID = 0;
							PageID = AS608_Wait_Find_Ack(AS608_uart_buff);

							switch (AS608_Wait_Ack(AS608_uart_buff))    //等待响应
							{
								case 0x00:
								{
//									OLED_ShowNum(0, 0, PageID, 2);
									OLED_ShowCHinese(16, 24, "指纹解锁成功");
									OLED_ShowString(44, 40, "ID:");
									OLED_ShowNum(68, 40, PageID, 2);
									delay_ms=1500;
									OLED_Clear_sign=1;
									printf(" 通过指纹解锁 ID:%d\r\n",PageID);
									sprintf(uart4_buff," 通过指纹解锁 ID:%d\r\n",PageID);
									ESP8266_SendData(uart4_buff);
									AS608_step = 1;
									mode = 9;
									break;
								}
								case 0xFF:
								{
									break;
								}
								default:
								{
									AS608_step = 1;
									break;
								}
							}
							break;
						}
					}
				}

				//人脸
				if(uart3_buff[0]==0xaa)
				{
					uart3_buff[0]=0;
					OLED_ShowCHinese(16, 24, "人脸解锁成功");
					OLED_ShowString(44, 40, "ID:");
					OLED_ShowNum(68, 40, uart3_buff[1], 2);
					delay_ms=1500;
					OLED_Clear_sign=1;
					printf(" 人脸解锁成功 ID:%d\r\n",uart3_buff[1]);
					sprintf(uart4_buff," 人脸解锁成功 ID:%d\r\n",uart3_buff[1]);
					ESP8266_SendData(uart4_buff);
					mode = 9;
				}
				OLED_ShowCHinese(0, 0, "状态");
				OLED_ShowString(32, 0, ":");
				OLED_ShowCHinese(40, 0, "关闭");
				break;
			}
			case 2:    //录指纹
			{
				static uint8_t sign=0;
				static uint8_t AS608_ID[3],ID_i=0;
				for (uint8_t i = 0; i < ID_i; i++)
				{
					OLED_ShowNum((128-8*ID_i)/2+i*8, 32, AS608_ID[i], 1);
				}
				switch(sign)
				{
					case 0:
					{
						OLED_ShowCHinese(0, 0, "请输入指纹");
						OLED_ShowString(80, 0, "ID");
						//ID
						if(key!=0)
						{
							if(key>=1&&key<=10)
							{
								if(ID_i>=2)
								{
									ID_i=1;
								}
								AS608_ID[ID_i]=key;
								ID_i++;

								key=0;
							}
							if(key==11)
							{
								if(ID_i>0)
								{

									ID_i--;
									OLED_Clear();
								}
								key=0;
							}
							if(key==12)
							{
								if(ID_i==1)
								{
									AS608_ID[2]=AS608_ID[0]%10;
									sign=1;
									OLED_Clear();
								}
								if(ID_i==2)
								{
									AS608_ID[2]=AS608_ID[0]%10*10+AS608_ID[1]%10;
									sign=1;
									OLED_Clear();
								}
								key=0;
							}

						}
						break;
					}

					case 1:
					{
						OLED_ShowCHinese(0, 0, "请录制指纹");
						if (AS608_scan_sign == 1)
						{
							AS608_scan_sign = 0;
							switch (AS608_step)
							{
								case 1:
								{
									AS608_Get_Img();    //获得指纹图像命令
									AS608_step = 2;
									break;
								}
								case 2:
								{
									switch (AS608_Wait_Ack(AS608_uart_buff))    //等待响应
									{
										case 0x00:
										{
											AS608_step = 3;
											break;
										}

										case 0xFF:
										{
											break;
										}

										default:
										{
											AS608_step = 1;
											break;
										}

									}

									break;
								}
								case 3:
								{
									AS608_Img_To_Buffer1();    //将图像转换成特征码存放在Buffer1中
									AS608_step = 4;
									break;
									case 4:
									switch (AS608_Wait_Ack(AS608_uart_buff))//等待响应
									{
										case 0x00:
										{
											AS608_step = 5;
											break;
										}

										case 0xFF:
										{
											break;
										}

										default:
										{
											AS608_step = 1;
											break;
										}

									}
									break;
								}
								case 5:
								{
									AS608_Get_Img();    //获得指纹图像命令
									AS608_step = 6;
									break;
								}
								case 6:
								{
									switch (AS608_Wait_Ack(AS608_uart_buff))    //等待响应
									{
										case 0x00:
										{
											AS608_step = 7;
											break;
										}
										case 0xFF:
										{
											break;
										}
										default:
										{
											AS608_step = 1;
											break;
										}
									}
									break;
								}
								case 7:
								{
									AS608_Img_To_Buffer2();    //将图像转换成特征码存放在Buffer2中
									AS608_step = 8;
									break;
								}
								case 8:
								{
									switch (AS608_Wait_Ack(AS608_uart_buff))    //等待响应
									{
										case 0x00:
										{
											AS608_step = 9;
											break;
										}
										case 0xFF:
										{
											break;
										}
										default:
										{
											AS608_step = 1;
											break;
										}
									}
									break;
								}
								case 9:
								{
									AS608_Reg_Model();    //将两个缓存区的特征码合并
									AS608_step = 10;
									break;
									case 10:
									switch (AS608_Wait_Ack(AS608_uart_buff))//等待响应
									{
										case 0x00:
										{
											AS608_step = 11;
											break;
										}
										case 0xFF:
										{
											break;
										}
										default:
										{
											AS608_step = 1;
											break;
										}
									}
									break;
								}
								case 11:
								{
									AS608_Save_Finger(AS608_ID[2]);    //保存指纹
									AS608_step = 12;
									break;
								}
								case 12:
								{
									switch (AS608_Wait_Ack(AS608_uart_buff))    //等待响应
									{
										case 0x00:
										{
											sign=0;
											ID_i=0;
											OLED_Clear();
											OLED_ShowCHinese(16, 24, "指纹录制成功");
											printf(" 成功录入指纹 ID:%d\r\n",AS608_ID[2]);
											sprintf(uart4_buff," 成功录入指纹 ID:%d\r\n",AS608_ID[2]);
											ESP8266_SendData(uart4_buff);
											delay_ms=1500;
											OLED_Clear_sign=1;
											mode=10;
											AS608_step = 1;
											break;
										}
										case 0xFF:
										{
											break;
										}
										default:
										{
											AS608_step = 1;
											break;
										}
									}
									break;
								}
							}

						}
						break;
					}
				}

				break;
			}
			case 3:
			{
				static uint8_t CODE[15],CODE_i=0;
				OLED_ShowCHinese(0, 0, "请输入新密码");
				for (uint8_t i = 0; i < CODE_i; i++)
				{
					OLED_ShowNum((128-8*CODE_i)/2+i*8, 32, CODE[i], 1);
				}
				if(key!=0)
				{
					if(key>=1&&key<=10)
					{
						if(CODE_i>=15)
						{
							CODE_i=14;
						}
						CODE[CODE_i]=key;
						CODE_i++;

						key=0;
					}
					if(key==11)
					{
						if(CODE_i>0)
						{

							CODE_i--;
							OLED_Clear();
						}
						key=0;
					}
					if(key==12)
					{
						if(CODE_i!=0)
						{
							for(uint8_t i=0;i<CODE_i;i++)
							{
								code[i]=CODE[i];

							}
							code[15]=CODE_i;
							OLED_Clear();
							OLED_ShowCHinese(16, 24, "修改密码成功");
							printf(" 修改密码成功\r\n");
							sprintf(uart4_buff," 修改密码成功\r\n");
							ESP8266_SendData(uart4_buff);
							delay_ms=2000;
							OLED_Clear_sign=1;
							mode=9;
							CODE_i=0;

						}
						key=0;
					}
				}

				break;
			}
			case 4:
			{
				static uint8_t sign=0;;
				switch(sign)
				{
					case 0:
					{
						OLED_ShowCHinese(0, 0, "请正视摄像头");
						OLED_ShowCHinese(0, 24, "按确认键继续");
						//ID
						if(key!=0)
						{
							if(key==11)
							{
								mode=9;
								key=0;
							}
							if(key==12)
							{
								char buff[1]={0xbb};
								uart_send_bytes(USART3,(char*)buff,1);

								delay_ms=3000;
								sign=1;
								key=0;
							}
						}
						break;
					}
					case 1:
					{
						if(delay_ms==0)
						{
							OLED_ShowCHinese(16, 24, "未检测到人脸");
							printf(" 录入人脸失败\r\n");
							sprintf(uart4_buff," 录入人脸失败\r\n");
							ESP8266_SendData(uart4_buff);
							delay_ms=2000;
							OLED_Clear_sign=1;
							sign=0;
							mode=9;
						}
						if(uart3_buff[0]==0xaa&&uart3_buff[1]==0xFF)
						{
							uart3_buff[0]=0;
							OLED_ShowCHinese(16, 24, "人脸录制成功");
							delay_ms=1500;
							OLED_Clear_sign=1;
							sign=0;
							printf(" 人脸录制成功\r\n");
							sprintf(uart4_buff," 人脸录制成功\r\n");
							ESP8266_SendData(uart4_buff);
							mode = 9;
						}
						break;
					}
				}
				break;
			}
			case 9:    //开门
			{
				static uint8_t CODE[15],CODE_i=0;
				if(CODE_i!=0)
				{
					OLED_ShowCHinese(0, 16, "输入口令");
				}

				//密码
				if(key!=0)
				{
					if(key>=1&&key<=10)
					{
						if(CODE_i>=15)
						{
							CODE_i=14;
						}
						CODE[CODE_i]=key;
						CODE_i++;

						key=0;
					}
					if(key==11)
					{
						if(CODE_i>0)
						{

							CODE_i--;
							OLED_Clear();
						}
						key=0;
					}
					if(key==12)
					{
						if(CODE_i!=0)
						{
							uint8_t CODE_bool=0;
							for(uint8_t j=0;j<3;j++)
							{
								if(CODE_i==CDK[j][15])
								{
									uint8_t i;
									for(i=0;i<CODE_i;i++)
									{
										if(CODE[i]!=CDK[j][i])
										{
											CODE_bool=0;
											break;
										}
									}
									if(i==CODE_i)
									{
										switch(j)
										{
											case 0:CODE_bool=2;break;
											case 1:CODE_bool=3;break;
											case 2:CODE_bool=4;break;
										}
										break;
									}
								}
								else
								{
									CODE_bool=0;
								}
							}

							OLED_Clear();
							if(CODE_bool==0)
							{
								OLED_ShowCHinese(32, 24, "口令错误");
								delay_ms=1500;
								OLED_Clear_sign=1;
								mode=9;
							}
							else
							{
								mode=CODE_bool;
							}

							CODE_i=0;
						}
						else
						{
							mode = 10;
							delay_ms=1000;

						}
						key=0;
					}
				}

				for (uint8_t i = 0; i < CODE_i; i++)
				{
					OLED_ShowNum((128-8*CODE_i)/2+i*8, 32, CODE[i], 1);
				}

				OLED_ShowCHinese(0, 0, "状态");
				OLED_ShowString(32, 0, ":");
				OLED_ShowCHinese(40, 0, "开启");
				OpenDoor();
				break;
			}
			case 10:    //关门延时
			{
				OLED_ShowCHinese(0, 0, "状态");
				OLED_ShowString(32, 0, ":");
				OLED_ShowCHinese(40, 0, "关闭");
				uart3_buff[0]=0;
				uart3_buff[1]=0;
				CloseDoor();
				if(delay_ms==0)
				{
					mode = 0;
				}
				break;
			}
		}
		if(OLED_Clear_sign==1&&delay_ms==0)
		{
			OLED_Clear_sign=0;
			OLED_Clear();
		}
//		OLED_ShowNum(64, 0, key, 3);
//		OLED_ShowNum(64, 48, Steer_Control(uart2_buff), 3);
//		OLED_ShowNum(0, 16, AS608_step, 2);
//		OLED_ShowNum(0, 32, frame++, 3);
//		OLED_ShowNum(64, 32, fps, 3);
//		OLED_ShowCHinese(47, 0, "蓝牙未连接");
		OLED_Refresh();
	}
}

void TIM1_UP_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

void TIM1_UP_IRQHandler(void)
{
	static uint16_t t = 0, led = 0;
	TIM_ClearFlag(TIM1, TIM_FLAG_Update); //清除标志位
	if (t % 1000 == 0)
	{

		fps = frame;
		frame = 0;
	}
	if (t % 100 == 0)
	{
		RC522_scan_sign = 1;
	}
	if (t % 80 == 0)
	{
		AS608_scan_sign = 1;
	}
	if (t % 5 == 0)
	{
		GetKey(&key);
	}

	if (t % 500 == 0)
	{
		num1++;
		GPIO_WriteBit(GPIOE, GPIO_Pin_11, led);
		GPIO_WriteBit(GPIOE, GPIO_Pin_12, led);
		led = ~led;
//		TIM_SetCompare2(TIM1, brightness);
	}
	if (delay_ms > 0)
		delay_ms--;
	if (++t == 60000)
		t = 0;

}
//串口中断服务程序
__attribute__((interrupt("WCH-Interrupt-fast")))
void USART1_IRQHandler(void)
{

	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		uint8_t RxData = 0;
		static uint8_t AS608_buff[20] = { 0 }, k = 0, AS608_uart_sign = 0, k_target = 0xff;
		RxData = (uint8_t) USART_ReceiveData(USART1);
		if (RxData == 0xEF && AS608_uart_sign == 0)
		{
			AS608_uart_sign = 1;
			k = 0;
		}
		if (AS608_uart_sign == 1)
		{
			AS608_buff[k] = RxData;
			if (k == 8)
			{

				k_target = RxData + 8;
			}
			if (k == k_target)
			{
				for (uint8_t j = 0; j <= k; j++)
				{
					AS608_uart_buff[j] = AS608_buff[j];
				}
				AS608_uart_sign = 0;
			}
			k++;
		}
	}

}

__attribute__((interrupt("WCH-Interrupt-fast")))
void USART2_IRQHandler(void)
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		uint8_t RxData = 0;
		RxData = (uint8_t) USART_ReceiveData(USART2);
		uart2_buff = RxData;

	}
}
__attribute__((interrupt("WCH-Interrupt-fast")))
void USART3_IRQHandler(void)
{
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{
		uint8_t RxData = 0;
		static uint8_t k = 0,k210_uart_sign=0,k210_buff[2]={0};
		RxData = (uint8_t) USART_ReceiveData(USART3);
		if (RxData == 0xAA && k210_uart_sign == 0)
		{
			k210_uart_sign = 1;
			k = 0;
		}
		if (k210_uart_sign == 1)
		{
			k210_buff[k] = RxData;
			if (k == 1)
			{
				uart3_buff[0] = k210_buff[0];
				uart3_buff[1] = k210_buff[1];
				k210_uart_sign = 0;
			}
			k++;
		}
	}
}

__attribute__((interrupt("WCH-Interrupt-fast")))
void UART4_IRQHandler(void)
{
	if (USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{
		USART_ReceiveData(UART4);
	}
}



