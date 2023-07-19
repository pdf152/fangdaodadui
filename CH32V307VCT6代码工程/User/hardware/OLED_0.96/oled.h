#ifndef __oled_H_
#define __oled_H_

#include "debug.h"


/* --------------------OLEDģʽѡ��-------------------- */
#define OLED_AGREEMENT_MODE 0	/*Э��ѡ��
									0ΪSPI���
									1ΪSPIӲ��
									2ΪIIC���
									3ΪIICӲ��*/

#if OLED_AGREEMENT_MODE == 0
									/* --------------------OLED_SPI�������-------------------- */
									/* -----���Ŷ���----- */


#define OLED_SCL_TYPE GPIOB         //D0
#define OLED_SCL_PIN GPIO_Pin_13

#define OLED_SDIN_TYPE GPIOB   //D1
#define OLED_SDIN_PIN GPIO_Pin_15

#define OLED_RES_TYPE GPIOB
#define OLED_RES_PIN GPIO_Pin_14

#define OLED_DC_TYPE  GPIOD
#define OLED_DC_PIN  GPIO_Pin_8

#define OLED_CS_TYPE  GPIOD
#define OLED_CS_PIN  GPIO_Pin_9

/* -----�˿ڶ���----- */
#define OLED_SCLK_Clr() GPIO_WriteBit(OLED_SCL_TYPE, OLED_SCL_PIN, 0) // CLK
#define OLED_SCLK_Set() GPIO_WriteBit(OLED_SCL_TYPE, OLED_SCL_PIN, 1)

#define OLED_SDIN_Clr() GPIO_WriteBit(OLED_SDIN_TYPE, OLED_SDIN_PIN, 0) // DIN
#define OLED_SDIN_Set() GPIO_WriteBit(OLED_SDIN_TYPE, OLED_SDIN_PIN, 1)

#define OLED_RST_Clr() GPIO_WriteBit(OLED_RES_TYPE, OLED_RES_PIN, 0) // RES
#define OLED_RST_Set() GPIO_WriteBit(OLED_RES_TYPE, OLED_RES_PIN, 1)

#define OLED_DC_Clr() GPIO_WriteBit(OLED_DC_TYPE, OLED_DC_PIN, 0) // DC
#define OLED_DC_Set() GPIO_WriteBit(OLED_DC_TYPE, OLED_DC_PIN, 1)

#define OLED_CS_Clr() GPIO_WriteBit(OLED_CS_TYPE, OLED_CS_PIN, 0) // CS
#define OLED_CS_Set() GPIO_WriteBit(OLED_CS_TYPE, OLED_CS_PIN, 1)





#elif OLED_AGREEMENT_MODE == 1
									/* --------------------OLED_SPIӲ������-------------------- */
#include "spi.h"
#define OLEDspi hspi1
/* -----���Ŷ���----- */
#define OLED_RES_TYPE GPIOA
#define OLED_RES_PIN GPIO_PIN_4

#define OLED_DC_TYPE  GPIOA
#define OLED_DC_PIN  GPIO_PIN_6

#define OLED_CS_TYPE  GPIOC
#define OLED_CS_PIN  GPIO_PIN_4

/* -----�˿ڶ���----- */
#define OLED_RST_Clr() HAL_GPIO_WritePin(OLED_RES_TYPE, OLED_RES_PIN, GPIO_PIN_RESET) // RES
#define OLED_RST_Set() HAL_GPIO_WritePin(OLED_RES_TYPE, OLED_RES_PIN, GPIO_PIN_SET)

#define OLED_DC_Clr() HAL_GPIO_WritePin(OLED_DC_TYPE, OLED_DC_PIN, GPIO_PIN_RESET) // DC
#define OLED_DC_Set() HAL_GPIO_WritePin(OLED_DC_TYPE, OLED_DC_PIN, GPIO_PIN_SET)

#define OLED_CS_Clr() HAL_GPIO_WritePin(OLED_CS_TYPE, OLED_CS_PIN, GPIO_PIN_RESET) // CS
#define OLED_CS_Set() HAL_GPIO_WritePin(OLED_CS_TYPE, OLED_CS_PIN, GPIO_PIN_SET)



#elif OLED_AGREEMENT_MODE == 2
									/* --------------------OLED_IIC�������-------------------- */
									/* -----���Ŷ���----- */
#define OLED_SCLK_TYPE GPIOB		// SCL
#define OLED_SCLK_PIN GPIO_PIN_6	// SDA

#define OLED_SDIN_TYPE  GPIOB
#define OLED_SDIN_PIN  GPIO_PIN_7


/* -----�˿ڶ���----- */
#define OLED_SCLK_Clr() HAL_GPIO_WritePin(OLED_SCLK_TYPE, OLED_SCLK_PIN, GPIO_PIN_RESET) // SCL
#define OLED_SCLK_Set() HAL_GPIO_WritePin(OLED_SCLK_TYPE, OLED_SCLK_PIN, GPIO_PIN_SET)

#define OLED_SDIN_Clr() HAL_GPIO_WritePin(OLED_SDIN_TYPE, OLED_SDIN_PIN, GPIO_PIN_RESET) // SDA
#define OLED_SDIN_Set() HAL_GPIO_WritePin(OLED_SDIN_TYPE, OLED_SDIN_PIN, GPIO_PIN_SET)


#elif OLED_AGREEMENT_MODE == 3
									/* --------------------OLED_IICӲ������-------------------- */
#include "i2c.h"
/* -----�˿ڶ���----- */
#define hi2cX &hi2c1
#endif


// #define SIZE 16
// #define XLevelL 0x00
// #define XLevelH 0x10
// #define Max_Column 128
// #define Max_Row 64
// #define Brightness 0xFF
#define X_WIDTH 128
// #define Y_WIDTH 64


#define OLED_CMD 0  //д����
#define OLED_DATA 1 //д����

/* -----����ԭ��----- */
void OLED_Set_Cover(uint8_t cover);
void OLED_ShowChar(int16_t x, int16_t y, uint8_t c);
void OLED_Clear(void);
//void OLED_ShowNum(int16_t x, int16_t y, float num, uint8_t len1, uint8_t len2);
void OLED_ShowNum(int16_t x, int16_t y, int32_t num, uint8_t len);
void OLED_ShowString(int16_t x, int16_t y, char* chr);
void OLED_ShowCHinese(int16_t x, int16_t y, char* s);
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t BMP[]);




void OLED_horizontal_scroll(void);
void OLED_vertical_scroll(void);
void OLED_Init(void);

void OLED_Refresh(void);
void OLED_DrawPoint(int16_t x, int16_t y);
void OLED_ClearPoint(int16_t x, int16_t y);
void OLED_DrawVerticalLine(int16_t x, int16_t y, int16_t len, uint8_t mode);
void OLED_DrawRect(int16_t x, int16_t y, int16_t w, int16_t h);
void OLED_DrawSelectRect(int16_t x, int16_t y, int16_t w, int16_t h);


uint8_t menu(uint8_t key1, uint8_t key2);



#endif /*__oled_SPI_H_*/







































