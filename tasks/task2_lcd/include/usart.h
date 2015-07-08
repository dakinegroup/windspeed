#ifndef __USART_H__
#define __USART_H__

#define STATS_ENABLED 0
/* Remember there is only 2k of RAM */
#define BUFFER_SIZE  200
#define RX_BUFFER_SIZE  100

void USART_Init( unsigned int ubrr);
void printByte(unsigned char p);
void USART_Transmit_String_P(const char *x);
void USART_Transmit_Bytes_P();
void USART_Transmit_String(char *x);
void USART_Transmit_String2(char *x);
#if STATS_ENABLED
void USART_PrintStats()
#endif
unsigned char* USART_Receive_String();
int USART_Receive_String2(char *buff, int len);

#endif /* __USART_H__ */