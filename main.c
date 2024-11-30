#include <stddef.h>
#include <stdlib.h>

#include <temp_task.h>
#include <pwm.h>
#include <display.h>
#include <potenciometro.h>
#include <spifeito.h>

// Global variable for target temperature
float target_temperature = POT_MIN;

void app_main() 
{
    // Initialize SPI
    spi_device_handle_t spi = spi_init(25); // Initialize SPI with CS pin 25
    if (spi == NULL) 
    {
        ESP_LOGE("MAIN", "Failed to initialize SPI");
        return;
    }

    // Initialize peripherals
    pwm_init();               // PWM for heating control
    potentiometer_init();     // ADC for potentiometer
    init_7segment_pins();     // Pins for 7-segment displays

    // Start temperature control task
    xTaskCreate(&temp_task, "Temperature Control Task", 4096, spi, 5, NULL);

    // Main loop (optional - can be left empty)
    while (true) 
    {
        // Optional: Perform additional main loop tasks if needed
        vTaskDelay(pdMS_TO_TICKS(1000)); // 1-second delay
    }
}
