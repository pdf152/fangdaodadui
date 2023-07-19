#include "oled.h"
#include "oledfont.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
uint8_t ShowData[8][X_WIDTH];
uint8_t oled_cover;

#if OLED_AGREEMENT_MODE == 0		//0Ϊ���SPI,1ΪӲ��SPI
/**
 * @brief	��OLEDд���������
 * @param	dat:Ҫд�������/����
 * @param	cmd:0(OLED_CMD)Ϊд����,1Ϊд����
 * @retval	��
 */
void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
	uint8_t i;
	if (cmd)
		OLED_DC_Set(); // DC����,д����
	else
		OLED_DC_Clr(); // DC����,д����
	//OLED_CS_Clr();	   //ʹ��������
	for (i = 0; i < 8; i++)
	{
		OLED_SCLK_Clr();	 //ʱ��������
		if (dat & 0x80)		 //��ȡ���λ����
			OLED_SDIN_Set(); //��1
		else
			OLED_SDIN_Clr(); //��0
		OLED_SCLK_Set();	 //ʱ����������
		dat <<= 1;			 //��λ
	}
}

void OLED_WR_Bytes(uint8_t* dat, uint8_t cmd, uint8_t len)
{
	uint8_t i, j, DAT;
	if (cmd)
		OLED_DC_Set(); // DC����,д����
	else
		OLED_DC_Clr(); // DC����,д����
	for (i = 0; i < len; i++)
	{
		DAT=dat[i];
		for (j = 0; j < 8; j++)
		{
			OLED_SCLK_Clr(); //ʱ��������
			if (DAT & 0x80) //��ȡ���λ����
			{
				OLED_SDIN_Set(); //��1
			}
			else
			{
				OLED_SDIN_Clr(); //��0
			}
			OLED_SCLK_Set(); //ʱ����������
			DAT <<= 1; //��λ
		}
	}
}



#elif OLED_AGREEMENT_MODE == 1

/**
 * @brief	��OLEDд���������
 * @param	dat:Ҫд�������/����
 * @param	cmd:0(OLED_CMD)Ϊд����,1Ϊд����
 * @retval	��
 */
void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
	if (cmd)
		OLED_DC_Set(); // DC����,д����
	else
		OLED_DC_Clr(); // DC����,д����
	//OLED_CS_Clr();	   //ʹ��������
	HAL_SPI_Transmit(&OLEDspi, &dat, 1, 1000);//����Ӳ��SPIд����
	//OLED_CS_Set(); //ʹ��������
}
/**
 * @brief	��OLEDд���������
 * @param	dat:Ҫд�������/����
 * @param	cmd:0(OLED_CMD)Ϊд����,1Ϊд����
 * @retval	��
 */
void OLED_WR_Bytes(uint8_t* dat, uint8_t cmd, uint8_t len)
{
	if (cmd)
		OLED_DC_Set(); // DC����,д����
	else
		OLED_DC_Clr(); // DC����,д����
	//OLED_CS_Clr();	   //ʹ��������
	HAL_SPI_Transmit(&OLEDspi, dat, len, 1000);//����Ӳ��SPIд����
	//OLED_CS_Set(); //ʹ��������
}



#elif OLED_AGREEMENT_MODE == 2
/**
  * @brief  IIC ��ʼ
  * @param  ��
  * @retval ��
  */
void OLED_IIC_Start()
{
	OLED_SDIN_Set();
	OLED_SCLK_Set();//ʱ�����ø�
	OLED_SDIN_Clr();//�������½���
}

/**
  * @brief  IIC ֹͣ
  * @param  ��
  * @retval ��
  */
void OLED_IIC_Stop()
{
	//OLED_SCLK_Clr();
	OLED_SDIN_Clr();
	OLED_SCLK_Set();//ʱ�����ø�
	OLED_SDIN_Set();//������������

}

/**
  * @brief  ����IIC��Ӧ�����
  * @param  ��
  * @retval ��
  */
void OLED_IIC_Wait_Ack()
{
	OLED_SCLK_Clr();
	OLED_SDIN_Set();
	OLED_SCLK_Set();
	OLED_SCLK_Clr();
}

/**
  * @brief  IIC д�ֽ�
  * @param  ��
  * @retval ��
  */
