#include "aht20.h"

#define AHT20_DEVICE_ADDR 0x38
#define AHT20_READ_ADDR ((AHT20_DEVICE_ADDR << 1) | 1)
#define AHT20_WRITE_ADDR ((AHT20_DEVICE_ADDR << 1) | 0)


#define GET_BIT(byte, index) ((byte & (1 << index)) >> index)

//////////////////////////////////////////////// GLOBAL_VARIABLE ////////////////////////////////////////////////

uint8_t data[7] = {0};
uint8_t busy;
uint8_t mode;
uint8_t cal;

//////////////////////////////////////////////// PRIVATE_DEFINITION ////////////////////////////////////////////////

/// @brief send init command to device, in order to init the device
/// @return esp_err_t for result
static esp_err_t m_AHT20_command_init(void);

/// @brief send reset command to device, in order to restart the device
/// @return esp_err_t for result
static esp_err_t m_AHT20_command_reset(void);

/// @brief send measure command to device
/// @return esp_err_t for result
static esp_err_t m_AHT20_command_measure(void);

/// @brief get the status of device
/// @param pAHT20_data a pointer to the struct of data
/// @return esp_err_t for result
static esp_err_t m_AHT20_get_status(AHT20_data_t *pAHT20_data);

/// @brief send a read command and get result form device, the data will be storaged in data[]
/// @param pAHT20_data a pointer to the struct of data
/// @return esp_err_t for result
static esp_err_t m_AHT20_get_result(AHT20_data_t *pAHT20_data);

//////////////////////////////////////////////// PRIVATE_DECLARATION ////////////////////////////////////////////////

