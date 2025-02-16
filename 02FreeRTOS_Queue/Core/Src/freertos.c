/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
char Infobuffer[512]={0};
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
osThreadId LED0Handle;
osThreadId LED1Handle;
osThreadId KEYHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
osThreadId SenderHandle;
QueueHandle_t QueueHandle;
/* USER CODE END FunctionPrototypes */

void LED0Task(void const * argument);
void LED1Task(void const * argument);
void KEY_Task(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

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
	QueueHandle=xQueueCreate(5,sizeof(int32_t));
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of LED0 */
  osThreadDef(LED0, LED0Task, osPriorityNormal, 0, 128);
  LED0Handle = osThreadCreate(osThread(LED0), (void *)100);

  /* definition and creation of LED1 */
  osThreadDef(LED1, LED1Task, osPriorityAboveNormal, 0, 128);
  LED1Handle = osThreadCreate(osThread(LED1), NULL);

  /* definition and creation of KEY */
  osThreadDef(KEY, KEY_Task, osPriorityAboveNormal, 0, 128);
  KEYHandle = osThreadCreate(osThread(KEY), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  osThreadDef(Sender, LED0Task, osPriorityNormal, 0, 128);
  SenderHandle = osThreadCreate(osThread(Sender), (void *)200);
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_LED0Task */
/**
  * @brief  Function implementing the LED0 thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_LED0Task */
void LED0Task(void const * argument)
{
  /* USER CODE BEGIN LED0Task */
  int32_t infoNum;
  
  BaseType_t xStatus;
  infoNum=(int32_t)argument;
  /* Infinite loop */
  for(;;)
  {
//	  printf("%d\r\n",infoNum);
	xStatus=xQueueSendToBack(QueueHandle,&infoNum,1000);
//	HAL_GPIO_TogglePin(GPIOF,GPIO_PIN_9);
	if(xStatus!=pdPASS)printf("Could not send to the queue!\r\n");
    osDelay(1);
  }
  /* USER CODE END LED0Task */
}

/* USER CODE BEGIN Header_LED1Task */
/**
* @brief Function implementing the LED1 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LED1Task */
void LED1Task(void const * argument)
{
  /* USER CODE BEGIN LED1Task */
  /* Infinite loop */
  int32_t infoNum;
  infoNum=(int32_t)argument;
  BaseType_t xStatus;
  for(;;)
  {
	xStatus=xQueueReceive(QueueHandle,&infoNum,0);
	if(xStatus==pdPASS)printf("%d\r\n",infoNum);
	else printf("could not receive the queue!\r\n");

	HAL_GPIO_TogglePin(GPIOF,GPIO_PIN_10);
//	vTaskList(Infobuffer);
//	printf("task name task\r\n");
//	printf("%s\r\n",Infobuffer);
    osDelay(500);
  }
  /* USER CODE END LED1Task */
}

/* USER CODE BEGIN Header_KEY_Task */
/**
* @brief Function implementing the KEY thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_KEY_Task */
void KEY_Task(void const * argument)
{
  /* USER CODE BEGIN KEY_Task */
	static uint8_t keynum=0;
  /* Infinite loop */
  for(;;)
  {
	keynum=keyScan();
	  if(keynum==1)vTaskSuspend(LED1Handle);
	  if(keynum==2)vTaskResume(LED1Handle);
	  
	  
//	printf("%d\r\n",keynum);
    osDelay(1);
  }
  /* USER CODE END KEY_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
uint8_t keyScan(void)
{
	static uint8_t keymark=0;
	if((HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)||HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4))&&!keymark)
	{
		keymark=1;
		osDelay(10);
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0))return 1;
		if(HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4))return 2;
	}
	else if(!HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)&&!HAL_GPIO_ReadPin(GPIOE,GPIO_PIN_4))keymark=0;
	return 0;
}
/* USER CODE END Application */
