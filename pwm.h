#include <driver/ledc.h>

#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_HIGH_SPEED_MODE
#define LEDC_OUTPUT_IO          (26) //gpio pin ssr
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_10_BIT
#define LEDC_FREQUENCY          (10) // frequencia em hz(verificar em leds)

void pwm_init(void) 
{
  ledc_timer_config_t ledc_timer = 
  {
    .duty_resolution = LEDC_DUTY_RES,
    .freq_hz = LEDC_FREQUENCY,
    .speed_mode = LEDC_MODE,
    .timer_num = LEDC_TIMER,
    .clk_cfg = LEDC_AUTO_CLK
  };
  ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

  ledc_channel_config_t ledc_channel = 
  {
    .channel    = LEDC_CHANNEL,
    .duty       = 0,
    .gpio_num   = LEDC_OUTPUT_IO,
    .speed_mode = LEDC_MODE,
    .hpoint     = 0,
    .timer_sel  = LEDC_TIMER
  };
  ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
}

void set_pwm_duty(uint32_t duty) 
{
  ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, duty));
  ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
}

uint32_t calculate_duty(float temperature, float target_temp)
{
    uint32_t max_duty = (1 << LEDC_DUTY_RES) - 1;
    float temp_diff = target_temp - temperature;
    float duty_cycle_percentage = 0.0;

    if (temp_diff > 40) 
    {
        duty_cycle_percentage = 1.00; // 100%
    } 
    else if (temp_diff > 35) 
    {
        duty_cycle_percentage = 0.90; // 90%
    } 
    else if (temp_diff > 30) 
    {
        duty_cycle_percentage = 0.80; // 80%
    } 
    else if (temp_diff > 25) 
    {
        duty_cycle_percentage = 0.70; // 70%
    } 
    else if (temp_diff > 20) 
    {
        duty_cycle_percentage = 0.60; // 60%
    } 
    else if (temp_diff > 15) 
    {
        duty_cycle_percentage = 0.50; // 50%
    } 
    else if (temp_diff > 10) 
    {
        duty_cycle_percentage = 0.40; // 50%
    } 
    else if (temp_diff > 5) 
    {
        duty_cycle_percentage = 0.25; // 25%
    } 
    else if (temp_diff > 0) 
    {
        duty_cycle_percentage = 0.1; // 10%
    } 
    else
    {
        duty_cycle_percentage = 0.0; // 0%
    }

    return (uint32_t)(duty_cycle_percentage * max_duty);
}