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
//cong define
#define     INIT_SYSTEM         0
#define     SET_HOUR_ALARM      1
#define     SET_MINUTE_ALARM    2
#define     BIT_ALARM           3
#define     COMPARE             4
#define     ALARM               5
#define     BACK_MENU           -1

#define     SET_HOUR            6
#define     SET_MINUTE          7
#define     SET_DAY             8
#define     SET_DATE            9
#define     SET_MONTH           10
#define     SET_YEAR            11
#define     ENABLE              1
#define     DISABLE             0

#define SUCCESS     99
#define HOUR_1      100
#define HOUR_2      101
#define MINUTE_1    102
#define MINUTE_2    103
#define SECOND_1    104
#define SECOND_2    105

#define CMD_DATE1   106
#define CMD_DATE2   107
#define CMD_DATE3   108
#define CMD_DATE4   109
#define YEAR_1      110
#define YEAR_2      111
#define YEAR_3      112
#define YEAR_4      113
#define MONTH_1     114
#define MONTH_2     115
#define DAY         116
#define DATE_1      117
#define DATE_2      118

#define CMD_DHT1    200
#define CMD_DHT2    201
#define CMD_DHT3    202
#define TEMP1       203
#define TEMP2       204
#define HUMI1       205
#define HUMI2       206
#define COLON       207

#define TOKYO      50
#define BEIJING    51
#define PARIS      52
#define NEWYORK    53
#define LONDON     54
#define HANOI      55
#define WAIT       56

#define VALUE0     57
#define VALUE1     58
#define VALUE2     59
#define VALUE3     60
#define VALUE4     61
#define VALUE5     62

#define BEGINNING_YEAR 2021

int modify = 0;
unsigned char timeBlink_1 = 0;
int last_modify = 0;
unsigned char timer_clock_config = 0;
unsigned char statusSetUpTime = INIT_SYSTEM;
unsigned char LunarDay, LunarMonth,  LunarYear;
int status_worldTime = TOKYO;
int last_statusWT = TOKYO;
int UTC = 7;
static rom int zone = 7;
int hour_change;
int uartBuff = -1;
int uartBuffDate = -1;
unsigned char returnOK = 0;
int hour_tc = 0, minute_tc = 0, second_tc = 0;
unsigned char blink = 0;
unsigned char run_stop = 0;
int last_sec = 0, curr_sec = 0;

unsigned char dht_state = CMD_INIT;
unsigned char temperature = 0;
unsigned char humidity = 0;
unsigned char dhtBuffer = 0;

unsigned char uart_mode = 0;
unsigned char uart_last_mode = 0;
unsigned char count = 0;
unsigned char flagDHT = 0;
unsigned char flagInternet = 0;
unsigned char flagUart = 0;
// CONG END
unsigned char second_mf = 0, minute_mf = 0, hour_mf = 0;
unsigned char day_mf = 0;
unsigned char date_mf = 0, month_mf = 0, year_mf = 0;
unsigned char bitEnable_1 = ENABLE;

int cmd_state = CMD_INIT;
int cmd_date = CMD_INIT; //cong
unsigned char hour_t, minute_t, second_t,
        temp1, temp2, returnFlag = 0, setTimeFlag = 0;
int year_t, month_t, date_t, day_t;
unsigned char run_stopwatch_flag = 0,
        hour_st = 0, minute_st = 0, second_st = 0;

struct time_stop_watch {
    unsigned char hour, minute, second;
};
struct time_stop_watch arr_time[6];
unsigned char index_arr = 0; //index for arr_time with max value is 5
unsigned char status_stop_time, status_last_stop_time = VALUE0;
void display_stop_time();
void reset_array_time();
void display_value(unsigned char position, struct time_stop_watch value);
int console_state = CONSOLE_WAIT;
void printCan(int can);
void printChi(int chi);
unsigned char check_year(int year);
void DisplayTimeForModify();
void SetUpTime();

unsigned char getUartBufferChar();
void uart_console();
void command_parse();
void display_stopwatch();
void run_stopwatch();

// here cong
void fsm_worldTime(); //fsm for world time function
void check_UTC(); //check current time zone
void change_UTC(int value); //change current time zone
void fsm_timeModify(); //fsm for modify time function
int check_number_uart(); //convert char to number and return else return -1
int validate(int number, int up); //check if number > up return false
int check_month(int month); //check current month have 31,
//30 day or current month is 2
unsigned char check_day(unsigned char mode); //check if the date has been
//changed due to we chagne time zone
void display_timer_clock(); //display lcd for timer clock function
void run_timer_clock(); //run timer clock function
void fsm_uart_mode();
unsigned int LUNAR_CALENDAR_LOOKUP_TABLE[] = {
	// 2021
	0x1B73, 0x0394, 0x1832, 0x1254, 0x1A74, 0x1095, 0x1AB6, 0x18D7, 0x12F9, 0x1919, 0x133B, 0x195B,

	// 2022
	0x1B7D, 0x0021, 0x1A3D, 0x1061, 0x1A81, 0x12A3, 0x1AC3, 0x18E4, 0x1306, 0x1926, 0x1348, 0x1968,

	// 2023
	0x1B8A, 0x002B, 0x1A4A, 0x104B, 0x186C, 0x128E, 0x1AAE, 0x18CF, 0x12F1, 0x1B11, 0x1132, 0x1B53,

	// 2024
	0x1974, 0x0B96, 0x1835, 0x1257, 0x1877, 0x1099, 0x1ABA, 0x18DB, 0x12FD, 0x1B1D, 0x1141, 0x1B61,

	// 2025
	0x1982, 0x0224, 0x1842, 0x1264, 0x1884, 0x10A6, 0x1AC7, 0x18C8, 0x12EA, 0x190A, 0x132C, 0x1B4C
	};
void Solar2Lunar(unsigned char SolarDay, unsigned char SolarMonth, unsigned int SolarYear,
				 unsigned char *LunarDay, unsigned char *LunarMonth, unsigned int *LunarYear);