void OLED_Write_IIC_Byte(uint8_t dat)
{
	uint8_t i;
	//OLED_SCLK_Clr();
	for (i = 0;i < 8;i++)
	{
		OLED_SCLK_Clr();
		if (dat & 0x80)
		{
			OLED_SDIN_Set();
		}
		else OLED_SDIN_Clr();
		OLED_SCLK_Set();
		OLED_SCLK_Clr();
		dat = dat << 1;
	}
}

/**
 * @brief	��OLEDд���������
 * @param	dat:Ҫд�������/����
 * @param	cmd:0(OLED_CMD)Ϊд����,1Ϊд����
 * @retval	��
 */
void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
	if (cmd)
	{
		OLED_IIC_Start();
		OLED_Write_IIC_Byte(0x78);	//D/C#=0; R/W#=0
		OLED_IIC_Wait_Ack();
		OLED_Write_IIC_Byte(0x40);	//write data
		OLED_IIC_Wait_Ack();
		OLED_Write_IIC_Byte(dat);
		OLED_IIC_Wait_Ack();
		OLED_IIC_Stop();
	}
	else
	{
		OLED_IIC_Start();
		OLED_Write_IIC_Byte(0x78);	//Slave address,SA0=0
		OLED_IIC_Wait_Ack();
		OLED_Write_IIC_Byte(0x00);	//write command
		OLED_IIC_Wait_Ack();
		OLED_Write_IIC_Byte(dat);
		OLED_IIC_Wait_Ack();
		OLED_IIC_Stop();
	}
}

void OLED_WR_Bytes(uint8_t* dat, uint8_t cmd, uint8_t len)
{
	uint8_t i;
	OLED_IIC_Start();
	OLED_Write_IIC_Byte(0x78); //Slave address,SA0=0
	OLED_IIC_Wait_Ack();
	if (cmd)
	{
		OLED_Write_IIC_Byte(0x40); //write data
	}
	else
	{
		OLED_Write_IIC_Byte(0x00); //write command
	}
	OLED_IIC_Wait_Ack();
	for (i = 0; i < len; i++)
	{
		OLED_Write_IIC_Byte(dat[i]);
		OLED_IIC_Wait_Ack();
	}
	OLED_IIC_Stop();
}



#elif OLED_AGREEMENT_MODE == 3
void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
	if (cmd)
	{
		HAL_I2C_Mem_Write(hi2cX, 0x78, 0x40, I2C_MEMADD_SIZE_8BIT, &dat, 1, 100);
	}
	else
	{
		HAL_I2C_Mem_Write(hi2cX, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, &dat, 1, 100);
	}
}
void OLED_WR_Bytes(uint8_t* dat, uint8_t cmd, uint8_t len)
{
	if (cmd)
	{
		HAL_I2C_Mem_Write(hi2cX, 0x78, 0x40, I2C_MEMADD_SIZE_8BIT, dat, len, 100);
	}
	else
	{
		HAL_I2C_Mem_Write(hi2cX, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, dat, len, 100);
	}
}


#endif

/**
 * @brief	ָ��OLED��λ������
 * @param	cover:1Ϊ����ģʽ,2Ϊ�ǵ���ģʽ
 * @retval	��
 */
void OLED_Set_Cover(uint8_t cover)
{
	oled_cover = cover;
}

/**
 * @brief	ָ��OLED��λ������
 * @param	x(�е�ַ0-127)
 * @param	y(ҳ��ַ0-7)
 * @retval	��
 */
void OLED_Set_Pos(uint8_t x, uint8_t y)
{
	OLED_WR_Byte(0xb0 + y, OLED_CMD);					//����Ŀ����ʾλ�õ�ҳ��ʼ��ַ
	OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);	//������ʾλ�á��иߵ�ַ
	OLED_WR_Byte((x & 0x0f) | 0x00, OLED_CMD);			//������ʾλ�á��е͵�ַ
}


/**
 * @brief	��ʾ�Դ������
 * @param	��
 * @retval	��
 */
void OLED_Refresh(void)
{
	uint8_t i;
	uint8_t cmd_buf[3];
	for (i = 0; i < 8; i++)
	{
		cmd_buf[0] = 0xb0 + i;
		cmd_buf[1] = 0x00;
		cmd_buf[2] = 0x10;
		OLED_WR_Bytes(cmd_buf, OLED_CMD, 3);
		OLED_WR_Bytes(ShowData[i], OLED_DATA, X_WIDTH);
	}
}

