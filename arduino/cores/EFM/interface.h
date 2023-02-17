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

#ifndef INTERFACE_H_
#define INTERFACE_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>

#include <xc.h>
#include <sys/attribs.h>
#include <sys/kmem.h>
#include <sys_devcon.h>

#include <user_config.h>
#include <irq_priority.h>
#include <swTimers.h>

#include <hal_dbg.h>
#include <hal_irq.h>
#include <hal_spi.h>

#ifdef FREERTOS
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"
#include "event_groups.h"
#ifndef ARDUINO_STACK_SIZE
#define ARDUINO_STACK_SIZE (10 * 1024)
#endif
#endif

#ifdef LWIP
#include <lwip/init.h>
#include <lwip/tcpip.h>
#include <lwip/udp.h>
#endif

#define INLINE inline __attribute__((always_inline))
#define WEAK __attribute__((weak))

#undef CONCAT_EX
#undef CONCAT
#define CONCAT_EX(X, Y, Z) X##Y##Z
#define CONCAT(X, Y, Z) CONCAT_EX(X, Y, Z)

#define PIN_NC (-1)
#define PIN_HARD (-2)

    typedef volatile uint32_t *TREG;

    typedef enum
    {
        REG = 0,
        REG_CLR,
        REG_SET,
        REG_INV,

        ANSEL = 0,
        ANSEL_CLR,
        ANSEL_SET,
        ANSEL_INV,
        TRIS,
        TRIS_CLR,
        TRIS_SET,
        TRIS_INV,
        PORT,
        PORT_CLR,
        PORT_SET,
        PORT_INV,
        LAT,
        LAT_CLR,
        LAT_SET,
        LAT_INV,
        ODC,
        ODC_CLR,
        ODC_SET,
        ODC_INV,
        CNPU,
        CNPU_CLR,
        CNPU_SET,
        CNPU_INV,
        CNPD,
        CNPD_CLR,
        CNPD_SET,
        CNPD_INV,
        CNCON,
        CNCON_CLR,
        CNCON_SET,
        CNCON_INV,
        CNEN,
        CNEN_CLR,
        CNEN_SET,
        CNEN_INV,
        CNSTAT,
        CNSTAT_CLR,
        CNSTAT_SET,
        CNSTAT_INV,
        CNNE,
        CNNE_CLR,
        CNNE_SET,
        CNNE_INV,
        CNF,
        CNF_CLR,
        CNF_SET,
        CNF_INV,
        SRCON0,
        SRCON0_CLR,
        SRCON0_SET,
        SRCON0_INV,
        SRCON1,
        SRCON1_CLR,
        SRCON1_SET,
        SRCON1_INV,

        GPIO_MAX = 7,
        GPIO_W_SIZE = 64,
        GPIO_B_SIZE = 64 * 4,
    } TREG_ENUM;

    void pinMode(uint32_t, uint32_t);
    void digitalWrite(uint32_t, uint32_t);
    int digitalRead(uint32_t);
    void pinMux(TREG reg, int val);
    void digitalToggle(uint32_t pin);
    typedef void (*voidFuncPtr)(void);

    // #define digitalPinToPort(p)
    // #define digitalPinToBitMask(p)
    // #define digitalPinToClkid(p)
    // #define digitalPinSPIAvailiable(p)
    // #define digitalPinToSPIDevice(p)
    // #define digitalPinToSPIClockId(p)

    int ADC_Read(uint32_t pin);
    int PWM_Begin(uint32_t pin, uint32_t frequency, float duty);
    void PWM_writeValule(uint32_t pin, int val);
    int PWM_writeDuty(uint32_t pin, float duty);

    // clang-format off
    // prettier-ignore-start

#define SYS_CLK_FREQ                        ((uint32_t)(F_CPU))
#define SYS_CLK_BUS_PERIPHERAL_1            ((uint32_t)(SYS_CLK_FREQ >> 1))
#define SYS_CT_FREQ                         ((uint32_t)(SYS_CLK_FREQ >> 1))
#define CORE_TIMER_INTERRUPT_TICKS          (SYS_CT_FREQ / 1000ul)
#define CORE_TIMER_TICKS_PER_MICROSECOND    (SYS_CT_FREQ / 1000000ul)

#define GetSystemClock()                    (SYS_CLK_FREQ)
#define GetPeripheralClock()                (SYS_CLK_BUS_PERIPHERAL_1)

#define ReadCoreTimer()                     _CP0_GET_COUNT()