void menuControl() {
    switch (status) {
        case TIME_SCREEN:
            // basic mode, display time and show the status of the alarm
            DisplayTime();
            Alarm();
            if (KEYOK > DEBOUNCE_THRS) {
                status = ALARM_CLOCK;
                last_state = TIME_SCREEN;
            }
            break;

        case ALARM_CLOCK:
            LcdPrintStringS(0, 0, ">ALARM CLOCK");
            LcdPrintStringS(1, 0, " LUNAR CALENDAR");
            if (KEYDOWN > DEBOUNCE_THRS) {
                last_state = status;
                status = LUNAR_CALENDAR;
            }
            if (KEYOK > DEBOUNCE_THRS) {
                status = SET_ALARM;
            }
            if (KEYUP > DEBOUNCE_THRS) {
                status = BACK;
            }
            break;
        case SET_ALARM:
            DisplayAlarmTime();
            SetUpAlarm();
            if (returnFlag) {
                returnFlag = 0;
                status = ALARM_CLOCK;
            }
            break;
        case LUNAR_CALENDAR:
            if (last_state == ALARM_CLOCK) {
                LcdPrintStringS(0, 0, " ALARM CLOCK");
                LcdPrintStringS(1, 0, ">LUNAR CALENDAR");
            } else {
                LcdPrintStringS(0, 0, ">LUNAR CALENDAR");
                LcdPrintStringS(1, 0, " STOP WATCH");
            }
            if (KEYDOWN > DEBOUNCE_THRS) {
                last_state = status;
                status = STOPWATCH;
            }
            if (KEYOK > DEBOUNCE_THRS) {
                status = DISPLAY_LUNAR;
                Solar2Lunar(Read_DS1307(ADDRESS_DATE),Read_DS1307(ADDRESS_MONTH),
                            Read_DS1307(ADDRESS_YEAR) + 2000, &LunarDay, &LunarMonth,
                            &LunarYear);
            }
            if (KEYUP > DEBOUNCE_THRS) {
                last_state = status;
                status = ALARM_CLOCK;
            }
            break;
        case DISPLAY_LUNAR:
            if(KEYOK){
                status = LUNAR_CALENDAR;
            }
            printCan((LunarYear + 2000) % 10);
            printChi((LunarYear + 2000) % 12);
            LcdPrintStringS(0,1,"Nam ");
            LcdPrintStringS(1,0,"Ngay ");
            LcdPrintNumS(1,5,LunarDay);
            LcdPrintStringS(1,8,"Thang ");
            LcdPrintNumS(1,14,LunarMonth);
            break;

        case STOPWATCH:
            if (last_state == LUNAR_CALENDAR) {
                LcdPrintStringS(0, 0, " LUNAR CALENDAR");
                LcdPrintStringS(1, 0, ">STOP WATCH");
            } else {
                LcdPrintStringS(0, 0, ">STOP WATCH");
                LcdPrintStringS(1, 0, " TIMER CLOCK");
            }
            if (KEYDOWN > DEBOUNCE_THRS) {
                last_state = status;
                status = TIMER_CLOCK;
            }
            if (KEYUP > DEBOUNCE_THRS) {
                last_state = status;
                status = LUNAR_CALENDAR;
            }
            if (KEYOK > DEBOUNCE_THRS) {
                status = RUN_STOPWATCH;
                reset_array_time();
                run_stopwatch_flag = 0;
            }
            break;

        case RUN_STOPWATCH:
            display_stopwatch();
            if (run_stopwatch_flag) run_stopwatch();
            if(KEYUP_HOLD){
                run_stopwatch_flag = 0;
                hour_st = 0;
                minute_st = 0;
                second_st = 0;
            }
            else if (KEYUP && run_stopwatch_flag == 0) {
                run_stopwatch_flag = 1; //key up act like start button
                last_sec = Read_DS1307(ADDRESS_SECOND);
                curr_sec = Read_DS1307(ADDRESS_SECOND);
            }
            else if(KEYUP && run_stopwatch_flag == 1){
                run_stopwatch_flag = 0;
            }
            if (KEYDOWN) {
                if (index_arr < 5) {
                    arr_time[index_arr].hour = hour_st;
                    arr_time[index_arr].minute = minute_st;
                    arr_time[index_arr].second = second_st;
                    index_arr++;
                }
            }
            if (KEYDOWN_HOLD) {
                status = DISPLAY_STOP_TIME;
                run_stopwatch_flag = 0;
                status_stop_time = VALUE1;
                status_last_stop_time = VALUE1;
            }
            if (KEYOK) {
                run_stopwatch_flag = 0;
                hour_st = 0;
                minute_st = 0;
                second_st = 0;
                returnFlag = 0;
                status = STOPWATCH;
            }
            break;

        case DISPLAY_STOP_TIME:
            display_stop_time();
            break;

        case TIMER_CLOCK:
            if (last_state == STOPWATCH) {
                LcdPrintStringS(0, 0, " STOP WATCH");
                LcdPrintStringS(1, 0, ">TIMER CLOCK");
            } else {
                LcdPrintStringS(0, 0, ">TIMER CLOCK");
                LcdPrintStringS(1, 0, " TIME MODIFY");
            }
            if (KEYDOWN > DEBOUNCE_THRS) {
                last_state = status;
                status = TIME_MODIFY;
            }
            if (KEYUP > DEBOUNCE_THRS) {
                last_state = status;
                status = STOPWATCH;
            }
            if (KEYOK > DEBOUNCE_THRS) {
                status = RUN_TIMER_CLOCK;
                timer_clock_config = 0;
            }
            break;
        case RUN_TIMER_CLOCK:
            display_timer_clock();
            run_timer_clock();
            break;
        case TIME_MODIFY:
            if (last_state == TIMER_CLOCK) {
                LcdPrintStringS(0, 0, " TIMER CLOCK");
                LcdPrintStringS(1, 0, ">TIME MODIFY");
            } else {
                LcdPrintStringS(0, 0, ">TIME MODIFY");
                LcdPrintStringS(1, 0, " GLOBAL TIME");
            }
            if (KEYDOWN > DEBOUNCE_THRS) {
                last_state = status;
                status = WORLD_TIME;
            }
            if (KEYUP > DEBOUNCE_THRS) {
                last_state = status;
                status = TIMER_CLOCK;
            }
            if (KEYOK > DEBOUNCE_THRS) {
                modify = 0;
                status = RUN_TIME_MODIFY;
            }
            break;

        case RUN_TIME_MODIFY:
            fsm_timeModify();
            break;
        case UART_CONSOLE:
            LcdPrintStringS(0, 0, "Please connect");
            LcdPrintStringS(1, 0, "UART to modify");
            uart_console();
            if (KEYOK > DEBOUNCE_THRS) {
                status = TIME_SCREEN;
            }
            if (flagInternet && uart_mode == 1) {
                status = TIME_SCREEN;
                flagInternet = 0;
            }
            break;

        case WORLD_TIME:
            if (last_state == TIME_MODIFY) {
                LcdPrintStringS(0, 0, " TIME MODIFY");
                LcdPrintStringS(1, 0, ">GLOBAL TIME");
            } else {
                LcdPrintStringS(0, 0, ">GLOBAL TIME");
                LcdPrintStringS(1, 0, " WEATHER");
            }
            if (KEYDOWN > DEBOUNCE_THRS) {
                last_state = status;
                status = TEMP_HUMI;
            }
            if (KEYUP > DEBOUNCE_THRS) {
                last_state = status;
                status = TIME_MODIFY;
            }
            if (KEYOK > DEBOUNCE_THRS) {
                status = RUN_WORLD_TIME;
                check_UTC();
            }
            break;
        case RUN_WORLD_TIME:
            fsm_worldTime();
            break;
        case TEMP_HUMI:
            if (last_state == WORLD_TIME) {
                LcdPrintStringS(0, 0, " GLOBAL TIME");
                LcdPrintStringS(1, 0, ">WEATHER");
            } else {
                LcdPrintStringS(0, 0, ">WEATHER");
                LcdPrintStringS(1, 0, " BACK");
            }
            if (KEYDOWN > DEBOUNCE_THRS) {
                last_state = status;
                status = BACK;
            }
            if (KEYUP > DEBOUNCE_THRS) {
                last_state = status;
                status = WORLD_TIME;
            }
            if (KEYOK > DEBOUNCE_THRS) {
                status = RUN_DHT;
                UartSendString("!DHT#\r\n");
                dht_state = CMD_INIT;
            }
            break;
        case RUN_DHT:
            LcdPrintStringS(0, 0, "Receving data..");
            count = 0;
            curr_sec = Read_DS1307(ADDRESS_SECOND);
            last_sec = curr_sec;
            uart_console();
            if (KEYOK > DEBOUNCE_THRS) {
                status = TEMP_HUMI;
            }
            break;
        case WEATHER_DISPLAY:
            LcdPrintStringS(0, 0, "TEMP:   C");
            LcdPrintStringS(1, 0, "HUMI:   %");
            LcdPrintNumS(0, 6, temperature);
            LcdPrintNumS(1, 6, humidity);
            curr_sec = Read_DS1307(ADDRESS_SECOND);
            if (curr_sec == last_sec + 1 || (last_sec == 59 && curr_sec == 0)) {
                last_sec = curr_sec;
                count++;
            }
            if (count == 5 && flagDHT == 1) {
                UartSendString("!DHT#\r\n");
                count = 0;
                flagDHT = 0;
            }
            uart_console();
            if (KEYOK > DEBOUNCE_THRS) {
                status = TEMP_HUMI;
            }
            break;
        case BACK:
            LcdPrintStringS(0, 0, " WEATHER");
            LcdPrintStringS(1, 0, ">BACK");
            if (KEYUP > DEBOUNCE_THRS) {
                last_state = status;
                status = TEMP_HUMI;
            }
            if (KEYDOWN > DEBOUNCE_THRS) {
                last_state = status;
                status = ALARM_CLOCK;
            }
            if (KEYOK > DEBOUNCE_THRS) {
                last_state = status;
                status = TIME_SCREEN;
            }
            break;

        default:
            break;
    }
}

void run_stopwatch() {
    curr_sec = Read_DS1307(ADDRESS_SECOND);
    if (curr_sec == last_sec + 1 || (last_sec == 59 && curr_sec == 0)) {
        second_st++;
        last_sec = curr_sec;
    }
    if (second_st >= 60) //1 minute elapsed
    {
        second_st = 0; // reset second
        minute_st++; // inc minute
    }
    if (minute_st >= 60) // 1 hour elapsed
    {
        minute_st = 0; // reset minute
        hour_st++; // inc hour
    }
}

void reset_array_time() {
    int i = 0;
    index_arr = 0;
    for(i; i < 6; i++) {
        arr_time[i].hour = 0;
        arr_time[i].minute = 0;
        arr_time[i].second = 0;
    }
}

