#include "driver/dac_oneshot.h"

#include "driver/gpio.h"
#include "hal/gpio_types.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_struct.h"

#include "driver/ledc.h"

////////////////////////////////////////////////////////////////////////////
// DAC

dac_oneshot_handle_t handle;
dac_oneshot_config_t config = {.chan_id = DAC_CHAN_0,};

void output_dac_config(){
    dac_oneshot_new_channel(&config, &handle);
}

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
// GPIO PWM

#define OUT_GPIO_PIN (25)

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

typedef void (*PWM_variant)();
void PWM(int *freq){
}
void VFPWM(int *freq){
    //4095
    *freq = *freq + (rand() & 8191);
}

void output_GPIO_PWM(float value, PWM_variant variant){
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

/////////////////////////////////////////////////////////////////////////////
// PWM ledc

#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_HIGH_SPEED_MODE
#define LEDC_GPIO (25)
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_DUTY_RES LEDC_TIMER_10_BIT
#define LEDC_FREQ (10000)

uint16_t ledc_scale = (1U << LEDC_DUTY_RES)/256;

void output_ledc_PWM_config(){
    ledc_timer_config_t ledc_timer_conf = {
        .speed_mode = LEDC_MODE,
        .timer_num = LEDC_TIMER,
        .duty_resolution = LEDC_DUTY_RES,
        .freq_hz = LEDC_FREQ,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&ledc_timer_conf);

    ledc_channel_config_t ledc_channel_conf = {
        .speed_mode = LEDC_MODE,
        .channel = LEDC_CHANNEL,
        .timer_sel = LEDC_TIMER,
        .intr_type = LEDC_INTR_DISABLE,
        .gpio_num = LEDC_GPIO,
        .duty = 0,
        .hpoint = 0,
    };
    ledc_channel_config(&ledc_channel_conf);
}

typedef void (*ledc_PWM_variant)();
void ledc_PWM(int *freq){
}
void ledc_VFPWM(int *freq){
    //4095
    ledc_set_freq(LEDC_MODE, LEDC_TIMER, 10000 + (rand() & 8191));
}

void output_ledc_PWM(float value, PWM_variant variant){
    variant();
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, (int)(value*ledc_scale));
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
}
