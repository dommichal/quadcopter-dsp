/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_15
#define LED_GPIO_Port GPIOC
#define VBAT_Pin GPIO_PIN_0
#define VBAT_GPIO_Port GPIOC
#define BMP_DI_Pin GPIO_PIN_1
#define BMP_DI_GPIO_Port GPIOC
#define BMP_DO_Pin GPIO_PIN_2
#define BMP_DO_GPIO_Port GPIOC
#define BMP_CSN_Pin GPIO_PIN_3
#define BMP_CSN_GPIO_Port GPIOC
#define BEEPER_Pin GPIO_PIN_4
#define BEEPER_GPIO_Port GPIOA
#define ICM_CLK_Pin GPIO_PIN_5
#define ICM_CLK_GPIO_Port GPIOA
#define ICM_DO_Pin GPIO_PIN_6
#define ICM_DO_GPIO_Port GPIOA
#define ICM_DI_Pin GPIO_PIN_7
#define ICM_DI_GPIO_Port GPIOA
#define ICM_INT1_Pin GPIO_PIN_4
#define ICM_INT1_GPIO_Port GPIOC
#define ICM_INT2_Pin GPIO_PIN_0
#define ICM_INT2_GPIO_Port GPIOB
#define ICM_CS_Pin GPIO_PIN_1
#define ICM_CS_GPIO_Port GPIOB
#define MEM_CLK_Pin GPIO_PIN_2
#define MEM_CLK_GPIO_Port GPIOB
#define BMP_CLK_Pin GPIO_PIN_13
#define BMP_CLK_GPIO_Port GPIOB
#define MEM_DI_Pin GPIO_PIN_9
#define MEM_DI_GPIO_Port GPIOC
#define USB_N_Pin GPIO_PIN_11
#define USB_N_GPIO_Port GPIOA
#define USB_P_Pin GPIO_PIN_12
#define USB_P_GPIO_Port GPIOA
#define MEM_DO_Pin GPIO_PIN_10
#define MEM_DO_GPIO_Port GPIOC
#define MEM_CSN_Pin GPIO_PIN_6
#define MEM_CSN_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_7
#define SDA_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_8
#define SCL_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
