#include "driver/gptimer.h"
#include "esp_attr.h"

volatile bool interrupt_flag = false;

//Funkcja timera
static bool IRAM_ATTR on_timer_alarm(gptimer_handle_t timer, const gptimer_alarm_event_data_t *event_data, void *user_data){
    interrupt_flag = true;
    return true;
}

volatile bool* initialize_timer(int freq, int ticks){
    
    //Konfiguracja timera
    gptimer_handle_t gptimer = NULL;
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = freq,
    };
    gptimer_new_timer(&timer_config, &gptimer);
    gptimer_event_callbacks_t cbs = {
        .on_alarm = on_timer_alarm,
    };
    gptimer_register_event_callbacks(gptimer, &cbs, NULL);
    gptimer_enable(gptimer);


    
    //Usawienie alarmu
    gptimer_alarm_config_t alarm_config = {
        .alarm_count = ticks,
        .reload_count = 0,
        .flags.auto_reload_on_alarm = true,
    };
    
    gptimer_set_alarm_action(gptimer, &alarm_config);
    gptimer_start(gptimer);

    return &interrupt_flag;
}

