#include "driver/dac_oneshot.h"

#include "driver/gpio.h"
#include "hal/gpio_types.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_struct.h"

#include "signals_data.c"

#define OUT_GPIO_PIN 25

//Deklaracje DAC
dac_oneshot_handle_t handle;
dac_oneshot_config_t config = {.chan_id = DAC_CHAN_0,};

gpio_config_t out_GPIO_conf = {
        .pin_bit_mask = (1ULL << OUT_GPIO_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = 0,
        .pull_down_en =0,
        .intr_type = GPIO_INTR_DISABLE,
    };



void output_dac_config(){
    dac_oneshot_new_channel(&config, &handle);
}

void output_GPIO_config(){
    gpio_config(&out_GPIO_conf);
}


void output_dac_quantized(int index){
    index = index & SINUS_MASK;
    dac_oneshot_output_voltage(handle, sinus[index]);
}

void output_dac_quantized_dithered(int index){
    index = index & SINUS_MASK;
    int output;
    output = (int)(sinus[index] + ((float)rand() / RAND_MAX));
    dac_oneshot_output_voltage(handle, output);
}

void output_GPIO(int index){
    index = index & 63;
    if(index == 63){
        GPIO.out_w1ts = (1 << OUT_GPIO_PIN);
    }
    else{
        GPIO.out_w1tc = (1 << OUT_GPIO_PIN);
    }
}
