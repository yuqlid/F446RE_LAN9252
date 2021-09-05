/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "esc_hw.h"
#include "ecat_slv.h"
#include "utypes.h"
#include "xprintf.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
uint32_t defaultTaskBuffer[ 128 ];
osStaticThreadDef_t defaultTaskControlBlock;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .cb_mem = &defaultTaskControlBlock,
  .cb_size = sizeof(defaultTaskControlBlock),
  .stack_mem = &defaultTaskBuffer[0],
  .stack_size = sizeof(defaultTaskBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for soes */
osThreadId_t soesHandle;
uint32_t soesBuffer[ 1024 ];
osStaticThreadDef_t soesControlBlock;
const osThreadAttr_t soes_attributes = {
  .name = "soes",
  .cb_mem = &soesControlBlock,
  .cb_size = sizeof(soesControlBlock),
  .stack_mem = &soesBuffer[0],
  .stack_size = sizeof(soesBuffer),
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void soesTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of soes */
  soesHandle = osThreadNew(soesTask, NULL, &soes_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  static uint16_t data = 0;
  static float dataf = 0.0f;
  /* Infinite loop */
  for(;;)
  {
    /*
    osDelay(200);
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    xprintf("hello %5d, %3.1f\r\n",data++, dataf);
    dataf += 0.1f;
    */
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_soesTask */
/**
* @brief Function implementing the soes thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_soesTask */
void soesTask(void *argument)
{
  /* USER CODE BEGIN soesTask */

  static esc_cfg_t config =
  {
     .user_arg = "/dev/lan9252",
     .use_interrupt = 0,
     .watchdog_cnt = 65000,
     .set_defaults_hook = NULL,
     .pre_state_change_hook = NULL,
     .post_state_change_hook = NULL,
     .application_hook = NULL,
     .safeoutput_override = NULL,
     .pre_object_download_hook = NULL,
     .post_object_download_hook = NULL,
     .rxpdo_override = NULL,
     .txpdo_override = NULL,
     .esc_hw_interrupt_enable = NULL,
     .esc_hw_interrupt_disable = NULL,
     .esc_hw_eep_handler = NULL,
     .esc_check_dc_handler = NULL,
  };

  volatile uint32_t value = 0;
  volatile uint16_t value16 = 0;
  volatile uint8_t value8 = 0;

  spi_select(1);
  ESC_reset();
  spi_unselect(1);

  do{
    value = lan9252_read_32(ESC_BYTE_TEST_REG);
    osDelay(10);
  }while(value != BYTE_TEST);

  value = lan9252_read_32(ESC_ID_REV_REG);
  xprintf("Chip ID : %x\n", value >> 16);
  xprintf("Chip Rev: %d\n", value & 0xFFFF);

  ESC_read(0xE08, &value, 4);
  xprintf("VenderID: %04X\n", value);

  ESC_read(0xE00, &value, 4);
  xprintf("ProductID: %04X\n", value);

  ESC_read(0x000, &value8, 1);
  xprintf("Controller: %04X\n", value8);

  ESC_read(0x001, &value16, 2);
  xprintf("Revision: %04X\n", value16);

  ESC_read(0x004, &value8, 1);
  xprintf("FMMU: %X\n", value8);

  ESC_read(0x005, &value8, 1);
  xprintf("SyncManager: %X\n", value8);


  ecat_slv_init(&config);
  /* Infinite loop */
  for(;;)
  {
    //osDelay(1);
    ecat_slv();
  }
  /* USER CODE END soesTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
