#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "timer.c"
#include "signals_processing.c"

volatile bool* interrupt_pointer = false;

#define TIMER_FREQ 1E7
#define SAMPLING_FREQ 44.1E3



void loop_task(void *param){
    int counter = 0;
    dac_oneshot_config();

    while(true){
        *interrupt_pointer = false;
        while (*interrupt_pointer == false);

        for(int i=0; i<=1023; ++i){
            *interrupt_pointer = false;
            counter = 0;

            //funkcje wyzwalane w pętli
            output_quantized(i);

            //Busy Loop
            while (*interrupt_pointer == false){
                ++counter;
            } 
        }

        printf("remaining ticks: %d \n", counter);
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
}


void app_main(void)
{
    
    interrupt_pointer = initialize_timer(TIMER_FREQ, (int)(TIMER_FREQ/SAMPLING_FREQ));

    //Tworzenie taska przypiętego do rdzenia 1
    xTaskCreate(loop_task, "Loop", 2048, NULL, 1, NULL);

}
