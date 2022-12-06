#ifndef _UART_H_
#define _UART_H_

#include <p18f4620.h>

#define SERIAL_BUFFER_SIZE 64

extern unsigned char dataReceive;
extern unsigned char dataFromUart[8];
extern unsigned char kindOfData;

extern unsigned char recvBuff[SERIAL_BUFFER_SIZE];  //initial recv buffer with 256 byte;
extern unsigned char writeIdx;
extern unsigned char readIdx;                       //index to read from serial databuffer
extern unsigned char recvDataFlag;                  //data flag, greater or equal zero, represent the number of byte recv

void init_uart();
void uart_putchar(unsigned char data);
void uart_send_str(const char *str);
void UartSendNum(long num);
void UartSendString(const rom char *str);
void uart_isr();

#endif