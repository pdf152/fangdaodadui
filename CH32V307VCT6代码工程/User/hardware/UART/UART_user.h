#ifndef __UART_user_H_
#define __UART_user_H_

#include "debug.h"



#define DEBUG_UART1 1
#define DEBUG_UART2 2
#define DEBUG_UART3 3
#define DEBUG DEBUG_UART2



void USARTx_Init(USART_TypeDef *USARTx, uint32_t baudrate);
void uart_send_bytes(USART_TypeDef *USARTx, char *c,uint32_t cnt);
#endif