void display_stop_time() {
    switch (status_stop_time) {
        case VALUE1:
            LcdPrintStringS(0, 0, ">1. ");
            LcdPrintStringS(1, 0, " 2. ");
            LcdPrintStringS(0, 6, ":");
            LcdPrintStringS(0, 9, ":");
            LcdPrintStringS(1, 6, ":");
            LcdPrintStringS(1, 9, ":");
            display_value(0,arr_time[0]);
            display_value(1,arr_time[1]);
            if(KEYDOWN > DEBOUNCE_THRS){
                status_stop_time = VALUE2;
                status_last_stop_time = VALUE1;
            }
            if(KEYUP > DEBOUNCE_THRS){
                status_stop_time = VALUE5;
                status_last_stop_time = VALUE1;
            }
            if(KEYUP_HOLD || KEYDOWN_HOLD){
                reset_array_time();
            }
            if(KEYOK){
                status = RUN_STOPWATCH;
            }
            break;
        case VALUE2:
            if(status_last_stop_time == VALUE1){
                LcdPrintStringS(0, 0, " 1. ");
                LcdPrintStringS(1, 0, ">2. ");
                display_value(0,arr_time[0]);
                display_value(1,arr_time[1]);
            }
            else {
                LcdPrintStringS(0, 0, ">2. ");
                LcdPrintStringS(1, 0, " 3. ");
                display_value(0,arr_time[1]);
                display_value(1,arr_time[2]);
            }
            LcdPrintStringS(0, 6, ":");
            LcdPrintStringS(0, 9, ":");
            LcdPrintStringS(1, 6, ":");
            LcdPrintStringS(1, 9, ":");
            if(KEYDOWN > DEBOUNCE_THRS){
                status_stop_time = VALUE3;
                status_last_stop_time = VALUE2;
            }
            if(KEYUP > DEBOUNCE_THRS){
                status_stop_time = VALUE1;
                status_last_stop_time = VALUE2;
            }
            if(KEYUP_HOLD || KEYDOWN_HOLD){
                reset_array_time();
            }
            if(KEYOK){
                status = RUN_STOPWATCH;
            }
            break;
        case VALUE3:
            if(status_last_stop_time == VALUE2){
                LcdPrintStringS(0, 0, " 2. ");
                LcdPrintStringS(1, 0, ">3. ");
                display_value(0,arr_time[1]);
                display_value(1,arr_time[2]);
            }
            else {
                LcdPrintStringS(0, 0, ">3. ");
                LcdPrintStringS(1, 0, " 4. ");
                display_value(0,arr_time[2]);
                display_value(1,arr_time[3]);
            }
            LcdPrintStringS(0, 6, ":");
            LcdPrintStringS(0, 9, ":");
            LcdPrintStringS(1, 6, ":");
            LcdPrintStringS(1, 9, ":");
            if(KEYDOWN > DEBOUNCE_THRS){
                status_stop_time = VALUE4;
                status_last_stop_time = VALUE3;
            }
            if(KEYUP > DEBOUNCE_THRS){
                status_stop_time = VALUE2;
                status_last_stop_time = VALUE3;
            }
            if(KEYUP_HOLD || KEYDOWN_HOLD){
                reset_array_time();
            }
            if(KEYOK){
                status = RUN_STOPWATCH;
            }
            break;
        case VALUE4:
            if(status_last_stop_time == VALUE3){
                LcdPrintStringS(0, 0, " 3. ");
                LcdPrintStringS(1, 0, ">4. ");
                display_value(0,arr_time[2]);
                display_value(1,arr_time[3]);
            }
            else {
                LcdPrintStringS(0, 0, ">4. ");
                LcdPrintStringS(1, 0, " 5. ");
                display_value(0,arr_time[3]);
                display_value(1,arr_time[4]);
            }
            LcdPrintStringS(0, 6, ":");
            LcdPrintStringS(0, 9, ":");
            LcdPrintStringS(1, 6, ":");
            LcdPrintStringS(1, 9, ":");
            if(KEYDOWN > DEBOUNCE_THRS){
                status_stop_time = VALUE5;
                status_last_stop_time = VALUE4;
            }
            if(KEYUP > DEBOUNCE_THRS){
                status_stop_time = VALUE3;
                status_last_stop_time = VALUE4;
            }
            if(KEYUP_HOLD || KEYDOWN_HOLD){
                reset_array_time();
            }
            if(KEYOK){
                status = RUN_STOPWATCH;
            }
            break;
        case VALUE5:
            LcdPrintStringS(0, 0, " 4. ");
            LcdPrintStringS(1, 0, ">5. ");
            display_value(0,arr_time[3]);
            display_value(1,arr_time[4]);
            LcdPrintStringS(0, 6, ":");
            LcdPrintStringS(0, 9, ":");
            LcdPrintStringS(1, 6, ":");
            LcdPrintStringS(1, 9, ":");
            if(KEYDOWN > DEBOUNCE_THRS){
                status_stop_time = VALUE1;
                status_last_stop_time = VALUE5;
            }
            if(KEYUP > DEBOUNCE_THRS){
                status_stop_time = VALUE4;
                status_last_stop_time = VALUE5;
            }
            if(KEYUP_HOLD || KEYDOWN_HOLD){
                reset_array_time();
            }
            if(KEYOK){
                status = RUN_STOPWATCH;
            }
            break;
        default:
            break;
    }
}

void display_value(unsigned char position, struct time_stop_watch value) {
    if (value.hour < 10) {
        LcdPrintNumS(position, 4, 0);
        LcdPrintNumS(position, 5, value.hour);
    } else {
        LcdPrintNumS(position, 4, value.hour);
    }
    if (value.minute < 10) {
        LcdPrintNumS(position, 7, 0);
        LcdPrintNumS(position, 8, value.minute);
    } else {
        LcdPrintNumS(position, 7, value.minute);
    }
    if (value.second < 10) {
        LcdPrintNumS(position, 10, 0);
        LcdPrintNumS(position, 11, value.second);
    } else {
        LcdPrintNumS(position, 10, value.second);
    }
}

void display_stopwatch() {
    LcdPrintStringS(0, 0, "   STOP WATCH  ");
    LcdPrintStringS(1, 0, "  ");
    if (hour_st < 10) {
        LcdPrintStringS(1, 4, "0");
        LcdPrintNumS(1, 5, hour_st);
    } else
        LcdPrintNumS(1, 4, hour_st);
    LcdPrintStringS(1, 6, ":");
    if (minute_st < 10) {
        LcdPrintStringS(1, 7, "0");
        LcdPrintNumS(1, 8, minute_st);
    } else
        LcdPrintNumS(1, 7, minute_st);
    LcdPrintStringS(1, 9, ":");
    if (second_st < 10) {
        LcdPrintStringS(1, 10, "0");
        LcdPrintNumS(1, 11, second_st);
    } else
        LcdPrintNumS(1, 10, second_st);
}

void display_timer_clock() {
    LcdPrintStringS(0, 0, "TIMER CLOCK BACK");
    LcdPrintStringS(1, 0, "  ");
    if (run_stop) {
        LcdPrintStringS(1, 12, "STOP");
    } else LcdPrintStringS(1, 12, "RUN");
    if (hour_tc < 10) {
        LcdPrintStringS(1, 1, "0");
        LcdPrintNumS(1, 2, hour_tc);
    } else
        LcdPrintNumS(1, 1, hour_tc);
    LcdPrintStringS(1, 3, ":");
    if (minute_tc < 10) {
        LcdPrintStringS(1, 4, "0");
        LcdPrintNumS(1, 5, minute_tc);
    } else
        LcdPrintNumS(1, 4, minute_tc);
    LcdPrintStringS(1, 6, ":");
    if (second_tc < 10) {
        LcdPrintStringS(1, 7, "0");
        LcdPrintNumS(1, 8, second_tc);
    } else
        LcdPrintNumS(1, 7, second_tc);
}

void run_timer_clock() {
    switch (timer_clock_config) {
        case 0: //modify hour
            blink = (blink + 1) % 20;
            if (blink > 15)
                LcdPrintStringS(1, 1, "  ");
            if (KEYUP > DEBOUNCE_THRS) {
                hour_tc += 1;
                if (hour_tc > 23)
                    hour_tc = 0;
            }
            if (KEYDOWN > DEBOUNCE_THRS) {
                hour_tc -= 1;
                if (hour_tc < 0)
                    hour_tc = 23;
            }
            if (KEYUP_HOLD > DEBOUNCE_THRS) {
                hour_tc += 5;
                if (hour_tc > 23)
                    hour_tc = 0;
            }
            if (KEYDOWN_HOLD > DEBOUNCE_THRS) {
                hour_tc -= 5;
                if (hour_tc < 0)
                    hour_tc = 23;
            }
            if (KEYOK > DEBOUNCE_THRS) {
                timer_clock_config = 1;
            }
            break;
        case 1: //modify minute
            blink = (blink + 1) % 20;
            if (blink > 15)
                LcdPrintStringS(1, 4, "  ");
            if (KEYUP > DEBOUNCE_THRS) {
                minute_tc += 1;
                if (minute_tc > 59)
                    minute_tc = 0;
            }
            if (KEYDOWN > DEBOUNCE_THRS) {
                minute_tc -= 1;
                if (minute_tc < 0) {
                    minute_tc = 59;
                }
            }
            if (KEYUP_HOLD > DEBOUNCE_THRS) {
                minute_tc += 20;
                if (minute_tc > 59)
                    minute_tc = 0;
            }
            if (KEYDOWN_HOLD > DEBOUNCE_THRS) {
                minute_tc -= 20;
                if (minute_tc < 0) {
                    minute_tc = 59;
                }
            }
            if (KEYOK > DEBOUNCE_THRS) {
                timer_clock_config = 2;
            }
            break;
        case 2: //modify second
            blink = (blink + 1) % 20;
            if (blink > 15)
                LcdPrintStringS(1, 7, "  ");
            if (KEYUP > DEBOUNCE_THRS) {
                second_tc += 1;
                if (second_tc > 59)
                    second_tc = 0;
            }
            if (KEYDOWN > DEBOUNCE_THRS) {
                second_tc -= 1;
                if (second_tc < 0) {
                    second_tc = 59;
                }
            }
            if (KEYUP_HOLD > DEBOUNCE_THRS) {
                second_tc += 20;
                if (second_tc > 59)
                    second_tc = 0;
            }
            if (KEYDOWN_HOLD > DEBOUNCE_THRS) {
                second_tc -= 20;
                if (second_tc < 0) {
                    second_tc = 59;
                }
            }
            if (KEYOK > DEBOUNCE_THRS) {
                timer_clock_config = 3;
            }
            break;
        case 3: //run timer clock
            if (KEYUP > DEBOUNCE_THRS) {
                run_stop = 1;
                last_sec = Read_DS1307(ADDRESS_SECOND);
                curr_sec = Read_DS1307(ADDRESS_SECOND);
            }
            if (KEYDOWN > DEBOUNCE_THRS) {
                run_stop = 0;
            }
            if (run_stop) {
                curr_sec = Read_DS1307(ADDRESS_SECOND);
                if (curr_sec == last_sec + 1 || (last_sec == 59 && curr_sec == 0)) {
                    last_sec = curr_sec;
                    if (second_tc > 0) {
                        second_tc--;
                    } else if (second_tc == 0) {
                        if (minute_tc != 0) {
                            second_tc = 59;
                            minute_tc--;
                        } else if (minute_tc == 0) {
                            if (hour_tc != 0) {
                                hour_tc--;
                                minute_tc = 59;
                                second_tc = 59;
                            }
                        }
                    }
                }
            }
            if (hour_tc == 0 && minute_tc == 0 && second_tc == 0) {
                run_stop = 0;
            }
            if (!run_stop) {
                blink = (blink + 1) % 20;
                if (blink > 15)
                    LcdPrintStringS(1, 12, "    ");
            }
            if (KEYOK > DEBOUNCE_THRS) {
                run_stop = 0;
                timer_clock_config = 4;
            }
            break;
        case 4: //back to menu
            blink = (blink + 1) % 20;
            if (blink > 15)
                LcdPrintStringS(0, 12, "    ");
            if (KEYUP || KEYDOWN) {
                timer_clock_config = 0;
            } else if (KEYOK > DEBOUNCE_THRS) {
                status = TIMER_CLOCK;
                hour_tc = 0;
                minute_tc = 0;
                second_tc = 0;
                timer_clock_config = NULL;
            }
            break;
        default:
            break;
    }
}

