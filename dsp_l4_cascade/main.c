#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "msp432_boostxl_init.h"
#include "msp432_arm_dsp.h"


typedef struct cascadestate {
    float32_t s[2];
    float32_t c[2];
} cascadestate_t;

float32_t cascadefir(float32_t x, cascadestate_t *p) {
    float32_t r = x + (p->s[0] * p->c[0]) +  (p->s[1] * p->c[1]);
    p->s[1] = p->s[0];
    p->s[0] = x;
    return r;
}

void createcascade(float32_t c0,
                   float32_t c1,
                   cascadestate_t *p) {
    p->c[0] = c0;
    p->c[1] = c1;
    p->s[0] = p->s[1] = 0.0f;
}

cascadestate_t stage1;
cascadestate_t stage2;
cascadestate_t stage3;
cascadestate_t stage4;

void initcascade() {
    createcascade(    0.0f,  1.0f, &stage1);
    createcascade( M_SQRT2,  1.0f, &stage2);
    createcascade(-M_SQRT2, 1.0f, &stage3);
    createcascade(    1.0f,  0.0f, &stage4);
}

uint16_t processCascade(uint16_t x) {

    float32_t input = adc14_to_f32(0x1800 + rand() % 0x1000);
    float32_t v;
    static float32_t d;

    v = cascadefir(d, &stage1);
    v = cascadefir(v, &stage2);
    v = cascadefir(v, &stage3);
    v = cascadefir(v, &stage4);
    d = input;

    return f32_to_dac14(v*0.125);
}

uint16_t processSample(uint16_t x) {
    static float32_t taps[9];

    // white noise
    float32_t input = adc14_to_f32(0x1800 + rand() % 0x1000);

    // test signal
    // float32_t input = adc14_to_f32(x);

    uint32_t i;
    for (i=0; i<8; i++)
       taps[8-i] = taps[7-i];
    taps[0] = input;

    // the filter. We're adding a scale factor to avoid overflow.
    float32_t r = 0.125f * (taps[1] + taps[2] + taps[3] + taps[4] +
                            taps[5] + taps[6] + taps[7] + taps[8]);

    return f32_to_dac14(r);

}

#include <stdio.h>

int main(void) {
    WDT_A_hold(WDT_A_BASE);

    initcascade();

    msp432_boostxl_init_intr(FS_32000_HZ, BOOSTXL_J1_2_IN, processCascade);
    msp432_boostxl_run();

    return 1;
}



