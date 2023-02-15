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

#include <interface.h>
#include "wdrv_mrf24wn_api.h"

#define MRF_IRQ _EXTERNAL_3_VECTOR
#define MRF_SPI_ID 2

static inline void SYS_INT_SourceEnable(int n)
{
    IRQ_Clear((IRQ_E)n);
    IRQ_Enable((IRQ_E)n);
}

static inline int SYS_INT_SourceDisable(int n)
{
    IRQ_Disable((IRQ_E)n);
    IRQ_Clear((IRQ_E)n);
    return 0;
}

static inline void SYS_INT_SourceStatusClear(int n)
{
    IRQ_Clear((IRQ_E)n);
}

// LIB
int SYS_INT_Disable(void)
{
    return disableInterrupts();
}

// LIB
void SYS_INT_Restore(int st)
{
    restoreInterrupts(st);
}

// LIB
void WDRV_INTR_SourceEnable(void)
{
    SYS_INT_SourceEnable(MRF_IRQ);
}

// LIB
void WDRV_INTR_SourceDisable(void)
{
    SYS_INT_SourceDisable(MRF_IRQ);
}

// LIB
void WDRV_INTR_Init(void)
{
    SYS_INT_SourceDisable(MRF_IRQ);
    SYS_INT_SourceStatusClear(MRF_IRQ);
}

// LIB
void WDRV_INTR_Deinit(void)
{
    SYS_INT_SourceDisable(MRF_IRQ);
}

// LIB
void WDRV_GPIO_PowerOn(void)
{
    LATASET = _LATA_LATA0_MASK; // WD_HIBERNATE_HI();
}

// LIB
void WDRV_GPIO_PowerOff(void)
{
    LATACLR = _LATA_LATA0_MASK; // WD_HIBERNATE_LO();
}

static inline void SPI_Write(uint8_t *buf, uint16_t size)
{
    while (size--)
        spi_xfer(MRF_SPI_ID, *buf++);
}

static inline void SPI_Read(uint8_t *buf, uint16_t size)
{
    while (size--)
        *buf++ = spi_xfer(MRF_SPI_ID, 0xFF);
}

// LIB
void WDRV_SPI_Out(uint8_t *const OutBuf, uint16_t OutSize)
{
    IRQ_Disable(MRF_IRQ);
    LATBCLR = _LATB_LATB8_MASK;
    SPI_Write(OutBuf, OutSize);
    LATBSET = _LATB_LATB8_MASK;
    IRQ_Enable(MRF_IRQ);
}

// LIB
void WDRV_SPI_In(uint8_t *const OutBuf, uint16_t OutSize, uint8_t *const InBuf, uint16_t InSize) // ok
{
    IRQ_Disable(MRF_IRQ);
    LATBCLR = _LATB_LATB8_MASK;
    if (OutSize > 0)
        SPI_Write(OutBuf, OutSize);
    if (InSize > 0)
        SPI_Read(InBuf, InSize);
    LATBSET = _LATB_LATB8_MASK;
    IRQ_Enable(MRF_IRQ);
}

void WDRV_HARD_Init(void)
{
    pinMux(&INT3R, 2);             // F4
    pinMux(&SDI3R, 6);             // B10
    pinMux(&RPB9R, 7);             // B9
    LATACLR = _LATA_LATA0_MASK;    // WD_HIBERNATE_LO
    TRISACLR = _TRISA_TRISA0_MASK; // WD_HIBERNATE_DIR_OUTPUT
    LATBSET = _LATB_LATB8_MASK;    // WD_CS_HI
    TRISBCLR = _TRISB_TRISB8_MASK; // WD_CS_DIR_OUTPUT
    spi_init(MRF_SPI_ID, 0, -1);   // 50 MHz
    IRQ_Priority(MRF_IRQ, 4, 0);
    IRQ_Clear(MRF_IRQ);
}

void __ISR(MRF_IRQ, IPL4AUTO) ISR_MRF24WN(void)
{
    IRQ_Disable(MRF_IRQ);
    IRQ_Clear(MRF_IRQ);
    WDRV_EXT_HWInterruptHandler(NULL);
}
