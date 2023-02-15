#include "arch/sys_arch.h"

#include "lwip/opt.h"
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/stats.h"

/* Very crude mechanism used to determine if the critical section handling
functions are being called from an interrupt context or not.  This relies on
the interrupt handler setting this variable manually. */
portBASE_TYPE xInsideISR = pdFALSE;

err_t sys_mbox_new(sys_mbox_t *pxMailBox, int iSize)
{
    err_t xReturn = ERR_MEM;
    *pxMailBox = xQueueCreate(iSize, sizeof(void *));
    if (*pxMailBox != NULL)
    {
        xReturn = ERR_OK;
        SYS_STATS_INC_USED(mbox);
    }
    return xReturn;
}

void sys_mbox_free(sys_mbox_t *pxMailBox)
{
    unsigned long ulMessagesWaiting;
    ulMessagesWaiting = uxQueueMessagesWaiting(*pxMailBox);
    configASSERT((ulMessagesWaiting == 0));
#if SYS_STATS
    {
        if (ulMessagesWaiting != 0UL)
        {
            SYS_STATS_INC(mbox.err);
        }
        SYS_STATS_DEC(mbox.used);
    }
#endif /* SYS_STATS */
    vQueueDelete(*pxMailBox);
}

void sys_mbox_post(sys_mbox_t *pxMailBox, void *pxMessageToPost)
{
    while (xQueueSendToBack(*pxMailBox, &pxMessageToPost, portMAX_DELAY) != pdTRUE)
        ;
}

err_t sys_mbox_trypost(sys_mbox_t *pxMailBox, void *pxMessageToPost)
{
    err_t xReturn;
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    if (xInsideISR != pdFALSE)
    {
        xReturn = xQueueSendFromISR(*pxMailBox, &pxMessageToPost, &xHigherPriorityTaskWoken);
    }
    else
    {
        xReturn = xQueueSend(*pxMailBox, &pxMessageToPost, (TickType_t)0);
    }
    if (xReturn == pdPASS)
    {
        xReturn = ERR_OK;
    }
    else
    {
        /* The queue was already full. */
        xReturn = ERR_MEM;
        SYS_STATS_INC(mbox.err);
    }
    return xReturn;
}

u32_t sys_arch_mbox_fetch(sys_mbox_t *pxMailBox, void **ppvBuffer, u32_t ulTimeOut)
{
    void *pvDummy;
    TickType_t xStartTime, xEndTime, xElapsed;
    unsigned long ulReturn;
    xStartTime = xTaskGetTickCount();
    if (NULL == ppvBuffer)
    {
        ppvBuffer = &pvDummy;
    }
    if (ulTimeOut != 0UL)
    {
        configASSERT(xInsideISR == (portBASE_TYPE)0);
        if (pdTRUE == xQueueReceive(*pxMailBox, &(*ppvBuffer), ulTimeOut / portTICK_PERIOD_MS))
        {
            xEndTime = xTaskGetTickCount();
            xElapsed = (xEndTime - xStartTime) * portTICK_PERIOD_MS;
            ulReturn = xElapsed;
        }
        else
        {
            /* Timed out. */
            *ppvBuffer = NULL;
            ulReturn = SYS_ARCH_TIMEOUT;
        }
    }
    else
    {
        while (pdTRUE != xQueueReceive(*pxMailBox, &(*ppvBuffer), portMAX_DELAY))
            ;
        xEndTime = xTaskGetTickCount();
        xElapsed = (xEndTime - xStartTime) * portTICK_PERIOD_MS;
        if (xElapsed == 0UL)
        {
            xElapsed = 1UL;
        }
        ulReturn = xElapsed;
    }
    return ulReturn;
}

u32_t sys_arch_mbox_tryfetch(sys_mbox_t *pxMailBox, void **ppvBuffer)
{
    void *pvDummy;
    unsigned long ulReturn;
    long lResult;
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    if (ppvBuffer == NULL)
    {
        ppvBuffer = &pvDummy;
    }
    if (xInsideISR != pdFALSE)
    {
        lResult = xQueueReceiveFromISR(*pxMailBox, &(*ppvBuffer), &xHigherPriorityTaskWoken);
    }
    else
    {
        lResult = xQueueReceive(*pxMailBox, &(*ppvBuffer), 0UL);
    }
    if (lResult == pdPASS)
    {
        ulReturn = ERR_OK;
    }
    else
    {
        ulReturn = SYS_MBOX_EMPTY;
    }
    return ulReturn;
}

