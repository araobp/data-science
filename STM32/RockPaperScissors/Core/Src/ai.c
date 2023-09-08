#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "app_x-cube-ai.h"
#include "bsp_ai.h"
#include "aiSystemPerformance.h"
#include "ai_datatypes_defines.h"
#include "ai_device_adaptor.h"

ai_handle handle;
const char *nn_name;

void rps_init(void) {
  MX_UARTx_Init();

  LC_PRINT("--- AI ROCK PAPER SCISSORS ---\r\n");
  nn_name = ai_mnetwork_find(NULL, 0);
  LC_PRINT("[NETWORK FIND] NN_NAME: %s\n", nn_name);

  ai_error ai_err = ai_mnetwork_create(nn_name, &handle, NULL);
  LC_PRINT("[NETWORK CREATION] err.type: 0x%02X, err.code: 0x%02X\n", ai_err.type, ai_err.code);

  bool success = ai_mnetwork_init(handle);
  LC_PRINT("[NETWORK INITIALIZATION] %d\n", success);  // 0: Failure, 1; Success

}

void rps_process(void) {
  ai_buffer ai_input[AI_MNETWORK_IN_NUM];
  ai_buffer ai_output[AI_MNETWORK_OUT_NUM];
}
