#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "msp432_boostxl_init.h"
#include "msp432_arm_dsp.h"
#include <stdlib.h>

#define MAXTAPS 64
float32_t taps[MAXTAPS];
float32_t B[MAXTAPS];

uint16_t NUMTAPS;

uint16_t processSampleDirectFull(uint16_t x) {
    float32_t input = adc14_to_f32(x);

    taps[0] = input;

    float32_t q = 0.0;
    uint16_t i;
    for (i = 0; i<NUMTAPS; i++)
        q += taps[i] * B[i];

    for (i = NUMTAPS-1; i>0; i--)
        taps[i] = taps[i-1];

    return f32_to_dac14(q);
}

//----------------------------------------------------------------
uint16_t head = 0;

uint16_t processSampleDirectFullCircular16(uint16_t x) {
    float32_t input = adc14_to_f32(x);

    taps[(16 - head) % 16] = input;

    float32_t q = 0.0;
    uint16_t i;
    for (i = 0; i<16; i++)
        q += taps[i] * B[(i + head) % 16];

    head = (head + 1) % 16;

    return f32_to_dac14(q);
}

//----------------------------------------------------------------
uint16_t processSampleDirectFullCircular32(uint16_t x) {
    float32_t input = adc14_to_f32(x);

    taps[(32 - head) % 32] = input;

    float32_t q = 0.0;
    uint16_t i;
    for (i = 0; i<32; i++)
        q += taps[i] * B[(i + head) % 32];

    head = (head + 1) % 32;

    return f32_to_dac14(q);
}

//----------------------------------------------------------------
uint16_t processSampleDirectFullCircular64(uint16_t x) {
    float32_t input = adc14_to_f32(x);

    taps[(64 - head) % 64] = input;

    float32_t q = 0.0;
    uint16_t i;
    for (i = 0; i<64; i++)
        q += taps[i] * B[(i + head) % 64];

    head = (head + 1) % 64;

    return f32_to_dac14(q);
}


//----------------------------------------------------------------
#include <stdio.h>

int main(void) {
    WDT_A_hold(WDT_A_BASE);

    uint32_t c;

    NUMTAPS = 16;
    c = measurePerfSample(processSampleDirectFull);
    printf("16 direct %d\n", c);

    NUMTAPS = 32;
    c = measurePerfSample(processSampleDirectFull);
    printf("32 direct %d\n", c);

    NUMTAPS = 64;
    c = measurePerfSample(processSampleDirectFull);
    printf("64 direct %d\n", c);

    c = measurePerfSample(processSampleDirectFullCircular16);
    printf("16 circular %d\n", c);

    c = measurePerfSample(processSampleDirectFullCircular32);
    printf("32 circular %d\n", c);

    c = measurePerfSample(processSampleDirectFullCircular64);
    printf("64 circular %d\n", c);

    return 1;
}