unsigned char getUartBufferChar() {
    unsigned char c = recvBuff[readIdx];
    readIdx = (readIdx + 1) % SERIAL_BUFFER_SIZE;
    return c;
}

void flushRecvBuff() {
    writeIdx = 0;
    readIdx = 0;
    recvDataFlag = 0;
}

void uart_console() {
    switch (console_state) {
        case CONSOLE_WAIT:
            if (recvDataFlag > 0) {
                command_parse();
                recvDataFlag--;
            }
            break;
        default:
            break;
    }
}

void command_parse() {
    char c = getUartBufferChar();
    uart_putchar(c);
    //for cmd !TIME:xxx#
    switch (cmd_state) {
        case CMD_INIT:
            if (c == '!') cmd_state = CMD_START;
            break;

        case CMD_START:
            if (c == 'T') cmd_state = CMD_TIME1;
            else if (c == '!') cmd_state = CMD_START;
            else cmd_state = CMD_INIT;
            break;

        case CMD_TIME1:
            if (c == 'I') cmd_state = CMD_TIME2;
            else if (c == '!') cmd_state = CMD_START;
            else cmd_state = CMD_INIT;
            break;

        case CMD_TIME2:
            if (c == 'M') cmd_state = CMD_TIME3;
            else if (c == '!') cmd_state = CMD_START;
            else cmd_state = CMD_INIT;
            break;

        case CMD_TIME3:
            if (c == 'E') cmd_state = CMD_TIME4;
            else if (c == '!') cmd_state = CMD_START;
            else cmd_state = CMD_INIT;
            break;

        case CMD_TIME4:
            if (c == ':') cmd_state = HOUR_1;
            else if (c == '!') cmd_state = CMD_START;
            else cmd_state = CMD_INIT;
            break;
            //already receive !TIME:, then we only accept number and #
            //anything else will return error immediately
        case HOUR_1: //first value of hour
            uartBuff = check_number_uart(c);
            if (uartBuff != -1) {
                hour_t = uartBuff * 10;
                cmd_state = HOUR_2;
            } else {
                UartSendString("\r\nPlease enter number!\r\n");
                cmd_state = CMD_INIT;
            }
            break;
        case HOUR_2: //second value of hour
            uartBuff = check_number_uart(c);
            if (uartBuff != -1) {
                hour_t += uartBuff;
                //validate hour
                if (validate(hour_t, 23)) {
                    cmd_state = MINUTE_1;
                } else {
                    UartSendString("\r\nHour must be in range [0,23] !\r\n");
                    UartSendString("please enter again!\r\n");
                    cmd_state = CMD_INIT;
                }
            } else {
                UartSendString("\r\nPlease enter number!\r\n");
                cmd_state = CMD_INIT;
            }
            break;
            //already receive !TIME:HH
        case MINUTE_1: //first value of minute
            uartBuff = check_number_uart(c);
            if (uartBuff != -1) {
                minute_t = uartBuff * 10;
                cmd_state = MINUTE_2;
            } else {
                UartSendString("\r\nPlease enter number!\r\n");
                cmd_state = CMD_INIT;
            }
            break;
        case MINUTE_2: //second value of minute
            uartBuff = check_number_uart(c);
            if (uartBuff != -1) {
                minute_t += uartBuff;
                //validate minute
                if (validate(minute_t, 59)) {
                    cmd_state = SECOND_1;
                } else {
                    UartSendString("\r\nMinute must be in range [0,59] !\r\n");
                    UartSendString("please enter again!\r\n");
                    cmd_state = CMD_INIT;
                }
            } else {
                UartSendString("\r\nPlease enter number!\r\n");
                cmd_state = CMD_INIT;
            }
            break;
            //already receive !TIME:HHMM
        case SECOND_1: //first value of second
            uartBuff = check_number_uart(c);
            if (uartBuff != -1) {
                second_t = uartBuff * 10;
                cmd_state = SECOND_2;
            } else {
                UartSendString("\r\nPlease enter number!\r\n");
                cmd_state = CMD_INIT;
            }
            break;
        case SECOND_2: //second value of second
            uartBuff = check_number_uart(c);
            if (uartBuff != -1) {
                second_t += uartBuff;
                //validate second
                if (validate(second_t, 59))
                    cmd_state = SUCCESS;
                else {
                    UartSendString("\r\nSecond must be in range [0,59] !\r\n");
                    UartSendString("please enter again!\r\n");
                    cmd_state = CMD_INIT;
                }
            } else {
                UartSendString("\r\nPlease enter number !\r\n");
                cmd_state = CMD_INIT;
            }
            break;
            //already receive !TIME:HHMSS just one char left is #
        case SUCCESS:
            if (c == '#') {
                cmd_state = CMD_INIT;
                Write_DS1307(ADDRESS_HOUR, hour_t);
                Write_DS1307(ADDRESS_MINUTE, minute_t);
                write_ds1307(ADDRESS_SECOND, second_t);
                if (flagUart) {
                    UartSendString("\r\nConfiguration Time Success!\r\n");
                }
            } else {
                UartSendString("\r\nNeed confirm time !\r\n");
                UartSendString("\r\nCharacter confirm is '#'\r\n");
                UartSendString("\r\nPlease enter again !\r\n");
                cmd_state = CMD_INIT;
            }
            break;
        default:
            break;

    }

    //for cmd !DATE:xxx#
    switch (cmd_date) {
        case CMD_INIT:
            if (c == '!') cmd_date = CMD_START;
            break;

        case CMD_START:
            if (c == 'D') cmd_date = CMD_DATE1;
            else if (c == '!') cmd_date = CMD_START;
            else cmd_date = CMD_INIT;
            break;

        case CMD_DATE1:
            if (c == 'A') cmd_date = CMD_DATE2;
            else if (c == '!') cmd_date = CMD_START;
            else cmd_date = CMD_INIT;
            break;

        case CMD_DATE2:
            if (c == 'T') cmd_date = CMD_DATE3;
            else if (c == '!') cmd_date = CMD_START;
            else cmd_date = CMD_INIT;
            break;

        case CMD_DATE3:
            if (c == 'E') cmd_date = CMD_DATE4;
            else if (c == '!') cmd_date = CMD_START;
            else cmd_date = CMD_INIT;
            break;

        case CMD_DATE4:
            if (c == ':') {
                cmd_date = YEAR_1;
            } else if (c == '!') cmd_date = CMD_START;
            else cmd_date = CMD_INIT;
            break;
            //already receive !DATE:, then we only accept number and #
            //anything else will return error immediately
        case YEAR_1: //firt number of year
            uartBuffDate = check_number_uart(c);
            if (uartBuffDate != -1) {
                year_t = uartBuffDate * 10;
                cmd_date = YEAR_2;
            } else {
                UartSendString("\r\nPlease enter number!\r\n");
                cmd_date = CMD_INIT;
            }
            break;
        case YEAR_2: //second number of year
            uartBuffDate = check_number_uart(c);
            if (uartBuffDate != -1) {
                year_t += uartBuffDate;
                cmd_date = MONTH_1;
            } else {
                UartSendString("\r\nPlease enter number!\r\n");
                cmd_date = CMD_INIT;
            }
            break;
            //already receive !DATE:YY
        case MONTH_1: //first number of month
            uartBuffDate = check_number_uart(c);
            if (uartBuffDate != -1) {
                month_t = uartBuffDate * 10;
                cmd_date = MONTH_2;
            } else {
                UartSendString("\r\nPlease enter number!\r\n");
                cmd_date = CMD_INIT;
            }
            break;
        case MONTH_2: //second number of month
            uartBuffDate = check_number_uart(c);
            if (uartBuffDate != -1) {
                month_t += uartBuffDate;
                //validate month
                if (validate(month_t, 12) && month_t != 0)
                    cmd_date = DATE_1;
                else {
                    UartSendString("\r\nMonth must be in range [1,12] !\r\n");
                    UartSendString("please enter again!\r\n");
                    cmd_date = CMD_INIT;
                }
            } else {
                UartSendString("\r\nPlease enter number !\r\n");
                cmd_date = CMD_INIT;
            }
            break;
            //already receive !DATE:YYMM
        case DATE_1: //first number of date
            uartBuffDate = check_number_uart(c);
            if (uartBuffDate != -1) {
                date_t = uartBuffDate * 10;
                cmd_date = DATE_2;
            } else {
                UartSendString("\r\nPlease enter number !\r\n");
                cmd_date = CMD_INIT;
            }
            break;
        case DATE_2: //second number of date
            uartBuffDate = check_number_uart(c);
            if (uartBuffDate != -1) {
                date_t += uartBuffDate;
                //validate date
                if (date_t == 0) {
                    UartSendString("\r\n Day must be > 0!\r\n");
                } else if (check_month(month_t) == 0 && date_t > 30) {
                    UartSendString("\r\nDay must be in range [1,30] !\r\n");
                    UartSendString("Please enter again!\r\n");
                    cmd_date = CMD_INIT;
                } else if (check_month(month_t) == 1 && date_t > 31) {
                    UartSendString("\r\nDay must be in range [1,31] !\r\n");
                    UartSendString("Please enter again!\r\n");
                    cmd_date = CMD_INIT;
                } else if (check_month(month_t) == 2 && check_year(year_t) && date_t > 29) {
                    UartSendString("\r\nDay must be in range [1,29] !\r\n");
                    UartSendString("Please enter again!\r\n");
                    cmd_date = CMD_INIT;
                } else if (check_month(month_t) == 2 && !check_year(year_t) && date_t > 28) {
                    UartSendString("\r\nDay must be in range [1,28] !\r\n");
                    UartSendString("Please enter again!\r\n");
                    cmd_date = CMD_INIT;
                } else {
                    cmd_date = DAY;
                }
            } else {
                UartSendString("\r\nPlease enter number !\r\n");
                cmd_date = CMD_INIT;
            }
            break;
            //already receive !DATE:YYMMDD
        case DAY: //receive day value
            uartBuffDate = check_number_uart(c);
            if (uartBuffDate != -1) {
                day_t = uartBuffDate;
                if (validate(day_t, 7) && day_t != 0)
                    cmd_date = SUCCESS;
                else {
                    //validate day
                    UartSendString("\r\nDATE must be in range [1,7] !\r\n");
                    UartSendString("Please enter again!\r\n");
                    cmd_date = CMD_INIT;
                }
            } else {
                UartSendString("\r\nPlease enter number !\r\n");
                cmd_date = CMD_INIT;
            }
            break;
            //already receive !DATE:YYMMDDd just one char left is #
        case SUCCESS:
            if (c == '#') {
                cmd_date = CMD_INIT;
                Write_DS1307(ADDRESS_YEAR, year_t);
                Write_DS1307(ADDRESS_MONTH, month_t);
                Write_DS1307(ADDRESS_DATE, date_t);
                Write_DS1307(ADDRESS_DAY, day_t);
                if (flagUart) {
                    UartSendString("\r\nConfiguration Date Success!\r\n");
                }
                flagInternet = 1;
            } else {
                UartSendString("\r\nNeed confirm date change !\r\n");
                UartSendString("\r\nCharacter confirm is '#'\r\n");
                UartSendString("\r\nPlease enter again !\r\n");
                cmd_state = CMD_INIT;
            }
            break;
        default:
            break;

    }
    //for dht cmd
    switch (dht_state) {
        case CMD_INIT:
            if (c == '!') dht_state = CMD_START;
            break;

        case CMD_START:
            if (c == 'D') dht_state = CMD_DHT1;
            else if (c == '!') dht_state = CMD_START;
            else dht_state = CMD_INIT;
            break;

        case CMD_DHT1:
            if (c == 'H') dht_state = CMD_DHT2;
            else if (c == '!') dht_state = CMD_START;
            else dht_state = CMD_INIT;
            break;

        case CMD_DHT2:
            if (c == 'T') dht_state = CMD_DHT3;
            else if (c == '!') dht_state = CMD_START;
            else dht_state = CMD_INIT;
            break;

        case CMD_DHT3:
            if (c == ':') dht_state = TEMP1;
            else if (c == '!') dht_state = CMD_START;
            else dht_state = CMD_INIT;
            break;
        case TEMP1:
            dhtBuffer = check_number_uart(c);
            if (dhtBuffer != -1) {
                temperature = dhtBuffer * 10;
                dht_state = TEMP2;
            } else {
                dht_state = CMD_INIT;
                UartSendString("Read error !\r\n");
            }
            break;
        case TEMP2:
            dhtBuffer = check_number_uart(c);
            if (dhtBuffer != -1) {
                temperature += dhtBuffer;
                dht_state = HUMI1;
            } else {
                dht_state = CMD_INIT;
                UartSendString("Read error !\r\n");
            }
            break;
        case HUMI1:
            dhtBuffer = check_number_uart(c);
            if (dhtBuffer != -1) {
                humidity = dhtBuffer * 10;
                dht_state = HUMI2;
            } else {
                dht_state = CMD_INIT;
                UartSendString("Read error !\r\n");
            }
            break;
        case HUMI2:
            dhtBuffer = check_number_uart(c);
            if (dhtBuffer != -1) {
                humidity += dhtBuffer;
                dht_state = SUCCESS;
            } else {
                dht_state = CMD_INIT;
                UartSendString("Read error !\r\n");
            }
            break;
        case SUCCESS:
            if (c == '#') {
                status = WEATHER_DISPLAY;
                flagDHT = 1;
                dht_state = CMD_INIT;
            } else {
                UartSendString("Confirm Error!\r\n");
                dht_state = CMD_INIT;
            }
            break;
        default:
            break;
    }
}

