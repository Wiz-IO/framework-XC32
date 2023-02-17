/*
SoftwareSerial.h

SoftwareSerial.cpp - Implementation of the Arduino software serial for PIC32.
Copyright (c) 2015-2016 Peter Lerup. All rights reserved.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

*/

#pragma once

#include <inttypes.h>
#include <Stream.h>
#include <RingBuffer.h>
#include <interface.h>
#include <constants.h>

#define SOFTWARE_SERIAL_DEFAULT_BRG 9600

class SoftwareSerial;

typedef struct
{
    SoftwareSerial *serial;
    void (*cb)(void);
} ss_context_t;
extern ss_context_t ss_context[SOFTWARE_SERIAL_MAX];
int ss_get_interface();

class SoftwareSerial : public Stream
{
#define WaitBitTime(wait) for (uint32_t start = ReadCoreTimer(); ReadCoreTimer() - start < wait;)

public:
    SoftwareSerial(int transmitPin, int receivePin = -1, bool inverse_logic = false, unsigned int buffSize = SOFTWARE_SERIAL_BUFFER_SIZE)
    {
        m_rx_id = -1;
        m_invert = inverse_logic;
        m_rxValid = m_txValid = m_txEnabled = m_rxEnabled = false;
        m_buffer = NULL;
        m_overflow = false;
        if (isValidPin(transmitPin))
        {
            m_txPin = transmitPin;
            pinMode(m_txPin, OUTPUT);
            digitalWrite(m_txPin, !m_invert);
            m_txValid = true;
        }
#if 1
        m_rx_id = ss_get_interface();
        if (isValidPin(receivePin) && m_rx_id < SOFTWARE_SERIAL_MAX)
        {
            m_buffer = (uint8_t *)malloc(buffSize);
            if (m_buffer != NULL)
            {
                m_buffSize = buffSize;
                m_inPos = m_outPos = 0;
                m_rxPin = receivePin;
                pinMode(m_rxPin, INPUT);
                ss_context[m_rx_id].serial = this;
                m_rxValid = true;
            }
        }
#endif
    }

    ~SoftwareSerial()
    {
        enableRx(false);
        if (m_rx_id < SOFTWARE_SERIAL_MAX)
            ss_context[m_rx_id].serial = NULL;
        if (m_buffer)
            free(m_buffer);
    }

    void end()
    {
        stopListening();
    }

    void begin(uint32_t speed = SOFTWARE_SERIAL_DEFAULT_BRG)
    {
        m_bitTime = SYS_CLK_BUS_PERIPHERAL_1 / speed;
        enableRx(true);
    }

    inline long baudRate()
    {
        return SYS_CLK_BUS_PERIPHERAL_1 / m_bitTime;
    }

    void setTransmitEnablePin(int transmitEnablePin)
    {
        if (isValidPin(transmitEnablePin))
        {
            m_txEnabled = true;
            m_txEnablePin = transmitEnablePin;
            pinMode(m_txEnablePin, OUTPUT);
            digitalWrite(m_txEnablePin, LOW);
        }
        else
        {
            m_txEnabled = false;
        }
    }

    inline bool overflow()
    {
        return m_overflow;
    }

    int peek()
    {
        if (!m_rxValid || (m_inPos == m_outPos))
            return -1;
        return m_buffer[m_outPos];
    }

    int available()
    {
        if (m_rxValid)
        {
            int avail = m_inPos - m_outPos;
            return (avail < 0) ? avail + m_buffSize : avail;
        }
        return 0;
    }

    void flush()
    {
        m_inPos = m_outPos = 0;
        m_overflow = false;
    }

