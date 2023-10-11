# DFSDM calibration for Knowles MEMS microphones

This page shows if pre-emphasis works or not.

## Condition

FIR(HPF) coefficient for pre-emphasis: 0.95

C code snippet
```
#define NN 512
#define ALPHA 0.95f
float32_t fir_coefficients[2] = { -ALPHA, 1.0f };
arm_fir_instance_f32 S_PRE;
arm_fir_init_f32(&S_PRE, 2, fir_coefficients, state_buf, NN);

void apply_pre_emphasis(float32_t *signal) {
  arm_fir_f32(&S_PRE, signal, signal, NN);
}
```

## Measurement result

#### Welch

Pre-emphasis enabled

<img src="calibration/calibration_welch_pre_emphasis_on.png" width=500>

Pre-emphasis disabled

<img src="calibration/calibration_welch_pre_emphasis_off.png" width=500>

#### MFSCs of Japanese vowels (a, i, u, e, o)

Pre-emphasis enabled

<img src="calibration/calibration_mfscs_japanese_vowels_pre_emphasis_on.png" width=500>

Pre-emphasis disabled

<img src="calibration/calibration_mfscs_japanese_vowels_pre_emphasis_off.png" width=500>
