# ESP-IDF-AHT20

一个基于ESP-IDF-v5.1的aht20驱动程序

## How to use example

首先需要声明两个结构体，一个用于初始化I2C库，另一个用于存储温湿度，以及为函数提供i2c端口
在结构体中存在了一个readable变量，这个变量在读取后会被置为1

初始化完成后，创建一个任务，需要获取温湿度值的时候，直接从结构体中获取即可

```
......
RH: 37.418938; TEMP: 34.059860
RH: 44.471741; TEMP: 33.962402
RH: 50.339413; TEMP: 33.685455
RH: 47.214031; TEMP: 33.440941
RH: 45.234871; TEMP: 33.282448
RH: 43.256283; TEMP: 33.120132
RH: 42.260742; TEMP: 32.969650
RH: 41.846943; TEMP: 32.840332
RH: 41.674995; TEMP: 32.714455
RH: 42.000866; TEMP: 32.541267
RH: 41.903973; TEMP: 32.424736
RH: 41.949940; TEMP: 32.277298
RH: 42.407227; TEMP: 32.168587
......
```
```C
#include "aht20.h"

#define I2C_MASTER_SDA_IO 5
#define I2C_MASTER_SCL_IO 4
#define I2C_MASTER_FREQ_HZ 100 * 1000
#define I2C_MASTER_PORT 0

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
        .TEMP = 0,
        .readable = 0,                  // the variable will be set 1 after measure, you can make it zero after read
        .i2c_port = I2C_MASTER_PORT     // i2c_port shouldn`t be changed during running
};

void app_main(void)
{
    i2c_param_config(I2C_MASTER_PORT, &conf);
    i2c_driver_install(I2C_MASTER_PORT, I2C_MODE_MASTER, 0, 0, 0);

    TaskHandle_t *AHT20_task_handle = 0;
    xTaskCreate((TaskFunction_t)AHT20_task, "AHT20_task", 4096, &AHT20_data, 0, AHT20_task_handle);
    while(1)
    {
        if(AHT20_data.readable == true)
        {
                printf("RH: %f; TEMP: %f\n", AHT20_data.RH, AHT20_data.TEMP);
                AHT20_data.readable = false;
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
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