/**
 * @brief	��ָ���������һ����
 * @param	x(0~127)
 * @param	y(0~63)
 * @retval	��
 */
void OLED_DrawPoint(int16_t x, int16_t y)
{
	int16_t i, m, n;
	i = y / 8;
	m = y % 8;
	n = 1 << m;
	ShowData[i][x] |= n;
}

void OLED_DrawVerticalLine(int16_t x, int16_t y, int16_t len, uint8_t mode)
{
	int16_t y1, y2, y3, y4;
	if (y + len >= 0 && y <= 63 && x >= 0 && x < X_WIDTH)
	{
		y1 = y / 8;
		y2 = (y + len) / 8;
		y3 = y % 8;
		while (y3 < 0) y3 += 8;
		y4 = (y + len) % 8;
		y4 = 8 - y4;
		if (mode == 1)
		{
			if (y1 < y2)
			{
				if (y1 >= 0)
				{
					ShowData[y1][x] |= 0xff << y3;
				}
				while (++y1 < y2)
				{
					if (y1 >= 0)
					{
						ShowData[y1][x] = 0xff;
					}

				}
				if (y2 < 8)
				{
					ShowData[y2][x] |= 0xff >> y4;
				}
			}
			else
			{
				ShowData[y1][x] |= (0xff << y3 & 0xff >> y4);
			}
		}
		else
		{
			if (y1 < y2)
			{
				if (y1 >= 0)
				{
					ShowData[y1][x] = ~(0xff << y3 & ShowData[y1][x]) & (0xff << y3 | ShowData[y1][x]);
				}
				while (++y1 < y2)
				{
					if (y1 >= 0)
					{
						ShowData[y1][x] = ~ShowData[y1][x];
					}

				}
				if (y2 < 8)
				{
					ShowData[y2][x] = ~(0xff >> y4 & ShowData[y2][x]) & (0xff >> y4 | ShowData[y2][x]);
				}
			}
			else
			{
				ShowData[y1][x] = ~((0xff << y3 & 0xff >> y4) & ShowData[y1][x]) & ((0xff << y3 & 0xff >> y4) | ShowData[y1][x]);
			}
		}

	}
}
void OLED_DrawRect(int16_t x, int16_t y, int16_t w, int16_t h)
{
	for (int i=x;i < x + w;i++)
	{
		OLED_DrawVerticalLine(i, y, h,1);
	}
}

void OLED_DrawSelectRect(int16_t x, int16_t y, int16_t w, int16_t h)
{

	for (int i = x;i < x + w;i++)
	{
		OLED_DrawVerticalLine(i, y, h,2);
	}
}


void OLED_DrawSelectRect1(int16_t x, int16_t y, int16_t w, int16_t h)
{

	for (int i = x;i < x + w;i++)
	{
		if (i - x == 0 || x + w - 1 - i == 0)
		{
			OLED_DrawVerticalLine(i, y + 5, h - 10, 2);
		}
		else if (i - x == 1 || x + w - 1 - i == 1)
		{
			OLED_DrawVerticalLine(i, y + 3, h - 6, 2);
		}
		else if (i - x == 2 || x + w - 1 - i == 2)
		{
			OLED_DrawVerticalLine(i, y + 2, h - 4, 2);
		}
		else if (i - x == 3 || x + w - 1 - i == 3)
		{
			OLED_DrawVerticalLine(i, y + 1, h - 2, 2);
		}
		else if (i - x == 4 || x + w - 1 - i == 4)
		{
			OLED_DrawVerticalLine(i, y+1, h-2, 2);
		}
		else
		{
			OLED_DrawVerticalLine(i, y, h, 2);
		}

	}
}


/**
 * @brief	��ָ������ɾ��һ����
 * @param	x(0~127)
 * @param	y(0~63)
 * @retval	��
 */
void OLED_ClearPoint(int16_t x, int16_t y)
{
	int16_t i, m, n;
	i = y / 8;
	m = y % 8;
	n = 1 << m;
	ShowData[i][x] = ~ShowData[i][x];
	ShowData[i][x] |= n;
	ShowData[i][x] = ~ShowData[i][x];
}



