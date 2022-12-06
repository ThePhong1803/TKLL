#include "menu.h"
#include <stdio.h>
#include "..\button_matrix\button.h"
#include "..\lcd\lcd.h"
#include "..\uart\uart.h"
#include "..\i2c\i2c.h"

int status = TIME_SCREEN;
int last_state = NULL;

#define DEBOUNCE_THRS 0


#define CONSOLE_WAIT 1

#define CMD_INIT 1
#define CMD_START 2
#define CMD_TIME1 10
#define CMD_TIME2 11
#define CMD_TIME3 12
#define CMD_TIME4 14
#define CMD_EXTRACT_TIME 20

int cmd_state = CMD_INIT;
unsigned char hour_t, minute_t, second_t, temp1, temp2, returnFlag = 0;
unsigned char run_stopwatch_flag = 0, hour_st = 0, minute_st = 0, second_st = 0, centi_second = 0, timer3_tick;

int console_state = CONSOLE_WAIT;

unsigned char getUartBufferChar();
void uart_console();
void command_parse();
void display_stopwatch();
void run_stopwatch();

void menuControl(){
    switch(status){
        case TIME_SCREEN:
            // basic mode, diaplay time and show the status of the alarm
            DisplayTime();
            Alarm();
            if(KEYOK > DEBOUNCE_THRS){
                status = ALARM_CLOCK;
                last_state = TIME_SCREEN;
            }
            break;

        case ALARM_CLOCK:
            LcdPrintStringS(0, 0, ">Alarm Clock");
            LcdPrintStringS(1, 0, " Counter Clock");
            if(KEYDOWN > DEBOUNCE_THRS){
                last_state = status;
                status = STOPWATCH;
            }
            if(KEYOK > DEBOUNCE_THRS){\
                status = SET_ALARM;
            }
            break;
            
        case SET_ALARM:
            DisplayAlarmTime();
            SetUpAlarm();
            if(returnFlag){
                returnFlag = 0;
                status = ALARM_CLOCK;
            }
            break;

        case STOPWATCH:
            if(last_state == ALARM_CLOCK){
                LcdPrintStringS(0, 0, " Alarm Clock");
                LcdPrintStringS(1, 0, ">Counter Clock");
            } else {
                LcdPrintStringS(0, 0, ">Counter Clock");
                LcdPrintStringS(1, 0, " Timer Clock");
            }
            if(KEYDOWN > DEBOUNCE_THRS){
                last_state = status;
                status = TIMER_CLOCK;
            }
            if(KEYUP > DEBOUNCE_THRS){
                last_state = status;
                status = ALARM_CLOCK;
            }
            if(KEYOK > DEBOUNCE_THRS){
                status = RUN_STOPWATCH;
            }
            break;

        case RUN_STOPWATCH:
            display_stopwatch();
            if(run_stopwatch_flag) run_stopwatch();
            if(KEYUP){
                run_stopwatch_flag = 1; //key up act like start button
            }
            if(KEYDOWN){
                run_stopwatch_flag = 0; // key down act like stop button
            }
            if(KEYDOWN_HOLD){
                hour_st = 0; minute_st = 0; second_st = 0; centi_second = 0; timer3_tick = 0;
            }
            if(KEYOK){
                run_stopwatch_flag = 0;
                hour_st = 0;
                minute_st = 0;
                second_st = 0;
                centi_second = 0;
                timer3_tick = 0;
                returnFlag = 0;
                status = STOPWATCH;
            }
            break;

        case TIMER_CLOCK:
            if(last_state == STOPWATCH){
                LcdPrintStringS(0, 0, " Counter Clock");
                LcdPrintStringS(1, 0, ">Timer Clock");
            } else {
                LcdPrintStringS(0, 0, ">Timer Clock");
                LcdPrintStringS(1, 0, " Time modify");
            }
            if(KEYDOWN > DEBOUNCE_THRS){
                last_state = status;
                status = TIME_MODIFY;
            }
            if(KEYUP > DEBOUNCE_THRS){
                last_state = status;
                status = STOPWATCH;
            }
            break;

        case TIME_MODIFY:
            if(last_state == TIMER_CLOCK){
                LcdPrintStringS(0, 0, " Timer Clock");
                LcdPrintStringS(1, 0, ">Time modify");
            } else {
                LcdPrintStringS(0, 0, ">Time modify");
                LcdPrintStringS(1, 0, " Language");
            }
            if(KEYDOWN > DEBOUNCE_THRS){
                last_state = status;
                status = LANGUAGE;
            }
            if(KEYUP > DEBOUNCE_THRS){
                last_state = status;
                status = TIMER_CLOCK;
            }
            if(KEYOK > DEBOUNCE_THRS){
                UartSendString("Welcome to Alarm Clock console memu\r\n");
                UartSendString("To synchronize time, using syntax as follow:\r\n");
                UartSendString("!TIME#-HHMMSS\r\n");
                UartSendString("The time need to be 24-h format\r\n");
                //set read index, write index, recvDataFlag to 0
                //ignore the data recv before we enter this mode
                writeIdx = 0;
                readIdx = 0;
                recvDataFlag = 0;
                status = UART_CONSOLE;
            }
            break;

        case UART_CONSOLE:
            uart_console();
            if(KEYOK > DEBOUNCE_THRS){
                status = TIME_MODIFY;
            }
            break;

        case LANGUAGE:
            if(last_state == TIME_MODIFY){
                LcdPrintStringS(0, 0, " Time modify");
                LcdPrintStringS(1, 0, ">Language");
            } else {
                LcdPrintStringS(0, 0, ">Language");
                LcdPrintStringS(1, 0, " Back");
            }
            if(KEYDOWN > DEBOUNCE_THRS){
                last_state = status;
                status = BACK;
            }
            if(KEYUP > DEBOUNCE_THRS){
                last_state = status;
                status = TIME_MODIFY;
            }
            break;

        case BACK:
            LcdPrintStringS(0, 0, " Language");
            LcdPrintStringS(1, 0, ">Back");
            if(KEYUP > DEBOUNCE_THRS){
                last_state = status;
                status = LANGUAGE;
            }
            if(KEYOK > DEBOUNCE_THRS){
                last_state = status;
                status = TIME_SCREEN;
            }
            break;

        default:
            break;
    }
}