void fsm_timeModify() {
    switch (modify) {
        case 0: //modify by use UART
            LcdPrintStringS(0, 0, ">Use UART");
            LcdPrintStringS(1, 0, " Use Button");
            if (KEYOK > DEBOUNCE_THRS) {
                modify = 4;
                uart_mode = 0;
                uart_last_mode = 0;
            }
            if (KEYDOWN > DEBOUNCE_THRS) {
                modify = 1;
                last_modify = 0;
            }
            if (KEYUP > DEBOUNCE_THRS) {
                modify = BACK;
            }
            break;
        case 1: //modify by use Button
            if (last_modify) {
                LcdPrintStringS(0, 0, ">Use Button");
                LcdPrintStringS(1, 0, " Back");
            } else {
                LcdPrintStringS(0, 0, " Use UART");
                LcdPrintStringS(1, 0, ">Use Button");
            }
            if (KEYDOWN > DEBOUNCE_THRS) {
                modify = BACK;
            }
            if (KEYUP > DEBOUNCE_THRS) {
                modify = 0;
            }
            if (KEYOK > DEBOUNCE_THRS) {
                modify = 3;
                returnOK = 1;
                second_mf = Read_DS1307(ADDRESS_SECOND);
                minute_mf = Read_DS1307(ADDRESS_MINUTE);
                hour_mf = Read_DS1307(ADDRESS_HOUR);
                day_mf = Read_DS1307(ADDRESS_DAY);
                date_mf = Read_DS1307(ADDRESS_DATE);
                month_mf = Read_DS1307(ADDRESS_MONTH);
                year_mf = Read_DS1307(ADDRESS_YEAR);
            }
            break;
        case 3: //display lcd to modify in mode use button
            DisplayTimeForModify();
            SetUpTime();
            if (setTimeFlag) {
                modify = 0;
                setTimeFlag = 0;
                status = TIME_SCREEN;
            }
            break;
        case 4:
            fsm_uart_mode();
            break;
        case BACK: //back to menu
            LcdPrintStringS(0, 0, " Use Button");
            LcdPrintStringS(1, 0, ">Back");
            if (KEYOK > DEBOUNCE_THRS) {
                status = TIME_MODIFY;
            }
            if (KEYDOWN > DEBOUNCE_THRS) {
                modify = 0;
            }
            if (KEYUP > DEBOUNCE_THRS) {
                modify = 1;
                last_modify = 1;
            }
            break;
        default:
            break;
    }
}

