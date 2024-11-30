#include <esp_log.h>
#include <freertos/FreeRTOS.h>

#define DISPLAY_1 16 // display para temperatura ajustada
#define DISPLAY_2 17 // display para temperatura medida
#define TAG "main"

void temp_task(void *pvParams) 
{
    spi_device_handle_t spi = (spi_device_handle_t)pvParams;
    uint16_t data;
    spi_transaction_t tM = 
    {
        .tx_buffer = NULL,
        .rx_buffer = &data,
        .length = 16,
        .rxlength = 16,
    };

    float current_temperature = 0.0;

    for (;;) 
    {
        target_temperature = read_potentiometer_temperature();
        esp_err_t ret = spi_device_acquire_bus(spi, portMAX_DELAY);
        if (ret == ESP_OK) 
        {
            ret = spi_device_transmit(spi, &tM);
            spi_device_release_bus(spi);

            if (ret == ESP_OK) 
            {
                int16_t res = (int16_t)SPI_SWAP_DATA_RX(data, 16);
                if (res & (1 << 2)) 
                {
                    ESP_LOGE(TAG, "sensor não conectado");
                } 
                else 
                {
                    res >>= 3;
                    current_temperature = res * 0.25;
                    ESP_LOGI(TAG, "temp medida: %.2f°C, temp foco: %.2f°C", current_temperature, target_temperature);

                    uint32_t duty = calculate_duty(current_temperature, target_temperature);
                    set_pwm_duty(duty);
                }
            } 
            else 
            {
                ESP_LOGE(TAG, "Error transmitting SPI bus: %s", esp_err_to_name(ret));
            }
        } 
        else 
        {
            ESP_LOGE(TAG, "Error acquiring SPI bus: %s", esp_err_to_name(ret));
        }
        set_display(DISPLAY_1, true);
        set_display(DISPLAY_2, false);
        show_number_on_display((int)target_temperature % 10); // ajuste para digito multiplo
        vTaskDelay(pdMS_TO_TICKS(5)); // display 1
        
        set_display(DISPLAY_1, false);
        set_display(DISPLAY_2, true);
        show_number_on_display((int)current_temperature % 10);
        vTaskDelay(pdMS_TO_TICKS(5)); // display 2
        vTaskDelay(pdMS_TO_TICKS(1000 - 10)); // Ensure 1-second cycle overall
    }
}


void temp_task(void *pvParams) 
{
    spi_device_handle_t spi = (spi_device_handle_t)pvParams;
    uint16_t data;
    spi_transaction_t tM = 
    {
        .tx_buffer = NULL,
        .rx_buffer = &data,
        .length = 16,
        .rxlength = 16,
    };
    for (;;) {
        // atualiza o valor do potenciômetro e da temperatura medida
        target_temperature = read_potentiometer_temperature();
        float current_temperature = ...; // obtem temperatura medida

        // temperatura ajustada no display 1
        set_display(DISPLAY_1, true);
        set_display(DISPLAY_2, false);
        show_number_on_display((int)target_temperature % 10); // ajuste para múltiplos dígitos
        vTaskDelay(pdMS_TO_TICKS(5)); // santém o display ativo brevemente

        // temperatura medida no display 2
        set_display(DISPLAY_1, false);
        set_display(DISPLAY_2, true);
        show_number_on_display((int)current_temperature % 10);
        vTaskDelay(pdMS_TO_TICKS(5)); // mantém o display ativo brevemente
    }
    
    
    for (;;) 
    {
        // Update target_temperature based on potentiometer
        target_temperature = read_potentiometer_temperature();

        esp_err_t ret = spi_device_acquire_bus(spi, portMAX_DELAY);
        if (ret != ESP_OK) 
        {
            ESP_LOGE(TAG, "error acquiring spibus: %s", esp_err_to_name(ret));
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        ret = spi_device_transmit(spi, &tM);
        spi_device_release_bus(spi);

        if (ret != ESP_OK) 
        {
            ESP_LOGE(TAG, "error transmitting spibus: %s", esp_err_to_name(ret));
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        int16_t res = (int16_t)SPI_SWAP_DATA_RX(data, 16);

        if (res & (1 << 2)) 
        {
            ESP_LOGE(TAG, "sensor not connected\n");
        } 
        else 
        {
            res >>= 3;
            float temperature = res * 0.25;
            ESP_LOGI(TAG, "Current temp: %.2f°C, Target temp: %.2f°C", temperature, target_temperature);

            uint32_t duty = calculate_duty(temperature, target_temperature);
            set_pwm_duty(duty);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void temp_task(void *pvParams) 
{
    spi_device_handle_t spi = (spi_device_handle_t)pvParams;

    for (;;) {
        // atualiza o valor do potenciômetro e da temperatura medida
        target_temperature = read_potentiometer_temperature();
        float current_temperature = ...; // obtem temperatura medida

        // temperatura ajustada no display 1
        set_display(DISPLAY_1, true);
        set_display(DISPLAY_2, false);
        show_number_on_display((int)target_temperature % 10); // ajuste para múltiplos dígitos
        vTaskDelay(pdMS_TO_TICKS(5)); // santém o display ativo brevemente

        // temperatura medida no display 2
        set_display(DISPLAY_1, false);
        set_display(DISPLAY_2, true);
        show_number_on_display((int)current_temperature % 10);
        vTaskDelay(pdMS_TO_TICKS(5)); // mantém o display ativo brevemente
    }
}

