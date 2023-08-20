#include "aht20.h"

#define I2C_MASTER_SDA_IO 4
#define I2C_MASTER_SCL_IO 3
#define I2C_MASTER_FREQ_HZ 400 * 1000
#define I2C_MASTER_PORT I2C_NUM_0

i2c_config_t conf =
{
    .mode = I2C_MODE_MASTER,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = I2C_MASTER_FREQ_HZ,
};

AHT20_data_t AHT20_data = 
{
    .status = 0,
    .RH = 0,
    .TEMP = 0
};

void app_main(void)
{
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    i2c_param_config(I2C_MASTER_PORT, &conf);
    i2c_driver_install(I2C_MASTER_PORT, I2C_MODE_MASTER, 0, 0, 0);
    while (1)
    {
        AHT20_measures(I2C_NUM_0, &AHT20_data);
    }
    
    

}
