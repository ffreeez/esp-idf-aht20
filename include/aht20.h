#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"

/// @brief a struce to storage the result
typedef struct AHT20
{
    float RH;
    float TEMP;
    bool readable;  // the variable will be set 1 after measure, you can make it zero after read
    i2c_port_t i2c_port;
} AHT20_data_t;

/// @brief begin a task for aht11 in order to measure the temp and rh
/// @param pAHT20_data pointer of struct
/// @return esp_err_t for result
void AHT20_task(AHT20_data_t *pAHT20_data);