/*

*/

err_t sys_sem_new(sys_sem_t *pxSemaphore, u8_t ucCount)
{
    err_t xReturn = ERR_MEM;
    vSemaphoreCreateBinary((*pxSemaphore));
    if (*pxSemaphore != NULL)
    {
        if (ucCount == 0U)
        {
            xSemaphoreTake(*pxSemaphore, 1UL);
        }
        xReturn = ERR_OK;
        SYS_STATS_INC_USED(sem);
    }
    else
    {
        SYS_STATS_INC(sem.err);
    }
    return xReturn;
}

u32_t sys_arch_sem_wait(sys_sem_t *pxSemaphore, u32_t ulTimeout)
{
    TickType_t xStartTime, xEndTime, xElapsed;
    unsigned long ulReturn;
    xStartTime = xTaskGetTickCount();
    if (ulTimeout != 0UL)
    {
        if (xSemaphoreTake(*pxSemaphore, ulTimeout / portTICK_PERIOD_MS) == pdTRUE)
        {
            xEndTime = xTaskGetTickCount();
            xElapsed = (xEndTime - xStartTime) * portTICK_PERIOD_MS;
            ulReturn = xElapsed;
        }
        else
        {
            ulReturn = SYS_ARCH_TIMEOUT;
        }
    }
    else
    {
        while (xSemaphoreTake(*pxSemaphore, portMAX_DELAY) != pdTRUE)
            ;
        xEndTime = xTaskGetTickCount();
        xElapsed = (xEndTime - xStartTime) * portTICK_PERIOD_MS;
        if (xElapsed == 0UL)
        {
            xElapsed = 1UL;
        }
        ulReturn = xElapsed;
    }
    return ulReturn;
}

void sys_sem_signal(sys_sem_t *pxSemaphore)
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
    if (xInsideISR != pdFALSE)
    {
        xSemaphoreGiveFromISR(*pxSemaphore, &xHigherPriorityTaskWoken);
    }
    else
    {
        xSemaphoreGive(*pxSemaphore);
    }
}

void sys_sem_free(sys_sem_t *pxSemaphore)
{
    SYS_STATS_DEC(sem.used);
    vQueueDelete(*pxSemaphore);
}

/*

*/

err_t sys_mutex_new(sys_mutex_t *pxMutex)
{
    err_t xReturn = ERR_MEM;
    *pxMutex = xSemaphoreCreateMutex();
    if (*pxMutex != NULL)
    {
        xReturn = ERR_OK;
        SYS_STATS_INC_USED(mutex);
    }
    else
    {
        SYS_STATS_INC(mutex.err);
    }
    return xReturn;
}

void sys_mutex_lock(sys_mutex_t *pxMutex)
{
    while (xSemaphoreTake(*pxMutex, portMAX_DELAY) != pdPASS)
        ;
}

void sys_mutex_unlock(sys_mutex_t *pxMutex)
{
    xSemaphoreGive(*pxMutex);
}

void sys_mutex_free(sys_mutex_t *pxMutex)
{
    SYS_STATS_DEC(mutex.used);
    vQueueDelete(*pxMutex);
}

/*

*/

void sys_init(void)
{
    //// srand(rand());
}

u32_t sys_now(void)
{
    return xTaskGetTickCount() * portTICK_PERIOD_MS;
}

sys_thread_t sys_thread_new(const char *pcName, void (*pxThread)(void *pvParameters), void *pvArg, int iStackSize, int iPriority)
{
    TaskHandle_t xCreatedTask;
    portBASE_TYPE xResult;
    sys_thread_t xReturn;
    xResult = xTaskCreate(pxThread, pcName, iStackSize, pvArg, iPriority, &xCreatedTask);
    if (xResult == pdPASS)
    {
        xReturn = xCreatedTask;
    }
    else
    {
        xReturn = NULL;
    }
    return xReturn;
}

sys_prot_t sys_arch_protect(void)
{
    if (xInsideISR == pdFALSE)
    {
        taskENTER_CRITICAL();
    }
    return (sys_prot_t)1;
}

void sys_arch_unprotect(sys_prot_t xValue)
{
    (void)xValue;
    if (xInsideISR == pdFALSE)
    {
        taskEXIT_CRITICAL();
    }
}

void sys_assert(const char *pcMessage)
{
    (void)pcMessage;
    printf("[ASSERT] sys_assert : loop forever\n");
    for (;;)
    {
    }
}
