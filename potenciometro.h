#ifndef POTENCIOMETRO_H
#define POTENCIOMETRO_H

#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <esp_log.h>

#ifndef POTENTIOMETER_ADC_CHANNEL
#define POTENTIOMETER_ADC_CHANNEL ADC1_CHANNEL_0 // GPIO36
#endif

#ifndef POT_MIN
#define POT_MIN 20.0 // temp min °c
#endif

#ifndef POT_MAX
#define POT_MAX 105.0 // temp max °c
#endif

void potentiometer_init(void) 
{
    ESP_LOGI(LOG_TAG, "Initializing potentiometer on ADC channel %d", POTENTIOMETER_ADC_CHANNEL);
    adc1_config_width(ADC_WIDTH_BIT_12); // Set ADC resolution to 12 bits
    adc1_config_channel_atten(POTENTIOMETER_ADC_CHANNEL, ADC_ATTEN_DB_11); // Attenuation for full range
}

// Read and map the potentiometer value to the temperature range
float read_potentiometer_temperature(void) 
{
    int adc_reading = adc1_get_raw(POTENTIOMETER_ADC_CHANNEL); // Read ADC value
    if (adc_reading < 0 || adc_reading > 4095) 
    {
        ESP_LOGE(LOG_TAG, "Invalid ADC reading: %d", adc_reading);
        return POT_MIN; // Default to minimum temperature
    }

    // Map ADC value to the temperature range
    float temperature = POT_MIN + ((float)adc_reading / 4095.0) * (POT_MAX - POT_MIN);
    ESP_LOGI(LOG_TAG, "ADC reading: %d, Mapped temperature: %.2f°C", adc_reading, temperature);
    return temperature;
}

#endif // POTENCIOMETRO_H
