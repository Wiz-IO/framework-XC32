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

#pragma once
#ifdef __cplusplus

#include <interface.h>
#include <HardwareSerial.h>
#include <RingBuffer.h>

typedef enum
{
    UART_DATA_SIZE_8_BITS = 0x0000,
    UART_DATA_SIZE_9_BITS = 0x0006,

    UART_PARITY_ODD = 0x0004,
    UART_PARITY_EVEN = 0x0002,
    UART_PARITY_NONE = 0x0000,

    UART_STOP_BITS_2 = 0x0001,
    UART_STOP_BITS_1 = 0x0000
} UART_LINE_CONTROL_MODE_E;

#define SERIAL_8N1 (UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1)
#define SERIAL_8N2 (UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_2)

#define SERIAL_8E1 (UART_DATA_SIZE_8_BITS | UART_PARITY_EVEN | UART_STOP_BITS_1)
#define SERIAL_8E2 (UART_DATA_SIZE_8_BITS | UART_PARITY_EVEN | UART_STOP_BITS_2)

#define SERIAL_8O1 (UART_DATA_SIZE_8_BITS | UART_PARITY_ODD | UART_STOP_BITS_1)
#define SERIAL_8O2 (UART_DATA_SIZE_8_BITS | UART_PARITY_ODD | UART_STOP_BITS_2)

#define SERIAL_9N1 (UART_DATA_SIZE_9_BITS | UART_STOP_BITS_1)
#define SERIAL_9N2 (UART_DATA_SIZE_9_BITS | UART_STOP_BITS_2)

#ifndef SERIAL_PRI
#define SERIAL_PRI DEFAULT_SERIAL_PRI
#endif
#ifndef SERIAL_SUB
#define SERIAL_SUB DEFAULT_SERIAL_SUB
#endif

#define SERIAL_DEFAULT_BRG 115200
#ifndef SOFTWARE_SERIAL_BUFFER_SIZE
#define SOFTWARE_SERIAL_BUFFER_SIZE 64
#endif

class Uart : public HardwareSerial
{
private:
    uint32_t id;
    uart_context_t *u;
    RingBuffer rxBuffer;

    inline void uart_set_brg(uint32_t brg)
    {

        int brgValueLow = ((SYS_CLK_BUS_PERIPHERAL_1 / brg) >> 4) - 1;
        int brgValueHigh = ((SYS_CLK_BUS_PERIPHERAL_1 / brg) >> 2) - 1;
        *((TREG)((u->base + 0x00)) + REG_CLR) = _U1MODE_BRGH_MASK;
        if ((brgValueHigh >= 0) && (brgValueHigh <= 0xFFFF))
        {
            *((TREG)((u->base + 0x00)) + REG_SET) = _U1MODE_BRGH_MASK;
            *(TREG)(u->base + 0x40) = (((SYS_CLK_BUS_PERIPHERAL_1 >> 2) + (brg >> 1)) / brg) - 1;
        }
        else if ((brgValueLow >= 0) && (brgValueLow <= 0xFFFF))
        {
            *(TREG)(u->base + 0x40) = (((SYS_CLK_BUS_PERIPHERAL_1 >> 4) + (brg >> 1)) / brg) - 1;
        }
        else // else ERROR ... 115200
        {
            *(TREG)(u->base + 0x40) = (((SYS_CLK_BUS_PERIPHERAL_1 >> 4) + (115200 >> 1)) / 115200) - 1;
        }
    }

public:
    void setRPins(TREG rpin_tx, uint32_t rval_tx, TREG rpin_rx, uint32_t rval_rx)
    {
        SystemUnlock();
        if (u->rpin_tx)
        {
            *rpin_tx = 0; // restore gpio
        }
        if (rpin_tx)
        {
            *rpin_tx = rval_tx;
        }
        if (rpin_rx)
        {
            *rpin_rx = rval_rx;
        }
        SystemLock();
    }

    void setPriority(uint8_t pri, uint8_t sub)
    {
        if (u)
        {
            pri &= 7;
            sub &= 7;
            IRQ_Priority(u->irq, pri, sub);
        }
    }

    /// @note ID = 0 is UART1
    Uart(uint32_t ID)
    {
        id = ID;
        u = uart_get_context(ID);
        ASSERT(u, "Wrong UART Context");
        IRQ_Priority(u->irq, SERIAL_PRI, SERIAL_SUB);
        setRPins(u->rpin_tx, u->rval_tx, u->rpin_rx, u->rval_rx);
    }

    ~Uart() { end(); }

    void end()
    {
        IRQ_Disable(u->irq);
        IRQ_Clear(u->irq);
        *(TREG)(u->base + 0x00) = 0;
        *(TREG)(u->base + 0x10) = 0;
        rxBuffer.clear();
    }

    void begin(unsigned long baudrate, unsigned long config)
    {
        end();
        uart_set_brg(baudrate);
        *(TREG)(u->base + 0x10) |= _U1STA_UTXEN_MASK | _U1STA_URXEN_MASK;
        *(TREG)(u->base + 0x00) |= _U1MODE_ON_MASK | _U1MODE_BRGH_MASK | config;
        IRQ_Enable(u->irq);
    }

    void begin(unsigned long baudrate = SERIAL_DEFAULT_BRG)
    {
        begin(baudrate, SERIAL_8N1);
    }

    void begin()
    {
        begin(115200, SERIAL_8N1);
    }

    int available()
    {
        return rxBuffer.available();
    }

    int peek()
    {
        int c = -1;
        IRQ_Disable(u->irq);
        c = rxBuffer.peek();
        IRQ_Enable(u->irq);
        return c;
    }

    void flush()
    {
        IRQ_Disable(u->irq);
        rxBuffer.clear();
        IRQ_Enable(u->irq);
    }

    int read()
    {
        int c = -1;
        IRQ_Disable(u->irq);
        c = rxBuffer.read_char();
        IRQ_Enable(u->irq);
        return c;
    }

    size_t write(uint8_t c)
    {
        TREG sta = (TREG)(u->base + 0x10);
        while (*sta & _U1STA_UTXBF_MASK)
            continue;
        *(TREG)(u->base + 0x20) = c;
        return 1;
    }

    virtual void isr_callback()
    {
        TREG sta = (TREG)(u->base + 0x10);
        TREG rx = (TREG)(u->base + 0x30);
        while (*sta & _U1STA_URXDA_MASK)
            rxBuffer.store_char(*rx);
        IRQ_Clear(u->irq);
    }

    operator bool() { return *(TREG)(u->base + 0x00) & _U1MODE_ON_MASK; }

    using Print::write;

    /// @brief Redirect printf for this Serial
    /// @param enable / disable
    void debug(bool enable)
    {
        __XC_UART = enable * (id + 1);
    }
};

extern Uart Serial;
extern Uart Serial1;
extern Uart Serial2;
extern Uart Serial3;
extern Uart Serial4;
extern Uart Serial5;

#endif
