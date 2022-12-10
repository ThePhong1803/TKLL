#include "time_modify.h"
#include <stdio.h>
#include "..\button_matrix\button.h"
#include "..\lcd\lcd.h"
#include "..\uart\uart.h"
#include "..\i2c\i2c.h"

#define DEBOUNCE_THRS 0
#define BACK                256
#define NULL                0
#define UART_CONSOLE        7
#define KEYUP           isKeyPressed(4)
#define KEYDOWN         isKeyPressed(5)
#define KEYOK           isKeyPressed(6)


#define CONSOLE_WAIT 1

int modify = 0;
int last_modify = 0;
int console_state = CONSOLE_WAIT;
int cmd_state = CMD_INIT;
int uartBuff = -1;
unsigned char hour_t, minute_t, second_t, temp1, temp2, returnFlag = 0;

unsigned char getUartBufferChar(){
    unsigned char c = recvBuff[readIdx];
    readIdx = (readIdx+1) % SERIAL_BUFFER_SIZE;
    return c;
}

void flushRecvBuff(){
    writeIdx = 0;
    readIdx = 0;
    recvDataFlag = 0;
}

void uart_console(){
    switch(console_state){
        case CONSOLE_WAIT:
            if(recvDataFlag > 0){
                command_parse();
                recvDataFlag--;
            }
            break;
        default:
            break;
    }
}

int check_number_uart(char c){
    if(c >= '0' && c <= '9'){
        return c - '0';
    }
    else return -1;
}

void fsm_timeModify(){
    switch(modify){
        case 0:
            LcdPrintStringS(0, 0, ">Use UART");
            LcdPrintStringS(1, 0, " Use Button");
            if(KEYOK > DEBOUNCE_THRS){
                UartSendString("Welcome to Alarm Clock console memu\r\n");
                UartSendString("To synchronize time, using syntax as follow:\r\n");
                UartSendString("!TIME:HHMMSS#\r\n");
                UartSendString("The time need to be 24-h format\r\n");
                //set read index, write index, recvDataFlag to 0
                //ignore the data recv before we enter this mode
                writeIdx = 0;
                readIdx = 0;
                recvDataFlag = 0;
                status = UART_CONSOLE;
            }
            if(KEYDOWN > DEBOUNCE_THRS){
                modify = 1;
                last_modify = 0;
            }
            if(KEYUP > DEBOUNCE_THRS){
                modify = BACK;
            }
            break;
        case 1:
            if(last_modify){
                LcdPrintStringS(0, 0, ">Use Button");
                LcdPrintStringS(1, 0, " Back");
            }
            else {
                LcdPrintStringS(0, 0, " Use UART");
                LcdPrintStringS(1, 0, ">Use Button");
            }
            if(KEYDOWN > DEBOUNCE_THRS){
                modify = BACK;
            }
            if(KEYUP > DEBOUNCE_THRS){
                modify = 0;
            }
            if(KEYOK > DEBOUNCE_THRS){

            }
            break;
        case BACK:
            LcdPrintStringS(0, 0, " Use Button");
            LcdPrintStringS(1, 0, ">Back");
            if(KEYOK > DEBOUNCE_THRS){
                status = TIME_MODIFY;
            }
            if(KEYDOWN > DEBOUNCE_THRS){
                modify = 0;
            }
            if(KEYUP > DEBOUNCE_THRS){
                modify = 1;
                last_modify = 1;
            }
            break;
        default:
            break;
    }
}

