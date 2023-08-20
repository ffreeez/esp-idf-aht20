#include "aht20.h"

#define AHT20_DEVICE_ADDR 0x38
#define AHT20_READ_ADDR ((AHT20_DEVICE_ADDR << 1) | I2C_MASTER_READ)
#define AHT20_WRITE_ADDR ((AHT20_DEVICE_ADDR << 1) | I2C_MASTER_WRITE)

//////////////////////////////////////////////// PUBLIC_FUNCTION ////////////////////////////////////////////////

esp_err_t AHT20_command_init(i2c_port_t i2c_port)
{
    esp_err_t ret = 1;
    i2c_cmd_handle_t handle = 0;

    vTaskDelay(200 / portTICK_PERIOD_MS);

    handle = i2c_cmd_link_create();
    i2c_master_start(handle);

    i2c_master_write_byte(handle, AHT20_WRITE_ADDR, true); // I2C_ADDR + WriteBit -> (00111000 << 1) | 00000001 = 0111001
    i2c_master_write_byte(handle, 0xBE, true);             // 0xBE -> It`s a command code in order to init measures
    i2c_master_write_byte(handle, 0x33, true);             // 0x33 -> It`s the first arg for the mearsure command
    i2c_master_write_byte(handle, 0x00, true);             // 0x00 -> It`s the second arg for the command

    i2c_master_stop(handle);
    i2c_master_cmd_begin(i2c_port, handle, 500 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(handle);
    ESP_LOGI("AHT20_command_init", "already send INIT code");

    ret = 0;
    return ret;
}

esp_err_t AHT20_command_reset(i2c_port_t i2c_port)
{
    esp_err_t ret = 1;
    i2c_cmd_handle_t handle = 0;

    vTaskDelay(200 / portTICK_PERIOD_MS);

    handle = i2c_cmd_link_create();
    i2c_master_start(handle);

    i2c_master_write_byte(handle, AHT20_WRITE_ADDR, true); // I2C_ADDR + WriteBit -> (00111000 << 1) | 00000001 = 0111001
    i2c_master_write_byte(handle, 0xBA, true);             // 0xBA -> It`s a command code in order to reset

    i2c_master_stop(handle);
    i2c_master_cmd_begin(i2c_port, handle, 500 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(handle);
    ESP_LOGI("AHT20_command_reset", "already send RESET code");

    ret = 0;
    return ret;
}

esp_err_t AHT20_command_measure(i2c_port_t i2c_port)
{
    esp_err_t ret = 1;
    i2c_cmd_handle_t handle = 0;

    vTaskDelay(200 / portTICK_PERIOD_MS);

    handle = i2c_cmd_link_create();
    i2c_master_start(handle);

    i2c_master_write_byte(handle, AHT20_WRITE_ADDR, true); // I2C_ADDR + WriteBit -> (00111000 << 1) | 00000001 = 0111001
    i2c_master_write_byte(handle, 0xAC, true);             // 0xAC -> It`s a command code in order to start measures
    i2c_master_write_byte(handle, 0x33, true);             // 0x33 -> It`s the first arg for the mearsure command
    i2c_master_write_byte(handle, 0x00, true);             // 0x00 -> It`s the second arg for the command

    i2c_master_stop(handle);
    i2c_master_cmd_begin(i2c_port, handle, 500 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(handle);
    ESP_LOGI("AHT20_command_measure", "already send MEASURES code");

    ret = 0;
    return ret;
}

esp_err_t AHT20_get_status(i2c_port_t i2c_port, AHT20_data_t *pAHT20_data)
{
    esp_err_t ret = 1;
    i2c_cmd_handle_t handle = 0;
    uint8_t status_code = 0;

    vTaskDelay(200 / portTICK_PERIOD_MS);

    handle = i2c_cmd_link_create();
    i2c_master_start(handle);

    i2c_master_write_byte(handle, AHT20_READ_ADDR, true); // I2C_ADDR + WriteBit -> (00111000 << 1) | 00000001 = 0111001
    i2c_master_write_byte(handle, 0x71, true);            // 0x71 -> It`s a command code in order to reset
    i2c_master_read_byte(handle, &status_code, I2C_MASTER_ACK);

    i2c_master_stop(handle);
    i2c_master_cmd_begin(i2c_port, handle, 500 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(handle);
    ESP_LOGI("AHT20_get_status", "AHT20 STATUS: %08x", status_code);

    ret = 0;
    return ret;
}

esp_err_t AHT20_get_result(i2c_port_t i2c_port, AHT20_data_t *pAHT20_data)
{
    esp_err_t ret = 1;
    i2c_cmd_handle_t handle = 0;
    uint8_t data[7] = {0};

    vTaskDelay(200 / portTICK_PERIOD_MS);

    handle = i2c_cmd_link_create();
    i2c_master_start(handle);

    i2c_master_write_byte(handle, AHT20_READ_ADDR, true); // I2C_ADDR + WriteBit -> (00111000 << 1) | 00000000 = 0111000
    i2c_master_read(handle, (uint8_t *)&data, 7, I2C_MASTER_ACK);

    i2c_master_stop(handle);
    i2c_master_cmd_begin(i2c_port, handle, 500 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(handle);

    uint32_t rh = (((uint32_t)data[1] << 16) | ((uint32_t)data[2] << 8) | (data[3])) >> 4;
    uint32_t temp = ((uint32_t)(data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | (uint32_t)data[5];

    pAHT20_data->TEMP = (temp * (0.00019073F) - 50);
    pAHT20_data->RH = (rh * (0.0000953674316F));
    ESP_LOGI("AHT20_get_result", "TEMP: %f; RH: %f", pAHT20_data->TEMP, pAHT20_data->RH);

    ret = 0;
    return ret;
}

esp_err_t AHT20_start_measure_tasks(i2c_port_t i2c_port, TickType_t delay_time, AHT20_data_t *pAHT20_data)
{
    esp_err_t ret = 1;

    ret = 0;
    return ret;
}

//////////////////////////////////////////////// PRIVATE_FUNCTION ////////////////////////////////////////////////