/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : soft_i2c.c
  * @brief          : Software I2C implementation source file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "soft_i2c.h"
#include <stdio.h> // 添加此头文件以支持printf函数

/* Private function prototypes -----------------------------------------------*/
static void SoftI2C_Delay(SoftI2C_HandleTypeDef *hi2c);
static void SoftI2C_GPIO_Init(GPIO_TypeDef *port, uint16_t pin, uint32_t mode, uint32_t pull, uint32_t speed);
static void SoftI2C_SDA_Input(SoftI2C_HandleTypeDef *hi2c);
static void SoftI2C_SDA_Output(SoftI2C_HandleTypeDef *hi2c);
static uint8_t SoftI2C_SDA_Read(SoftI2C_HandleTypeDef *hi2c);

/**
  * @brief  Initialize software I2C
  * @param  hi2c: pointer to a SoftI2C_HandleTypeDef structure
  * @retval None
  */
void SoftI2C_Init(SoftI2C_HandleTypeDef *hi2c)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    /* Configure SDA pin as output open-drain */
    GPIO_InitStruct.Pin = hi2c->sda_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(hi2c->sda_port, &GPIO_InitStruct);
    
    /* Configure SCL pin as output open-drain */
    GPIO_InitStruct.Pin = hi2c->scl_pin;
    HAL_GPIO_Init(hi2c->scl_port, &GPIO_InitStruct);
    
    /* Set both lines high (idle state) */
    HAL_GPIO_WritePin(hi2c->sda_port, hi2c->sda_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, GPIO_PIN_SET);
}

/**
  * @brief  Generate I2C start condition
  * @param  hi2c: pointer to a SoftI2C_HandleTypeDef structure
  * @retval None
  */
void SoftI2C_Start(SoftI2C_HandleTypeDef *hi2c)
{
    /* SDA high, SCL high */
    HAL_GPIO_WritePin(hi2c->sda_port, hi2c->sda_pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, GPIO_PIN_SET);
    SoftI2C_Delay(hi2c);
    
    /* SDA low while SCL high */
    HAL_GPIO_WritePin(hi2c->sda_port, hi2c->sda_pin, GPIO_PIN_RESET);
    SoftI2C_Delay(hi2c);
    
    /* SCL low */
    HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, GPIO_PIN_RESET);
    SoftI2C_Delay(hi2c);
}

/**
  * @brief  Generate I2C stop condition
  * @param  hi2c: pointer to a SoftI2C_HandleTypeDef structure
  * @retval None
  */
void SoftI2C_Stop(SoftI2C_HandleTypeDef *hi2c)
{
    /* SDA low, SCL low */
    HAL_GPIO_WritePin(hi2c->sda_port, hi2c->sda_pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, GPIO_PIN_RESET);
    SoftI2C_Delay(hi2c);
    
    /* SCL high while SDA low */
    HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, GPIO_PIN_SET);
    SoftI2C_Delay(hi2c);
    
    /* SDA high while SCL high */
    HAL_GPIO_WritePin(hi2c->sda_port, hi2c->sda_pin, GPIO_PIN_SET);
    SoftI2C_Delay(hi2c);
}

/**
  * @brief  Send one byte via I2C
  * @param  hi2c: pointer to a SoftI2C_HandleTypeDef structure
  * @param  data: byte to send
  * @retval uint8_t: ACK/NACK status (0 = ACK, 1 = NACK)
  */
uint8_t SoftI2C_WriteByte(SoftI2C_HandleTypeDef *hi2c, uint8_t data)
{
    uint8_t i;
    uint8_t ack;
    
    /* Send 8 bits */
    for (i = 0; i < 8; i++)
    {
        /* Set SDA according to data bit (MSB first) */
        if (data & 0x80)
            HAL_GPIO_WritePin(hi2c->sda_port, hi2c->sda_pin, GPIO_PIN_SET);
        else
            HAL_GPIO_WritePin(hi2c->sda_port, hi2c->sda_pin, GPIO_PIN_RESET);
        
        data <<= 1;
        SoftI2C_Delay(hi2c);
        
        /* Clock high */
        HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, GPIO_PIN_SET);
        SoftI2C_Delay(hi2c);
        
        /* Clock low */
        HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, GPIO_PIN_RESET);
        SoftI2C_Delay(hi2c);
    }
    
    /* Release SDA for ACK */
    HAL_GPIO_WritePin(hi2c->sda_port, hi2c->sda_pin, GPIO_PIN_SET);
    SoftI2C_Delay(hi2c);
    
    /* Clock high for ACK */
    HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, GPIO_PIN_SET);
    SoftI2C_Delay(hi2c);
    
    /* Read ACK bit */
    SoftI2C_SDA_Input(hi2c);
    ack = SoftI2C_SDA_Read(hi2c);
    SoftI2C_SDA_Output(hi2c);
    
    /* Clock low */
    HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, GPIO_PIN_RESET);
    SoftI2C_Delay(hi2c);
    
    return ack;
}

