#include "driver/dac_oneshot.h"

#include "driver/gpio.h"
#include "hal/gpio_types.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_struct.h"

#define OUT_GPIO_PIN 25

dac_oneshot_handle_t handle;
dac_oneshot_config_t config = {.chan_id = DAC_CHAN_0,};

void output_dac_config(){
    dac_oneshot_new_channel(&config, &handle);
}

void output_GPIO_PWM_config(){
    gpio_config_t out_GPIO_conf = {
        .pin_bit_mask = (1ULL << OUT_GPIO_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en =0,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&out_GPIO_conf);
}

////////////////////////////////////////////////////////////////////////////

typedef uint8_t (*quant_variant)(float value);
uint8_t quantize(float value){
    return (uint8_t)value;
}
uint8_t quantize_dither(float value){
    return (uint8_t)(value + ((float)rand() / RAND_MAX));
}

void output_dac(float value, quant_variant variant){
    dac_oneshot_output_voltage(handle, variant(value));
}

/////////////////////////////////////////////////////////////////////////////

typedef void (*PWM_variant)();
void PWM(int *freq){
}
void VFPWM(int *freq){
    //4095
    *freq = *freq + (rand() & 8191);
}

void output_PWM(float value, PWM_variant variant){
    static float phase = 0;
    int freq = 10000;
    variant(&freq);
    float phase_inc = ((float)freq) / 44100 * 255;

    if(phase < value){
        GPIO.out_w1ts = (1 << OUT_GPIO_PIN);
    }
    else{
        GPIO.out_w1tc = (1 << OUT_GPIO_PIN);
    }
    phase = (int)(phase + phase_inc) & 255;
}