/**
 * @brief	����
 * @param	��
 * @retval	��
 */
void OLED_Clear(void)
{
//	uint8_t i, n;
	// for (i = 0;i < 8;i++)
	// {
	// 	for (n = 0;n < 132;n++)
	// 	{
	// 		ShowData[i][n] = 0;//���Դ�����
	// 	}
	// }
	memset(ShowData, 0, sizeof(ShowData));
}


/**
 * @brief	��ָ��λ����ʾһ���ַ�
 * @param	x(��)
 * @param	y(ҳ)
 * @param	chr(�ַ���Ӧ��ASCII��)
 * @retval	��
 */
void OLED_ShowChar(int16_t x, int16_t y, uint8_t c)
{
	uint8_t i = 0;
	int16_t  y1 = 0, y2 = 0, y3 = 0;
	c = c - 32;//�õ�ƫ�ƺ��ֵ
	y1 = (y + 16) / 8 - 2;
	y2 = (y + 16) % 8;
	if (y2 >= 0)
	{
		y3 = 8 - y2;
		for (i = 0; i < 8; i++)
		{
			if (x + i >= 0 && x + i < X_WIDTH)
			{
				if (oled_cover)
				{
					if (y1 >= 0 && y1 < 8)
					{
						ShowData[y1][x + i] |= F8X16[c * 16 + i] << y2;
					}
					if (y1 + 1 >= 0 && y1 + 1 < 8)
					{
						ShowData[y1 + 1][x + i] |= F8X16[c * 16 + i] >> y3;
						ShowData[y1 + 1][x + i] |= F8X16[c * 16 + i + 8] << y2;
					}
					if (y1 + 2 >= 0 && y1 + 2 < 8)
					{
						ShowData[y1 + 2][x + i] |= F8X16[c * 16 + i + 8] >> y3;
					}
				}
				else
				{
					if (y1 >= 0 && y1 < 8)
					{
						ShowData[y1][x + i] &= ~(0xff << y2);
						ShowData[y1][x + i] |= F8X16[c * 16 + i] << y2;
					}
					if (y1 + 1 >= 0 && y1 + 1 < 8)
					{
						ShowData[y1 + 1][x + i] &= ~(0xff >> y3);
						ShowData[y1 + 1][x + i] |= F8X16[c * 16 + i] >> y3;
						ShowData[y1 + 1][x + i] &= ~(0xff << y2);
						ShowData[y1 + 1][x + i] |= F8X16[c * 16 + i + 8] << y2;
					}
					if (y1 + 2 >= 0 && y1 + 2 < 8)
					{
						ShowData[y1 + 2][x + i] &= ~(0xff >> y3);
						ShowData[y1 + 2][x + i] |= F8X16[c * 16 + i + 8] >> y3;
					}
				}
			}
		}
	}
}


/**
 * @brief	��ʾһ���ַ���
 * @param	x(��)
 * @param	y(ҳ)
 * @param	chr(�ַ���)
 * @retval	��
 */
void OLED_ShowString(int16_t x, int16_t y, char* chr)
{
	uint8_t j = 0;
	while (chr[j] != '\0')
	{
		OLED_ShowChar(x, y, chr[j]);
		x += 8;
		j++;
	}
}


//
///**
// * @brief	��ʾlen������
// * @param	x(��)
// * @param	y(ҳ)
// * @param	num(����)
// * @retval	��
// */
//void OLED_ShowNum(int16_t x, int16_t y, float num, uint8_t len1, uint8_t len2)
//{
//
//	char buffer[20];
//
//	if (num >= 0)
//	{
//		if (len1 < 1) len1 = 1;
//		num = (float)((int)(num) % (int)pow(10, len1)) + (float)((int)((num - (int)num) * pow(10, len2))) / pow(10, len2);
//	}
//	else
//	{
//		if (len1 < 2) len1 = 2;
//		num = (float)((int)(num) % (int)pow(10, len1 - 1)) + (float)((int)((num - (int)num) * pow(10, len2))) / pow(10, len2);
//	}
//	if (len2 == 0)
//	{
//		sprintf(buffer, "%*.*f", len1 + len2, len2, num);
//	}
//	else
//	{
//		sprintf(buffer, "%*.*f", len1 + len2 + 1, len2, num);
//	}
//	OLED_ShowString(x, y, buffer);
//
//}

