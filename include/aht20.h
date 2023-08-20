#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"

typedef struct AHT20
{
    float RH;
    float TEMP;
} AHT20_data_t;

esp_err_t AHT20_read(i2c_port_t i2c_port);
float AHT20_get_RH();
float AHT20_get_TEMP();

