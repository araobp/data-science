/*
 * rn4020.h
 *
 *  Created on: Oct 30, 2023
 *      Author: shiny
 */

#ifndef INC_RN4020_H_
#define INC_RN4020_H_

#include<stdint.h>

#define NOTIFY_COMMAND "SUW,010203040506070809000A0B0C0D0E0F,"
#define NOTIFY_MAX_PAYLOAD_LENGTH 20

void RN4020_Init(void);
void RN4020_Process(void);
void sendData(uint8_t *data, int len);

#endif /* INC_RN4020_H_ */
