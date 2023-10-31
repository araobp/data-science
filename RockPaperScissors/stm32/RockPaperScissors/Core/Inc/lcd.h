/*
 * lcd.h
 *
 *  Created on: Sep 14, 2023
 *      Author: shiny
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include <stdint.h>
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_i2c.h"

#define LCD_I2C_ADDRESS 0x3e // AQM1602XA-RN-GBW

void write_command(uint8_t command);
void write_data(uint8_t data);
void lcd_init(I2C_HandleTypeDef *phi2c);
void lcd_clear(void);
void lcd_newline(void);
void lcd_move_left(void);
void lcd_move_right(void);
void lcd_string(char *pbuf, uint8_t len);
void lcd_test(void);

#endif /* INC_LCD_H_ */
