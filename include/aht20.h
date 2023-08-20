#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"

/// @brief a struce to storage the result
typedef struct AHT20
{
    float RH;
    float TEMP;
    bool busy;
} AHT20_data_t;

//////////////////////////////////////////////// PUBLIC_FUNCTION ////////////////////////////////////////////////

/// @brief send init command to device, in order to init the device
/// @param i2c_port i2c bus port
/// @return esp_err_t for result
esp_err_t AHT20_command_init(i2c_port_t i2c_port);

/// @brief send reset command to device, in order to restart the device
/// @param i2c_port i2c bus port
/// @return esp_err_t for result
esp_err_t AHT20_command_reset(i2c_port_t i2c_port);

/// @brief send measure command to device, in order to measure the temp and rh
/// @param i2c_port i2c bus port
/// @return esp_err_t for result
esp_err_t AHT20_command_measure(i2c_port_t i2c_port);

/// @brief send a read command and get the status of device
/// @param i2c_port i2c bus port
/// @param pAHT20_data a pointer to the struct of data
/// @return esp_err_t for result
esp_err_t AHT20_get_status(i2c_port_t i2c_port, AHT20_data_t *pAHT20_data);

/// @brief send a read command and get result form device
/// @param i2c_port i2c bus port
/// @param pAHT20_data a pointer to the struct of data
/// @return esp_err_t for result
esp_err_t AHT20_get_result(i2c_port_t i2c_port, AHT20_data_t *pAHT20_data);

/// @brief start a task for measure the temp and rh
/// @param i2c_port i2c bus port
/// @param delay_time the time to delay between measure twice
/// @param pAHT20_data a pointer to the struct of data
/// @return esp_err_t for result
esp_err_t AHT20_start_measure_tasks(i2c_port_t i2c_port, TickType_t delay_time, AHT20_data_t *pAHT20_data);

//////////////////////////////////////////////// PRIVATE_FUNCTION ////////////////////////////////////////////////