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
#define TIME_MODIFY         5
#define LANGUAGE            6
#define UART_CONSOLE        7
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

extern unsigned char returnFlag;

#endif	/* MENU_H */

