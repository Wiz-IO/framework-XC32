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

#ifndef __SPI_CLASS_H__
#define __SPI_CLASS_H__
#ifdef __cplusplus

#include <Arduino.h>

#define SPI_HELPER /* helper functions */

typedef enum
{
    SPI_MODE0 = 0, // CPOL : 0  | CPHA : 0
    SPI_MODE1,     // CPOL : 0  | CPHA : 1
    SPI_MODE2,     // CPOL : 1  | CPHA : 0
    SPI_MODE3      // CPOL : 1  | CPHA : 1
} SPIDataMode;

class SPISettings
{
public:
    SPISettings(uint32_t clockFrequency, uint8_t bitOrder, uint8_t dataMode)
    {
        clock = clockFrequency; // Hz
        order = bitOrder;
        mode = dataMode;
    }

    SPISettings()
    {
        clock = 1000000; // Hz
        order = MSBFIRST;
        mode = SPI_MODE0;
    }

private:
    uint32_t clock;
    uint8_t order;
    uint8_t mode;
    friend class SPIClass;
};

class SPIClass
{
private:
    uint32_t id;
    spi_context_t *u;
    uint32_t mode;
    uint32_t brg_hz;
    uint32_t bit_order;
    uint32_t bit_size;

    inline uint8_t reverseByte(uint8_t b)
    {
        b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
        b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
        b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
        return b;
    }

public:
    void setRPins(TREG rpin_miso, uint32_t rval_miso, TREG rpin_mosi, uint32_t rval_mosi, TREG rpin_cs, uint32_t rval_cs)
    {
        SystemUnlock();
        if (u->rpin_mosi)
            *rpin_mosi = 0; // restore gpio
        if (u->rpin_cs)
            *rpin_cs = 0; // restore gpio
        if (rpin_miso)
            *rpin_miso = rval_miso;
        if (rpin_mosi)
            *rpin_mosi = rval_mosi;
        if (rpin_cs)
            *rpin_cs = rval_cs;
        SystemLock();
    }

    void setPins(int sck = PIN_HARD, int miso = PIN_HARD, int mosi = PIN_HARD, int cs = PIN_NC)
    {
        TREG rpin_miso = NULL;
        uint32_t rval_miso = 0;
        TREG rpin_mosi = NULL;
        uint32_t rval_mosi = 0;
        TREG rpin_cs = NULL;
        uint32_t rval_cs = 0;
        TREG con = (TREG)(0xBF821000 + (0x200 * id));
        *(con + REG_CLR) = _SPI1CON_MSSEN_MASK;
        if (miso == PIN_HARD)
        {
            rpin_miso = u->rpin_miso;
            rval_miso = u->rval_miso;
        }
        if (mosi == PIN_HARD)
        {
            rpin_mosi = u->rpin_mosi;
            rval_mosi = u->rval_mosi;
        }
        if (cs == PIN_HARD)
        {
            rpin_cs = u->rpin_cs;
            rval_cs = u->rval_cs;
            if (rpin_cs)
            {
                *(con + REG_SET) = _SPI1CON_MSSEN_MASK;
            }
        }
        if (cs > -1)
            pinMode(cs, OUTPUT);
        setRPins(rpin_miso, rval_miso, rpin_mosi, rval_mosi, rpin_cs, rval_cs);
    }

    ~SPIClass() { end(); }

    SPIClass(uint32_t ID)
    {
        id = ID;
        u = spi_get_context(id);
        ASSERT(u, "Wrong SPI Context");
        mode = -1;
        brg_hz = -1;
        bit_order = -1;
        bit_size = 8;
        setRPins(u->rpin_miso, u->rval_miso, u->rpin_mosi, u->rval_mosi, u->rpin_cs, u->rval_cs);
    }

    void begin(int sck = PIN_HARD, int miso = PIN_HARD, int mosi = PIN_HARD, int cs = PIN_NC)
    {
        end();
        setPins(sck, miso, mosi, cs);
        beginTransaction();
        *(((TREG)(u->base + 0x00)) + REG_SET) = _SPI1CON_ON_MASK | _SPI1CON_MSTEN_MASK;
    }

    void end()
    {
        *((TREG)(u->base + 0x00)) = 0;
        *((TREG)(u->base + 0x10)) = 0;
    }

    void setFrequency(uint32_t Hz)
    {
        if (brg_hz != Hz)
        {
            brg_hz = Hz;
            *((TREG)(u->base + 0x30)) = ((SYS_CLK_BUS_PERIPHERAL_1 / Hz) >> 1) - 1;
        }
    }

    void setDataMode(uint8_t mode)
    {
        if (mode != mode)
        {
            mode = mode;
            TREG con_clr = ((TREG)(u->base + 0x00)) + REG_CLR;
            *(con_clr + (mode & 1)) = _SPI1CON_CKE_MASK;
            mode >>= 1;
            *(con_clr + (mode & 1)) = _SPI1CON_CKP_MASK;
        }
    }

