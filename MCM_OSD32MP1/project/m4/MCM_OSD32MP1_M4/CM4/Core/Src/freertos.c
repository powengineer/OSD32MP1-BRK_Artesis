/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "cmsis_os2.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mcm_acquisition.h"
#include "mcm_shared.h"
#include "openamp.h"
#include "mcm_ipc.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
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
volatile uint32_t mcm_ready_block = 0U;
volatile uint32_t mcm_ready_sequence = 0U;

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for acquisitionTask */
osThreadId_t acquisitionTaskHandle;
const osThreadAttr_t acquisitionTask_attributes = {
  .name = "acquisitionTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for communicationTa */
osThreadId_t communicationTaHandle;
const osThreadAttr_t communicationTa_attributes = {
  .name = "communicationTa",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartAcquisitionTask(void *argument);
void StartCommunicationTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* Hook prototypes */
void vApplicationStackOverflowHook(xTaskHandle xTask, char *pcTaskName);
void vApplicationMallocFailedHook(void);

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, char *pcTaskName)
{
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
}
/* USER CODE END 4 */

/* USER CODE BEGIN 5 */
void vApplicationMallocFailedHook(void)
{
   /* vApplicationMallocFailedHook() will only be called if
   configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h. It is a hook
   function that will get called if a call to pvPortMalloc() fails.
   pvPortMalloc() is called internally by the kernel whenever a task, queue,
   timer or semaphore is created. It is also called by various parts of the
   demo application. If heap_1.c or heap_2.c are used, then the size of the
   heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
   FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
   to query the size of free heap space that remains (although it does not
   provide information on how the remaining heap might be fragmented). */
}
/* USER CODE END 5 */

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

  /* creation of acquisitionTask */
  acquisitionTaskHandle = osThreadNew(StartAcquisitionTask, NULL, &acquisitionTask_attributes);

  /* creation of communicationTa */
  communicationTaHandle = osThreadNew(StartCommunicationTask, NULL, &communicationTa_attributes);

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
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartAcquisitionTask */
/**
* @brief Function implementing the acquisitionTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartAcquisitionTask */
void StartAcquisitionTask(void *argument)
{
	/* USER CODE BEGIN StartAcquisitionTask */
	/* Infinite loop */
	uint32_t flags;
	uint32_t block_index;
	uint32_t sequence;

	MCM_Shared_Init();

	if (MCM_Acquisition_Start() != HAL_OK)
	{
	    Error_Handler();
	}

	for (;;)
	{
	    flags = osThreadFlagsWait(
	            MCM_HALF_ALL,
	            osFlagsWaitAll,
	            osWaitForever);

	    if ((flags & MCM_HALF_ALL) == MCM_HALF_ALL)
	    {
	        if (MCM_Shared_WriteBlock(
	                0U,
	                mcm_dma_va,
	                mcm_dma_vb,
	                mcm_dma_vc,
	                mcm_dma_ia,
	                mcm_dma_ib,
	                mcm_dma_ic,
	                &block_index,
	                &sequence) == 0)
	        {
	            mcm_ready_block = block_index;
	            mcm_ready_sequence = sequence;

	            __DMB();

	            (void)osThreadFlagsSet(
	                communicationTaHandle,
	                MCM_COMM_BLOCK_READY_FLAG);
	        }
	    }

	    flags = osThreadFlagsWait(
	                    MCM_FULL_ALL,
	                    osFlagsWaitAll,
	                    osWaitForever);

	    if ((flags & MCM_FULL_ALL) == MCM_FULL_ALL)
	    {
	    	if (MCM_Shared_WriteBlock(
	    		MCM_DMA_HALF_SAMPLES,
	    	    mcm_dma_va,
	    	    mcm_dma_vb,
	    	    mcm_dma_vc,
	    	    mcm_dma_ia,
	    	    mcm_dma_ib,
	    	    mcm_dma_ic,
	    	    &block_index,
	    	    &sequence) == 0)
	    	    {
	    	    	mcm_ready_block = block_index;
	    	        mcm_ready_sequence = sequence;

	    	        __DMB();

	    	        osThreadFlagsSet(
	    	        communicationTaHandle,
	    	        MCM_COMM_BLOCK_READY_FLAG);
	    	    }
	    }
	}

  /* USER CODE END StartAcquisitionTask */
}

/* USER CODE BEGIN Header_StartCommunicationTask */
/**
* @brief Function implementing the communicationTa thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartCommunicationTask */
void StartCommunicationTask(void *argument)
{

	    /* USER CODE BEGIN StartCommunicationTask */

	        uint32_t flags;
	        uint32_t command;

	        if (MCM_IPC_Init() != 0)
	        {
	            Error_Handler();
	        }

	        for (;;)
	        {
	            /*
	             * Linux'tan gelen RPMsg mesajlarını işle.
	             */
	            MCM_IPC_Process();

	            /*
	             * acquisitionTask tarafından yeni DDR bloğu
	             * bildirildi mi?
	             *
	             * 1 tick beklemek communicationTask'in sürekli
	             * CPU tüketmesini engeller.
	             */
	            flags = osThreadFlagsWait(
	                        MCM_COMM_BLOCK_READY_FLAG,
	                        osFlagsWaitAny,
	                        1U);

	            if ((flags & MCM_COMM_BLOCK_READY_FLAG) != 0U)
	            {
	                /*
	                 * Birazdan bunu descriptor queue ile
	                 * daha sağlam hale getireceğiz.
	                 */
	                (void)MCM_IPC_SendBlockReady(
	                    mcm_ready_block,
	                    mcm_ready_sequence);
	            }

	            /*
	             * Linux'tan gelen kontrol komutlarını incele.
	             */
	            command = MCM_IPC_GetPendingCommand();

	            switch (command)
	            {
	                case MCM_MSG_START:
	                    /*
	                     * Şimdilik boş.
	                     * Bir sonraki aşamada acquisition state machine'e
	                     * bağlayacağız.
	                     */
	                    MCM_IPC_ClearPendingCommand();
	                    break;

	                case MCM_MSG_STOP:
	                    MCM_IPC_ClearPendingCommand();
	                    break;

	                case MCM_MSG_GET_STATUS:
	                    MCM_IPC_ClearPendingCommand();
	                    break;

	                default:
	                    break;
	            }
	        }

	        /* USER CODE END StartCommunicationTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