void fsm_uart_mode() {
    switch (uart_mode) {
        case 0:
            LcdPrintStringS(0, 0, ">VirtualTerminal");
            LcdPrintStringS(1, 0, " Internet");
            if (KEYOK > DEBOUNCE_THRS) {
                //send guide to user
                UartSendString("\r\nWelcome to Alarm Clock console memu\r\n");
                UartSendString("\r\nTo synchronize time, using syntax as follow:\r\n");
                UartSendString("!TIME:HHMMSS#\r\n");
                UartSendString("The time need to be 24-h format\r\n");
                UartSendString("\r\nTo synchronize date, using syntax as follow:\r\n");
                UartSendString("!DATE:YYMMDDd#\r\n");
                writeIdx = 0;
                readIdx = 0;
                recvDataFlag = 0;
                flagUart = 1;
                status = UART_CONSOLE;
            }
            if (KEYUP > DEBOUNCE_THRS) {
                uart_mode = 2;
                uart_last_mode = 0;
            }
            if (KEYDOWN > DEBOUNCE_THRS) {
                uart_mode = 1;
                uart_last_mode = 0;
            }
            break;
        case 1:
            if (uart_last_mode == 0) {
                LcdPrintStringS(0, 0, " VirtualTerminal");
                LcdPrintStringS(1, 0, ">Internet");
            } else {
                LcdPrintStringS(0, 0, ">Internet");
                LcdPrintStringS(1, 0, " Back");
            }
            if (KEYOK > DEBOUNCE_THRS) {
                UartSendString("!TIME#\r\n");
                flagUart = 0;
                writeIdx = 0;
                readIdx = 0;
                recvDataFlag = 0;
                status = UART_CONSOLE;
                zone = 7;
                flagInternet = 0;
            }
            if (KEYUP > DEBOUNCE_THRS) {
                uart_mode = 0;
                uart_last_mode = 1;
            }
            if (KEYDOWN > DEBOUNCE_THRS) {
                uart_mode = 2;
                uart_last_mode = 1;
            }
            break;
        case 2:
            LcdPrintStringS(0, 0, " Internet");
            LcdPrintStringS(1, 0, ">Back");
            if (KEYOK > DEBOUNCE_THRS) {
                status = TIME_MODIFY;
            }
            if (KEYUP > DEBOUNCE_THRS) {
                uart_mode = 1;
                uart_last_mode = 2;
            }
            if (KEYDOWN > DEBOUNCE_THRS) {
                uart_mode = 0;
                uart_last_mode = 2;
            }
            break;
        default:
            break;
    }
}

void fsm_worldTime() {
    switch (status_worldTime) {
        case TOKYO:
            LcdPrintStringS(0, 0, ">TOKYO    UTC+09");
            LcdPrintStringS(1, 0, " BEIJING  UTC+08");
            if (KEYDOWN > DEBOUNCE_THRS) {
                last_statusWT = TOKYO;
                status_worldTime = BEIJING;
            }
            if (KEYUP > DEBOUNCE_THRS) {
                status_worldTime = BACK;
            }
            change_UTC(9);
            break;
        case BEIJING:
            if (last_statusWT == TOKYO) {
                LcdPrintStringS(0, 0, " TOKYO    UTC+09");
                LcdPrintStringS(1, 0, ">BEIJING  UTC+08");
            } else {
                LcdPrintStringS(0, 0, ">BEIJING  UTC+08");
                LcdPrintStringS(1, 0, " PARIS    UTC+01");
            }
            if (KEYDOWN > DEBOUNCE_THRS) {
                last_statusWT = BEIJING;
                status_worldTime = PARIS;
            }
            if (KEYUP > DEBOUNCE_THRS) {
                last_statusWT = BEIJING;
                status_worldTime = TOKYO;
            }
            change_UTC(8);
            break;
        case PARIS:
            if (last_statusWT == BEIJING) {
                LcdPrintStringS(0, 0, " BEIJING  UTC+08");
                LcdPrintStringS(1, 0, ">PARIS    UTC+01");
            } else {
                LcdPrintStringS(0, 0, ">PARIS    UTC+01");
                LcdPrintStringS(1, 0, " NEWYORK  UTC-05");
            }
            if (KEYDOWN > DEBOUNCE_THRS) {
                last_statusWT = PARIS;
                status_worldTime = NEWYORK;
            }
            if (KEYUP > DEBOUNCE_THRS) {
                last_statusWT = PARIS;
                status_worldTime = BEIJING;
            }
            change_UTC(1);
            break;
        case NEWYORK:
            if (last_statusWT == PARIS) {
                LcdPrintStringS(0, 0, " PARIS    UTC+01");
                LcdPrintStringS(1, 0, ">NEWYORK  UTC-05");
            } else {
                LcdPrintStringS(0, 0, ">NEWYORK  UTC-05");
                LcdPrintStringS(1, 0, " LONDON   UTC+00");
            }
            if (KEYDOWN > DEBOUNCE_THRS) {
                last_statusWT = NEWYORK;
                status_worldTime = LONDON;
            }
            if (KEYUP > DEBOUNCE_THRS) {
                last_statusWT = NEWYORK;
                status_worldTime = PARIS;
            }
            change_UTC(-5);
            break;
        case LONDON:
            if (last_statusWT == NEWYORK) {
                LcdPrintStringS(0, 0, " NEWYORK  UTC-05");
                LcdPrintStringS(1, 0, ">LONDON   UTC+00");
            } else {
                LcdPrintStringS(0, 0, ">LONDON   UTC+00");
                LcdPrintStringS(1, 0, " HANOI    UTC+07");
            }
            if (KEYDOWN > DEBOUNCE_THRS) {
                last_statusWT = LONDON;
                status_worldTime = HANOI;
            }
            if (KEYUP > DEBOUNCE_THRS) {
                last_statusWT = LONDON;
                status_worldTime = NEWYORK;
            }
            change_UTC(0);
            break;
        case HANOI:
            if (last_statusWT == LONDON) {
                LcdPrintStringS(0, 0, " LONDON   UTC+00");
                LcdPrintStringS(1, 0, ">HANOI    UTC+07");
            } else {
                LcdPrintStringS(0, 0, ">HANOI    UTC+07");
                LcdPrintStringS(1, 0, " BACK");
            }
            if (KEYDOWN > DEBOUNCE_THRS) {
                last_statusWT = HANOI;
                status_worldTime = BACK;
            }
            if (KEYUP > DEBOUNCE_THRS) {
                last_statusWT = HANOI;
                status_worldTime = LONDON;
            }
            change_UTC(7);
            break;
        case BACK:
            LcdPrintStringS(0, 0, " HANOI    UTC+07");
            LcdPrintStringS(1, 0, ">BACK");
            if (KEYUP > DEBOUNCE_THRS) {
                status_worldTime = HANOI;
                last_statusWT = BACK;
            }
            if (KEYDOWN > DEBOUNCE_THRS) {
                status_worldTime = TOKYO;
                last_statusWT = BACK;
            }
            if (KEYOK > DEBOUNCE_THRS) {
                status = WORLD_TIME;
            }
            break;
        default:
            break;
    }
}
//chage current time zone

void change_UTC(int value) {
    if (KEYOK > DEBOUNCE_THRS && zone != value) {
        //current time zone difference with time zone we want to change
        UTC = value - zone;
        zone = value;
        hour_change = Read_DS1307(ADDRESS_HOUR);
        hour_change = (hour_change + UTC);
        if (hour_change < 0) {
            hour_change = 24 + hour_change;
            check_day(0);
        } else if (hour_change > 23) {
            hour_change = hour_change - 24;
            check_day(1);
        }
        Write_DS1307(ADDRESS_HOUR, hour_change);
        status = TIME_SCREEN;
        status_worldTime = WAIT;
    } else if (KEYOK > DEBOUNCE_THRS) {
        //current time zone equal to time zone we want to change
        //do nothing
        status = TIME_SCREEN;
    }
}
//check current time zone

void check_UTC() {
    switch (zone) {
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
            status_worldTime = HANOI;
            break;
        default:
            break;
    }
}
//return number of char else return -1

int check_number_uart(char str) {
    if (str >= '0' && str <= '9') {
        return str - '0';
    } else return -1;
}

int validate(int number, int up) {
    if (number > up)
        return 0;
    return 1;
}
//check month have 31, 30 day or this month is 2

int check_month(int month) {
    if (month == 2) {
        return 2; //current month is 2
    }
    if (month == 4 || month == 6 || month == 9 || month == 11)
        return 0; //month has 31 days
    return 1; //month has 30 days
}
unsigned char check_year(int year){
    if((year % 4 == 0 && year % 100 != 0) || year % 400 == 0){
        return 1;
    }
    return 0;
}
//increase the date if time zone change causes