void run_stopwatch()
{
//    timer3_tick++; // timer 0 tick inc every 1 ms
//    if(timer3_tick >= 10) // 10 ms elapsed
//    {
//        timer3_tick = 0; // reset timer 3 tick
//        centi_second++;  // inc centi second
//    }
    centi_second+=5;
    if(centi_second >= 100) // 1 second elapsed
    {
        centi_second = 0;   // reset centi second;
        second_st++;        // inc second
    }
    if(second_st >= 60) //1 minute elapsed
    {
        second_st = 0;  // reset second
        minute_st++;    // inc minute
    }
    if(minute_st >= 60) // 1 hour elapsed
    {
        minute_st = 0;  // reset minute
        hour_st++;      // inc hour
    }
}

void display_stopwatch()
{
    LcdPrintStringS(0,0,"   STOPWATCH   ");
    LcdPrintStringS(1,0,"  ");
    if(hour_st < 10)
    {
        LcdPrintStringS(1,2,"0");
        LcdPrintNumS(1,3,hour_st);
    }
    else
        LcdPrintNumS(1,2,hour_st);
    LcdPrintStringS(1,4,":");
    if(minute_st < 10)
    {
        LcdPrintStringS(1,5,"0");
        LcdPrintNumS(1,6,minute_st);
    }
    else
        LcdPrintNumS(1,5,minute_st);
    LcdPrintStringS(1,7,":");
    if(second_st < 10)
    {
        LcdPrintStringS(1,8,"0");
        LcdPrintNumS(1,9,second_st);
    }
    else
        LcdPrintNumS(1,8,second_st);
    LcdPrintStringS(1,10,":");
    if(centi_second < 10)
    {
        LcdPrintStringS(1,11,"0");
        LcdPrintNumS(1,12,centi_second);
    }
    else
        LcdPrintNumS(1,11,centi_second);
}

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

char msg[64];
void extractTime(){
    // hour extraction
    if(recvDataFlag > 0) {
        temp1 = getUartBufferChar();
        recvDataFlag--;
    } else {
        UartSendString("\r\nInvalid time\r\n");
        return;
    }

    if(temp1 >= '0' && temp1 <= '9') temp1 -= '0';
    else {
        UartSendString("\r\nInvalid time\r\n");
        return;
    }
    
    if(recvDataFlag > 0) {
        temp2 = getUartBufferChar();
        recvDataFlag--;
    } else {
        UartSendString("\r\nInvalid time\r\n");
        return;
    }

    if(temp2 >= '0' && temp1 <= '9') temp2 -= '0';
    else {
        UartSendString("\r\nInvalid time\r\n");
        return;
    }
    hour_t = temp1*10 + temp2;

    // minute extraction
    if(recvDataFlag > 0) {
        temp1 = getUartBufferChar();
        recvDataFlag--;
    } else {
        UartSendString("\r\nInvalid time\r\n");
        return;
    }

    if(temp1 >= '0' && temp1 <= '9') temp1 -= '0';
    else {
        UartSendString("\r\nInvalid time\r\n");
        return;
    }

    if(recvDataFlag > 0) {
        temp2 = getUartBufferChar();
        recvDataFlag--;
    } else {
        UartSendString("\r\nInvalid time\r\n");
        return;
    }

    if(temp2 >= '0' && temp1 <= '9') temp2 -= '0';
    else {
        UartSendString("\r\nInvalid time\r\n");
        return;
    }
    minute_t = temp1*10 + temp2;

    // second extraction
    if(recvDataFlag > 0) {
        temp1 = getUartBufferChar();
        recvDataFlag--;
    } else {
        UartSendString("\r\nInvalid time\r\n");
        return;
    }

    if(temp1 >= '0' && temp1 <= '9') temp1 -= '0';
    else {
        UartSendString("\r\nInvalid time\r\n");
        return;
    }

    if(recvDataFlag > 0) {
        temp2 = getUartBufferChar();
        recvDataFlag--;
    } else {
        UartSendString("\r\nInvalid time\r\n");
        return;
    }

    if(temp2 >= '0' && temp1 <= '9') temp2 -= '0';
    else {
        UartSendString("\r\nInvalid time\r\n");
        return;
    }
    second_t = temp1*10 + temp2;
    Write_DS1307(ADDRESS_SECOND, second_t);
    Write_DS1307(ADDRESS_MINUTE, minute_t);
    Write_DS1307(ADDRESS_HOUR, hour_t);
//    Write_DS1307(ADDRESS_DAY, day);
//    Write_DS1307(ADDRESS_DATE, date);
//    Write_DS1307(ADDRESS_MONTH, month);
//    Write_DS1307(ADDRESS_YEAR, year);
    sprintf(msg, "\r\nTime is set to %.02d:%.02d:%.02d\r\n", hour_t, minute_t, second_t);
    uart_send_str(msg);
    UartSendString("Time Update complete!\r\n");
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
            if(c == '#') cmd_state = CMD_EXTRACT_TIME;
            else if (c == '!') cmd_state = CMD_START;
            else cmd_state = CMD_INIT;
            break;

        case CMD_EXTRACT_TIME:
            extractTime();
            cmd_state = CMD_INIT;
            break;

    }
}