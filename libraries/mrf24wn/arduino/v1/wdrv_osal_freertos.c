///////////////////////////////////////////////////////////////////////////////
//
//      WizIO 2023 Georgi Angelov
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
///////////////////////////////////////////////////////////////////////////////

#include "wdrv_osal_freertos.h"
#include "wdrv_mrf24wn_common.h"

OSAL_RESULT OSAL_SEM_Create(OSAL_SEM_HANDLE_TYPE *semID, OSAL_SEM_TYPE type, uint8_t maxCount, uint8_t initialCount)
{
  switch (type)
  {
  case OSAL_SEM_TYPE_BINARY:
    vSemaphoreCreateBinary(*(SemaphoreHandle_t *)semID);
    break;
  case OSAL_SEM_TYPE_COUNTING:
    *(SemaphoreHandle_t *)semID = xSemaphoreCreateCounting((UBaseType_t)maxCount, (UBaseType_t)initialCount);
    break;
  default:
    *(SemaphoreHandle_t *)semID = NULL;
  }
  return (*(SemaphoreHandle_t *)semID == NULL) ? OSAL_RESULT_FALSE : OSAL_RESULT_TRUE;
}

OSAL_RESULT OSAL_SEM_Delete(OSAL_SEM_HANDLE_TYPE *semID)
{
  vSemaphoreDelete(*(SemaphoreHandle_t *)semID);
  *(SemaphoreHandle_t *)semID = NULL;
  return OSAL_RESULT_TRUE;
}

OSAL_RESULT OSAL_SEM_Pend(OSAL_SEM_HANDLE_TYPE *semID, uint16_t waitMS)
{
  TickType_t timeout = 0;
  if (waitMS == OSAL_WAIT_FOREVER)
    timeout = portMAX_DELAY;
  else
    timeout = (TickType_t)(waitMS / portTICK_PERIOD_MS);
  return (xSemaphoreTake(*(SemaphoreHandle_t *)semID, timeout) == pdTRUE) ? OSAL_RESULT_TRUE : OSAL_RESULT_FALSE;
}

OSAL_RESULT OSAL_SEM_Post(OSAL_SEM_HANDLE_TYPE *semID)
{
  return (xSemaphoreGive(*(SemaphoreHandle_t *)semID) == pdTRUE) ? OSAL_RESULT_TRUE : OSAL_RESULT_FALSE;
}

OSAL_RESULT OSAL_SEM_PostISR(OSAL_SEM_HANDLE_TYPE *semID)
{
  BaseType_t _taskWoken = pdFALSE;
  if (xSemaphoreGiveFromISR(*(SemaphoreHandle_t *)semID, &_taskWoken))
  {
    portEND_SWITCHING_ISR(_taskWoken);
    return OSAL_RESULT_TRUE;
  }
  return OSAL_RESULT_FALSE;
}

/*

*/

OSAL_RESULT OSAL_MUTEX_Create(OSAL_MUTEX_HANDLE_TYPE *mutexID)
{
  if (*(SemaphoreHandle_t *)mutexID != NULL)
    return OSAL_RESULT_FALSE;
  *(SemaphoreHandle_t *)mutexID = xSemaphoreCreateMutex();
  return OSAL_RESULT_TRUE;
}

OSAL_RESULT xOSAL_MUTEX_Delete(OSAL_MUTEX_HANDLE_TYPE *mutexID)
{
  if (*(SemaphoreHandle_t *)mutexID == NULL)
    return OSAL_RESULT_FALSE;
  vSemaphoreDelete(*(SemaphoreHandle_t *)mutexID);
  *(SemaphoreHandle_t *)mutexID = NULL;
  return OSAL_RESULT_TRUE;
}

OSAL_RESULT OSAL_MUTEX_Lock(OSAL_MUTEX_HANDLE_TYPE *mutexID, uint16_t waitMS)
{
  TickType_t timeout = 0;
  if (waitMS == OSAL_WAIT_FOREVER)
    timeout = portMAX_DELAY;
  else
    timeout = (TickType_t)(waitMS / portTICK_PERIOD_MS);
  return (xSemaphoreTake(*(SemaphoreHandle_t *)mutexID, timeout) == pdTRUE) ? OSAL_RESULT_TRUE : OSAL_RESULT_FALSE;
}

OSAL_RESULT OSAL_MUTEX_Unlock(OSAL_MUTEX_HANDLE_TYPE *mutexID)
{
  return (xSemaphoreGive(*(SemaphoreHandle_t *)mutexID) == pdTRUE) ? OSAL_RESULT_TRUE : OSAL_RESULT_FALSE;
}

/*

*/

BaseType_t xQueueGenericReceive(QueueHandle_t xQueue, void *const pvBuffer, TickType_t xTicksToWait, const BaseType_t xJustPeeking)
{
  PRINT_FUNC;
  configASSERT(xJustPeeking == 0);
  return xQueueReceive(xQueue, pvBuffer, xTicksToWait);
}