#define clockCyclesPerMicrosecond()         (F_CPU / 1000000L)
#define clockCyclesToMicroseconds(a)        ((a) / clockCyclesPerMicrosecond())
#define microsecondsToClockCycles(a)        ((a) * clockCyclesPerMicrosecond())

    // prettier-ignore-end
    // clang-format on

    extern volatile int Timeout; // a decrementing counter mS counter
    uint32_t Elapsed(uint32_t startTime, uint32_t currentTime);
    uint32_t Seconds(void);

    uint32_t millis(void);
    uint32_t micros(void);
    void delay(uint32_t);
    void delayMicroseconds(uint32_t us);

    void yield(void);
    void interrupts(void);
    void noInterrupts(void);
    void attachInterrupt(uint32_t, void (*)(void), int mode);
    void detachInterrupt(uint32_t);
    uint32_t disableInterrupts(void);
    void restoreInterrupts(uint32_t);

#define GET_INT_DISABLE() (int)__builtin_disable_interrupts()
#define INT_ENABLE() __builtin_mtc0(12, 0, (__builtin_mfc0(12, 0) | 0x0001))

#define ENTER_CRITICAL() int isrStatus = GET_INT_DISABLE()
#define EXIT_CRITICAL() \
    if (isrStatus)      \
    INT_ENABLE()

    static INLINE void SysKeyUnlock(void)
    {
        SYSKEY = 0xAA996655;
        SYSKEY = 0x556699AA;
    }

    static INLINE void SysKeyLock(void)
    {
        SYSKEY = 0x33333333;
    }

    static INLINE void SystemUnlock(void)
    {
        asm volatile("di");
        SysKeyUnlock();
    }

    static INLINE void SystemLock(void)
    {
        SysKeyLock();
        asm volatile("ei");
    }

    // LOOP ///////////////////////////////////////////////////////////////////

    void addProcess(voidFuncPtr cb);
    void delProcess(voidFuncPtr cb);
    void exeProcess();

    // c++14 //////////////////////////////////////////////////////////////////

#ifndef isascii
#define isascii(x) ((x >= 0x00) && (x <= 0x7f))
#endif
#ifndef toascii
#define toascii(c) ((c)&0177)
#endif

    ///////////////////////////////////////////////////////////////////////////

#define ABORT()       \
    do                \
    {                 \
        while (1)     \
            continue; \
    } while (0)

    void my_assert(bool expression, const char *message);
#define ASSERT(E, M) my_assert(E, M)

    // GPIO ///////////////////////////////////////////////////////////////////

    typedef struct
    {
        int mode;
        const IRQ_E irq;
        uint8_t pri, sub;
        TREG RPin;
        uint32_t RVal;
    } periphery_eint_t; // INTx

    typedef struct
    {
        TREG OCxCON;
        TREG TxCON;
        TREG TxCON_ALT;
        TREG RPin;
        uint32_t RVal;
    } periphery_pwm_t; // PWMx

    typedef struct
    {
        uint32_t channel;
        uint32_t an_pin;
    } periphery_adc_t; // ADCx

    typedef struct periphery_s
    {
        union
        {
            periphery_eint_t eint;
            periphery_adc_t adc;
            periphery_pwm_t pwm;
        };
    } periphery_t;

    typedef struct
    {
        TREG gpio;
        uint16_t mask;

        periphery_t *eint;
        periphery_t *adc;
        periphery_t *pwm;

        voidFuncPtr callback;
    } pin_t;

    // UART ///////////////////////////////////////////////////////////////////

#define DEFAULT_SERIAL_IPL CONCAT(IPL, DEFAULT_SERIAL_PRI, AUTO)
#define UART_CONTEXT_MAX (6)
#define UART_BASE (_UART1_BASE_ADDRESS)
    typedef struct
    {
        uint32_t base;
        IRQ_E irq;
        uint8_t pri, sub;
        TREG rpin_tx;
        uint32_t rval_tx;
        TREG rpin_rx;
        uint32_t rval_rx;
    } uart_context_t;
    extern uart_context_t uart_context[UART_CONTEXT_MAX];
    static INLINE uart_context_t *uart_get_context(int id)
    {
        return (id < UART_CONTEXT_MAX) ? &uart_context[id] : NULL;
    }

    // SPI ////////////////////////////////////////////////////////////////////

#define SPI_CONTEXT_MAX (6)
#define SPI_BASE (_SPI1_BASE_ADDRESS)
    typedef struct
    {
        uint32_t base;
        TREG rpin_miso; // clock pin is constant
        uint32_t rval_miso;
        TREG rpin_mosi;
        uint32_t rval_mosi;
        TREG rpin_cs;
        uint32_t rval_cs;
    } spi_context_t;
    extern spi_context_t spi_context[SPI_CONTEXT_MAX];
    static INLINE spi_context_t *spi_get_context(int id)
    {
        return (id < SPI_CONTEXT_MAX) ? &spi_context[id] : NULL;
    }

    ///////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}

void loop();
void setup();

#endif
#endif /* INTERFACE_H_ */