void OLED_ShowNum(int16_t x, int16_t y, int32_t num, uint8_t len)
{
	char buffer[20]={0};
	if(num>=0)
	{
		num = num%(int)pow((double)10,(double)(len));
		sprintf(buffer, "%*d", len, num);

	}
	OLED_ShowString(x, y, buffer);

}



/**
 * @brief	��ʾһ���ַ���
 * @param	x(��)
 * @param	y(ҳ)
 * @param	no(�������)
 * @retval	��
 */
void OLED_ShowCHinese(int16_t x, int16_t y, char* s)
{
	uint8_t t;
	uint16_t HZnum, k;

	while (*s != 0)
	{
		HZnum = sizeof(Hzk) / sizeof(typFNT_GB16);
		for (k = 0;k < HZnum;k++)
		{
			if ((Hzk[k].Index[0] == *(s)) && (Hzk[k].Index[1] == *(s + 1)) && (Hzk[k].Index[2] == *(s + 2)))
			{
				int16_t  y1 = 0, y2 = 0, y3 = 0;
				y1 = (y + 16) / 8 - 2;
				y2 = (y + 16) % 8;
				if (y2 >= 0)
				{
					y3 = 8 - y2;
					for (t = 0; t < 16; t++)
					{
						if (x + t >= 0 && x + t < X_WIDTH)
						{
							if (oled_cover)
							{
								if (y1 >= 0 && y1 < 8)
								{
									ShowData[y1][x + t] |= Hzk[k].Msk[t] << y2;
								}
								if (y1 + 1 >= 0 && y1 + 1 < 8)
								{
									ShowData[y1 + 1][x + t] |= Hzk[k].Msk[t] >> y3;
									ShowData[y1 + 1][x + t] |= Hzk[k].Msk[t + 16] << y2;
								}
								if (y1 + 2 >= 0 && y1 + 2 < 8)
								{
									ShowData[y1 + 2][x + t] |= Hzk[k].Msk[t + 16] >> y3;
								}
							}
							else
							{
								if (y1 >= 0 && y1 < 8)
								{
									ShowData[y1][x + t] &= ~(0xff << y2);
									ShowData[y1][x + t] |= Hzk[k].Msk[t] << y2;
								}
								if (y1 + 1 >= 0 && y1 + 1 < 8)
								{
									ShowData[y1 + 1][x + t] &= ~(0xff >> y3);
									ShowData[y1 + 1][x + t] |= Hzk[k].Msk[t] >> y3;
									ShowData[y1 + 1][x + t] &= ~(0xff << y2);
									ShowData[y1 + 1][x + t] |= Hzk[k].Msk[t + 16] << y2;
								}
								if (y1 + 2 >= 0 && y1 + 2 < 8)
								{
									ShowData[y1 + 2][x + t] &= ~(0xff >> y3);
									ShowData[y1 + 2][x + t] |= Hzk[k].Msk[t + 16] >> y3;
								}
							}
						}
					}
					x += 16;
				}
				break;
			}
		}
		s += 1;
	}

}

/**
 * @brief	OLED��ʾBMPͼƬ
 * @param	x0
 * @param	y0
 * @param	x1
 * @param	y1
 * @param	BMP[] (ͼƬ)
 * @retval	��
 */
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t BMP[])
{
	uint32_t j = 0;
	uint8_t x, y;
	if (y1 % 8 == 0)
		y = y1 / 8;
	else
		y = y1 / 8 + 1;
	for (y = y0; y < y1; y++)
	{
		for (x = x0; x < x1; x++)
		{
			ShowData[y][x] = BMP[j];
			j++;
		}
	}
}








/**
 * @brief	OLEDˮƽ����
 * @param	��
 * @retval	��
 */
