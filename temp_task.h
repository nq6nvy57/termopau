#ifndef TEMP_TASK_H
#define TEMP_TASK_H

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "display.h"
#include "pwm.h"
#include "potenciometro.h"
#include "spifeito.h"

#define DISPLAY_1 16 // Display for set temperature
#define DISPLAY_2 17 // Display for measured temperature
#define TAG "TEMP_TASK"

// Timing intervals (in milliseconds)
#define DISPLAY_UPDATE_INTERVAL_MS 5
#define TASK_CYCLE_INTERVAL_MS 1000

/**
 * @brief Reads the measured temperature from the thermocouple via SPI.
 * @param spi Handle to the SPI device.
 * @return Measured temperature in °C or -1 on failure.
 */
float get_measured_temperature(spi_device_handle_t spi) 
{
    uint16_t data = 0;
    spi_transaction_t tM = 
    {
        .tx_buffer = NULL,
        .rx_buffer = &data,
        .length = 16,
        .rxlength = 16,
    };

    esp_err_t ret = spi_device_transmit(spi, &tM);
    if (ret != ESP_OK) 
    {
        ESP_LOGE(TAG, "SPI transmission failed: %s", esp_err_to_name(ret));
        return -1; // Error code
    }

    int16_t res = (int16_t)SPI_SWAP_DATA_RX(data, 16);
    if (res & (1 << 2)) 
    {
        ESP_LOGE(TAG, "Sensor not connected.");
        return -1; // Sensor error
    }

    res >>= 3; // Extract valid temperature data
    return res * 0.25;
}

/**
 * @brief Updates a specific display with a given temperature.
 * @param display Display identifier (DISPLAY_1 or DISPLAY_2).
 * @param temperature Temperature to display (0-999°C).
 */
void update_display(uint8_t display, int temperature) 
{
    set_display_state(display, true);
    show_temperature(display, temperature);
    vTaskDelay(pdMS_TO_TICKS(DISPLAY_UPDATE_INTERVAL_MS));
    set_display_state(display, false);
}

/**
 * @brief Adjusts the PWM duty cycle based on the temperature difference.
 * @param current_temp Current measured temperature.
 * @param target_temp Target temperature.
 */
void control_heater(float current_temp, float target_temp) 
{
    uint32_t duty = calculate_duty(current_temp, target_temp);
    set_pwm_duty(duty);
    ESP_LOGI(TAG, "PWM Updated: Current Temp = %.2f°C, Target Temp = %.2f°C, Duty = %u", 
             current_temp, target_temp, duty);
}

/**
 * @brief Main task for temperature monitoring and control.
 * @param pvParams SPI handle passed as a task parameter.
 */
void temp_task(void *pvParams) 
{
    spi_device_handle_t spi = (spi_device_handle_t)pvParams;

    while (true) 
    {
        // Read the target temperature from the potentiometer
        float target_temperature = read_potentiometer_temperature();

        // Read the measured temperature from the thermocouple
        float current_temperature = get_measured_temperature(spi);
        if (current_temperature < 0) 
        {
            ESP_LOGW(TAG, "Skipping cycle due to sensor error.");
            vTaskDelay(pdMS_TO_TICKS(TASK_CYCLE_INTERVAL_MS));
            continue;
        }

        // Update displays
        update_display(DISPLAY_1, (int)target_temperature);
        update_display(DISPLAY_2, (int)current_temperature);

        // Control heater based on temperature difference
        control_heater(current_temperature, target_temperature);

        // Wait for the next cycle
        vTaskDelay(pdMS_TO_TICKS(TASK_CYCLE_INTERVAL_MS));
    }
}

#endif // TEMP_TASK_H
