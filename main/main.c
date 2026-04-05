#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "timer.c"
#include "signals_processing.c"
#include "signals_data.c"

volatile bool* interrupt_pointer = false;

#define TIMER_FREQ 1E7
#define SAMPLING_FREQ 44.1E3

conf_variant_type conf_variant[3] = {conf_dac, conf_ledc, conf_GPIO};
out_variant_type out_variant[6] = {out_dac_quantize, out_dac_quantize_dither, out_ledc_PWM, out_ledc_VFPWM, out_GPIO, out_GPIO_VFPWM};
uint32_t variant_selector = 0;

void loop_task(void *param){
    int counter = 0;
    int sample = 0;

    global_config(conf_variant[variant_selector>>1]);
    //GPIO25

    while(true){
        *interrupt_pointer = false;
        while (*interrupt_pointer == false);


        for(int i=0; i<220500; ++i){    //5s
            *interrupt_pointer = false;
            counter = 0;

            //funkcje wyzwalane w pętli
            //variant_selector = (REG_READ(GPIO_IN_REG)>>16)&7;
            global_out(sinus1378_low[sample], out_variant[variant_selector]);
            sample = (sample + 1) & RAMP_MASK;

            //Busy Loop
            while (*interrupt_pointer == false){
                ++counter;
            }
        }

        printf("variant: %ld remaining ticks: %d \n", variant_selector, counter);
        //sample = (sample + 1) & RAMP_MASK;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


void app_main(void)
{

    interrupt_pointer = initialize_timer(TIMER_FREQ, (int)(TIMER_FREQ/SAMPLING_FREQ));

    variant_selector_config();
    //variant_selector = (REG_READ(GPIO_IN_REG)>>16)&7;
    variant_selector = 1;

    //Tworzenie taska przypiętego do rdzenia 1
    xTaskCreate(loop_task, "Loop", 16384, NULL, 1, NULL);

}
