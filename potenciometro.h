#ifndef POTENCIOMETRO_H
#define POTENCIOMETRO_H

#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <esp_log.h>

// Default ADC configuration
#ifndef POTENTIOMETER_ADC_CHANNEL
#define POTENTIOMETER_ADC_CHANNEL ADC1_CHANNEL_0 // GPIO36
#endif

#ifndef POT_MIN
#define POT_MIN 20.0 // Minimum temperature (°C)
#endif

#ifndef POT_MAX
#define POT_MAX 105.0 // Maximum temperature (°C)
#endif

#define LOG_TAG "POTENTIOMETER"

/**
 * @brief Initialize the potentiometer ADC channel.
 */
void potentiometer_init(void) 
{
    ESP_LOGI(LOG_TAG, "Initializing potentiometer on ADC channel %d", POTENTIOMETER_ADC_CHANNEL);

    // Configure ADC width and attenuation
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(POTENTIOMETER_ADC_CHANNEL, ADC_ATTEN_DB_11);

    // Validate the temperature range
    if (POT_MIN >= POT_MAX) 
    {
        ESP_LOGE(LOG_TAG, "Invalid temperature range: POT_MIN (%.2f) >= POT_MAX (%.2f)", POT_MIN, POT_MAX);
    }
}

/**
 * @brief Read the potentiometer value and map it to the temperature range.
 * @return Mapped temperature in °C or -1 if an error occurs.
 */
float read_potentiometer_temperature(void) 
{
    int adc_reading = adc1_get_raw(POTENTIOMETER_ADC_CHANNEL);

    // Validate ADC reading
    if (adc_reading < 0 || adc_reading > 4095) 
    {
        ESP_LOGE(LOG_TAG, "Invalid ADC reading: %d", adc_reading);
        return -1; // Error code
    }

    // Map ADC value to temperature range
    float temperature = POT_MIN + ((float)adc_reading / 4095.0) * (POT_MAX - POT_MIN);

    // Log the result
    ESP_LOGI(LOG_TAG, "ADC reading: %d, Mapped temperature: %.2f°C", adc_reading, temperature);

    return temperature;
}

#endif // POTENCIOMETRO_H