void command_parse(){
    char c = getUartBufferChar();
    uart_putchar(c);
    switch(cmd_state){
        case CMD_INIT:
            if(c == '!') cmd_state = CMD_START;
            break;

        case CMD_START:
            if(c == 'T') cmd_state = CMD_TIME1;
            else if (c == '!') cmd_state = CMD_START;
            else cmd_state = CMD_INIT;
            break;

        case CMD_TIME1:
            if(c == 'I') cmd_state = CMD_TIME2;
            else if (c == '!') cmd_state = CMD_START;
            else cmd_state = CMD_INIT;
            break;

        case CMD_TIME2:
            if(c == 'M') cmd_state = CMD_TIME3;
            else if (c == '!') cmd_state = CMD_START;
            else cmd_state = CMD_INIT;
            break;

        case CMD_TIME3:
            if(c == 'E') cmd_state = CMD_TIME4;
            else if (c == '!') cmd_state = CMD_START;
            else cmd_state = CMD_INIT;
            break;

        case CMD_TIME4:
            if(c == ':') cmd_state = HOUR_1;
            else if (c == '!') cmd_state = CMD_START;
            else cmd_state = CMD_INIT;
            break;

        case HOUR_1:
//            extractTime();
//            cmd_state = CMD_INIT;
            uartBuff = check_number_uart(c);
            if(uartBuff != -1) {
                hour_t = uartBuff * 10;
//              UartSendString("Succes! 1");
                cmd_state = HOUR_2;
            }
            else  {
                UartSendString("\r\nPlease enter number!\r\n");
                cmd_state = CMD_INIT;
            }
            break;
        case HOUR_2:
            uartBuff = check_number_uart(c);
            if(uartBuff != -1) {
                hour_t += uartBuff;
                if(validate(hour_t, 23)){
//                    UartSendString("Success! 2");
                    cmd_state = MINUTE_1;
                }
                else {
                    UartSendString("\r\nHour must be in range [0,23] !\r\n");
                    cmd_state = CMD_INIT;
                }
            }
            else {
                UartSendString("\r\nPlease enter number!\r\n");
                cmd_state = CMD_INIT;
            }
            break;
         case MINUTE_1:
            uartBuff = check_number_uart(c);
            if(uartBuff != -1) {
                minute_t = uartBuff * 10;
//                UartSendString("Success! 2");
                cmd_state = MINUTE_2;
            }
            else {
                UartSendString("\r\nPlease enter number!\r\n");
                cmd_state = CMD_INIT;
            }
            break;
        case MINUTE_2:
            uartBuff = check_number_uart(c);
            if(uartBuff != -1) {
                minute_t += uartBuff;
                if(validate(minute_t, 59)){
//                    UartSendString("Success! 2");
                    cmd_state = SECOND_1;
                }
                else {
                    UartSendString("\r\nMinute must be in range [0,59] !\r\n");
                    cmd_state = CMD_INIT;
                }
            }
            else {
                UartSendString("\r\nPlease enter number!\r\n");
                cmd_state = CMD_INIT;
            }
            break;
         case SECOND_1:
            uartBuff = check_number_uart(c);
            if(uartBuff != -1) {
                second_t = uartBuff * 10;
//                UartSendString("Success! 2");
                cmd_state = SECOND_2;
            }
            else {
                UartSendString("\r\nPlease enter number!\r\n");
                cmd_state = CMD_INIT;
            }
            break;
         case SECOND_2:
            uartBuff = check_number_uart(c);
            if(uartBuff != -1) {
                second_t += uartBuff;
                if(validate(second_t, 59))
//                UartSendString("Success! 2");
                cmd_state = SUCCESS;
                else {
                    UartSendString("\r\nSecond must be in range [0,59] !\r\n");
                    cmd_state = CMD_INIT;
                }
            }
            else {
                UartSendString("\r\nPlease enter number !\r\n");
                cmd_state = CMD_INIT;
            }
            break;
        case SUCCESS:
            if(c == '#'){
                cmd_state = CMD_INIT;
                Write_DS1307(ADDRESS_HOUR, hour_t);
                Write_DS1307(ADDRESS_MINUTE, minute_t);
                write_ds1307(ADDRESS_SECOND, second_t);
                UartSendString("\r\nConfiguration Success!\r\n");
            }
            break;
        default:
            break;

    }
}

int validate(int number, int up){
    if(number > up)
        return 0;
    return 1;
}