    void setBitOrder(uint8_t order)
    {
        if (bit_order != order)
            bit_order = order;
    }

    void setBitSize(uint8_t size)
    {
        if (bit_size != size)
        {
            bit_size = size;
            TREG con = ((TREG)(u->base + 0x00));
            *con &= ~(3 << 10); /* else 8-bit data width */
            switch (size)
            {
            case 16:
                *con |= (1 << 10); /* 16-bit data width */
                break;
            case 32:
                *con |= (2 << 10); /* 32-bit data width */
                break;
            }
        }
    }

    uint8_t transfer(uint8_t tx)
    {
        TREG sta = ((TREG)(u->base + 0x10));
        TREG buf = ((TREG)(u->base + 0x20));

        if (bit_order == LSBFIRST)
            tx = reverseByte(tx); //  __builtin_mips_bitrev() _bswapw() ?

        *buf = tx;
        while (!(*sta & _SPI1STAT_SPIRBF_MASK))
            continue;
        uint8_t rx = *buf;

        if (bit_order == LSBFIRST)
            rx = reverseByte(rx);

        return rx;
    }

    int transfer(int tx) { return transfer((uint8_t)tx); }

    uint32_t transfer(uint32_t tx) { return transfer((uint8_t)tx); }

    uint16_t transfer(uint16_t data)
    {
        union
        {
            uint16_t val;
            struct
            {
                uint8_t lsb;
                uint8_t msb;
            };
        } t;
        t.val = data;
        if (bit_order == LSBFIRST)
        {
            t.lsb = transfer(t.lsb);
            t.msb = transfer(t.msb);
        }
        else
        {
            t.msb = transfer(t.msb);
            t.lsb = transfer(t.lsb);
        }
        return t.val;
    }

    int transfer(uint8_t *buf, size_t count)
    {
        for (size_t i = 0; i < count; i++)
            transfer(*buf++);
        return count;
    }

    int transfer(uint16_t *buf, size_t count)
    {
        for (size_t i = 0; i < count; i++)
            transfer(*buf++);
        return count;
    }

    void write(uint8_t *buf, size_t count) { transfer(buf, count); }

    void write(uint16_t *buf, size_t count) { transfer(buf, count); }

    void beginTransaction(SPISettings settings)
    {
        setFrequency(settings.clock);
        setDataMode(settings.mode);
        setBitOrder(settings.order);
    }

    void beginTransaction()
    {
        setFrequency(1000000);
        setDataMode(SPI_MODE0);
        setBitOrder(MSBFIRST);
    }

    uint32_t getClockDivider() { return *((TREG)(u->base + 0x30)); }

    void setClockDivider(uint32_t div) { *((TREG)(u->base + 0x30)) = div; }

    /*** not used ***/

    void endTransaction(void) {}
    void usingInterrupt(int interruptNumber) {}
    void notUsingInterrupt(int interruptNumber) {}
    void attachInterrupt(){};
    void detachInterrupt(){};

    /*** helper functions ***/

    void MosiEnable(int pin)
    {
        *(((TREG)(u->base + 0x00)) + REG_SET) = _SPI1CON_DISSDO_MASK;
    }

    void MosiDisable(int pin)
    {
        static int once = 0;
        if (0 == once)
        {
            once = 1;
            pinMode(pin, OUTPUT);
            digitalWrite(pin, 1);
        }
        *(((TREG)(u->base + 0x00)) + REG_CLR) = _SPI1CON_DISSDO_MASK;
    }

    void flush()
    {
        for (int i = 0; i < 16; i++)
            *((TREG)(0xBF821020 + (0x200 * id)));
    }

#ifdef SPI_HELPER /* for 16 bit collor */

    void fillColor(size_t size, uint32_t data) // MSBFIRST
    {
        TREG con = (TREG)(u->base + 0x00);
        TREG sta = (TREG)(u->base + 0x10);
        int temp = *con;
        *con &= ~(3 << 10);
        *con |= (1 << 10); /* 16-bit data width */
        while (size--)
        {
            *(TREG)(u->base + 0x20) = data;
            while (!(*sta & _SPI1STAT_SPIRBF_MASK))
                continue;
            *(TREG)(u->base + 0x20);
        }
        *con = temp;
    }

    void drawImage(size_t size, uint16_t *data) // MSBFIRST
    {
        TREG con = (TREG)(u->base + 0x00);
        TREG sta = (TREG)(u->base + 0x10);
        int temp = *con;
        *con &= ~(3 << 10);
        *con |= (1 << 10); /* 16-bit data width */
        while (size--)
        {
            *(TREG)(u->base + 0x20) = *data++;
            while (!(*sta & _SPI1STAT_SPIRBF_MASK))
                continue;
            *(TREG)(u->base + 0x20);
        }
        *con = temp;
    }

#endif
};

extern SPIClass SPI;

#endif // __cplusplus
#endif //__SPI_CLASS_H__