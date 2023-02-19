////////////////////////////////////////////////////////////////////////////////////////
//      2023 Georgi Angelov
//
//      Copyright (c) 2021 Earle F. Philhower, III <earlephilhower@yahoo.com>
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
////////////////////////////////////////////////////////////////////////////////////////

#ifndef _SERIAL_USB_H_
#define _SERIAL_USB_H_

#ifdef __cplusplus

#include "HardwareSerial.h"
#include "interface.h"
#include "tusb.h"

#define DEFAULT_USB_SERIAL_PRIO 2
#define DEFAULT_USB_SERIAL_PRIO_SUB 0

#ifndef USB_SERIAL_PRIO
#define USB_SERIAL_PRIO DEFAULT_USB_SERIAL_PRIO
#endif

#ifndef USB_SERIAL_PRIO_SUB
#define USB_SERIAL_PRIO_SUB DEFAULT_USB_SERIAL_PRIO_SUB
#endif

class SerialUSB : public HardwareSerial
{
public:
    SerialUSB() {}
    ~SerialUSB() {}
    void end(void) {}

    void begin(unsigned long baud, int config)
    {
        if (0 == (IEC4 & _IEC4_USBIE_MASK))
        {
            IPC33CLR = _IPC33_USBIP_MASK;
            IPC33SET = (USB_SERIAL_PRIO << _IPC33_USBIP_POSITION);
            IPC33CLR = _IPC33_USBIS_MASK;
            IPC33SET = (USB_SERIAL_PRIO_SUB << _IPC33_USBIS_POSITION);
            USBCRCONbits.USBIE = 0;
            IFS4CLR = _IFS4_USBIF_MASK;
            IEC4SET = _IEC4_USBIE_MASK;
            USBCRCONbits.PHYIDEN = 0;
            USBCRCONbits.USBIDVAL = 1;
            USBCRCONbits.USBIDOVEN = 1;
        }

        if (!tusb_inited())
            tusb_init();

        //extern void (*SerialUsbProcess)(void); SerialUsbProcess = tud_task;
        addProcess(tud_task);

        while (!tud_cdc_connected())
            tud_task();
    }

    void begin(unsigned long baud = 0) { begin(0, 0); }

    size_t write(const uint8_t *buf, size_t size)
    {
        uint32_t n, a, i = 0;
        if (tud_cdc_connected())
        {
            for (i = 0; i < size;)
            {
                n = size - i;
                tud_task();
                a = tud_cdc_write_available();
                if (n > a)
                    n = a;
                if (n)
                {
                    i += tud_cdc_write(buf + i, n);
                    tud_task();
                    tud_cdc_write_flush();
                }
                else
                {
                    break;
                }
            }
            tud_task();
        }
        return i;
    }

    inline size_t write(uint8_t c) { return write(&c, 1); }

    int read()
    {
        int c = -1;
        tud_task();
        if (tud_cdc_connected() && tud_cdc_available())
            c = tud_cdc_read_char();
        return c;
    }

    int available(void)
    {
        int res = 0;
        tud_task();
        if (tud_cdc_connected())
            res = tud_cdc_available();
        return res;
    }

    int peek(void)
    {
        int c = 0, res = 0;
        tud_task();
        if (tud_cdc_connected())
            res = tud_cdc_n_peek(0, (uint8_t *)&c) ? (int)c : -1;
        return res;
    }

    void flush(void)
    {
        tud_task();
        if (tud_cdc_connected())
            tud_cdc_write_flush();
    }

    operator bool()
    {
        tud_task();
        return tud_cdc_connected();
    }

    using Print::write;
};

#endif
#endif // _SERIAL_USB_H_