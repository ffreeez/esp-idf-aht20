#include "include/aht20.h"

#define AHT20_DEVICE_ADDR   0x38
#define AHT20_READ_ADDR     ((0x38<<1) | I2C_MASTER_READ)
#define AHT20_WRITE_ADDR    ((0x38<<1) | I2C_MASTER_WRITE)

extern AHT20_data_t aht20_data;

esp_err_t AHT20_read(i2c_port_t i2c_port)
{
    esp_err_t ret = 1;
    i2c_cmd_handle_t handle;
    handle = i2c_cmd_link_create();
    i2c_master_start(handle);

    ESP_ERROR_CHECK(i2c_master_write_byte(handle, AHT20_WRITE_ADDR, true));
    ESP_ERROR_CHECK(i2c_master_read_byte(handle, AHT20_READ_ADDR, I2C_MASTER_ACK));

    i2c_master_stop(handle);
    i2c_master_cmd_begin(i2c_port, handle, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(handle);
}

float AHT20_get_RH()
{
    return aht20_data.RH;
}

float AHT20_get_TEMP()
{
    return aht20_data.TEMP;
}