/*
 * dct.h
 *
 *  Created on: 2019/01/08
 *
 *  https://github.com/araobp/stm32-mcu/tree/master/NUCLEO-F401RE/DCT
 */

#ifndef INC_DCT_H_
#define INC_DCT_H_

#include "stdbool.h"
#include "arm_math.h"

/**
 * DCT Type-II instance
 *
 * Y = W * X;
 */
typedef struct {
  uint16_t width;
  uint16_t height;
  arm_matrix_instance_f32 Y;
  arm_matrix_instance_f32 W;
  arm_matrix_instance_f32 W_I;
  arm_matrix_instance_f32 X;
} dct2_instance_f32;

typedef enum {
  ONE, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT
} zigzag_path;

void dct2_init_f32(dct2_instance_f32 *S, uint16_t width);

void dct2_f32(dct2_instance_f32 *S, float32_t *pSrc, float32_t *pDst, uint8_t idctFlag);

void dct2_2d_init_f32(dct2_instance_f32 *S, uint16_t height, uint16_t width);

void dct2_2d_f32(dct2_instance_f32 *S, float32_t *pSrc, float32_t *pDst,
    uint8_t idctFlag);

void zigzag_scan_f32(dct2_instance_f32 *S, float32_t *pSrc, float32_t *pDst);

#endif /* INC_DCT_H_ */