/**
  * @brief  Read one byte via I2C
  * @param  hi2c: pointer to a SoftI2C_HandleTypeDef structure
  * @param  ack: send ACK (0) or NACK (1) after reading
  * @retval uint8_t: received byte
  */
uint8_t SoftI2C_ReadByte(SoftI2C_HandleTypeDef *hi2c, uint8_t ack)
{
    uint8_t i;
    uint8_t data = 0;
    
    /* Configure SDA as input */
    SoftI2C_SDA_Input(hi2c);
    
    /* Read 8 bits */
    for (i = 0; i < 8; i++)
    {
        data <<= 1;
        
        /* Clock high */
        HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, GPIO_PIN_SET);
        SoftI2C_Delay(hi2c);
        
        /* Read data bit */
        if (SoftI2C_SDA_Read(hi2c))
            data |= 0x01;
        
        /* Clock low */
        HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, GPIO_PIN_RESET);
        SoftI2C_Delay(hi2c);
    }
    
    /* Configure SDA as output for ACK */
    SoftI2C_SDA_Output(hi2c);
    
    /* Send ACK/NACK */
    if (ack == SOFT_I2C_ACK)
        HAL_GPIO_WritePin(hi2c->sda_port, hi2c->sda_pin, GPIO_PIN_RESET);
    else
        HAL_GPIO_WritePin(hi2c->sda_port, hi2c->sda_pin, GPIO_PIN_SET);
    
    SoftI2C_Delay(hi2c);
    
    /* Clock high for ACK */
    HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, GPIO_PIN_SET);
    SoftI2C_Delay(hi2c);
    
    /* Clock low */
    HAL_GPIO_WritePin(hi2c->scl_port, hi2c->scl_pin, GPIO_PIN_RESET);
    SoftI2C_Delay(hi2c);
    
    /* Release SDA */
    HAL_GPIO_WritePin(hi2c->sda_port, hi2c->sda_pin, GPIO_PIN_SET);
    
    return data;
}

/**
  * @brief  Write data to I2C device
  * @param  hi2c: pointer to a SoftI2C_HandleTypeDef structure
  * @param  addr: device address (7-bit)
  * @param  data: pointer to data buffer
  * @param  len: data length
  * @retval HAL_StatusTypeDef: HAL_OK if successful, HAL_ERROR otherwise
  */
HAL_StatusTypeDef SoftI2C_Write(SoftI2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t *data, uint16_t len)
{
    uint16_t i;

    /* Generate start condition */
    SoftI2C_Start(hi2c);

    /* Send device address with write bit (0) */
    if (SoftI2C_WriteByte(hi2c, (addr << 1) | 0x00) != SOFT_I2C_ACK)
    {
        SoftI2C_Stop(hi2c);
        printf("I2C Write Error: NACK received for address 0x%02X\n", addr);
        return HAL_ERROR;
    }
    
    /* Send data */
    for (i = 0; i < len; i++)
    {
        if (SoftI2C_WriteByte(hi2c, data[i]) != SOFT_I2C_ACK)
        {
            SoftI2C_Stop(hi2c);
            printf("I2C Write Error: NACK received at byte %d\n", i);
            return HAL_ERROR;
        }
    }

    /* Generate stop condition */
    SoftI2C_Stop(hi2c);

    return HAL_OK;
}

/**
  * @brief  Read data from I2C device
  * @param  hi2c: pointer to a SoftI2C_HandleTypeDef structure
  * @param  addr: device address (7-bit)
  * @param  data: pointer to data buffer
  * @param  len: data length
  * @retval HAL_StatusTypeDef: HAL_OK if successful, HAL_ERROR otherwise
  */
