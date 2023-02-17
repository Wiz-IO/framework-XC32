#include "interface.h"

// a decrementing counter miliseconds counter, Timeout = 100; ... while( Timeout );
volatile int Timeout;

static volatile uint32_t gMillis = 0;
static volatile uint32_t gOldCoreTime = 0;

// userware miliseconds handler
static void (*msHandler)(void) = NULL;
void msSetup(void (*cb)(void))
{
    ENTER_CRITICAL();
    msHandler = cb;
    EXIT_CRITICAL();
}

void __ISR(_CORE_TIMER_VECTOR, CONCAT(IPL, CT_PRI, AUTO)) CoreTimerHandler(void)

{
    uint32_t current = ReadCoreTimer();
    uint32_t elapsed = current - gOldCoreTime;
    uint32_t increment = elapsed / CORE_TIMER_INTERRUPT_TICKS;
    gMillis += increment;
    Timeout -= increment;
    if (Timeout < 0)
    {
        Timeout = 0;
    }
    _CP0_SET_COMPARE(current + CORE_TIMER_INTERRUPT_TICKS);
    mCTClearIntFlag();
    gOldCoreTime = current;
    if (msHandler)
    {
        msHandler();
    }
}

INLINE uint32_t millis(void) { return gMillis; }

uint32_t micros(void)
{
    asm volatile("di");
    uint32_t result = millis() * 1000;
    uint32_t current = ReadCoreTimer();
    current -= gOldCoreTime;
    current += CORE_TIMER_TICKS_PER_MICROSECOND / 2;
    current /= CORE_TIMER_TICKS_PER_MICROSECOND;
    asm volatile("ei");
    return (result + current);
}

INLINE uint32_t Seconds(void) { return millis() / 1000; }

INLINE void delayMicroseconds(uint32_t us)
{
    uint32_t startTime = micros();
    while ((micros() - startTime) < us)
        continue;
}

INLINE void __delay(uint32_t ms)
{
    uint32_t startTime = millis();
    while ((millis() - startTime) < ms)
        continue;
}

INLINE void delay(uint32_t ms)
{
#ifndef FREERTOS
    __delay(ms);
#else
    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)
    {
        __delay(ms); // use this before scheduler
    }
    else
    {
        vTaskDelay(ms / portTICK_PERIOD_MS);
    }
#endif
}

INLINE void yield(void)
{
#ifndef FREERTOS
    _CP0_GET_COUNT();
#else
    vTaskDelay(1);
#endif
}

INLINE void __attribute__((nomips16)) interrupts(void)
{
    asm volatile("ei");
}

INLINE void __attribute__((nomips16)) noInterrupts(void)
{
    asm volatile("di");
}

INLINE uint32_t __attribute__((nomips16)) disableInterrupts(void)
{
    uint32_t status = 0;
    asm volatile("di   %0"
                 : "=r"(status));
    return status;
}

INLINE void __attribute__((nomips16)) restoreInterrupts(uint32_t st)
{
    if (st & 1)
    {
        asm volatile("ei");
    }
    else
    {
        asm volatile("di");
    }
}
