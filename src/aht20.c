#include "aht20.h"

#define AHT20_DEVICE_ADDR 0x38
#define AHT20_READ_ADDR ((AHT20_DEVICE_ADDR << 1) | 1)
#define AHT20_WRITE_ADDR ((AHT20_DEVICE_ADDR << 1) | 0)

#define AHT20_CMD_MEASUREMENT 0xAC
#define AHT20_CMD_READ_STATUS 0x71
#define AHT20_CMD_INIT 0xBE
#define AHT20_CMD_RST 0xBA

#define AHT20_DELAY_MEASUREMENT 80
#define AHT20_DELAY_INIT 10

#define GET_BIT(byte, index) ((byte & (1 << index)) >> index)

//////////////////////////////////////////////// GLOBAL_VARIABLE ////////////////////////////////////////////////

uint8_t data[7] = {0};
uint8_t mode;
uint8_t cal;

//////////////////////////////////////////////// PRIVATE_DEFINITION ////////////////////////////////////////////////

/// @brief send init command to device, in order to init the device
/// @return esp_err_t indicates if the operation is successful or not
static esp_err_t m_AHT20_command_init(void);

/// @brief send reset command to device, in order to restart the device
/// @return esp_err_t indicates if the operation is successful or not
static esp_err_t m_AHT20_command_reset(void);

/// @brief send measure command to device
/// @return esp_err_t indicates if the operation is successful or not
static esp_err_t m_AHT20_command_measure(void);

/// @brief get the status of device
/// @param pAHT20_data a pointer to the struct of data
/// @return esp_err_t indicates if the operation is successful or not
static esp_err_t m_AHT20_decode_status(AHT20_data_t *pAHT20_data);

/// @brief send a read command and get result form device, the data will be storaged in data[]
/// @param pAHT20_data a pointer to the struct of data
/// @return esp_err_t indicates if the operation is successful or not
static esp_err_t m_AHT20_get_result(AHT20_data_t *pAHT20_data, size_t len);

//////////////////////////////////////////////// PRIVATE_DECLARATION ////////////////////////////////////////////////

