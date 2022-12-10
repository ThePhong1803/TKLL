/* 
 * File:   time_modify.h
 * Author: PC
 *
 * Created on October 14, 2022, 5:37 PM
 */

#ifndef TIME_MODIFY_H
#define	TIME_MODIFY_H

#include <p18f4620.h>

#define CMD_INIT 1
#define CMD_START 2
#define CMD_TIME1 10
#define CMD_TIME2 11
#define CMD_TIME3 12
#define CMD_TIME4 14
#define CMD_EXTRACT_TIME 20

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

void fsm_timeModify();
int check_number_uart();
int validate(int number, int up);

unsigned char getUartBufferChar();
void uart_console();
void command_parse();

extern int modify = 0;
extern int last_modify = 0;

#endif	/* TIME_MODIFY_H */
