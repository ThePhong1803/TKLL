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
#define SET_ALARM           20
#define STOPWATCH           3
#define RUN_STOPWATCH       30
#define TIMER_CLOCK         4
#define RUN_TIMER_CLOCK     40
#define TIME_MODIFY         5
#define RUN_TIME_MODIFY     6
#define WORLD_TIME          7
#define RUN_WORLD_TIME      8
#define UART_CONSOLE        9
#define TEMP_HUMI           10
#define RUN_DHT             11
#define WEATHER_DISPLAY     12
#define BACK                256
#define NULL                0

#define KEYUP           isKeyPressed(4)
#define KEYDOWN         isKeyPressed(5)
#define KEYOK           isKeyPressed(6)
#define KEYOK_HOLD      isKeyHold(6)
#define KEYUP_HOLD      isKeyHold(4)
#define KEYDOWN_HOLD    isKeyHold(5)

void recvUartData();
void menuControl();
void uart_console();

extern unsigned char setTimeFlag;
extern unsigned char returnFlag;
extern unsigned char returnOK;

#endif	/* MENU_H */

