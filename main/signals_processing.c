#include "driver/dac_oneshot.h"

#include "signals_data.c"

//Deklaracje DAC
dac_oneshot_handle_t handle;
dac_oneshot_config_t config = {.chan_id = DAC_CHAN_0,};


void dac_oneshot_config(){
    dac_oneshot_new_channel(&config, &handle);
}


void output_quantized(int index){
    index = index & 0x07;
    dac_oneshot_output_voltage(handle, sinus_quantized[index]);
}

