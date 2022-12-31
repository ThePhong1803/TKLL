/* 
 * File:   menu.h
 * Author: PC
 *
 * Created on October 14, 2022, 5:37 PM
 */

#ifndef MENU_H
#define	MENU_H

#include <p18f4620.h>

#define TIME_SCREEN         1
#define ALARM_CLOCK         2
#define SET_ALARM           3
#define STOPWATCH           4
#define RUN_STOPWATCH       5
#define TIMER_CLOCK         6
#define RUN_TIMER_CLOCK     7
#define TIME_MODIFY         8
#define RUN_TIME_MODIFY     9
#define WORLD_TIME          10
#define RUN_WORLD_TIME      11
#define UART_CONSOLE        12
#define TEMP_HUMI           13
#define RUN_DHT             14
#define WEATHER_DISPLAY     15
#define LUNAR_CALENDAR      16
#define DISPLAY_LUNAR       17
#define DISPLAY_STOP_TIME   18
#define BACK                256
#define NULL                0

#define KEYUP           isKeyPressed(4)
#define KEYDOWN         isKeyPressed(5)
#define KEYOK           isKeyPressed(6)
#define KEYOK_HOLD      isKeyHold(6)
#define KEYUP_HOLD      isKeyHold(4)
#define KEYDOWN_HOLD    isKeyHold(5)
#define KEYUP_HOLD_NONRESET  isKeyHold_nonreset(4)
#define KEYDOWN_HOLD_NONRESET isKeyHold_nonreset(5)


void recvUartData();
void menuControl();
void uart_console();

extern unsigned char setTimeFlag;
extern unsigned char returnFlag;
extern unsigned char returnOK;
#endif	/* MENU_H */

