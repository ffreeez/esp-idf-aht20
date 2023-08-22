#include "aht20.h"

#define I2C_MASTER_SDA_IO 5
#define I2C_MASTER_SCL_IO 4
#define I2C_MASTER_FREQ_HZ 100 * 1000
#define I2C_MASTER_PORT 0

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
    .TEMP = 0,
    .readable = 0,                 // the variable will be set 1 after measure, you can make it zero after read
    .i2c_port = I2C_MASTER_PORT    // i2c_port shouldn`t be changed during running
};

void aht20_setup()
{
    i2c_param_config(I2C_MASTER_PORT, &conf);
    i2c_driver_install(I2C_MASTER_PORT, I2C_MODE_MASTER, 0, 0, 0);

    TaskHandle_t *AHT20_task_handle = 0;
    xTaskCreate((TaskFunction_t)AHT20_task, "AHT20_task", 10000, &AHT20_data, 0, AHT20_task_handle);
}

void app_main(void)
{
    aht20_setup();
    while (1)
    {
        if (AHT20_data.readable == true)
        {
            printf("RH: %.2f; TEMP: %.2f\n", AHT20_data.RH, AHT20_data.TEMP);
            AHT20_data.readable = false;
        }
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
