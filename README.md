# ESP-IDF-AHT20

一个基于ESP-IDF-v5.1的aht20驱动程序

## How to use example

首先需要声明两个结构体，一个用于初始化I2C库，另一个用于存储温湿度。

```C
#include "aht20.h"

#define I2C_MASTER_SDA_IO 3
#define I2C_MASTER_SCL_IO 4
#define I2C_MASTER_FREQ_HZ 400 * 1000
#define I2C_MASTER_PORT I2C_NUM_0

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
        .busy = 0
};

void app_main(void)
{
    i2c_param_config(I2C_MASTER_PORT, &conf);
    i2c_driver_install(I2C_MASTER_PORT, I2C_MODE_MASTER, 0, 0, 0);
    AHT20_command_init(I2C_NUM_0);
    while (1)
    {
        AHT20_command_reset(I2C_NUM_0);
        AHT20_command_measure(I2C_NUM_0);
        AHT20_get_result(I2C_NUM_0, &AHT20_data);
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

## TODO

1. 完成ATH20_get_status函数
2. 完成AHT20_start_measure_tasks函数
3. 补全中文注释与英文README