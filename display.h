#ifndef DISPLAY_H
#define DISPLAY_H

#include <driver/gpio.h>
#include <esp_log.h>

#define SEG_A 25
#define SEG_B 26
#define SEG_C 27
#define SEG_D 14
#define SEG_E 12
#define SEG_F 13
#define SEG_G 32
#define SEG_DP 33 // Optional
#define DISPLAY_1 16
#define DISPLAY_2 17

#define DISPLAY_ON  1
#define DISPLAY_OFF 0

const uint8_t DIGITOS[10] = 
{
    0b0111111, // 0
    0b0000110, // 1
    0b1011011, // 2
    0b1001111, // 3
    0b1100110, // 4
    0b1101101, // 5
    0b1111101, // 6
    0b0000111, // 7
    0b1111111, // 8
    0b1101111  // 9
};

/**
 * @brief Initialize GPIO pins for dual 7-segment displays.
 */
void init_dual_displays(void) 
{
    const uint8_t pins[] = {SEG_A, SEG_B, SEG_C, SEG_D, SEG_E, SEG_F, SEG_G, SEG_DP, DISPLAY_1, DISPLAY_2};
    for (int i = 0; i < sizeof(pins) / sizeof(pins[0]); i++) 
    {
        gpio_pad_select_gpio(pins[i]);
        gpio_set_direction(pins[i], GPIO_MODE_OUTPUT);
    }
    gpio_set_level(DISPLAY_1, DISPLAY_OFF);
    gpio_set_level(DISPLAY_2, DISPLAY_OFF);
}

/**
 * @brief Set the state of a display (on or off).
 * @param display Display identifier (DISPLAY_1 or DISPLAY_2).
 * @param state True for on, false for off.
 */
void set_display_state(uint8_t display, bool state) 
{
    if (display == DISPLAY_1 || display == DISPLAY_2) 
    {
        gpio_set_level(display, state ? DISPLAY_ON : DISPLAY_OFF);
    }
}

/**
 * @brief Set the segments of a 7-segment display based on a bitmask.
 * @param segments Bitmask representing the active segments.
 */
void set_segments(uint8_t segments) 
{
    gpio_set_level(SEG_A, segments & 0b0000001);
    gpio_set_level(SEG_B, segments & 0b0000010);
    gpio_set_level(SEG_C, segments & 0b0000100);
    gpio_set_level(SEG_D, segments & 0b0001000);
    gpio_set_level(SEG_E, segments & 0b0010000);
    gpio_set_level(SEG_F, segments & 0b0100000);
    gpio_set_level(SEG_G, segments & 0b1000000);
    gpio_set_level(SEG_DP, segments & 0b10000000); // Optional decimal point
}

/**
 * @brief Display a temperature value (0-999) on a specific display.
 * @param display Display identifier (DISPLAY_1 or DISPLAY_2).
 * @param temperature Temperature value to display (0-999).
 */
void show_temperature(uint8_t display, int temperature) 
{
    if (temperature < 0 || temperature > 999) 
    {
        ESP_LOGE("DISPLAY", "Temperature out of range: %d. Only 0-999 supported.", temperature);
        return;
    }

    uint8_t hundreds = temperature / 100;   // Get the hundreds digit
    uint8_t tens = (temperature / 10) % 10; // Get the tens digit
    uint8_t ones = temperature % 10;       // Get the ones digit

    // Show hundreds digit
    set_display_state(display, true);
    set_segments(DIGITOS[hundreds]);
    vTaskDelay(pdMS_TO_TICKS(5));

    // Show tens digit
    set_segments(DIGITOS[tens]);
    vTaskDelay(pdMS_TO_TICKS(5));

    // Show ones digit
    set_segments(DIGITOS[ones]);
    vTaskDelay(pdMS_TO_TICKS(5));

    // Turn off display to avoid ghosting
    set_display_state(display, false);
}

/**
 * @brief Update both displays with set and measured temperatures.
 * @param set_temp Set temperature (from potentiometer).
 * @param measured_temp Measured temperature (from thermocouple).
 */
void update_displays(int set_temp, int measured_temp) 
{
    while (true) 
    {
        // Display set temperature on DISPLAY_1
        show_temperature(DISPLAY_1, set_temp);

        // Display measured temperature on DISPLAY_2
        show_temperature(DISPLAY_2, measured_temp);

        // Adjust delay as necessary
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

#endif // DISPLAY_H