    virtual size_t write(uint8_t b)
    {
        if (!m_txValid)
            return 0;
        if (m_invert)
            b = ~b;
        // Disable interrupts in order to get a clean transmit
        noInterrupts();
        if (m_txEnabled)
        {
            digitalWrite(m_txEnablePin, HIGH);
        }
        // create tx interrupts to start bit.
        digitalWrite(m_txPin, HIGH), digitalWrite(m_txPin, LOW);
        WaitBitTime(m_bitTime);
        for (uint8_t i = 0; i != 8; i++)
        {
            digitalWrite(m_txPin, (b & 1) ? HIGH : LOW);
            WaitBitTime(m_bitTime);
            b >>= 1;
        }
        // Stop bit
        digitalWrite(m_txPin, HIGH);
        WaitBitTime(m_bitTime);
        if (m_txEnabled)
        {
            digitalWrite(m_txEnablePin, LOW);
        }
        interrupts();
        return 1;
    }

    virtual int read()
    {
        if (!m_rxValid || (m_inPos == m_outPos))
            return -1;
        uint8_t ch = m_buffer[m_outPos];
        m_outPos = (m_outPos + 1) % m_buffSize;
        return ch;
    }

    void rxRead()
    {
#if 1

        // Advance the starting point for the samples but compensate for the
        // initial delay which occurs before the interrupt is delivered
        // uint32_t wait_time = m_bitTime + m_bitTime / 3 - 500, start_time = ReadCoreTimer();

        uint32_t bit_time = m_bitTime;
        // uint32_t start_time = bit_time / 5;
        // uint32_t end_time = bit_time * 4 / 5;
        uint32_t start_time = bit_time / 9;
        uint32_t end_time = bit_time * 8 / 9;
        uint8_t rec = 0;

        // (m_invert) flag invert set Start 1 And Stop 0 invert
        // But myself not need, so need extra added by yourself

        // Wait Start Bit To Start
        WaitBitTime(start_time);
        if (0 == digitalRead(m_rxPin))
        {
            for (uint8_t i = 0; i != 8; i++)
            {
                rec >>= 1;
                WaitBitTime(bit_time);
                if (digitalRead(m_rxPin))
                {
                    rec |= 0x80;
                }
            }
            // Wait Start Bit To End
            WaitBitTime(end_time);
            if (1 == digitalRead(m_rxPin))
            {
                // Stop bit Allow Into RecvBuffer
                // Store the received value in the buffer unless we have an overflow
                int next = (m_inPos + 1) % m_buffSize;
                if (next != m_outPos)
                {
                    m_buffer[m_inPos] = (m_invert) ? ~rec : rec;
                    m_inPos = next;
                }
                else
                {
                    m_overflow = true;
                }
            }
        }

#endif
    }

    void enableRx(bool on)
    {
        if (m_rxValid && m_rxEnabled != on)
        {
            if (on)
                attachInterrupt(m_rxPin, ss_context[m_rx_id].cb, m_invert ? RISING : FALLING);
            else
                detachInterrupt(m_rxPin);
            m_rxEnabled = on;
        }
    }

    bool isListening() { return m_rxEnabled; }

    bool stopListening()
    {
        enableRx(false);
        return true;
    }

    bool listen()
    {
        enableRx(true);
        return true;
    }

    operator bool() { return m_rxValid || m_txValid; }

    using Print::write;

private:
    bool isValidPin(uint8_t pin) { return (pin < PIN_MAX); }

    int m_rxPin, m_txPin, m_txEnablePin;
    bool m_rxValid, m_rxEnabled;
    bool m_txValid, m_txEnabled;
    bool m_invert, m_overflow;
    uint32_t m_bitTime, rx_bit_time;
    volatile uint32_t m_inPos, m_outPos;
    int m_buffSize;
    uint8_t *m_buffer;

    unsigned int m_rx_id;
    int ss_get_interface()
    {
        for (int i = 0; i < SOFTWARE_SERIAL_MAX; i++)
            if (NULL == ss_context[i].serial)
                return i;
        return -1;
    }
};

// If only one tx or rx wanted then use this as parameter for the unused pin
#define SW_SERIAL_UNUSED_PIN -1
