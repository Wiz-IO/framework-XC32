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

#ifndef _WDRV__OSAL_FREERTOS_H
#define _WDRV__OSAL_FREERTOS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <interface.h>

  typedef SemaphoreHandle_t OSAL_SEM_HANDLE_TYPE;
  typedef SemaphoreHandle_t OSAL_MUTEX_HANDLE_TYPE;
  typedef BaseType_t OSAL_CRITSECT_DATA_TYPE;

#define OSAL_WAIT_FOREVER (uint16_t)0xFFFF

  typedef enum OSAL_SEM_TYPE
  {
    OSAL_SEM_TYPE_BINARY,
    OSAL_SEM_TYPE_COUNTING
  } OSAL_SEM_TYPE;

  typedef enum OSAL_CRIT_TYPE
  {
    OSAL_CRIT_TYPE_LOW,
    OSAL_CRIT_TYPE_HIGH
  } OSAL_CRIT_TYPE;

  typedef enum OSAL_RESULT
  {
    OSAL_RESULT_NOT_IMPLEMENTED = -1,
    OSAL_RESULT_FALSE = 0,
    OSAL_RESULT_TRUE = 1
  } OSAL_RESULT;

  OSAL_RESULT OSAL_SEM_Create(OSAL_SEM_HANDLE_TYPE *semID, OSAL_SEM_TYPE type, uint8_t maxCount, uint8_t initialCount);
  OSAL_RESULT OSAL_SEM_Delete(OSAL_SEM_HANDLE_TYPE *semID);
  OSAL_RESULT OSAL_SEM_Pend(OSAL_SEM_HANDLE_TYPE *semID, uint16_t waitMS);
  OSAL_RESULT OSAL_SEM_Post(OSAL_SEM_HANDLE_TYPE *semID);
  OSAL_RESULT OSAL_SEM_PostISR(OSAL_SEM_HANDLE_TYPE *semID);

  OSAL_RESULT OSAL_MUTEX_Create(OSAL_MUTEX_HANDLE_TYPE *mutexID);
  OSAL_RESULT OSAL_MUTEX_Delete(OSAL_MUTEX_HANDLE_TYPE *mutexID);
  OSAL_RESULT OSAL_MUTEX_Lock(OSAL_MUTEX_HANDLE_TYPE *mutexID, uint16_t waitMS);
  OSAL_RESULT OSAL_MUTEX_Unlock(OSAL_MUTEX_HANDLE_TYPE *mutexID);

#ifdef __cplusplus
}
#endif

#endif // _WDRV__OSAL_FREERTOS_H