void OLED_horizontal_scroll()
{
	OLED_WR_Byte(0x2E, OLED_CMD); //�رչ���
	OLED_WR_Byte(0x26, OLED_CMD); //ˮƽ��������ҹ��� 26/27
	OLED_WR_Byte(0x00, OLED_CMD); //�����ֽ�
	OLED_WR_Byte(0x00, OLED_CMD); //��ʼҳ 0
	OLED_WR_Byte(0x07, OLED_CMD); /*����ʱ����
									0x00: 5 frams
									0x01: 64 frams
									0x02: 128 frams
									0x03: 256 frams
									0x04: 3 frams
									0x05: 4 frams
									0x06: 25 fram
									0x07: 2 fram */
	OLED_WR_Byte(0x07, OLED_CMD); //��ֹҳ 7
	OLED_WR_Byte(0x00, OLED_CMD); //�����ֽ�
	OLED_WR_Byte(0xFF, OLED_CMD); //�����ֽ�
	OLED_WR_Byte(0x2F, OLED_CMD); //��������
}

/**
 * @brief	OLEDˮƽ��ֱ����
 * @param	��
 * @retval	��
 */
void OLED_vertical_scroll()
{
	OLED_WR_Byte(0x2e, OLED_CMD); //�رչ���
	OLED_WR_Byte(0x29, OLED_CMD); //ˮƽ��ֱ��ˮƽ�������� 29/2a
	OLED_WR_Byte(0x00, OLED_CMD); //�����ֽ�
	OLED_WR_Byte(0x00, OLED_CMD); //��ʼҳ 0
	OLED_WR_Byte(0x07, OLED_CMD); /*����ʱ����
									0x00: 5 frams
									0x01: 64 frams
									0x02: 128 frams
									0x03: 256 frams
									0x04: 3 frams
									0x05: 4 frams
									0x06: 25 fram
									0x07: 2 fram */
	OLED_WR_Byte(0x07, OLED_CMD); //��ֹҳ 1
	OLED_WR_Byte(0x01, OLED_CMD); /*��ֱ����ƫ����
									0x01��0*3F ��Ӧ 1��63*/
	OLED_WR_Byte(0x2F, OLED_CMD); //��������
}



/**
 * @brief	��ʼ��OLED
 * @param	��
 * @retval	��
 */
void OLED_Init(void)
{
#if OLED_AGREEMENT_MODE == 0 || OLED_AGREEMENT_MODE==1	
	OLED_RST_Clr();			//RES����
	OLED_RST_Set();			//RES����
	OLED_DC_Clr(); 			// DC����
#endif


	OLED_WR_Byte(0xAE, OLED_CMD);//--������ʾ��

	OLED_WR_Byte(0x00, OLED_CMD); //---�����е�λ��ַ
	OLED_WR_Byte(0x10, OLED_CMD); //---�����и�λ��ַ

	OLED_WR_Byte(0x40, OLED_CMD); //--����RAM��ʾ��ʼ��

	OLED_WR_Byte(0x81, OLED_CMD); //--���öԱȶ�
	OLED_WR_Byte(0xFF, OLED_CMD); //--�Աȶ���ֵ

	OLED_WR_Byte(0xA1, OLED_CMD); //--������Ļˮƽ����     0xa0���ҷ��� 0xa1����
	OLED_WR_Byte(0xC8, OLED_CMD); //--������Ļ��ֱ����   0xc0���·��� 0xc8����

	OLED_WR_Byte(0xA6, OLED_CMD); //--����������ʾ		0xA7Ϊ��ת��ʾ

	OLED_WR_Byte(0xA8, OLED_CMD); //--���ö�·������(16 to 64)
	OLED_WR_Byte(0x3F, OLED_CMD); //--1/64 duty

	OLED_WR_Byte(0xD3, OLED_CMD); //-������ʾƫ�� (0x00~0x3F)
	OLED_WR_Byte(0x00, OLED_CMD); //-ƫ����ֵ

	OLED_WR_Byte(0xD5, OLED_CMD); //--������ʾʱ�ӷ�Ƶ��/����Ƶ��
	OLED_WR_Byte(0x80, OLED_CMD); //--���÷�Ƶ�ȣ���ʱ������Ϊ100֡/��

	OLED_WR_Byte(0xD9, OLED_CMD); //--����Ԥ�������
	OLED_WR_Byte(0xF1, OLED_CMD); // ��Ԥ�������Ϊ15��ʱ�ӣ��ŵ�����Ϊ1��ʱ��

	OLED_WR_Byte(0xDA, OLED_CMD); //--����com����Ӳ������
	OLED_WR_Byte(0x12, OLED_CMD); //--

	OLED_WR_Byte(0xDB, OLED_CMD); //--����VCOMHȡ��ѡ��ȼ�
	OLED_WR_Byte(0x40, OLED_CMD); //--Set VCOM Deselect Level

	OLED_WR_Byte(0x8D, OLED_CMD); //--set Charge Pump enable/disable
	OLED_WR_Byte(0x14, OLED_CMD); //--set(0x10) disable

	OLED_WR_Byte(0x20, OLED_CMD); //--�����ڴ�ѡַģʽ (0x00/0x01/0x02)
	OLED_WR_Byte(0x02, OLED_CMD); //--ҳ��ѡַģʽ

	OLED_WR_Byte(0xA4, OLED_CMD); // ������ʾRAM���ݣ�����λ (0xa4/0xa5)
	OLED_WR_Byte(0xAF, OLED_CMD); //--������ʾ����

	OLED_Clear();
	OLED_Refresh();//��ʾ
	OLED_Set_Pos(0, 0);
}


