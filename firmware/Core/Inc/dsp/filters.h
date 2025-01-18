#if !defined(_FILTERS_H_)
#define _FILTERS_H_

#include <inttypes.h>

typedef struct EMA_filter_t {
    float alpha;
    uint8_t buffer_size;
} EMA_filter_t;

typedef struct IIR_filter_t
{
    float tau;
    float samplingTime;

    float a0;
    float a1;
}IIR_filter_t;

/* The coefficients must be normalized so that a0 = 1 
   a1/a0, a2/a0, b0/a0 ... bn/a0*/
typedef struct IIR_second_order_filter_t
{
    float b[3]; /* b0, b1, b2 */
    float a[2]; /* a1, a2 */
}IIR_second_order_filter_t;

/* The coefficients must be normalized so that a0 = 1 
   a1/a0, a2/a0, b0/a0 ... bn/a0*/
typedef struct IIR_4th_order_filter_t
{
    float b[5]; /* b0, b1, b2, ... */
    float a[4]; /* a1, a2, ... */
}IIR_4th_order_filter_t;


void EMA_filter(EMA_filter_t* ema, float* output, float input);
void Low_Pass_IIR_Filter_Init(IIR_filter_t* iir);
void Low_Pass_IIR_Filter(IIR_filter_t* iir, float output[2], float input[2]);
void High_Pass_IIR_Filter_Init(IIR_filter_t* iit);
void High_Pass_IIR_Filter(IIR_filter_t* iir, float output[2], float input[2]);

void Second_Order_IIR_Filter(IIR_second_order_filter_t* iir, float output[3], float input[3]);
void Butterwort_Band_Pass_Filter(float output[3], float input[3]);
void Chebyshev1_Band_Pass_Filter(float output[3], float input[3]);
void Butterwort_High_Pass_Filter(float output[3], float input[3]);
void Butterwort_Low_Pass_Filter(float output[3], float input[3]);

void IIR_4th_Order_Filter(IIR_4th_order_filter_t* iir, float output[5], float input[5]);
void Butterwort_4th_Band_Filter(float output[5], float input[5]);

#endif