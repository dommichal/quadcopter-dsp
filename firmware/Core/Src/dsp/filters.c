/**
 * @file filters.c
 * @author Dominik Michalczyk
 * @brief Digital filters liblary
 * @version 0.1
 * @date 2024-01-26
 */
#include <dsp/filters.h>

void EMA_filter(EMA_filter_t* ema, float* output, float input) {
    output[0] = ema->alpha * input + (1 - ema->alpha) * output[1];
    
    output[1] = output[0]; // shift old output samples;
}

void Low_Pass_IIR_Filter_Init(IIR_filter_t* iir){
    iir->a0 = iir->samplingTime/(iir->samplingTime + 2*iir->tau);
    iir->a1 = (iir->samplingTime - 2*iir->tau)/(iir->samplingTime + 2*iir->tau);
}

/**
 * @brief First order low pass IIR filter based on analog prototype
 */
void Low_Pass_IIR_Filter(IIR_filter_t* iir, float output[2], float input[2]){
    output[0] = iir->a0 * (input[0] + input[1]) - iir->a1 * output[1];
    
    /* Shift values */
    output[1] = output[0];
    input[1]  = input[0];
}

void High_Pass_IIR_Filter_Init(IIR_filter_t* iir){
    iir->a0 = (2*iir->tau/iir->samplingTime)/(1 + 2*iir->tau/iir->samplingTime);
    iir->a1 = (2*iir->tau/iir->samplingTime - 1)/(1 + 2*iir->tau/iir->samplingTime);
}

/**
 * @brief First order high pass IIR filter based on analog prototype
 */
void High_Pass_IIR_Filter(IIR_filter_t* iir, float output[2], float input[2]){
    output[0] = iir->a0 * (input[0] - input[1]) + iir->a1 * output[1];
    
    /* Shift values */
    output[1] = output[0];
    input[1]  = input[0];
}

/**
 * @brief Second order band IIR filter based on analog prototype 
 */
void Second_Order_IIR_Filter(IIR_second_order_filter_t* iir, float output[3], float input[3]){
    output[0] = iir->b[0] * input[0] + iir->b[1] * input[1] + iir->b[2] * input[2] - 
                output[1] * iir->a[0] - output[2] * iir->a[1];

    /* Shift values */
    output[2] = output[1];
    output[1] = output[0];
    input[2]  = input[1];
    input[1]  = input[0];
}

/**
 * @brief Band-pass second order Butterwort filter with 
 *        fs = 1kHz, fgl = 20Hz, fgh = 50Hz
  */
IIR_second_order_filter_t butterwort_filter = {
    .b = { 0.08636403f,  0.0f,      -0.08636403f},
    .a = {-1.7912148f,   0.82727195f}

};

void Butterwort_Band_Pass_Filter(float output[3], float input[3]){
    Second_Order_IIR_Filter(&butterwort_filter, output, input);
}

/**
 * @brief Band-pass second order Chebyshev type 1 filter with
 *        fs = 1kHz, fgl = 5Hz, fgh = 90Hz, maximum ripple 
 *        allowed below unity gain in the pass band 6dB
  */
IIR_second_order_filter_t chebyshev1_filter = {
    .b = { 0.97803f,  -1.956061f,  0.97803f},
    .a = {-1.955578f,  0.956544f}
};

void Chebyshev1_Band_Pass_Filter(float output[3], float input[3]){
    Second_Order_IIR_Filter(&chebyshev1_filter, output, input);
}

/**
 * @brief High-pass second order Butterwort filter with
 *        fs = 1kHz, fg = 15Hz 
  */
IIR_second_order_filter_t butterwort_high_filter = {
    .b = { 0.914969f, -1.829938f,  0.914969f},
    .a = {-1.822695f,  0.837182f}
};

void Butterwort_High_Pass_Filter(float output[3], float input[3]){
    Second_Order_IIR_Filter(&butterwort_high_filter, output, input);
}


IIR_second_order_filter_t butterwort_low_filter = {
    .b = {0.000241f, 0.000483f, 0.000241f},
    .a = {-1.955578f,  0.956544f}
};

void Butterwort_Low_Pass_Filter(float output[3], float input[3]){
    Second_Order_IIR_Filter(&butterwort_low_filter, output, input);
}

/**
 * @brief Second order band IIR filter based on analog prototype 
 */
void IIR_4th_Order_Filter(IIR_4th_order_filter_t* iir, float output[5], float input[5]){
    output[0] = iir->b[0] * input[0] + iir->b[1] * input[1] + iir->b[2] * input[2] 
                + iir->b[3] * input[3] + iir->b[4] * input[4] - iir->a[0] * output[1]
                - iir->a[1] * output[2] - iir->a[2] * output[3] - iir->a[3] * output[4];
    /* Shift values */
    output[4] = output[3];
    output[3] = output[2];
    output[2] = output[1];
    output[1] = output[0];

    input[4]  = input[3];
    input[3]  = input[2];
    input[2]  = input[1];
    input[1]  = input[0];
}



/**
 * @brief Band-pass 4th order Butterwort filter with 
 *        fs = 1kHz, fgl = 20Hz, fgh = 60Hz
  */
IIR_4th_order_filter_t butterwort_band_4th_filter = {
    .b = { 0.013359, 0.0, -0.026718, 0.0, 0.013359 },
    .a = {-3.560947, 4.838864, -2.97693, 0.700897 }
};

void Butterwort_4th_Band_Filter(float output[5], float input[5]){
    IIR_4th_Order_Filter(&butterwort_band_4th_filter, output, input);
}