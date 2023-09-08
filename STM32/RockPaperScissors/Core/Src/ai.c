#include <stdio.h>
#include "app_x-cube-ai.h"
#include "dct.h"
#include "ai.h"

ai_handle handle;
const char *nn_name;

// DCT Type-II instance
dct2_instance_f32 S;

ai_buffer ai_input;
ai_buffer ai_output;
ai_float *output_;
ai_float normalized_data[AI_MNETWORK_IN_1_SIZE_BYTES];
ai_float feature[WIDTH * HEIGHT];

// Standard normalization
void normalize(ai_float *in_data, ai_float *normalized_data, int len) {

  float mean = 0.0;
  float std = 0.0;

  // Sum and mean
  for (int i = 0; i < len; i++) {
    mean += in_data[i];
  }
  mean = mean / len;

  // Standard deviation
  for (int i = 0; i < len; i++) {
    std += pow(in_data[i] - mean, 2);
  }
  std = sqrt(std / len);

  // Normalization
  for (int i = 0; i < len; i++) {
    normalized_data[i] = (in_data[i] - mean) / std;
  }
}

void rps_init(void) {

  printf("--- AI ROCK PAPER SCISSORS ---\r\n");
  nn_name = ai_mnetwork_find(NULL, 0);
  printf("[NETWORK FIND] NN_NAME: %s\n", nn_name);

  ai_error ai_err = ai_mnetwork_create(nn_name, &handle, NULL);
  printf("[NETWORK CREATION] err.type: 0x%02X, err.code: 0x%02X\n",
      ai_err.type, ai_err.code);

  bool success = ai_mnetwork_init(handle);
  printf("[NETWORK INITIALIZATION] %d\n", success);  // 0: Failure, 1; Success

  dct2_2d_init_f32(&S, WIDTH, HEIGHT);
}

// Run inference
void rps_infer(ai_float *input_data, ai_float *output_data) {

  // DCT Type-II 2D for extracting feature
  dct2_2d_f32(&S, (float32_t*) input_data, (float32_t*) feature, 0);

  // Zigzag scan
  zigzag_scan_f32(&S, (float32_t*) feature, (float32_t*) input_data);

  // Normalize the feature data (DC is discarded)
  normalize(input_data + 1, normalized_data, AI_MNETWORK_IN_1_SIZE_BYTES);

  // Input parameters for running inference
  ai_input.data = AI_HANDLE_PTR(normalized_data);
  ai_output.data = AI_HANDLE_PTR(output_data);

  // Run inference
  ai_mnetwork_run(handle, &ai_input, &ai_output);

  // Copy inference result
  for (int i = 0; i < AI_NETWORK_OUT_1_SIZE; i++) {
    output_data[i] = ((ai_float*) (ai_output.data))[i];
  }
}
