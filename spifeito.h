#ifndef SPIFEITO_H
#define SPIFEITO_H

#include <driver/spi_master.h>
#include <esp_log.h>

#define LOG_TAG "SPI_FEITO"

// Default SPI configuration macros
#ifndef DMA_CHAN
#define DMA_CHAN 2
#endif

#ifndef SPI_CLOCK_SPEED
#define SPI_CLOCK_SPEED (2 * 1000 * 1000) // Default: 2 MHz
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

/**
 * @brief Initialize the SPI bus and device.
 * @param cs_pin Chip select pin for the SPI device.
 * @param clock_speed Clock speed for SPI communication.
 * @param spi_mode SPI mode (0, 1, 2, 3).
 * @return Handle to the SPI device or NULL on failure.
 */
spi_device_handle_t spi_init(int cs_pin, int clock_speed, int spi_mode) 
{
    spi_device_handle_t spi;
    esp_err_t ret;

    ESP_LOGI(LOG_TAG, "Initializing SPI with CS pin %d, clock speed %d Hz, mode %d", cs_pin, clock_speed, spi_mode);

    // Configure SPI bus
    spi_bus_config_t buscfg = 
    {
        .miso_io_num = PIN_NUM_MISO,
        .mosi_io_num = PIN_NUM_MOSI,
        .sclk_io_num = PIN_NUM_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 64 // Adjust as needed
    };

    ret = spi_bus_initialize(VSPI_HOST, &buscfg, DMA_CHAN);
    if (ret != ESP_OK) 
    {
        ESP_LOGE(LOG_TAG, "SPI bus initialization failed: %s", esp_err_to_name(ret));
        return NULL;
    }

    // Configure SPI device
    spi_device_interface_config_t devcfg = 
    {
        .mode = spi_mode,
        .clock_speed_hz = clock_speed,
        .spics_io_num = cs_pin,
        .queue_size = 3
    };

    ret = spi_bus_add_device(VSPI_HOST, &devcfg, &spi);
    if (ret != ESP_OK) 
    {
        ESP_LOGE(LOG_TAG, "SPI device addition failed: %s", esp_err_to_name(ret));
        return NULL;
    }

    ESP_LOGI(LOG_TAG, "SPI initialized successfully");
    return spi;
}

/**
 * @brief Add an additional SPI device.
 * @param cs_pin Chip select pin for the new device.
 * @param clock_speed Clock speed for the new device.
 * @param spi_mode SPI mode (0, 1, 2, 3) for the new device.
 * @return ESP_OK on success or an error code.
 */
esp_err_t add_spi_device(int cs_pin, int clock_speed, int spi_mode) 
{
    spi_device_interface_config_t devcfg = 
    {
        .mode = spi_mode,
        .clock_speed_hz = clock_speed,
        .spics_io_num = cs_pin,
        .queue_size = 3
    };

    return spi_bus_add_device(VSPI_HOST, &devcfg, NULL);
}

/**
 * @brief Perform a test transaction on the SPI bus.
 * @param spi Handle to the SPI device.
 * @return ESP_OK on success or an error code.
 */
esp_err_t test_spi_communication(spi_device_handle_t spi) 
{
    uint8_t test_data[1] = {0xAA}; // Test byte
    spi_transaction_t t = 
    {
        .length = 8,
        .tx_buffer = test_data,
    };

    esp_err_t ret = spi_device_transmit(spi, &t);
    if (ret != ESP_OK) 
    {
        ESP_LOGE(LOG_TAG, "SPI test failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(LOG_TAG, "SPI test successful");
    return ESP_OK;
}

/**
 * @brief Dynamically calculate the duty cycle based on custom temperature ranges.
 * @param spi Handle to the SPI device.
 * @param data Pointer to the data buffer for SPI transaction.
 * @param length Length of the data buffer.
 * @return ESP_OK on success or an error code.
 */
esp_err_t spi_write(spi_device_handle_t spi, const uint8_t *data, size_t length)
{
    spi_transaction_t t = 
    {
        .length = length * 8, // Length in bits
        .tx_buffer = data,
        .rx_buffer = NULL
    };
    return spi_device_transmit(spi, &t);
}

#endif // SPIFEITO_H
