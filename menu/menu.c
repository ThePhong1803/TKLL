#include "menu.h"
#include <stdio.h>
#include "..\button_matrix\button.h"
#include "..\lcd\lcd.h"
#include "..\uart\uart.h"
#include "..\i2c\i2c.h"
#include "..\day_modify\day_modify.h"
//#include "..\time_modify\time_modify.h"

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
//cong define

#define SUCCESS     99
#define HOUR_1      100
#define HOUR_2      101
#define MINUTE_1    102
#define MINUTE_2    103
#define SECOND_1    104
#define SECOND_2    105
#define YEAR_1      106
#define YEAR_2      107
#define YEAR_3      108
#define YEAR_4      109


#define TOKYO      50
#define BEIJING    51
#define PARIS      52
#define NEWYORK    53
#define LONDON     54
#define VIETNAM    55
#define WAIT       56

int modify = 0;
int last_modify = 0;

int status_worldTime = TOKYO;
int last_statusWT = TOKYO;
int UTC = 7;
int zone = 7;
unsigned char hour_change;
int uartBuff = -1;
// CONG END

int cmd_state = CMD_INIT;
unsigned char hour_t, minute_t, second_t, temp1, temp2, returnFlag = 0;
unsigned char run_stopwatch_flag = 0, hour_st = 0, minute_st = 0, second_st = 0, centi_second = 0, timer3_tick;

int console_state = CONSOLE_WAIT;

unsigned char getUartBufferChar();
void uart_console();
void command_parse();
void display_stopwatch();
void run_stopwatch();

