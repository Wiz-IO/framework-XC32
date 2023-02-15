#ifndef __ARCH_SYS_ARCH_H__
#define __ARCH_SYS_ARCH_H__

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define SYS_MBOX_NULL					( ( QueueHandle_t ) NULL )
#define SYS_SEM_NULL					( ( SemaphoreHandle_t ) NULL )
#define SYS_DEFAULT_THREAD_STACK_DEPTH	configMINIMAL_STACK_SIZE

typedef SemaphoreHandle_t               sys_sem_t;
typedef SemaphoreHandle_t               sys_mutex_t;
typedef QueueHandle_t                   sys_mbox_t;
typedef TaskHandle_t                    sys_thread_t;

#define sys_mbox_valid( x )             ( ( ( *x ) == NULL) ? pdFALSE : pdTRUE )
#define sys_mbox_set_invalid( x )       ( ( *x ) = NULL )
#define sys_sem_valid( x )              ( ( ( *x ) == NULL) ? pdFALSE : pdTRUE )
#define sys_sem_set_invalid( x )        ( ( *x ) = NULL )

#endif /* __ARCH_SYS_ARCH_H__ */

