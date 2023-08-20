#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"

typedef struct AHT20
{
    float RH;
    float TEMP;
    uint8_t status;
} AHT20_data_t;

esp_err_t AHT20_measures(i2c_port_t i2c_port, AHT20_data_t *AHT20_data);
esp_err_t AHT20_read_status(i2c_port_t i2c_port, AHT20_data_t *AHT20_data);