// here cong
void fsm_worldTime();
void check_UTC();
void change_UTC(int value);
void fsm_timeModify();
int check_number_uart();
int validate(int number, int up);

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
            if(KEYOK > DEBOUNCE_THRS){
                status = SET_ALARM;
            }
            if(KEYUP > DEBOUNCE_THRS){
                status = BACK;
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
            if(KEYOK > DEBOUNCE_THRS){
                testFunction();
            }
            break;

        case TIME_MODIFY:
            if(last_state == TIMER_CLOCK){
                LcdPrintStringS(0, 0, " Timer Clock");
                LcdPrintStringS(1, 0, ">Time Modify");
            } else {
                LcdPrintStringS(0, 0, ">Time Modify");
                LcdPrintStringS(1, 0, " World Time");
            }
            if(KEYDOWN > DEBOUNCE_THRS){
                last_state = status;
                status = WORLD_TIME;
            }
            if(KEYUP > DEBOUNCE_THRS){
                last_state = status;
                status = TIMER_CLOCK;
            }
            if(KEYOK > DEBOUNCE_THRS){
                modify = 0;
                status = 7895;
            }
            break;

        case 7895:
            fsm_timeModify();
            break;
        case UART_CONSOLE:
            LcdPrintStringS(0, 0, "Please open soft");
            LcdPrintStringS(1, 0, "ware to modify");
            uart_console();
            if(KEYOK > DEBOUNCE_THRS){
                status = TIME_SCREEN;
            }
            break;

        case WORLD_TIME:
            if(last_state == TIME_MODIFY){
                LcdPrintStringS(0, 0, " Time Modify");
                LcdPrintStringS(1, 0, ">World Time");
            } else {
                LcdPrintStringS(0, 0, ">World Time");
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
            if(KEYOK > DEBOUNCE_THRS){
                status = 1247;
                check_UTC();
            }
            break;
        case 1247:
            fsm_worldTime();
            break;
        case BACK:
            LcdPrintStringS(0, 0, " World Time");
            LcdPrintStringS(1, 0, ">Back");
            if(KEYUP > DEBOUNCE_THRS){
                last_state = status;
                status = WORLD_TIME;
            }
            if(KEYDOWN > DEBOUNCE_THRS){
                last_state = status;
                status = ALARM_CLOCK;
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
                SetUpTime();
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

void fsm_worldTime(){
    switch(status_worldTime){
        case TOKYO:
            LcdPrintStringS(0, 0, ">TOKYO    UTC+09");
            LcdPrintStringS(1, 0, " BEIJING  UTC+08");
            if(KEYDOWN > DEBOUNCE_THRS){
                last_statusWT = TOKYO;
                status_worldTime = BEIJING;
            }
            if(KEYUP > DEBOUNCE_THRS){
                status_worldTime = BACK;
            }
            change_UTC(9);
            break;
        case BEIJING:
            if(last_statusWT == TOKYO){
                LcdPrintStringS(0, 0, " TOKYO    UTC+09");
                LcdPrintStringS(1, 0, ">BEIJING  UTC+08");
            }
            else {
                LcdPrintStringS(0, 0, ">BEIJING  UTC+08");
                LcdPrintStringS(1, 0, " PARIS    UTC+01");
            }
            if(KEYDOWN > DEBOUNCE_THRS){
                last_statusWT = BEIJING;
                status_worldTime = PARIS;
            }
            if(KEYUP > DEBOUNCE_THRS){
                last_statusWT = BEIJING;
                status_worldTime = TOKYO;
            }
            change_UTC(8);
            break;
        case PARIS:
            if(last_statusWT == BEIJING){
                LcdPrintStringS(0, 0, " BEIJING  UTC+08");
                LcdPrintStringS(1, 0, ">PARIS    UTC+01");
            }
            else {
                LcdPrintStringS(0, 0, ">PARIS    UTC+01");
                LcdPrintStringS(1, 0, " NEWYORK  UTC-05");
            }
            if(KEYDOWN > DEBOUNCE_THRS){
                last_statusWT = PARIS;
                status_worldTime = NEWYORK;
            }
            if(KEYUP > DEBOUNCE_THRS){
                last_statusWT = PARIS;
                status_worldTime = BEIJING;
            }
            change_UTC(1);
            break;
        case NEWYORK:
            if(last_statusWT == PARIS){
                LcdPrintStringS(0, 0, " PARIS    UTC+01");
                LcdPrintStringS(1, 0, ">NEWYORK  UTC-05");
            }
            else {
                LcdPrintStringS(0, 0, ">NEWYORK  UTC-05");
                LcdPrintStringS(1, 0, " LONDON   UTC+00");
            }
            if(KEYDOWN > DEBOUNCE_THRS){
                last_statusWT = NEWYORK;
                status_worldTime = LONDON;
            }
            if(KEYUP > DEBOUNCE_THRS){
                last_statusWT = NEWYORK;
                status_worldTime = PARIS;
            }
            change_UTC(-5);
            break;
        case LONDON:
            if(last_statusWT == NEWYORK){
                LcdPrintStringS(0, 0, " NEWYORK  UTC-05");
                LcdPrintStringS(1, 0, ">LONDON   UTC+00");
            }
            else {
                LcdPrintStringS(0, 0, ">LONDON   UTC+00");
                LcdPrintStringS(1, 0, " VIETNAM  UTC+07");
            }
            if(KEYDOWN > DEBOUNCE_THRS){
                last_statusWT = LONDON;
                status_worldTime = VIETNAM;
            }
            if(KEYUP > DEBOUNCE_THRS){
                last_statusWT = LONDON;
                status_worldTime = NEWYORK;
            }
            change_UTC(0);
            break;
        case VIETNAM:
            if(last_statusWT == LONDON){
                LcdPrintStringS(0, 0, " LONDON   UTC+00");
                LcdPrintStringS(1, 0, ">VIETNAM  UTC+07");
            }
            else {
                LcdPrintStringS(0, 0, ">VIETNAM  UTC+07");
                LcdPrintStringS(1, 0, " BACK");
            }
            if(KEYDOWN > DEBOUNCE_THRS){
                last_statusWT = VIETNAM;
                status_worldTime = BACK;
            }
            if(KEYUP > DEBOUNCE_THRS){
                last_statusWT = VIETNAM;
                status_worldTime = LONDON;
            }
            change_UTC(7);
            break;
        case BACK:
            LcdPrintStringS(0, 0, " VIETNAM  UTC+07");
            LcdPrintStringS(1, 0, ">BACK");
            if(KEYUP > DEBOUNCE_THRS){
                status_worldTime = VIETNAM;
                last_statusWT = BACK;
            }
            if(KEYDOWN > DEBOUNCE_THRS){
                status_worldTime = TOKYO;
                last_statusWT = BACK;
            }
            if(KEYOK > DEBOUNCE_THRS){
                status = WORLD_TIME;
            }
            break;
        default:
            break;
    }
}

void change_UTC(int value){
    if(KEYOK > DEBOUNCE_THRS && zone != value){
        UTC = value - zone;
        zone = value;
        hour_change = Read_DS1307(ADDRESS_HOUR);
        hour_change = (hour_change + UTC);
        if(hour_change < 0) hour_change = 0 - hour_change;
        Write_DS1307(ADDRESS_HOUR, hour_change);
        status = TIME_SCREEN;
        status_worldTime = WAIT;
     }
     else if(KEYOK > DEBOUNCE_THRS) {
        status = TIME_SCREEN;
     }
}

void check_UTC(){
    switch(zone){
        case 9:
            status_worldTime = TOKYO;
            break;
        case 8:
            status_worldTime = BEIJING;
            break;
        case 1:
            status_worldTime = PARIS;
            break;
        case -5:
            status_worldTime = NEWYORK;
            break;
        case 0:
            status_worldTime = LONDON;
            break;
        case 7:
            status_worldTime = VIETNAM;
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

int validate(int number, int up){
    if(number > up)
        return 0;
    return 1;
}

