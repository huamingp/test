# 软件I2C实现说明

## 概述
这是一个用C语言实现的软件I2C（Software I2C）库，通过GPIO模拟I2C协议，适用于STM32F1系列微控制器。

## 特性
- 完整的I2C协议实现（起始条件、停止条件、数据发送/接收、ACK/NACK处理）
- 支持标准模式（100kHz）和快速模式（400kHz）
- 灵活的GPIO配置
- 与STM32 HAL库兼容的API设计

## 文件结构
```
Core/Inc/soft_i2c.h    - 软件I2C头文件
Core/Src/soft_i2c.c    - 软件I2C实现文件
Core/Src/main.c        - 示例使用代码
```

## API函数说明

### 初始化函数
```c
void SoftI2C_Init(SoftI2C_HandleTypeDef *hi2c);
```
初始化软件I2C，配置GPIO引脚。

### 基本操作函数
```c
void SoftI2C_Start(SoftI2C_HandleTypeDef *hi2c);
void SoftI2C_Stop(SoftI2C_HandleTypeDef *hi2c);
uint8_t SoftI2C_WriteByte(SoftI2C_HandleTypeDef *hi2c, uint8_t data);
uint8_t SoftI2C_ReadByte(SoftI2C_HandleTypeDef *hi2c, uint8_t ack);
```

### 高级操作函数
```c
HAL_StatusTypeDef SoftI2C_Write(SoftI2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t *data, uint16_t len);
HAL_StatusTypeDef SoftI2C_Read(SoftI2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t *data, uint16_t len);
HAL_StatusTypeDef SoftI2C_IsDeviceReady(SoftI2C_HandleTypeDef *hi2c, uint8_t addr);
```

## 使用示例

### 1. 初始化软件I2C
```c
SoftI2C_HandleTypeDef hi2c;

// 配置I2C参数
hi2c.sda_port = GPIOA;
hi2c.sda_pin = GPIO_PIN_9;
hi2c.scl_port = GPIOA;
hi2c.scl_pin = GPIO_PIN_10;
hi2c.delay_us = 10;  // 10us延迟对应100kHz

// 初始化
SoftI2C_Init(&hi2c);
```

### 2. 检测设备是否存在
```c
uint8_t device_addr = 0x50;  // 设备地址（7位）
if (SoftI2C_IsDeviceReady(&hi2c, device_addr) == HAL_OK) {
    // 设备响应
} else {
    // 设备无响应
}
```

### 3. 写入数据
```c
uint8_t data[] = {0x00, 0x12, 0x34};  // 要写入的数据
if (SoftI2C_Write(&hi2c, 0x50, data, 3) == HAL_OK) {
    // 写入成功
} else {
    // 写入失败
}
```

### 4. 读取数据
```c
uint8_t read_data[2];
if (SoftI2C_Read(&hi2c, 0x50, read_data, 2) == HAL_OK) {
    // 读取成功，数据在read_data中
} else {
    // 读取失败
}
```

## 时序配置

### 标准模式（100kHz）
- `delay_us = 5` （5us延迟）

### 快速模式（400kHz）
- `delay_us = 1` （1us延迟）

### 低速模式（10kHz）
- `delay_us = 50` （50us延迟）

## 注意事项

1. **GPIO配置**：确保使用的GPIO引脚配置为开漏输出模式，并启用上拉电阻
2. **时序精度**：`delay_us`参数影响I2C通信速度，需要根据MCU时钟频率调整
3. **中断处理**：软件I2C操作期间应避免被高优先级中断打断
4. **多任务环境**：在多任务系统中需要添加互斥锁保护

## 常见问题

### Q: 为什么设备检测失败？
A: 检查以下问题：
- 设备地址是否正确（7位地址）
- SDA和SCL线路连接是否正确
- 上拉电阻是否连接（通常4.7kΩ）
- 时序参数是否合适

### Q: 如何提高通信速度？
A: 减小`delay_us`参数，但需要确保设备支持更快的通信速度。

### Q: 软件I2C和硬件I2C有什么区别？
A: 
- 软件I2C：通过GPIO模拟，灵活但占用CPU资源
- 硬件I2C：使用专用外设，效率高但不灵活

## 兼容性
- STM32F1系列微控制器
- 需要STM32 HAL库支持
- 理论上可移植到其他ARM Cortex-M系列MCU

## 许可证
本项目基于STM32 HAL库的许可证条款。
