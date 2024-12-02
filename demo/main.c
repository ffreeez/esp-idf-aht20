#include "aht20.h"
#include <stdio.h>
#define I2C_MASTER_SDA_IO 17
#define I2C_MASTER_SCL_IO 16
#define I2C_MASTER_FREQ_HZ 100 * 1000


// 1. i2C configuration
i2c_config_t conf =
{
    .mode = I2C_MODE_MASTER,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .sda_pullup_en = GPIO_PULLUP_DISABLE,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .scl_pullup_en = GPIO_PULLUP_DISABLE,
    .master.clk_speed = I2C_MASTER_FREQ_HZ,
};

// 2. Declares the struct for the AHT2x driver.
AHT20_data_t AHT20_data =
{
    .r_humidity = 0, 
    .temperature = 0,
    .is_busy = 0
};

void aht20_setup()
{
    // 3. Configures i2C with the config
    i2c_param_config(I2C_MASTER_PORT, &conf);
    i2c_driver_install(I2C_MASTER_PORT, I2C_MODE_MASTER, 0, 0, 0);
    // 4. initialize the AHT2x sensor by calling this.
    AHT20_begin(&AHT20_data);
}

void app_main(void)
{
    aht20_setup();
    while (1)
    {
        // 5. Start a measurement task
        esp_err_t ret = AHT20_measure(&AHT20_data);
        if(ret == 0)
        // 6. Reported relative humidity and temperature will be store in the struct.
        printf("RH: %.2f; TEMP: %.2f\n", AHT20_data.r_humidity, AHT20_data.temperature);
        // 7. A proper delay.
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
