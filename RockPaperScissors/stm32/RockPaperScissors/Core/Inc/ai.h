#ifndef INC_AI_H
#define INC_AI_H

#include "ai_platform.h"

#define WIDTH 8
#define HEIGHT 8

typedef struct ai_info_ {

} ai_info;

void rps_init(void);
void rps_infer(ai_float *input_data, ai_float *output_data);

#endif
