#include <stdio.h>
#include "app_x-cube-ai.h"
#include "network.h"
#include "network_data.h"
#include "dct.h"
#include "ai.h"

static ai_handle network;
const char *nn_name;

// DCT Type-II instance
dct2_instance_f32 S;

static ai_buffer *ai_input;
static ai_buffer *ai_output;
ai_float *output_;
ai_float normalized_data[AI_NETWORK_IN_1_SIZE];
ai_float feature[WIDTH * HEIGHT];

ai_u8 activations[AI_NETWORK_DATA_ACTIVATIONS_SIZE];
const char *activities[AI_NETWORK_OUT_1_SIZE] = { "paper", "rock", "scissors" };

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

  const ai_handle act_addr[] = { activations };
  printf("TEST\n");
  /* Create an instance of the model */
  ai_error err = ai_network_create_and_init(&network, act_addr, NULL);
  if (err.type != AI_ERROR_NONE) {
    printf("ai_network_create error - type=%d code=%d\r\n", err.type, err.code);
  } else {
    printf("ai_network_create success\n");
  }
  ai_input = ai_network_inputs_get(network, NULL);
  ai_output = ai_network_outputs_get(network, NULL);

  dct2_2d_init_f32(&S, WIDTH, HEIGHT);
}

// Run inference
void rps_infer(ai_float *input_data, ai_float *output_data) {

  // DCT Type-II 2D for extracting feature
  dct2_2d_f32(&S, (float32_t*) input_data, (float32_t*) feature, 0);

  // Zigzag scan
  zigzag_scan_f32(&S, (float32_t*) feature, (float32_t*) input_data);

  // Normalize the feature data (DC is discarded)
  normalize(input_data + 1, normalized_data, AI_NETWORK_IN_1_SIZE);

  // Input parameters for running inference
  ai_input[0].data = AI_HANDLE_PTR(normalized_data);
  //ai_input[0].size = AI_NETWORK_IN_1_SIZE;
  //ai_input[0].format = AI_BUFFER_FORMAT_FLOAT;
  ai_output[0].data = AI_HANDLE_PTR(output_data);
  //ai_output[0].size = AI_NETWORK_OUT_1_SIZE;
  //ai_output[0].format = AI_BUFFER_FORMAT_FLOAT;

  ai_i32 batch = ai_network_run(network, ai_input, ai_output);
  if (batch != 1) {
    ai_error err = ai_network_get_error(network);
    printf("AI ai_network_run error - type=%d code=%d\r\n", err.type, err.code);
  } else {
    printf("AI ai_network_run success\n");
  }

  // Copy inference result
  for (int i = 0; i < AI_NETWORK_OUT_1_SIZE; i++) {
    output_data[i] = ((ai_float*) (ai_output[0].data))[i];
  }
}
