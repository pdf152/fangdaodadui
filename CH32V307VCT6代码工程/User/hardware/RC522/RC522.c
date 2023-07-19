#include "RC522.h"

#include "UART_user.h"

/* RC522��������˵����SPI1�����ţ� ��
 CS:PA4( �ӵ�SDA���� )
 SCK:PA5
 MISO:PA6
 MOSI:PA7
 RST:PB0
 */
void RC522_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_Init( GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init( GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init( GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init( GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init( GPIOA, &GPIO_InitStructure);
}

/* IO�ڲ������� */
#define   RC522_CS_Enable()         GPIO_WriteBit(GPIOA, GPIO_Pin_4, 0)
#define   RC522_CS_Disable()        GPIO_WriteBit(GPIOA, GPIO_Pin_4, 1)

#define   RC522_Reset_Enable()      GPIO_WriteBit(GPIOB, GPIO_Pin_0, 0)
#define   RC522_Reset_Disable()     GPIO_WriteBit(GPIOB, GPIO_Pin_0, 1)

#define   RC522_SCK_0()             GPIO_WriteBit(GPIOA, GPIO_Pin_5, 0)
#define   RC522_SCK_1()             GPIO_WriteBit(GPIOA, GPIO_Pin_5, 1)

#define   RC522_MOSI_0()            GPIO_WriteBit(GPIOA, GPIO_Pin_7, 0)
#define   RC522_MOSI_1()            GPIO_WriteBit(GPIOA, GPIO_Pin_7, 1)

#define   RC522_MISO_GET()          GPIO_ReadInputDataBit( GPIOA, GPIO_Pin_6 )

/* ���ģ��SPI����һ���ֽ����ݣ���λ���� */
void RC522_SPI_SendByte(uint8_t byte)
{
	uint8_t n;
	for (n = 0; n < 8; n++)
	{
		if (byte & 0x80)
			RC522_MOSI_1();
		else
			RC522_MOSI_0();

		//Delay_Us(5);
		RC522_SCK_0();
		//Delay_Us(5);
		RC522_SCK_1();
		//Delay_Us(5);

		byte <<= 1;
	}
}

/* ���ģ��SPI��ȡһ���ֽ����ݣ��ȶ���λ */
uint8_t RC522_SPI_ReadByte(void)
{
	uint8_t n, data = 0;
	for (n = 0; n < 8; n++)
	{
		data <<= 1;
		RC522_SCK_0();
		//Delay_Us(5);

		if ( RC522_MISO_GET() == 1)
			data |= 0x01;

		//Delay_Us(5);
		RC522_SCK_1();
		//Delay_Us(5);

	}
	return data;
}

/**
 * @brief  ����ȡRC522ָ���Ĵ�����ֵ
 * @param  ��Address:�Ĵ����ĵ�ַ
 * @retval ���Ĵ�����ֵ
 */
uint8_t RC522_Read_Register(uint8_t Address)
{
	uint8_t data, Addr;

	Addr = ((Address << 1) & 0x7E) | 0x80;

	RC522_CS_Enable();
	RC522_SPI_SendByte(Addr);
	data = RC522_SPI_ReadByte(); //��ȡ�Ĵ����е�ֵ
	RC522_CS_Disable();

	return data;
}

/**
 * @brief  ����RC522ָ���Ĵ�����д��ָ��������
 * @param  ��Address���Ĵ�����ַ
 data��Ҫд��Ĵ���������
 * @retval ����
 */
void RC522_Write_Register(uint8_t Address, uint8_t data)
{
	uint8_t Addr;

	Addr = (Address << 1) & 0x7E;

	RC522_CS_Enable();
	RC522_SPI_SendByte(Addr);
	RC522_SPI_SendByte(data);
	RC522_CS_Disable();

}

/**
 * @brief  ����λRC522ָ���Ĵ�����ָ��λ
 * @param  ��Address���Ĵ�����ַ
 mask����λֵ
 * @retval ����
 */
void RC522_SetBit_Register(uint8_t Address, uint8_t mask)
{
	uint8_t temp;
	/* ��ȡ�Ĵ�����ǰֵ */
	temp = RC522_Read_Register(Address);
	/* ��ָ��λ������λ�������ٽ�ֵд��Ĵ��� */
	RC522_Write_Register(Address, temp | mask);
}

/**
 * @brief  ����λRC522ָ���Ĵ�����ָ��λ
 * @param  ��Address���Ĵ�����ַ
 mask����λֵ
 * @retval ����
 */
void RC522_ClearBit_Register(uint8_t Address, uint8_t mask)
{
	uint8_t temp;
	/* ��ȡ�Ĵ�����ǰֵ */
	temp = RC522_Read_Register(Address);
	/* ��ָ��λ������λ�������ٽ�ֵд��Ĵ��� */
	RC522_Write_Register(Address, temp & (~mask));
}

/**
 * @brief  ����������
 * @param  ����
 * @retval ����
 */
void RC522_Antenna_On(void)
{
	uint8_t k;
	k = RC522_Read_Register( TxControlReg);
	/* �ж������Ƿ��� */
	if (!(k & 0x03))
		RC522_SetBit_Register( TxControlReg, 0x03);
}

/**
 * @brief  ���ر�����
 * @param  ����
 * @retval ����
 */
void RC522_Antenna_Off(void)
{
	/* ֱ�Ӷ���Ӧλ���� */
	RC522_ClearBit_Register( TxControlReg, 0x03);
}

/**
 * @brief  ����λRC522
 * @param  ����
 * @retval ����
 */
void RC522_Rese(void)
{
	RC522_Reset_Disable();
	//Delay_Us(1);
	RC522_Reset_Enable();
	//Delay_Us(1);
	RC522_Reset_Disable();
	//Delay_Us(1);
	RC522_Write_Register( CommandReg, 0x0F);
	while( RC522_Read_Register( CommandReg )&0x10 )
	;

	/* ����һ�� */
	//Delay_Us(1);
	RC522_Write_Register( ModeReg, 0x3D);       //���巢�ͺͽ��ճ���ģʽ
	RC522_Write_Register( TReloadRegL, 30);     //16λ��ʱ����λ
	RC522_Write_Register( TReloadRegH, 0);      //16λ��ʱ����λ
	RC522_Write_Register( TModeReg, 0x8D);      //�ڲ���ʱ��������
	RC522_Write_Register( TPrescalerReg, 0x3E); //���ö�ʱ����Ƶϵ��
	RC522_Write_Register( TxAutoReg, 0x40);     //���Ʒ����ź�Ϊ100%ASK
}

/**
 * @brief  ������RC522�Ĺ�����ʽ
 * @param  ��Type��������ʽ
 * @retval ����
 M500PcdConfigISOType
 */
void RC522_Config_Type(char Type)
{
	if (Type == 'A')
	{
		RC522_ClearBit_Register( Status2Reg, 0x08);
		RC522_Write_Register( ModeReg, 0x3D);
		RC522_Write_Register( RxSelReg, 0x86);
		RC522_Write_Register( RFCfgReg, 0x7F);
		RC522_Write_Register( TReloadRegL, 30);
		RC522_Write_Register( TReloadRegH, 0);
		RC522_Write_Register( TModeReg, 0x8D);
		RC522_Write_Register( TPrescalerReg, 0x3E);
		//Delay_Us(2);
		/* ������ */
		RC522_Antenna_On();
	}
}

/**
 * @brief  ��ͨ��RC522��ISO14443��ͨѶ
 * @param  ��ucCommand��RC522������
 *          pInData��ͨ��RC522���͵���Ƭ������
 *          ucInLenByte���������ݵ��ֽڳ���
 *          pOutData�����յ��Ŀ�Ƭ��������
 *          pOutLenBit���������ݵ�λ����
 * @retval ��״ֵ̬MI_OK���ɹ�
 */
char PcdComMF522(uint8_t ucCommand, uint8_t * pInData, uint8_t ucInLenByte, uint8_t * pOutData, uint32_t * pOutLenBit)
{
	char cStatus = MI_ERR;
	uint8_t ucIrqEn = 0x00;
	uint8_t ucWaitFor = 0x00;
	uint8_t ucLastBits;
	uint8_t ucN;
	uint32_t ul;

	switch (ucCommand)
	{
		case PCD_AUTHENT:		//Mifare��֤
			ucIrqEn = 0x12;		//��������ж�����ErrIEn  ��������ж�IdleIEn
			ucWaitFor = 0x10;		//��֤Ѱ���ȴ�ʱ�� ��ѯ�����жϱ�־λ
			break;

		case PCD_TRANSCEIVE:		//���շ��� ���ͽ���
			ucIrqEn = 0x77;		//����TxIEn RxIEn IdleIEn LoAlertIEn ErrIEn TimerIEn
			ucWaitFor = 0x30;		//Ѱ���ȴ�ʱ�� ��ѯ�����жϱ�־λ�� �����жϱ�־λ
			break;

		default:
			break;

	}

	RC522_Write_Register( ComIEnReg, ucIrqEn | 0x80);		//IRqInv��λ�ܽ�IRQ��Status1Reg��IRqλ��ֵ�෴
	RC522_ClearBit_Register( ComIrqReg, 0x80);			//Set1��λ����ʱ��CommIRqReg������λ����
	RC522_Write_Register( CommandReg, PCD_IDLE);		//д��������
	RC522_SetBit_Register( FIFOLevelReg, 0x80);			//��λFlushBuffer����ڲ�FIFO�Ķ���дָ���Լ�ErrReg��BufferOvfl��־λ�����

	for (ul = 0; ul < ucInLenByte; ul++)
		RC522_Write_Register( FIFODataReg, pInData[ul]);    		//д���ݽ�FIFOdata
	RC522_Write_Register( CommandReg, ucCommand);					//д����
	if (ucCommand == PCD_TRANSCEIVE)
		RC522_SetBit_Register(BitFramingReg, 0x80);  				//StartSend��λ�������ݷ��� ��λ���շ�����ʹ��ʱ����Ч

	ul = 1000;  				//����ʱ��Ƶ�ʵ���������M1�����ȴ�ʱ��25ms

	do 														//��֤ ��Ѱ���ȴ�ʱ��
	{
		ucN = RC522_Read_Register( ComIrqReg);							//��ѯ�¼��ж�

		ul--;
	} while ((ul != 0) && (!(ucN & 0x01)) && (!(ucN & ucWaitFor)));		//�˳�����i=0,��ʱ���жϣ���д��������

	RC522_ClearBit_Register( BitFramingReg, 0x80);					//��������StartSendλ

	if (ul != 0)
	{
		if (!(RC522_Read_Register( ErrorReg) & 0x1B))			//�������־�Ĵ���BufferOfI CollErr ParityErr ProtocolErr
		{
			cStatus = MI_OK;

			if (ucN & ucIrqEn & 0x01)					//�Ƿ�����ʱ���ж�
				cStatus = MI_NOTAGERR;

			if (ucCommand == PCD_TRANSCEIVE)
			{
				ucN = RC522_Read_Register( FIFOLevelReg);			//��FIFO�б�����ֽ���

				ucLastBits = RC522_Read_Register( ControlReg) & 0x07;	//�����յ����ֽڵ���Чλ��

				if (ucLastBits)
					*pOutLenBit = (ucN - 1) * 8 + ucLastBits;   	//N���ֽ�����ȥ1�����һ���ֽڣ�+���һλ��λ�� ��ȡ����������λ��
				else
					*pOutLenBit = ucN * 8;   					//�����յ����ֽ������ֽ���Ч

				if (ucN == 0)
					ucN = 1;

				if (ucN > MAXRLEN)
					ucN = MAXRLEN;

				for (ul = 0; ul < ucN; ul++)
					pOutData[ul] = RC522_Read_Register( FIFODataReg);
			}
		}
		else
			cStatus = MI_ERR;
	}

	RC522_SetBit_Register( ControlReg, 0x80);           // stop timer now
	RC522_Write_Register( CommandReg, PCD_IDLE);

	return cStatus;
}

/**
 * @brief  ��Ѱ��
 * @param  ucReq_code��Ѱ����ʽ
 *                      = 0x52��Ѱ��Ӧ�������з���14443A��׼�Ŀ�
 *                     = 0x26��Ѱδ��������״̬�Ŀ�
 *         pTagType����Ƭ���ʹ���
 *                   = 0x4400��Mifare_UltraLight
 *                   = 0x0400��Mifare_One(S50)
 *                   = 0x0200��Mifare_One(S70)
 *                   = 0x0800��Mifare_Pro(X))
 *                   = 0x4403��Mifare_DESFire
 * @retval ��״ֵ̬MI_OK���ɹ�
 */
char PcdRequest(uint8_t ucReq_code, uint8_t * pTagType)
{
	char cStatus;
	uint8_t ucComMF522Buf[MAXRLEN];
	uint32_t ulLen;

	RC522_ClearBit_Register( Status2Reg, 0x08);	//����ָʾMIFARECyptol��Ԫ��ͨ�Լ����п�������ͨ�ű����ܵ����
	RC522_Write_Register( BitFramingReg, 0x07);	//	���͵����һ���ֽڵ� ��λ
	RC522_SetBit_Register( TxControlReg, 0x03);	//TX1,TX2�ܽŵ�����źŴ��ݾ����͵��Ƶ�13.56�������ز��ź�

	ucComMF522Buf[0] = ucReq_code;		//����Ѱ����ʽ
	/* PCD_TRANSCEIVE�����Ͳ��������ݵ����RC522��Ƭ����Ѱ�������Ƭ���ؿ����ͺŴ��뵽ucComMF522Buf�� */
	cStatus = PcdComMF522( PCD_TRANSCEIVE, ucComMF522Buf, 1, ucComMF522Buf, &ulLen);	//Ѱ��

	if ((cStatus == MI_OK) && (ulLen == 0x10))	//Ѱ���ɹ����ؿ�����
	{
		/* ���տ�Ƭ���ͺŴ��� */
		*pTagType = ucComMF522Buf[0];
		*(pTagType + 1) = ucComMF522Buf[1];
	}
	else
		cStatus = MI_ERR;
	return cStatus;
}

/**
 * @brief  ������ͻ
 * @param  ��Snr����Ƭ���У�4�ֽڣ��᷵��ѡ�п�Ƭ������
 * @retval ��״ֵ̬MI_OK���ɹ�
 */
char PcdAnticoll(uint8_t * pSnr)
{
	char cStatus;
	uint8_t uc, ucSnr_check = 0;
	uint8_t ucComMF522Buf[MAXRLEN];
	uint32_t ulLen;

	RC522_ClearBit_Register( Status2Reg, 0x08);		//��MFCryptol Onλ ֻ�гɹ�ִ��MFAuthent����󣬸�λ������λ
	RC522_Write_Register( BitFramingReg, 0x00);		//����Ĵ��� ֹͣ�շ�
	RC522_ClearBit_Register( CollReg, 0x80);			//��ValuesAfterColl���н��յ�λ�ڳ�ͻ�����

	ucComMF522Buf[0] = 0x93;	//��Ƭ����ͻ����
	ucComMF522Buf[1] = 0x20;

	/* ����Ƭ����ͻ����ͨ��RC522������Ƭ�У����ص��Ǳ�ѡ�п�Ƭ������ */
	cStatus = PcdComMF522( PCD_TRANSCEIVE, ucComMF522Buf, 2, ucComMF522Buf, &ulLen);	//�뿨Ƭͨ��

	if (cStatus == MI_OK)		//ͨ�ųɹ�
	{
		for (uc = 0; uc < 4; uc++)
		{
			*(pSnr + uc) = ucComMF522Buf[uc];			//����UID
			ucSnr_check ^= ucComMF522Buf[uc];
		}

		if (ucSnr_check != ucComMF522Buf[uc])
			cStatus = MI_ERR;
	}
	RC522_SetBit_Register( CollReg, 0x80);
	return cStatus;
}

/**
 * @brief   :��RC522����CRC16��ѭ������У�飩
 * @param  ��pIndata������CRC16������
 *            ucLen������CRC16�������ֽڳ���
 *            pOutData����ż�������ŵ��׵�ַ
 * @retval ��״ֵ̬MI_OK���ɹ�
 */
void CalulateCRC(uint8_t * pIndata, u8 ucLen, uint8_t * pOutData)
{
	uint8_t uc, ucN;

	RC522_ClearBit_Register(DivIrqReg, 0x04);
	RC522_Write_Register(CommandReg, PCD_IDLE);
	RC522_SetBit_Register(FIFOLevelReg, 0x80);

	for (uc = 0; uc < ucLen; uc++)
		RC522_Write_Register( FIFODataReg, *(pIndata + uc));

	RC522_Write_Register( CommandReg, PCD_CALCCRC);

	uc = 0xFF;

	do
	{
		ucN = RC522_Read_Register( DivIrqReg);
		uc--;
	} while ((uc != 0) && !(ucN & 0x04));

	pOutData[0] = RC522_Read_Register( CRCResultRegL);
	pOutData[1] = RC522_Read_Register( CRCResultRegM);

}

/**
 * @brief   :ѡ����Ƭ
 * @param  ��pSnr����Ƭ���кţ�4�ֽ�
 * @retval ��״ֵ̬MI_OK���ɹ�
 */
char PcdSelect(uint8_t * pSnr)
{
	char ucN;
	uint8_t uc;
	uint8_t ucComMF522Buf[MAXRLEN];
	uint32_t ulLen;
	/* PICC_ANTICOLL1������ͻ���� */
	ucComMF522Buf[0] = PICC_ANTICOLL1;
	ucComMF522Buf[1] = 0x70;
	ucComMF522Buf[6] = 0;

	for (uc = 0; uc < 4; uc++)
	{
		ucComMF522Buf[uc + 2] = *(pSnr + uc);
		ucComMF522Buf[6] ^= *(pSnr + uc);
	}

	CalulateCRC(ucComMF522Buf, 7, &ucComMF522Buf[7]);

	RC522_ClearBit_Register( Status2Reg, 0x08);

	ucN = PcdComMF522( PCD_TRANSCEIVE, ucComMF522Buf, 9, ucComMF522Buf, &ulLen);

	if ((ucN == MI_OK) && (ulLen == 0x18))
		ucN = MI_OK;
	else
		ucN = MI_ERR;

	return ucN;

}

/**
 * @brief   :У�鿨Ƭ����
 * @param  ��ucAuth_mode��������֤ģʽ
 *                     = 0x60����֤A��Կ
 *                     = 0x61����֤B��Կ
 *           ucAddr�����ַ
 *           pKey������
 *           pSnr����Ƭ���кţ�4�ֽ�
 * @retval ��״ֵ̬MI_OK���ɹ�
 */
char PcdAuthState(uint8_t ucAuth_mode, uint8_t ucAddr, uint8_t * pKey, uint8_t * pSnr)
{
	char cStatus;
	uint8_t uc, ucComMF522Buf[MAXRLEN];
	uint32_t ulLen;

	ucComMF522Buf[0] = ucAuth_mode;
	ucComMF522Buf[1] = ucAddr;
	/* ǰ���ֽڴ洢��֤ģʽ�Ϳ��ַ��2~8�ֽڴ洢���루6���ֽڣ���8~14�ֽڴ洢���к� */
	for (uc = 0; uc < 6; uc++)
		ucComMF522Buf[uc + 2] = *(pKey + uc);

	for (uc = 0; uc < 6; uc++)
		ucComMF522Buf[uc + 8] = *(pSnr + uc);
	/* ��������У�飬14~16�����ֽڴ洢У���� */
	cStatus = PcdComMF522( PCD_AUTHENT, ucComMF522Buf, 12, ucComMF522Buf, &ulLen);
	/* �ж���֤�Ƿ�ɹ� */
	if ((cStatus != MI_OK) || (!(RC522_Read_Register( Status2Reg) & 0x08)))
		cStatus = MI_ERR;

	return cStatus;

}

/**
 * @brief   :��M1����ָ�����ַд��ָ������
 * @param  ��ucAddr�����ַ
 *           pData��д������ݣ�16�ֽ�
 * @retval ��״ֵ̬MI_OK���ɹ�
 */
char PcdWrite(uint8_t ucAddr, uint8_t * pData)
{
	char cStatus;
	uint8_t uc, ucComMF522Buf[MAXRLEN];
	uint32_t ulLen;

	ucComMF522Buf[0] = PICC_WRITE;			//д������
	ucComMF522Buf[1] = ucAddr;			//д���ַ

	/* ����ѭ������У�飬������洢��& ucComMF522Buf [ 2 ] */
	CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);

	/* PCD_TRANSCEIVE:���Ͳ������������ͨ��RC522��Ƭ����д������ */
	cStatus = PcdComMF522( PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &ulLen);

	/* ͨ����Ƭ���ص���Ϣ�жϣ�RC522�Ƿ��뿨Ƭ����ͨ�� */
	if ((cStatus != MI_OK) || (ulLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
		cStatus = MI_ERR;

	if (cStatus == MI_OK)
	{
		//memcpy(ucComMF522Buf, pData, 16);
		/* ��Ҫд���16�ֽڵ����ݣ�����ucComMF522Buf������ */
		for (uc = 0; uc < 16; uc++)
			ucComMF522Buf[uc] = *(pData + uc);
		/* ����У�� */
		CalulateCRC(ucComMF522Buf, 16, &ucComMF522Buf[16]);
		/* ͨ��RC522����16�ֽ����ݰ���2�ֽ�У����д�뿨Ƭ�� */
		cStatus = PcdComMF522( PCD_TRANSCEIVE, ucComMF522Buf, 18, ucComMF522Buf, &ulLen);
		/* �ж�д��ַ�Ƿ�ɹ� */
		if ((cStatus != MI_OK) || (ulLen != 4) || ((ucComMF522Buf[0] & 0x0F) != 0x0A))
			cStatus = MI_ERR;
	}
	return cStatus;
}

/**
 * @brief   :��ȡM1����ָ�����ַ������
 * @param  ��ucAddr�����ַ
 *           pData�����������ݣ�16�ֽ�
 * @retval ��״ֵ̬MI_OK���ɹ�
 */
char PcdRead(uint8_t ucAddr, uint8_t * pData)
{
	char cStatus;
	uint8_t uc, ucComMF522Buf[MAXRLEN];
	uint32_t ulLen;

	ucComMF522Buf[0] = PICC_READ;
	ucComMF522Buf[1] = ucAddr;
	/* ����У�� */
	CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);
	/* ͨ��RC522���������Ƭ */
	cStatus = PcdComMF522( PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &ulLen);

	/* �����������������ȡ�������ݴ���pData�� */
	if ((cStatus == MI_OK) && (ulLen == 0x90))
	{
		for (uc = 0; uc < 16; uc++)
			*(pData + uc) = ucComMF522Buf[uc];
	}

	else
		cStatus = MI_ERR;

	return cStatus;

}

/**
 * @brief   :�ÿ�Ƭ��������ģʽ
 * @param  ����
 * @retval ��״ֵ̬MI_OK���ɹ�
 */
char PcdHalt(void)
{
	uint8_t ucComMF522Buf[MAXRLEN];
	uint32_t ulLen;

	ucComMF522Buf[0] = PICC_HALT;
	ucComMF522Buf[1] = 0;

	CalulateCRC(ucComMF522Buf, 2, &ucComMF522Buf[2]);
	PcdComMF522( PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, &ulLen);

	return MI_OK;

}

