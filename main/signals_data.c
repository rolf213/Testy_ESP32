#include <stdint.h>

#include "esp_attr.h"

//zmienne w DRAM są wczytywane szybciej
DRAM_ATTR const uint8_t sinus_quantized[8] = {
    4,
    3,
    2,
    1,
    0,
    1,
    2,
    3
};