unsigned char check_day(unsigned char mode) {
    //read current value form DS 1307
    unsigned char tmp_date = Read_DS1307(ADDRESS_DATE);
    unsigned char tmp_day = Read_DS1307(ADDRESS_DAY);
    unsigned char tmp_month = Read_DS1307(ADDRESS_MONTH);
    unsigned char tmp_year = Read_DS1307(ADDRESS_YEAR);
    if (!mode) { //decrease date
        tmp_date -= 1;
        tmp_day -= 1;
        if (tmp_day < 1) tmp_day = 7;
        if (tmp_date < 1) {
            if (tmp_month == 1) { //change date leads to change month, year
                tmp_date = 31;
                tmp_month = 12;
                tmp_year -= 1;
            }//change date leads to change month
            else if (check_month(tmp_month) == 0) {
                //previous month has 31 days
                tmp_month -= 1;
                tmp_date = 31;
            } else if (check_month(tmp_month) == 1 && tmp_month != 3) {
                //previous month has 30 days
                tmp_month = 1;
                tmp_date = 30;
            } else if (check_month(tmp_month) == 1) {
                //previous month is 2 so has two case
                if (tmp_year % 4 == 0) {
                    //has 29 days
                    tmp_date = 29;
                } else tmp_date = 28; //has 28days
            }
        }
    } else { //increase date
        tmp_date += 1;
        tmp_day += 1;
        if (tmp_day > 7) tmp_day = 1;
        //increase day leads to change month, year
        if (tmp_month == 12 && tmp_date > 31) {
            tmp_month = 1;
            tmp_date = 1;
            tmp_year += 1;
        }//increase day lead to change month
        else if (check_month(tmp_month) == 0 && tmp_date > 30) {
            //current month has 30 days
            tmp_month += 1;
            tmp_date = 1;
        } else if (check_month(tmp_month) == 1 && tmp_date > 31) {
            //current month has 31 days
            tmp_month += 1;
            tmp_date = 1;
        } else if (check_month(tmp_month) == 2 && tmp_year % 4 == 0 && tmp_date > 29) {
            //current month has 29 days
            tmp_month += 1;
            tmp_date = 1;
        } else if (check_month(tmp_month) == 2 && tmp_year % 4 != 0 && tmp_date > 28) {
            //current month has 28 days
            tmp_month += 1;
            tmp_date = 1;
        }
    }
    //write to DS 1307
    Write_DS1307(ADDRESS_DATE, tmp_date);
    Write_DS1307(ADDRESS_DAY, tmp_day);
    Write_DS1307(ADDRESS_MONTH, tmp_month);
    Write_DS1307(ADDRESS_YEAR, tmp_year);
}

void DisplayTimeForModify() {
    //////day_mf
    switch (day_mf) {
        case 1:
            LcdPrintStringS(0, 0, "SUN");
            break;
        case 2:
            LcdPrintStringS(0, 0, "MON");
            break;
        case 3:
            LcdPrintStringS(0, 0, "TUE");
            break;
        case 4:
            LcdPrintStringS(0, 0, "WED");
            break;
        case 5:
            LcdPrintStringS(0, 0, "THU");
            break;
        case 6:
            LcdPrintStringS(0, 0, "FRI");
            break;
        case 7:
            LcdPrintStringS(0, 0, "SAT");
            break;
    }
    if (hour_mf < 10) {
        LcdPrintStringS(0, 4, "0");
        LcdPrintNumS(0, 5, hour_mf);
    } else
        LcdPrintNumS(0, 4, hour_mf);

    LcdPrintStringS(0, 6, ":");
    if (minute_mf < 10) {
        LcdPrintStringS(0, 7, "0");
        LcdPrintNumS(0, 8, minute_mf);
    } else
        LcdPrintNumS(0, 7, minute_mf);

    LcdPrintStringS(0, 9, ":");
    if (second_mf < 10) {
        LcdPrintStringS(0, 10, "0");
        LcdPrintNumS(0, 11, second_mf);
    } else
        LcdPrintNumS(0, 10, second_mf);


    if (returnOK == 1) {
        LcdPrintStringS(0, 13, "OK ");
    } else if (returnOK == 2) {
        LcdPrintStringS(0, 12, "EXIT");
    }

    switch (month_mf) {
        case 1:
            LcdPrintStringS(1, 2, "JAN");
            break;
        case 2:
            LcdPrintStringS(1, 2, "FEB");
            break;
        case 3:
            LcdPrintStringS(1, 2, "MAR");
            break;
        case 4:
            LcdPrintStringS(1, 2, "APR");
            break;
        case 5:
            LcdPrintStringS(1, 2, "MAY");
            break;
        case 6:
            LcdPrintStringS(1, 2, "JUN");
            break;
        case 7:
            LcdPrintStringS(1, 2, "JUL");
            break;
        case 8:
            LcdPrintStringS(1, 2, "AUG");
            break;
        case 9:
            LcdPrintStringS(1, 2, "SEP");
            break;
        case 10:
            LcdPrintStringS(1, 2, "OCT");
            break;
        case 11:
            LcdPrintStringS(1, 2, "NOV");
            break;
        case 12:
            LcdPrintStringS(1, 2, "DEC");
            break;
    }

    LcdPrintStringS(1, 5, " ");
    if (date_mf < 10) {
        LcdPrintStringS(1, 6, " ");
        LcdPrintNumS(1, 7, date_mf);
    } else
        LcdPrintNumS(1, 6, date_mf);
    LcdPrintStringS(1, 8, " ");
    LcdPrintNumS(1, 9, 20);
    if (year_mf < 10) {
        LcdPrintNumS(1, 11, 0);
        LcdPrintNumS(1, 12, year_mf);
    } else LcdPrintNumS(1, 11, year_mf);
}

