#include "aht20.h"
#include <stdio.h>
#define I2C_MASTER_SDA_IO 17
#define I2C_MASTER_SCL_IO 16
#define I2C_MASTER_FREQ_HZ 100 * 1000


i2c_config_t conf =
{
    .mode = I2C_MODE_MASTER,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .sda_pullup_en = GPIO_PULLUP_DISABLE,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .scl_pullup_en = GPIO_PULLUP_DISABLE,
    .master.clk_speed = I2C_MASTER_FREQ_HZ,
};

AHT20_data_t AHT20_data =
{
    .RH = 0,
    .TEMP = 0
};

void aht20_setup()
{
    i2c_param_config(I2C_MASTER_PORT, &conf);
    i2c_driver_install(I2C_MASTER_PORT, I2C_MODE_MASTER, 0, 0, 0);
}

void app_main(void)
{
    aht20_setup();
    while (1)
    {
        esp_err_t ret = AHT20_measure(&AHT20_data);
        if(ret == 0)
        printf("RH: %.2f; TEMP: %.2f\n", AHT20_data.RH, AHT20_data.TEMP);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