/// @brief send init command to the device, in order to init the device
/// @return esp_err_t indicates if the operation is successful or not
static esp_err_t m_AHT20_command_init(void)
{
    esp_err_t ret = 1;
    i2c_cmd_handle_t handle = 0;

    handle = i2c_cmd_link_create();
    i2c_master_start(handle);

    i2c_master_write_byte(handle, AHT20_WRITE_ADDR, true);
    i2c_master_write_byte(handle, AHT20_CMD_INIT, true);
    i2c_master_write_byte(handle, 0x08, true);
    i2c_master_write_byte(handle, 0x00, true);  

    i2c_master_stop(handle);
    i2c_master_cmd_begin(I2C_MASTER_PORT, handle, 500 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(handle);
    ESP_LOGD("m_AHT20_command_init", "completed");

    ret = 0;
    return ret;
}

/// @brief send reset command to the device, in order to restart the device
/// @return esp_err_t indicates if the operation is successful or not
static esp_err_t m_AHT20_command_reset(void)
{
    esp_err_t ret = 1;
    i2c_cmd_handle_t handle = 0;

    handle = i2c_cmd_link_create();
    i2c_master_start(handle);

    i2c_master_write_byte(handle, AHT20_WRITE_ADDR, true);
    i2c_master_write_byte(handle, AHT20_CMD_RST, true); 

    i2c_master_stop(handle);
    i2c_master_cmd_begin(I2C_MASTER_PORT, handle, 500 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(handle);
    ESP_LOGD("m_AHT20_command_reset", "completed");

    ret = 0;
    return ret;
}

static esp_err_t m_AHT20_command_read_status(void){
    esp_err_t ret = 1;
    i2c_cmd_handle_t handle = 0;

    handle = i2c_cmd_link_create();

    i2c_master_start(handle);
    i2c_master_write_byte(handle, AHT20_WRITE_ADDR, true);
    i2c_master_write_byte(handle, AHT20_CMD_READ_STATUS, true);  
    i2c_master_stop(handle);

    i2c_master_cmd_begin(I2C_MASTER_PORT, handle, 500 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(handle);
    ESP_LOGD("m_AHT20_command_read_status", "completed");

    ret = 0;
    return ret;
}

/// @brief send measure command to device
/// @return esp_err_t indicates if the operation is successful or not
static esp_err_t m_AHT20_command_measure(void)
{
    esp_err_t ret = 1;
    i2c_cmd_handle_t handle = 0;

    handle = i2c_cmd_link_create();

    i2c_master_start(handle);
    i2c_master_write_byte(handle, AHT20_WRITE_ADDR, true);
    i2c_master_write_byte(handle, 0xAC, true);
    i2c_master_write_byte(handle, 0x33, true); 
    i2c_master_write_byte(handle, 0x00, true);    
    i2c_master_stop(handle);

    i2c_master_cmd_begin(I2C_MASTER_PORT, handle, 500 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(handle);
    ESP_LOGD("m_AHT20_command_measure", "completed");

    ret = 0;
    return ret;
}

/// @brief get the status of device
/// @param pAHT20_data a pointer to the struct of data
/// @return esp_err_t indicates if the operation is successful or not
static esp_err_t m_AHT20_decode_status(AHT20_data_t *pAHT20_data)
{
    esp_err_t ret = 1;

    // if bit[7] == 1, device is busy
    if (GET_BIT(data[0], 7) == 1)
        pAHT20_data->is_busy = 1;
    else
        pAHT20_data->is_busy = 0;

    // if bit[3] == 1, device has been calibrated
    if (GET_BIT(data[0], 3) == 1)
        cal = 1;
    else
        cal = 0;

    // if bit[6] == 1, device is under CMD mode
    if (GET_BIT(data[0], 6) == 1)
        mode = 2;
    // if bit[5] == 1 and bit[6] == 0, device is under CYC mode
    else if (GET_BIT(data[0], 6) == 0 || GET_BIT(data[0], 5) == 1)
        mode = 1;
    // if bit[5] == 0 and bit[6] == 0, device is under NOR mode
    else
        mode = 0;

    ESP_LOGD("m_AHT20_decode_status", "AHT20 STATUS: BUSY=%d; CAL=%d; MODE=%d; BIN:%d%d%d%d%d%d%d%d; HEX:%x",
             pAHT20_data->is_busy, cal, mode,
             GET_BIT(data[0], 0), GET_BIT(data[0], 1), GET_BIT(data[0], 2), GET_BIT(data[0], 3),
             GET_BIT(data[0], 4), GET_BIT(data[0], 5), GET_BIT(data[0], 6), GET_BIT(data[0], 7), data[0]);

    ret = 0;
    return ret;
}

/// @brief send a read command and store the response in the buffer.
/// @param pAHT20_data  
/// @return esp_err_t indicates if the operation is successful or not
static esp_err_t m_AHT20_get_result(AHT20_data_t *pAHT20_data, size_t len) {
    esp_err_t ret = 1;
    i2c_cmd_handle_t handle = 0;

    handle = i2c_cmd_link_create();
    i2c_master_start(handle);

    // PATCH START
    i2c_master_write_byte(handle, AHT20_READ_ADDR, true);
    if (len == 1){
        i2c_master_read_byte(handle, (uint8_t*) data, I2C_MASTER_NACK);
    }else {
        for (int i = 0; i < len - 1; ++i) {
            i2c_master_read_byte(handle, (uint8_t*) &data + i, I2C_MASTER_ACK);
        }
        i2c_master_read_byte(handle, (uint8_t*) &data + (len - 1), I2C_MASTER_NACK);
    }

    i2c_master_stop(handle);
    i2c_master_cmd_begin(I2C_MASTER_PORT, handle, 500 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(handle);
    ESP_LOGD("m_AHT20_get_result", "completed");

    ret = 0;
    return ret;
}

//////////////////////////////////////////////// PUBLIC_DECLARATION ////////////////////////////////////////////////

/// @brief start a measurement task
/// @param pAHT20_data 
/// @return esp_err_t indicates if the operation is successful or not
esp_err_t AHT20_measure(AHT20_data_t *pAHT20_data)
{
    esp_err_t ret = 1;
    uint32_t rh_raw, temp_raw = 0;
    float rh, temp = 0;

    m_AHT20_command_measure();
    vTaskDelay(AHT20_DELAY_MEASUREMENT / portTICK_PERIOD_MS);
    m_AHT20_get_result(pAHT20_data, 7);
    m_AHT20_decode_status(pAHT20_data);
    // continue if busy
    if (pAHT20_data->is_busy == 1)
    {
        ESP_LOGI("AHT20", "the device is busy");
        return ret;
    }

    rh_raw = (((uint32_t)data[1] << 16) | ((uint32_t)data[2] << 8) | (data[3])) >> 4;
    temp_raw = ((uint32_t)(data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | (uint32_t)data[5];

    rh = (rh_raw * (0.0000953674316F));
    temp = (temp_raw * (0.00019073F) - 50);

    if (rh > 100 || rh < 0 || temp > 120 || temp < -40)
    {
        ESP_LOGD("AHT20", "the measurement is out of bounds %f, %f", rh, temp);
        return ret;
    }

    pAHT20_data->r_humidity = rh;
    pAHT20_data->temperature = temp;

    ESP_LOGD("AHT20", "Temperature: %f, Relative Humidity: %f", pAHT20_data->temperature, pAHT20_data->r_humidity);
    ret = 0;
    return ret;
}

esp_err_t AHT20_begin(AHT20_data_t *pAHT20_data){
    vTaskDelay(40 / portTICK_PERIOD_MS);
    m_AHT20_command_reset();
    vTaskDelay(40 / portTICK_PERIOD_MS);
    m_AHT20_command_read_status();
    m_AHT20_get_result(pAHT20_data, 1);
    m_AHT20_decode_status(pAHT20_data);
    ESP_LOGD("AHT20", "begin: calibrated=%d", cal);

    if(cal){
        return ESP_OK;
    }

    ESP_LOGD("AHT20", "begin: init");
    m_AHT20_command_init();
    vTaskDelay(AHT20_DELAY_INIT / portTICK_PERIOD_MS);

    while (!cal){
        vTaskDelay(40 / portTICK_PERIOD_MS);
        m_AHT20_command_read_status();
        m_AHT20_get_result(pAHT20_data, 1);
        m_AHT20_decode_status(pAHT20_data);
        ESP_LOGD("AHT20", "begin: calibrated=%d", cal);
    }
    return cal ? ESP_OK: -1;
}