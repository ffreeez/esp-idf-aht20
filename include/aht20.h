#ifndef AHT_20_H
#define AHT_20_H

#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"

struct aht20_data{
    float temperature;
    float rel_humidity;
}aht20_data;





#endif