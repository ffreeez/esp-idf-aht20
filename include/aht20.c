#include "aht20.h"

#define AHT20_DEVICE_ADDR   0x38
#define AHT20_READ_ADDR     ((AHT20_DEVICE_ADDR<<1) | I2C_MASTER_READ)
#define AHT20_WRITE_ADDR    ((AHT20_DEVICE_ADDR<<1) | I2C_MASTER_WRITE)



/// @brief in order to send
/// @param i2c_port 
/// @param AHT20_data 
/// @return 
esp_err_t AHT20_read_status(i2c_port_t i2c_port, AHT20_data_t *AHT20_data)
{
    uint8_t status = 0;
    i2c_cmd_handle_t handle = 0;

    handle = i2c_cmd_link_create();
    i2c_master_start(handle);
    
    i2c_master_write_byte(handle, AHT20_READ_ADDR, true);
    i2c_master_read_byte(handle, &status, I2C_MASTER_ACK);
    i2c_master_stop(handle);
    i2c_master_cmd_begin(i2c_port, handle, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(handle);
    ESP_LOGI("ATH20_read_status", "AHT20 STATUS: %08x", status);
    return 0;
}



/// @brief send the command to the device, in order to measure the RH and the TEMP,
///        the data will be storaged in the struct of data
/// @param i2c_port the port of i2c
/// @param data the struct of AHT20_data_t
/// @return return an error code in type esp_err_t
esp_err_t AHT20_measures(i2c_port_t i2c_port, AHT20_data_t *AHT20_data)
{
    esp_err_t ret = 1;
    i2c_cmd_handle_t handle = 0;
    uint8_t data[7] = {0};



    /// @brief
    /// send measure command to the device
    handle = i2c_cmd_link_create();
    i2c_master_start(handle);

    i2c_master_write_byte(handle, AHT20_WRITE_ADDR, true);  // I2C_ADDR + WriteBit -> (00111000 << 1) | 00000001 = 0111001
    i2c_master_write_byte(handle, 0xAC, true);              // 0xAC -> It`s a command code in order to start measures
    i2c_master_write_byte(handle, 0x33, true);              // 0x33 -> It`s the first arg for the mearsure command
    i2c_master_write_byte(handle, 0x00, true);              // 0x00 -> It`s the second arg for the command

    i2c_master_stop(handle);
    i2c_master_cmd_begin(i2c_port, handle, 500 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(handle);
    ESP_LOGI("AHT20_measures", "already send measures code");

    vTaskDelay(1000 / portTICK_PERIOD_MS);



    /// @brief
    /// recv the data from device
    handle = i2c_cmd_link_create();
    i2c_master_start(handle);

    i2c_master_write_byte(handle, AHT20_READ_ADDR, true);   // I2C_ADDR + WriteBit -> (00111000 << 1) | 00000000 = 0111000
    i2c_master_read(handle, (uint8_t *)&data, 7, I2C_MASTER_ACK);
    for(uint8_t i = 0; i < 6; i++)
    {
        ESP_LOGI("AHT20_measures", "data[%d]=%08x", i, data[i]);
    }
    
    i2c_master_stop(handle);
    i2c_master_cmd_begin(i2c_port, handle, 500 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(handle);

    uint32_t rh = ( ((uint32_t)data[1] << 16) | ((uint32_t)data[2] << 8) | (data[3]) ) >> 4 ;
    uint32_t temp = ((uint32_t)(data[3]&0x0F) << 16) | ((uint32_t)data[4] << 8) | (uint32_t)data[5] ;

    AHT20_data->TEMP = (temp *(0.00019073F)-50);
    AHT20_data->RH = (rh * (0.0000953674316F));
    printf("Temperature is %3.2f\n", (float)temp);
    printf("Humidity is %3.2f\n", (float)rh);
    vTaskDelay(300 / portTICK_PERIOD_MS);
    ret = 0;
    return ret;
}
