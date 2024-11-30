#ifndef SPIFEITO_H
#define SPIFEITO_H

#include <driver/spi_master.h>
#include <esp_log.h>

// Default configuration macros (can be overridden in main.c)
#ifndef DMA_CHAN
#define DMA_CHAN 2
#endif

#ifndef SPI_CLOCK_SPEED
#define SPI_CLOCK_SPEED (2 * 1000 * 1000) // 2 MHz
#endif

#ifndef SPI_MODE
#define SPI_MODE 0
#endif

#ifndef PIN_NUM_MISO
#define PIN_NUM_MISO 19
#endif

#ifndef PIN_NUM_MOSI
#define PIN_NUM_MOSI 23
#endif

#ifndef PIN_NUM_CLK
#define PIN_NUM_CLK 18
#endif

// SPI Initialization Function
spi_device_handle_t spi_init(int cs_pin) 
{
    spi_device_handle_t spi;
    esp_err_t ret;

    // SPI Bus Configuration
    spi_bus_config_t buscfg = 
    {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = (4 * 8)
    };

    ret = spi_bus_initialize(VSPI_HOST, &buscfg, DMA_CHAN);
    if (ret != ESP_OK) 
    {
        ESP_LOGE(LOG_TAG, "SPI bus initialization failed: %s", esp_err_to_name(ret));
        return NULL;
    }

    // SPI Device Configuration
    spi_device_interface_config_t devCfg = 
    {
        .mode = SPI_MODE,
        .clock_speed_hz = SPI_CLOCK_SPEED,
        .spics_io_num = cs_pin,
        .queue_size = 3
    };

    ret = spi_bus_add_device(VSPI_HOST, &devCfg, &spi);
    if (ret != ESP_OK) 
    {
        ESP_LOGE(LOG_TAG, "SPI device addition failed: %s", esp_err_to_name(ret));
        return NULL;
    }

    ESP_LOGI(LOG_TAG, "SPI initialized successfully");
    return spi;
}

#endif // SPIFEITO_H
