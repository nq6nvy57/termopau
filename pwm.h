#ifndef PWM_H
#define PWM_H

#include <driver/ledc.h>
#include <esp_log.h>

#define LOG_TAG "PWM_CONTROL"

// Default PWM configuration
#ifndef LEDC_OUTPUT_IO
#define LEDC_OUTPUT_IO 26 // Default GPIO pin for PWM output
#endif

#ifndef LEDC_FREQUENCY
#define LEDC_FREQUENCY 10 // Frequency in Hz
#endif

#ifndef LEDC_TIMER_RES
#define LEDC_TIMER_RES LEDC_TIMER_10_BIT // Duty resolution
#endif

/**
 * @brief Initialize the PWM module.
 * @param gpio_num GPIO pin for PWM output.
 * @param frequency Frequency of the PWM signal in Hz.
 */
void pwm_init(int gpio_num, int frequency) 
{
    ESP_LOGI(LOG_TAG, "Initializing PWM on GPIO %d with frequency %d Hz", gpio_num, frequency);

    // Configure the LEDC timer
    ledc_timer_config_t ledc_timer = 
    {
        .duty_resolution = LEDC_TIMER_RES,
        .freq_hz = frequency,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Configure the LEDC channel
    ledc_channel_config_t ledc_channel = 
    {
        .channel    = LEDC_CHANNEL_0,
        .duty       = 0,
        .gpio_num   = gpio_num,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_TIMER_0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

/**
 * @brief Set the PWM duty cycle.
 * @param duty Duty cycle value (0–1023 for 10-bit resolution).
 */
void set_pwm_duty(uint32_t duty) 
{
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, duty));
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0));
}

/**
 * @brief Calculate the PWM duty cycle based on the temperature difference.
 * @param temperature Current temperature.
 * @param target_temp Target temperature.
 * @return Calculated duty cycle (0–1023 for 10-bit resolution).
 */
uint32_t calculate_duty(float temperature, float target_temp) 
{
    uint32_t max_duty = (1 << LEDC_TIMER_RES) - 1;
    float temp_diff = target_temp - temperature;

    // Linear mapping of temperature difference to duty cycle
    float duty_percentage = (temp_diff > 0) ? (temp_diff / 50.0) : 0.0; // Scale factor: 50°C max
    if (duty_percentage > 1.0) 
    {
        duty_percentage = 1.0; // Cap at 100%
    }

    uint32_t duty = (uint32_t)(duty_percentage * max_duty);

    ESP_LOGI(LOG_TAG, "Temperature: %.2f°C, Target: %.2f°C, Duty Cycle: %u", temperature, target_temp, duty);

    return duty;
}

/**
 * @brief Dynamically calculate the duty cycle based on custom temperature ranges.
 * @param temperature Current temperature.
 * @param target_temp Target temperature.
 * @param max_temp_diff Maximum temperature difference for full duty.
 * @return Calculated duty cycle (0–1023 for 10-bit resolution).
 */
uint32_t calculate_duty_dynamic(float temperature, float target_temp, float max_temp_diff)
{
    uint32_t max_duty = (1 << LEDC_TIMER_RES) - 1;
    float temp_diff = target_temp - temperature;

    // Map the temperature difference proportionally to max_temp_diff
    float duty_percentage = (temp_diff > 0) ? (temp_diff / max_temp_diff) : 0.0;
    if (duty_percentage > 1.0) 
    {
        duty_percentage = 1.0;
    }

    return (uint32_t)(duty_percentage * max_duty);
}

/**
 * @brief Set a new PWM frequency dynamically.
 * @param frequency New frequency in Hz.
 */
void set_pwm_frequency(int frequency)
{
    ledc_timer_config_t ledc_timer = 
    {
        .duty_resolution = LEDC_TIMER_RES,
        .freq_hz = frequency,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
    ESP_LOGI(LOG_TAG, "PWM frequency set to %d Hz", frequency);
}

/**
 * @brief Stop PWM output in case of overload or safety conditions.
 * @param temperature Current temperature.
 * @param max_safe_temp Maximum safe temperature.
 */
void stop_pwm_on_overload(float temperature, float max_safe_temp)
{
    if (temperature > max_safe_temp) 
    {
        ESP_LOGW(LOG_TAG, "Overload detected! Stopping PWM.");
        set_pwm_duty(0); // Stop PWM
    }
}

#endif // PWM_H