HAL_StatusTypeDef SoftI2C_Read(SoftI2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t *data, uint16_t len)
{
    uint16_t i;

    /* Generate start condition */
    SoftI2C_Start(hi2c);

    /* Send device address with read bit (1) */
    if (SoftI2C_WriteByte(hi2c, (addr << 1) | 0x01) != SOFT_I2C_ACK)
    {
        SoftI2C_Stop(hi2c);
        printf("I2C Read Error: NACK received for address 0x%02X\n", addr);
        return HAL_ERROR;
    }

    /* Read data */
    for (i = 0; i < len; i++)
    {
        /* Send ACK for all bytes except the last one */
        if (i < len - 1)
            data[i] = SoftI2C_ReadByte(hi2c, SOFT_I2C_ACK);
        else
            data[i] = SoftI2C_ReadByte(hi2c, SOFT_I2C_NACK);
    }

    /* Generate stop condition */
    SoftI2C_Stop(hi2c);

    return HAL_OK;
}

/**
  * @brief  Check if device is present on I2C bus
  * @param  hi2c: pointer to a SoftI2C_HandleTypeDef structure
  * @param  addr: device address (7-bit)
  * @retval HAL_StatusTypeDef: HAL_OK if device responds, HAL_ERROR otherwise
  */
HAL_StatusTypeDef SoftI2C_IsDeviceReady(SoftI2C_HandleTypeDef *hi2c, uint8_t addr)
{
    HAL_StatusTypeDef status;
    
    /* Generate start condition */
    SoftI2C_Start(hi2c);
    
    /* Send device address with write bit (0) */
    if (SoftI2C_WriteByte(hi2c, (addr << 1) | 0x00) == SOFT_I2C_ACK)
        status = HAL_OK;
    else
        status = HAL_ERROR;
    
    /* Generate stop condition */
    SoftI2C_Stop(hi2c);
    
    return status;
}

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Delay function for timing
  * @param  hi2c: pointer to a SoftI2C_HandleTypeDef structure
  * @retval None
  */
static void SoftI2C_Delay(SoftI2C_HandleTypeDef *hi2c)
{
    HAL_Delay(hi2c->delay_us / 1000); // 假设delay_us为微秒，转换为毫秒
}

/**
  * @brief  Configure GPIO
  * @param  port: GPIO port
  * @param  pin: GPIO pin
  * @param  mode: GPIO mode
  * @param  pull: GPIO pull-up/pull-down
  * @param  speed: GPIO speed
  * @retval None
  */
static void SoftI2C_GPIO_Init(GPIO_TypeDef *port, uint16_t pin, uint32_t mode, uint32_t pull, uint32_t speed)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = pin;
    GPIO_InitStruct.Mode = mode;
    GPIO_InitStruct.Pull = pull;
    GPIO_InitStruct.Speed = speed;
    HAL_GPIO_Init(port, &GPIO_InitStruct);
}

/**
  * @brief  Configure SDA as input
  * @param  hi2c: pointer to a SoftI2C_HandleTypeDef structure
  * @retval None
  */
static void SoftI2C_SDA_Input(SoftI2C_HandleTypeDef *hi2c)
{
    SoftI2C_GPIO_Init(hi2c->sda_port, hi2c->sda_pin, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH);
}

/**
  * @brief  Configure SDA as output
  * @param  hi2c: pointer to a SoftI2C_HandleTypeDef structure
  * @retval None
  */
static void SoftI2C_SDA_Output(SoftI2C_HandleTypeDef *hi2c)
{
    SoftI2C_GPIO_Init(hi2c->sda_port, hi2c->sda_pin, GPIO_MODE_OUTPUT_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH);
}

/**
  * @brief  Read SDA pin state
  * @param  hi2c: pointer to a SoftI2C_HandleTypeDef structure
  * @retval uint8_t: SDA pin state (0 or 1)
  */
static uint8_t SoftI2C_SDA_Read(SoftI2C_HandleTypeDef *hi2c)
{
    return (HAL_GPIO_ReadPin(hi2c->sda_port, hi2c->sda_pin) == GPIO_PIN_SET) ? 1 : 0;
}
