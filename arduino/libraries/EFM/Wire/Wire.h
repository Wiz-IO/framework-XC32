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

#ifndef __WIRE_H__
#define __WIRE_H__
#ifdef __cplusplus

#include "interface.h"
#include "Stream.h"
#include "variant.h"
#include "RingBuffer.h"

class TwoWire : public Stream
{
#define I2C_READ 1
#define I2C_WRITE 0
private:
    int id;
    volatile uint32_t *con;
    volatile uint32_t *sta;

    uint32_t _speed, _timeout;
    bool _transmissionBegun;
    uint8_t _slave_address; // 7-bit address
    RingBuffer rx, tx;

    inline void _stop(void)
    {
        *(con + REG_SET) = _I2C1CON_PEN_MASK; /* PEN */
        while (*con & _I2C1CON_PEN_MASK)
            continue;
    }

    inline void _restart(void)
    {
        *(con + REG_SET) = _I2C1CON_RSEN_MASK; /*RSEN */
        while (*con & _I2C1CON_RSEN_MASK)
            continue;
    }

    inline void _start(void)
    {
        *(con + REG_SET) = _I2C1CON_SEN_MASK; /* SEN */
        while (*con & _I2C1CON_SEN_MASK)
            continue;
    }

    void _ack(bool ack)
    {
        *(con + REG_CLR + ack) = _I2C1CON_ACKDT_MASK; // ACKDT
        *(con + REG_SET) = _I2C1CON_ACKEN_MASK;       // ACKEN
        while (*con & _I2C1CON_ACKEN_MASK)            // ACKEN
            continue;
    }

    int _idle(void)
    {
        uint32_t T = _timeout;
        while (T--)
        {
            if (!(*con & 0b11111)) // SEN RSEN PEN RCEN ACKEN
                break;
            if (!(*sta & _I2C1STAT_TRSTAT_MASK)) // TRSTAT
                break;
        }
        if (0 == T) // Timeout
            return -1;
        return (*sta & _I2C1STAT_ACKSTAT_MASK) ? -2 : 0; // ACKSTAT
    }

    int _send(unsigned char data)
    {
        if (_idle())
            return -1;
        volatile uint32_t *trn = (volatile uint32_t *)(0xBF820050 + (0x200 * id));
        *trn = data;
        if (*sta & _I2C1STAT_IWCOL_MASK) // IWCOL
        {
            *(sta + REG_CLR) = _I2C1STAT_IWCOL_MASK; // IWCOL
            return -2;
        }
        uint32_t T = _timeout;
        while (*sta & _I2C1STAT_TBF_MASK) // TBF
            if (0 == T--)
                return -3;
        while (*sta & _I2C1STAT_TRSTAT_MASK) // TRSTAT wait for the ACK
            if (0 == T--)
                return -4;
        return (*sta & _I2C1STAT_ACKSTAT_MASK) ? -5 : 0; // ACKSTAT test for ACK condition received
    }

    int _recv()
    {
        if (_idle())
            return -1;
        *(con + REG_SET) = _I2C1CON_RCEN_MASK; // RCEN
        while (*con & _I2C1CON_RCEN_MASK)      // RCEN
            continue;
        volatile uint32_t *rcv = (volatile uint32_t *)(0xBF820060 + (0x200 * id));
        return *rcv;
    }

    int i2c_start(bool operation)
    {
        int res = -1;
        _start();
        if ((res = _send((_slave_address << 1) | operation)))
        {
            return res;
        }
        return _idle();
    }

public:
    TwoWire(int ID)
    {
        id = ID;
        con = (volatile uint32_t *)(0xBF820000 + (0x200 * id));
        sta = (volatile uint32_t *)(0xBF820010 + (0x200 * id));
        _speed = 100000;
        _timeout = 0x8000;
        _slave_address = 0x3C;
        _transmissionBegun = false;
    }

    ~TwoWire() { end(); }

    void end() { *sta = *con = 0; }

    void begin(uint8_t address)
    {
        _slave_address = address;
        volatile uint32_t *brg = (volatile uint32_t *)(0xBF820040 + (0x200 * id));
        *brg = 157; // 100k
        *con = 0x8000;
        flush();
        _stop(); // set bus to idle
    }

    void begin(void) { begin(_slave_address); }

    void setTimeOut(uint32_t timeout) { _timeout = timeout; }

    void setClock(uint32_t speed_Hz)
    {
        if (_speed != speed_Hz)
        {
            _speed = speed_Hz;
            volatile uint32_t *brg = (volatile uint32_t *)(0xBF820040 + (0x200 * id));
            *brg = (SYS_CLK_BUS_PERIPHERAL_1 / _speed) - (SYS_CLK_BUS_PERIPHERAL_1 / 10000000U) - 1; // ????
        }
    }

    void beginTransmission(uint8_t address)
    {
        _slave_address = address;
        tx.clear();
        _transmissionBegun = true;
    }

    uint8_t endTransmission(bool);
    uint8_t endTransmission() { return endTransmission(true); }

    uint8_t requestFrom(uint8_t address, size_t size, bool stopBit);
    uint8_t requestFrom(uint8_t address, size_t size) { return requestFrom(address, size, true); }

    size_t write(uint8_t ucData)
    {
        if (!_transmissionBegun || tx.isFull())
            return 0;
        tx.store_char(ucData);
        return 1;
    }

    size_t write(const uint8_t *data, size_t size)
    {
        for (size_t i = 0; i < size; ++i)
        {
            if (!write(data[i]))
                return i;
        }
        return size;
    }

    virtual int available(void) { return rx.available(); }
    virtual int read(void) { return rx.read_char(); }
    virtual int peek(void) { return rx.peek(); }
    virtual void flush(void)
    {
        tx.clear();
        rx.clear();
        volatile uint32_t *rcv = (volatile uint32_t *)(0xBF820060 + (0x200 * id));
        int r = *rcv; // read to clear
        *sta = 0x00;
    }

    using Print::write;

    void onService(void){};
    void onReceive(void (*)(int)){};
    void onRequest(void (*)(void)){};
};

extern TwoWire Wire;

#endif // cpp
#endif // h