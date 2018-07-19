/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     
#include "gpio.h"
#include "nand_flash_io.h"
#include "log.h"
#define  LOG_MODULE_LEVEL    LOG_LEVEL_DEBUG
#define  LOG_MODULE_NAME     "freertos"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;

/* USER CODE BEGIN Variables */
extern nand_flash_hal_io_t nand_hal_io;
/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */
 uint32_t log_time(void)
{
return osKernelSysTick();
}
/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

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

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 256);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();

  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  uint8_t id[5],cmd[3];
  uint8_t *buff=0;
  int status;
  nand_flash_register_hal_io(&nand_hal_io);
  nand_flash_init();


  buff = pvPortMalloc(2112);
  if(buff == 0){
  log_error("malloc error.\r\n");
  }else{
  log_info("malloc ok.\r\n");
  }
  log_debug("wait cmd...\r\n");
  for(;;)
  {
   status =SEGGER_RTT_Read(0,cmd,3);
   if(status == 3){
   switch(cmd[0])
   {
   case 'e':
   log_debug("erase block :%d.\r\n",cmd[1]-'0');
   status = nand_flash_block_erase(cmd[1]-'0');
   if(status == -1){
   log_error("block erase error.\r\n");
   }else{
   log_info("block erase ok.\r\n");
   }
   break;
   
   case 'p':
   log_debug("program page :%d.\r\n",cmd[1]-'0');
   status = nand_flash_page_program(cmd[1]-'0',0,buff,2112);
   if(status == -1){
   log_error("page program error.\r\n");
   }else{
   log_info("page program ok.\r\n");
   }
   break;
   
   case 'P':
   log_debug("program page spare:%d.\r\n",cmd[1]-'0');
   status = nand_flash_page_program(cmd[1]-'0',2048,buff,64);
   if(status == -1){
   log_error("page program error.\r\n");
   }else{
   log_info("page program ok.\r\n");
   }
   break;
   
   case 'r':
   log_debug("read page :%d.\r\n",cmd[1]-'0');
   status = nand_flash_page_read(cmd[1]-'0',0,buff,2112);
   if(status == -1){
   log_error("page read error.\r\n");
   }else{
   log_info("page read ok.\r\n"); 
   } 
   break;
   
   case 'R':
   log_debug("read page spare:%d.\r\n",cmd[1]-'0');
   status = nand_flash_page_read(cmd[1]-'0',2048,buff,64);
   if(status == -1){
   log_error("page read error.\r\n");
   }else{
   log_info("page read ok.\r\n"); 
   } 
   break;
   
   case 'i':
   status = nand_flash_id_read(id);
   if(status == -1){
   log_error("read id err.\r\n");
   }else{
   log_info("read id ok.\r\n");
   for(uint8_t i=0;i<5;i++){
   log_info("id[%d]: %d.\r\n",i,id[i]);
   }
   }
   break;
   
   case 'f':
   for(uint16_t i=0;i<2112;i++){
   buff[i] =i;
   }
   log_debug("fill 0 1 ...2111 ok.\r\n");
   break;
   
   case 'o':
   for(uint16_t i=0;i<2112;i++){
   log_debug("buff[%d]:%d.\r\n",i,buff[i]);
   osDelay(2); 
   }
   break;
   default:
   log_error("error cmd:%d.\r\n",cmd[0]);    
   }
   }
   
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
    osDelay(40);
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    osDelay(40);
    
    
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