uint8_t menu(uint8_t key1, uint8_t key2)
{
	static uint8_t UI = 1, mode = 0;
	static uint8_t key = 0, x = 0, xx = 0;
	static uint16_t t1 = 0, t2 = 0;
	static int16_t Rect_X, Rect_Y, NowRect_X, NowRect_Y;
	if (key1 == 0) //��ⰴ���Ƿ���
	{
		t1++;
		if (t1 == 3)//��ⰴ���Ƿ���50ms
		{
			x = 1;
		}
	}
	else
	{
		t1 = 0;
	}

	if (key2 == 0)
	{
		t2++;
		if (t2 == 3)
		{
			x = 2;
		}
	}
	else
	{
		t2 = 0;
	}
	if (key1 != 0 && key2 != 0)  //�ж��Ƿ����ֻ򳤰�
	{
		xx = x;
		x = 0;
	}
	if (xx != 0)  //�ı䰴����
	{
		key = xx;
	}



	OLED_Clear();
	OLED_ShowCHinese(2, 3, "��ǰģʽ");
	OLED_ShowString(66, 3, ":");
	switch (mode)
	{
	case 0:
		OLED_ShowCHinese(80, 3, "��");
		break;
	case 1:
		OLED_ShowCHinese(80, 3, "������");
		break;
	case 2:
		OLED_ShowCHinese(80, 3, "˫����");
		break;
	case 3:
		OLED_ShowCHinese(80, 3, "Ѳ��");
		break;
	case 4:
		OLED_ShowCHinese(80, 3, "��");
		break;
	}
	OLED_ShowCHinese(9, 24, "������");
	OLED_ShowCHinese(73, 24, "˫����");
	OLED_ShowCHinese(17, 44, "Ѳ��");
	// OLED_ShowNum(64, 48, key1, 1, 0);
	switch (UI)
	{
	case 1:
		switch (key)
		{
		case 1:
			UI = 2;
			break;
		case 2:
			mode = UI;
			break;
		}
		Rect_X = 2;
		Rect_Y = 23;
		break;
	case 2:
		switch (key)
		{
		case 1:
			UI = 3;
			break;
		case 2:
			mode = UI;
			break;
		}
		Rect_X = 64;
		Rect_Y = 23;
		break;
	case 3:
		switch (key)
		{
		case 1:
			UI = 1;
			break;
		case 2:
			mode = UI;
			break;
		}
		Rect_X = 2;
		Rect_Y = 43;
		break;
	}
	if (NowRect_X < Rect_X)
	{
		if (Rect_X - NowRect_X > 3)
		{
			NowRect_X += 3;
		}
		else
		{
			NowRect_X += 1;
		}

	}
	if (NowRect_X > Rect_X)
	{
		if (NowRect_X- Rect_X   > 3)
		{
			NowRect_X -= 3;
		}
		else
		{
			NowRect_X -= 1;
		}
	}
	if (NowRect_Y < Rect_Y) NowRect_Y++;
	if (NowRect_Y > Rect_Y) NowRect_Y--;

	OLED_DrawSelectRect1(NowRect_X, NowRect_Y, 64, 18);
	OLED_Refresh();
	key = 0;
	return mode;


}

