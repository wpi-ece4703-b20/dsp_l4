#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include "msp432_boostxl_init.h"
#include "msp432_arm_dsp.h"

uint16_t processSample(uint16_t x) {
    static float32_t fwd_taps[25];
    static float32_t rec_taps[2];

    // white noise
    float32_t input = adc14_to_f32(0x1800 + rand() % 0x1000);

    // test signal
    // float32_t input = adc14_to_f32(x);

    fwd_taps[0] = 0.25*input;

    float32_t v = fwd_taps[0] - fwd_taps[24];
    float32_t r = v + rec_taps[0] - rec_taps[1];

    // shift delay line
    uint32_t i;
    for (i=24; i>0; i--)
        fwd_taps[i] = fwd_taps[i-1];

    rec_taps[1] = rec_taps[0];
    rec_taps[0] = r;

    return f32_to_dac14(r);

}

#include <stdio.h>

int main(void) {
    WDT_A_hold(WDT_A_BASE);

    msp432_boostxl_init_intr(FS_16000_HZ, BOOSTXL_J1_2_IN, processSample);
    msp432_boostxl_run();

    return 1;
}



