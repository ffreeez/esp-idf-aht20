# ESP-IDF-AHT20

一个基于ESP-IDF-v5.1的aht20驱动程序

## How to use example

首先需要声明两个结构体，一个用于初始化I2C库，另一个用于存储温湿度，以及为函数提供i2c端口
在结构体中存在了一个readable变量，这个变量在读取后会被置为1

初始化完成后，创建一个任务，需要获取温湿度值的时候，直接从结构体中获取即可

```
......
RH: 73.92; TEMP: 27.50
RH: 73.86; TEMP: 27.52
RH: 73.91; TEMP: 27.51
RH: 73.90; TEMP: 27.52
RH: 73.98; TEMP: 27.51
RH: 73.97; TEMP: 27.49
RH: 73.96; TEMP: 27.49
RH: 73.93; TEMP: 27.51
RH: 73.98; TEMP: 27.49
RH: 73.93; TEMP: 27.50
RH: 74.01; TEMP: 27.50
RH: 74.03; TEMP: 27.50
RH: 74.01; TEMP: 27.50
RH: 73.96; TEMP: 27.52
RH: 73.99; TEMP: 27.51
RH: 74.01; TEMP: 27.50
RH: 74.02; TEMP: 27.49
......
```
```C
#include "aht20.h"
#include <stdio.h>
#define I2C_MASTER_SDA_IO 17
#define I2C_MASTER_SCL_IO 16
#define I2C_MASTER_FREQ_HZ 100 * 1000


i2c_config_t conf =
{
    .mode = I2C_MODE_MASTER,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .sda_pullup_en = GPIO_PULLUP_DISABLE,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .scl_pullup_en = GPIO_PULLUP_DISABLE,
    .master.clk_speed = I2C_MASTER_FREQ_HZ,
};

AHT20_data_t AHT20_data =
{
    .RH = 0,
    .TEMP = 0
};

void aht20_setup()
{
    i2c_param_config(I2C_MASTER_PORT, &conf);
    i2c_driver_install(I2C_MASTER_PORT, I2C_MODE_MASTER, 0, 0, 0);
}

void app_main(void)
{
    aht20_setup();
    while (1)
    {
        esp_err_t ret = AHT20_measure(&AHT20_data);
        if(ret == 0)
        printf("RH: %.2f; TEMP: %.2f\n", AHT20_data.RH, AHT20_data.TEMP);
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}

```

## Example folder contents

```
├── CMakeLists.txt
├── include
│   ├── aht20.h
│   └── aht20.c
├── main
│   ├── CMakeLists.txt
│   └── main.c
└── README.md                  This is the file you are currently reading
```