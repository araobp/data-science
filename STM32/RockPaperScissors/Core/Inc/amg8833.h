/*
 * amg8833.h
 *
 *  Created on: Sep 9, 2023
 *      Author: shiny
 */

#ifndef INC_AMG8833_H_
#define INC_AMG8833_H_

#include "stdbool.h"
#include "stdint.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_i2c.h"

// Infrared array sensor "Panasonic AMG8833"
#define AMG8833_DEV_ADDR 0x68 << 1
#define AMG8833_T01L_ADDR 0x80
#define AMG8833_PIXEL_DATA_LENGTH 128U
#define AMG8833_PIXEL_DATA_LENGTH_16bit 64U
#define AMG8833_TTHL_ADDR 0x0E
#define AMG8833_1F_ADDR 0x1F
#define AMG8833_AVE_ADDR 0x07
#define AMG8833_RESOLUTION 0.25

void amg8833_adaptor_init(I2C_HandleTypeDef *phi2c);
void amg8833_set_moving_average(bool enable);
void amg8833_read_registors(uint8_t reg_addr, uint8_t *buffer, uint8_t length);

#endif /* INC_AMG8833_H_ */
