/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : soft_i2c.h
  * @brief          : Software I2C implementation header file
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

#ifndef __SOFT_I2C_H
#define __SOFT_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_def.h"
/* Exported types ------------------------------------------------------------*/
typedef struct {
    GPIO_TypeDef* sda_port;
    uint16_t sda_pin;
    GPIO_TypeDef* scl_port;
    uint16_t scl_pin;
    uint32_t delay_us;
} SoftI2C_HandleTypeDef;

/* Exported constants --------------------------------------------------------*/
#define SOFT_I2C_ACK     0
#define SOFT_I2C_NACK    1

/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/

/**
  * @brief  Initialize software I2C
  * @param  hi2c: pointer to a SoftI2C_HandleTypeDef structure
  * @retval None
  */
void SoftI2C_Init(SoftI2C_HandleTypeDef *hi2c);

/**
  * @brief  Generate I2C start condition
  * @param  hi2c: pointer to a SoftI2C_HandleTypeDef structure
  * @retval None
  */
void SoftI2C_Start(SoftI2C_HandleTypeDef *hi2c);

/**
  * @brief  Generate I2C stop condition
  * @param  hi2c: pointer to a SoftI2C_HandleTypeDef structure
  * @retval None
  */
void SoftI2C_Stop(SoftI2C_HandleTypeDef *hi2c);

/**
  * @brief  Send one byte via I2C
  * @param  hi2c: pointer to a SoftI2C_HandleTypeDef structure
  * @param  data: byte to send
  * @retval uint8_t: ACK/NACK status (0 = ACK, 1 = NACK)
  */
uint8_t SoftI2C_WriteByte(SoftI2C_HandleTypeDef *hi2c, uint8_t data);

/**
  * @brief  Read one byte via I2C
  * @param  hi2c: pointer to a SoftI2C_HandleTypeDef structure
  * @param  ack: send ACK (0) or NACK (1) after reading
  * @retval uint8_t: received byte
  */
uint8_t SoftI2C_ReadByte(SoftI2C_HandleTypeDef *hi2c, uint8_t ack);

/**
  * @brief  Write data to I2C device
  * @param  hi2c: pointer to a SoftI2C_HandleTypeDef structure
  * @param  addr: device address (7-bit)
  * @param  data: pointer to data buffer
  * @param  len: data length
  * @retval HAL_StatusTypeDef: HAL_OK if successful, HAL_ERROR otherwise
  */
HAL_StatusTypeDef SoftI2C_Write(SoftI2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t *data, uint16_t len);

/**
  * @brief  Read data from I2C device
  * @param  hi2c: pointer to a SoftI2C_HandleTypeDef structure
  * @param  addr: device address (7-bit)
  * @param  data: pointer to data buffer
  * @param  len: data length
  * @retval HAL_StatusTypeDef: HAL_OK if successful, HAL_ERROR otherwise
  */
HAL_StatusTypeDef SoftI2C_Read(SoftI2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t *data, uint16_t len);

/**
  * @brief  Check if device is present on I2C bus
  * @param  hi2c: pointer to a SoftI2C_HandleTypeDef structure
  * @param  addr: device address (7-bit)
  * @retval HAL_StatusTypeDef: HAL_OK if device responds, HAL_ERROR otherwise
  */
HAL_StatusTypeDef SoftI2C_IsDeviceReady(SoftI2C_HandleTypeDef *hi2c, uint8_t addr);

#ifdef __cplusplus
}
#endif

#endif /* __SOFT_I2C_H */
