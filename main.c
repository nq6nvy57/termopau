#include <stddef.h>
#include <stdlib.h>
#include "temp_task.h"
#include "pwm.h"
#include "display.h"
#include "potenciometro.h"
#include "spifeito.h"

#define TAG "MAIN"

// Global variable for target temperature
float target_temperature = POT_MIN;

/**
 * @brief Initialize all peripherals and tasks.
 */
void initialize_system(void) 
{
    ESP_LOGI(TAG, "Initializing system...");

    // Initialize SPI
    spi_device_handle_t spi = spi_init(25, SPI_CLOCK_SPEED, SPI_MODE); // Initialize SPI with CS pin 25
    if (spi == NULL) 
    {
        ESP_LOGE(TAG, "Failed to initialize SPI");
        return;
    }

    // Initialize peripherals
    pwm_init(LEDC_OUTPUT_IO, LEDC_FREQUENCY); // PWM for heating control
    potentiometer_init(); // ADC for potentiometer
    init_dual_displays(); // Pins for 7-segment displays

    // Start temperature control task
    if (xTaskCreate(&temp_task, "Temperature Control Task", 4096, spi, 5, NULL) != pdPASS) 
    {
        ESP_LOGE(TAG, "Failed to create temperature control task");
    }

    ESP_LOGI(TAG, "System initialization complete");
}

/**
 * @brief Main entry point.
 */
void app_main() 
{
    ESP_LOGI(TAG, "Starting application...");
    initialize_system();

    // Main loop (optional - can be used for additional tasks)
    while (true) 
    {
        vTaskDelay(pdMS_TO_TICKS(1000)); // 1-second delay
    }
}
