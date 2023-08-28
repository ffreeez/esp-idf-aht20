#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"

#define I2C_MASTER_PORT 0
/// @brief a struce to storage the result
typedef struct AHT20
{
    float RH;
    float TEMP;
} AHT20_data_t;

//////////////////////////////////////////////// PUBLIC_DEFINITION ////////////////////////////////////////////////

/// @brief measure the temp and rh
/// @param pAHT20_data pointer of struct
/// @return esp_err_t for result
esp_err_t AHT20_measure(AHT20_data_t *pAHT20_data);