void SetUpTime() {
    switch (statusSetUpTime) {
        case INIT_SYSTEM:
            if (KEYOK && (bitEnable_1 == ENABLE))
                statusSetUpTime = SET_HOUR;
            break;
        case SET_HOUR:
            bitEnable_1 = DISABLE;
            timeBlink_1 = (timeBlink_1 + 1) % 20;
            if (timeBlink_1 > 15)
                LcdPrintStringS(0, 4, "  ");
            if (KEYUP) {
                hour_mf = (hour_mf + 1) % 24;
                //                Write_DS1307(ADDRESS_HOUR, hour);
            }
            if (KEYDOWN) {
                hour_mf = (hour_mf - 1);
                if (hour_mf > 23)
                    hour_mf = 23;
                //                Write_DS1307(ADDRESS_HOUR, hour);
            }
            if (KEYUP_HOLD) {
                hour_mf = (hour_mf + 5) % 24;
                //                Write_DS1307(ADDRESS_HOUR, hour);
            }
            if (KEYDOWN_HOLD) {
                hour_mf = (hour_mf - 5);
                if (hour_mf > 23)
                    hour_mf = 23;
                //                Write_DS1307(ADDRESS_HOUR, hour);
            }
            if (KEYOK)
                statusSetUpTime = SET_MINUTE;
            if (KEYOK_HOLD) {
                LcdClearS();
                bitEnable_1 = ENABLE;
                statusSetUpTime = BACK_MENU;
            }
            break;
        case SET_MINUTE:
            bitEnable_1 = DISABLE;
            timeBlink_1 = (timeBlink_1 + 1) % 20;
            if (timeBlink_1 > 15)
                LcdPrintStringS(0, 7, "  ");
            if (KEYUP) {
                minute_mf = (minute_mf + 1) % 60;
                //                Write_DS1307(ADDRESS_MINUTE, minute);
            }
            if (KEYDOWN) {
                minute_mf = (minute_mf - 1);
                if (minute_mf > 59)
                    minute_mf = 59;
                //                Write_DS1307(ADDRESS_MINUTE, minute);
            }
            if (KEYUP_HOLD) {
                minute_mf = (minute_mf + 20) % 60;
                //                Write_DS1307(ADDRESS_MINUTE, minute);
            }
            if (KEYDOWN_HOLD) {
                minute_mf = (minute_mf - 20);
                if (minute_mf > 59)
                    minute_mf = 59;
                //                Write_DS1307(ADDRESS_MINUTE, minute);
            }
            if (KEYOK)
                statusSetUpTime = SET_DAY;
            if (KEYOK_HOLD) {
                LcdClearS();
                bitEnable_1 = ENABLE;
                statusSetUpTime = BACK_MENU;
            }
            break;
        case SET_DAY:
            bitEnable_1 = DISABLE;
            timeBlink_1 = (timeBlink_1 + 1) % 20;
            if (timeBlink_1 > 15)
                LcdPrintStringS(0, 0, "   ");
            if (KEYUP) {
                day_mf = day_mf + 1;
                if (day_mf > 7)
                    day_mf = 1;
                //                Write_DS1307(ADDRESS_DAY, day);
            }
            if (KEYDOWN) {
                day_mf = day_mf - 1;
                if (day_mf > 7)
                    day_mf = 7;
                //                Write_DS1307(ADDRESS_DAY, day);
            }
            if (KEYOK)
                statusSetUpTime = SET_DATE;
            if (KEYOK_HOLD) {
                LcdClearS();
                bitEnable_1 = ENABLE;
                statusSetUpTime = BACK_MENU;
            }
            break;
        case SET_DATE:
            bitEnable_1 = DISABLE;
            timeBlink_1 = (timeBlink_1 + 1) % 20;
            if (timeBlink_1 > 15)
                LcdPrintStringS(1, 6, "  ");
            if (KEYUP) {
                date_mf = date_mf + 1;
                if (date_mf > 31)
                    date_mf = 1;
                //                Write_DS1307(ADDRESS_DATE, date);
            }
            if (KEYDOWN) {
                date_mf = date_mf - 1;
                if (date_mf < 1)
                    date_mf = 31;
                //                Write_DS1307(ADDRESS_DATE, date);
            }
            if (KEYUP_HOLD) {
                date_mf = date_mf + 10;
                if (date_mf > 31)
                    date_mf = 1;
                //                Write_DS1307(ADDRESS_DATE, date);
            }
            if (KEYDOWN_HOLD) {
                date_mf = date_mf - 10;
                if (date_mf < 1)
                    date_mf = 31;
                //                Write_DS1307(ADDRESS_DATE, date);
            }
            if (KEYOK)
                statusSetUpTime = SET_MONTH;
            if (KEYOK_HOLD) {
                LcdClearS();
                bitEnable_1 = ENABLE;
                statusSetUpTime = BACK_MENU;
            }
            break;
        case SET_MONTH:
            bitEnable_1 = DISABLE;
            timeBlink_1 = (timeBlink_1 + 1) % 20;
            if (timeBlink_1 > 15)
                LcdPrintStringS(1, 2, "   ");
            if (KEYUP) {
                month_mf = month_mf + 1;
                if (month_mf > 12)
                    month_mf = 1;
                //                Write_DS1307(ADDRESS_MONTH, month);
            }
            if (KEYDOWN) {
                month_mf = month_mf - 1;
                if (month_mf < 1)
                    month_mf = 12;
                //                Write_DS1307(ADDRESS_MONTH, month);
            }
            if (KEYOK)
                statusSetUpTime = SET_YEAR;
            if (KEYOK_HOLD) {
                LcdClearS();
                bitEnable_1 = ENABLE;
                statusSetUpTime = BACK_MENU;
            }
            break;
        case SET_YEAR:
            bitEnable_1 = DISABLE;
            timeBlink_1 = (timeBlink_1 + 1) % 20;
            if (timeBlink_1 > 15)
                LcdPrintStringS(1, 9, "    ");
            if (KEYUP) {
                year_mf = year_mf + 1;
                if (year_mf > 99)
                    year_mf = 0;
                //                Write_DS1307(ADDRESS_YEAR, year);
            }
            if (KEYDOWN) {
                year_mf = year_mf - 1;
                if (year_mf < 99)
                    year_mf = 99;
                //                Write_DS1307(ADDRESS_YEAR, year);
            }
            if (KEYOK) {
                LcdClearS();
                bitEnable_1 = ENABLE;
                statusSetUpTime = BACK_MENU;
            }
            if (KEYOK_HOLD) {
                LcdClearS();
                bitEnable_1 = ENABLE;
                statusSetUpTime = BACK_MENU;
            }
            break;
        case BACK_MENU:
            bitEnable_1 = DISABLE;
            DisplayTimeForModify();
            timeBlink_1 = (timeBlink_1 + 1) % 20;
            if (timeBlink_1 > 15)
                LcdPrintStringS(0, 12, "    ");
            if (KEYOK && returnOK == 1) {
                //if key ok is pressed, set the return flags;
                LcdClearS();
                statusSetUpTime = INIT_SYSTEM;
                bitEnable_1 = ENABLE;
                setTimeFlag = 1;
                returnOK = 0;
                Write_DS1307(ADDRESS_HOUR, hour_mf);
                Write_DS1307(ADDRESS_MINUTE, minute_mf);
                Write_DS1307(ADDRESS_DAY, day_mf);
                Write_DS1307(ADDRESS_DATE, date_mf);
                Write_DS1307(ADDRESS_MONTH, month_mf);
                Write_DS1307(ADDRESS_YEAR, year_mf);
            }
            if (KEYOK && returnOK == 2) {
                LcdClearS();
                statusSetUpTime = INIT_SYSTEM;
                bitEnable_1 = ENABLE;
                setTimeFlag = 1;
                returnOK = 0;
            }
            if (KEYDOWN) {
                if (returnOK == 1) {
                    returnOK = 2;
                } else if (returnOK == 2) {
                    returnOK = 1;
                }
            }
            if (KEYUP) {
                //if key up is press, back to set up time
                LcdClearS();
                statusSetUpTime = INIT_SYSTEM;
                bitEnable_1 = ENABLE;
            }
        default:
            break;

    }
}

void Solar2Lunar(unsigned char SolarDay, unsigned char SolarMonth, unsigned int SolarYear,
        unsigned char *LunarDay, unsigned char *LunarMonth, unsigned int *LunarYear) {
    unsigned char N_AL_DT_DL;
    unsigned char T_AL_DT_DL;
    unsigned char SN_CT_AL;
    unsigned char TN_B_THT;
    unsigned char N_AL_DT_DL_TT;
    unsigned char T_AL_DT_DL_TT;

    union LUNAR_RECORD lr;

    lr.Word = LUNAR_CALENDAR_LOOKUP_TABLE[(SolarYear - BEGINNING_YEAR) * 12 + SolarMonth - 1];
    N_AL_DT_DL = lr.Info.N_AL_DT_DL;
    T_AL_DT_DL = lr.Info.T_AL_DT_DL;
    SN_CT_AL = lr.Info.SN_CT_AL + 29;
    TN_B_THT = lr.Info.TN_B_THT;

    lr.Word = LUNAR_CALENDAR_LOOKUP_TABLE[(SolarYear - BEGINNING_YEAR) * 12 + SolarMonth];
    N_AL_DT_DL_TT = lr.Info.N_AL_DT_DL;
    T_AL_DT_DL_TT = lr.Info.T_AL_DT_DL;

    // Tinh ngay & thang
    if (N_AL_DT_DL == SN_CT_AL && N_AL_DT_DL_TT == 2) {
        if (SolarDay == 1) {
            (*LunarDay) = N_AL_DT_DL;
            (*LunarMonth) = T_AL_DT_DL;
        } else if (SolarDay == 31) {
            (*LunarDay) = 1;
            (*LunarMonth) = T_AL_DT_DL_TT;
        } else {
            (*LunarDay) = SolarDay - 1;
            if (TN_B_THT) {
                (*LunarMonth) = T_AL_DT_DL;
            } else {
                (*LunarMonth) = T_AL_DT_DL == 12 ? 1 : (T_AL_DT_DL + 1);
            }
        }
    } else {
        (*LunarDay) = SolarDay + N_AL_DT_DL - 1;
        if ((*LunarDay) <= SN_CT_AL) {
            (*LunarMonth) = T_AL_DT_DL;
        } else {
            (*LunarDay) -= SN_CT_AL;

            (*LunarMonth) = T_AL_DT_DL + 1 - TN_B_THT;
            if ((*LunarMonth) == 13)
                (*LunarMonth) = 1;
        }
    }

    // Tinh Nam
    if (SolarMonth >= (*LunarMonth)) {
        (*LunarYear) = SolarYear;
    } else {
        (*LunarYear) = SolarYear - 1;
    }
    *LunarYear -= 2000;
}

void printCan(int can) {
    switch (can) {
        case 4:
            LcdPrintStringS(0, 5, "Giap ");
            break;
        case 5:
            LcdPrintStringS(0, 5, "At ");
            break;
        case 6:
            LcdPrintStringS(0, 5, "Binh ");
            break;
        case 7:
            LcdPrintStringS(0, 5, "Dinh ");
            break;
        case 8:
            LcdPrintStringS(0, 5, "Mau ");
            break;
        case 9:
            LcdPrintStringS(0, 5, "Ky ");
            break;
        case 0:
            LcdPrintStringS(0, 5, "Canh ");
            break;
        case 1:
            LcdPrintStringS(0, 5, "Tan ");
            break;
        case 2:
            LcdPrintStringS(0, 5, "Nham ");
            break;
        case 3:
            LcdPrintStringS(0, 5, "Quy ");
            break;
    }
}

void printChi(int chi) {
    switch (chi) {
        case 4:
            LcdPrintStringS(0, 10, "Ti");
            break;
        case 5:
            LcdPrintStringS(0, 10, "Suu");
            break;
        case 6:
            LcdPrintStringS(0, 10, "Dan");
            break;
        case 7:
            LcdPrintStringS(0, 10, "Meo");
            break;
        case 8:
            LcdPrintStringS(0, 10, "Thin");
            break;
        case 9:
            LcdPrintStringS(0, 10, "Ty");
            break;
        case 10:
            LcdPrintStringS(0, 10, "Ngo");
            break;
        case 11:
            LcdPrintStringS(0, 10, "Mui");
            break;
        case 0:
            LcdPrintStringS(0, 10, "Than");
            break;
        case 1:
            LcdPrintStringS(0, 10, "Dau");
            break;
        case 2:
            LcdPrintStringS(0, 10, "Tuat");
            break;
        case 3:
            LcdPrintStringS(0, 10, "Hoi");
            break;
    }
}