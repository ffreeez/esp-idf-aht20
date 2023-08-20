#include "aht20.h"

#define I2C_MASTER_SDA_IO 4
#define I2C_MASTER_SCL_IO 3
#define I2C_MASTER_FREQ_HZ 1
#define I2C_MASTER_PORT I2C_NUM_0

i2c_config_t i2c_master_conf = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = I2C_MASTER_FREQ_HZ,
};

void app_main(void)
{
    i2c_param_config(I2C_MASTER_PORT, &i2c_master_conf);
    i2c_driver_install(I2C_MASTER_PORT, I2C_MODE_MASTER, 0, 0, 0);
}
