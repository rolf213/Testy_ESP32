idf_component_register(SRCS "signals_data.c" "signals_processing.c" "timer.c" "main.c"
                    PRIV_REQUIRES spi_flash
                    INCLUDE_DIRS ""
                    REQUIRES driver)