/// @brief send init command to device, in order to init the device
/// @return esp_err_t for result
static esp_err_t m_AHT20_command_init(void)
{
    esp_err_t ret = 1;
    i2c_cmd_handle_t handle = 0;

    handle = i2c_cmd_link_create();
    i2c_master_start(handle);

    i2c_master_write_byte(handle, AHT20_WRITE_ADDR, true); // I2C_ADDR + WriteBit -> (00111000 << 1) | 00000001 = 0111001
    i2c_master_write_byte(handle, 0xBE, true);             // 0xBE -> It`s a command code in order to init measures
    i2c_master_write_byte(handle, 0x08, true);             // 0x08 -> It`s the first arg for the mearsure command
    i2c_master_write_byte(handle, 0x00, true);             // 0x00 -> It`s the second arg for the command

    i2c_master_stop(handle);
    i2c_master_cmd_begin(I2C_MASTER_PORT, handle, 500 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(handle);
    ESP_LOGD("m_AHT20_command_init", "already send INIT code");

    ret = 0;
    return ret;
}

/// @brief send reset command to device, in order to restart the device
/// @return esp_err_t for result
static esp_err_t m_AHT20_command_reset(void)
{
    esp_err_t ret = 1;
    i2c_cmd_handle_t handle = 0;

    handle = i2c_cmd_link_create();
    i2c_master_start(handle);

    i2c_master_write_byte(handle, AHT20_WRITE_ADDR, true); // I2C_ADDR + WriteBit -> (00111000 << 1) | 00000001 = 0111001
    i2c_master_write_byte(handle, 0xBA, true);             // 0xBA -> It`s a command code in order to reset

    i2c_master_stop(handle);
    i2c_master_cmd_begin(I2C_MASTER_PORT, handle, 500 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(handle);
    ESP_LOGD("m_AHT20_command_reset", "already send RESET code");

    ret = 0;
    return ret;
}

/// @brief send measure command to device
/// @return esp_err_t for result
static esp_err_t m_AHT20_command_measure(void)
{
    esp_err_t ret = 1;
    i2c_cmd_handle_t handle = 0;

    handle = i2c_cmd_link_create();
    i2c_master_start(handle);

    i2c_master_write_byte(handle, AHT20_WRITE_ADDR, true); // I2C_ADDR + WriteBit -> (00111000 << 1) | 00000001 = 0111001
    i2c_master_write_byte(handle, 0xAC, true);             // 0xAC -> It`s a command code in order to start measures
    i2c_master_write_byte(handle, 0x33, true);             // 0x33 -> It`s the first arg for the mearsure command
    i2c_master_write_byte(handle, 0x00, true);             // 0x00 -> It`s the second arg for the command

    i2c_master_stop(handle);
    i2c_master_cmd_begin(I2C_MASTER_PORT, handle, 500 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(handle);
    ESP_LOGD("m_AHT20_command_measure", "already send MEASURES code");

    ret = 0;
    return ret;
}

/// @brief get the status of device
/// @param pAHT20_data a pointer to the struct of data
/// @return esp_err_t for result
static esp_err_t m_AHT20_get_status(AHT20_data_t *pAHT20_data)
{
    esp_err_t ret = 1;

    // if bit[7] == 1, device is busy
    if (GET_BIT(data[0], 7) == 1)
        busy = 1;
    else
        busy = 0;

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

    ESP_LOGD("m_AHT20_get_status", "AHT20 STATUS: BUSY=%d; CAL=%d; MODE=%d; BIN:%d%d%d%d%d%d%d%d; HEX:%x",
             busy, cal, mode,
             GET_BIT(data[0], 0), GET_BIT(data[0], 1), GET_BIT(data[0], 2), GET_BIT(data[0], 3),
             GET_BIT(data[0], 4), GET_BIT(data[0], 5), GET_BIT(data[0], 6), GET_BIT(data[0], 7), data[0]);

    ret = 0;
    return ret;
}

/// @brief send a read command and get result form device, the data will be storaged in data[]
/// @param pAHT20_data a pointer to the struct of data
/// @return esp_err_t for result
static esp_err_t m_AHT20_get_result(AHT20_data_t *pAHT20_data)
{
    esp_err_t ret = 1;
    i2c_cmd_handle_t handle = 0;

    handle = i2c_cmd_link_create();
    i2c_master_start(handle);

    i2c_master_write_byte(handle, AHT20_READ_ADDR, true); // I2C_ADDR + WriteBit -> (00111000 << 1) | 00000000 = 0111000
    i2c_master_read(handle, (uint8_t *)&data, 7, I2C_MASTER_ACK);

    i2c_master_stop(handle);
    i2c_master_cmd_begin(I2C_MASTER_PORT, handle, 500 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(handle);
    ESP_LOGD("m_AHT20_get_result", "already get the result");

    ret = 0;
    return ret;
}

//////////////////////////////////////////////// PUBLIC_DECLARATION ////////////////////////////////////////////////

/// @brief measure the temp and rh
/// @param pAHT20_data pointer of struct
/// @return esp_err_t for result
esp_err_t AHT20_measure(AHT20_data_t *pAHT20_data)
{
    esp_err_t ret = 1;
    uint32_t rh, temp = 0;
    float rh_buf, temp_buf = 0;

    vTaskDelay(40 / portTICK_PERIOD_MS);
    m_AHT20_command_measure();
    vTaskDelay(75 / portTICK_PERIOD_MS);
    m_AHT20_get_result(pAHT20_data);
    m_AHT20_get_status(pAHT20_data);

    // continue if busy
    if (busy == 1)
    {
        ESP_LOGI("AHT20_task", "DEVICE IS BUSY");
        return ret;
    }

    // if cal == 0, reset the device
    if (cal == 0)
    {
        vTaskDelay(40 / portTICK_PERIOD_MS);
        m_AHT20_command_reset();
        vTaskDelay(40 / portTICK_PERIOD_MS);
        m_AHT20_command_init();
        vTaskDelay(40 / portTICK_PERIOD_MS);
        ESP_LOGI("AHT20_task", "NOT CAL OR DEVICE IS NOT CONNECTED");
        return ret;
    }

    rh = (((uint32_t)data[1] << 16) | ((uint32_t)data[2] << 8) | (data[3])) >> 4;
    temp = ((uint32_t)(data[3] & 0x0F) << 16) | ((uint32_t)data[4] << 8) | (uint32_t)data[5];

    rh_buf = (rh * (0.0000953674316F));
    temp_buf = (temp * (0.00019073F) - 50);

    // check the measurement results and reset the device if it is out of range
    if (pAHT20_data->RH == rh_buf || pAHT20_data->TEMP == temp_buf || rh_buf > 85 || rh_buf < 15 || temp_buf > 95 || temp_buf < -18)
    {
        vTaskDelay(40 / portTICK_PERIOD_MS);
        m_AHT20_command_reset();
        vTaskDelay(40 / portTICK_PERIOD_MS);
        m_AHT20_command_init();
        vTaskDelay(40 / portTICK_PERIOD_MS);
        ESP_LOGI("AHT20_task", "OUT OF RANGE");
        return ret;
    }

    pAHT20_data->RH = rh_buf;
    pAHT20_data->TEMP = temp_buf;

    ESP_LOGI("AHT20_task", "TEMP: %f; RH: %f", pAHT20_data->TEMP, pAHT20_data->RH);
    ret = 0;
    return ret;
}