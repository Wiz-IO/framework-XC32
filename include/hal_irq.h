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

#ifndef HAL_IRQ_H_
#define HAL_IRQ_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <xc.h>
#include <stdbool.h>

    ///////////////////////////////////////////////////////////////////////////

    typedef enum
    {
        IRQ_CORE_TIMER = _CORE_TIMER_VECTOR,
        IRQ_CORE_SOFTWARE_0 = _CORE_SOFTWARE_0_VECTOR,
        IRQ_CORE_SOFTWARE_1 = _CORE_SOFTWARE_1_VECTOR,
        IRQ_EXTERNAL_0 = _EXTERNAL_0_VECTOR,
        IRQ_TIMER_1 = _TIMER_1_VECTOR,
        IRQ_INPUT_CAPTURE_1_ERROR = _INPUT_CAPTURE_1_ERROR_VECTOR,
        IRQ_INPUT_CAPTURE_1 = _INPUT_CAPTURE_1_VECTOR,
        IRQ_OUTPUT_COMPARE_1 = _OUTPUT_COMPARE_1_VECTOR,
        IRQ_EXTERNAL_1 = _EXTERNAL_1_VECTOR,
        IRQ_TIMER_2 = _TIMER_2_VECTOR,
        IRQ_INPUT_CAPTURE_2_ERROR = _INPUT_CAPTURE_2_ERROR_VECTOR,
        IRQ_INPUT_CAPTURE_2 = _INPUT_CAPTURE_2_VECTOR,
        IRQ_OUTPUT_COMPARE_2 = _OUTPUT_COMPARE_2_VECTOR,
        IRQ_EXTERNAL_2 = _EXTERNAL_2_VECTOR,
        IRQ_TIMER_3 = _TIMER_3_VECTOR,
        IRQ_INPUT_CAPTURE_3_ERROR = _INPUT_CAPTURE_3_ERROR_VECTOR,
        IRQ_INPUT_CAPTURE_3 = _INPUT_CAPTURE_3_VECTOR,
        IRQ_OUTPUT_COMPARE_3 = _OUTPUT_COMPARE_3_VECTOR,
        IRQ_EXTERNAL_3 = _EXTERNAL_3_VECTOR,
        IRQ_TIMER_4 = _TIMER_4_VECTOR,
        IRQ_INPUT_CAPTURE_4_ERROR = _INPUT_CAPTURE_4_ERROR_VECTOR,
        IRQ_INPUT_CAPTURE_4 = _INPUT_CAPTURE_4_VECTOR,
        IRQ_OUTPUT_COMPARE_4 = _OUTPUT_COMPARE_4_VECTOR,
        IRQ_EXTERNAL_4 = _EXTERNAL_4_VECTOR,
        IRQ_TIMER_5 = _TIMER_5_VECTOR,
        IRQ_INPUT_CAPTURE_5_ERROR = _INPUT_CAPTURE_5_ERROR_VECTOR,
        IRQ_INPUT_CAPTURE_5 = _INPUT_CAPTURE_5_VECTOR,
        IRQ_OUTPUT_COMPARE_5 = _OUTPUT_COMPARE_5_VECTOR,
        IRQ_TIMER_6 = _TIMER_6_VECTOR,
        IRQ_INPUT_CAPTURE_6_ERROR = _INPUT_CAPTURE_6_ERROR_VECTOR,
        IRQ_INPUT_CAPTURE_6 = _INPUT_CAPTURE_6_VECTOR,
        IRQ_OUTPUT_COMPARE_6 = _OUTPUT_COMPARE_6_VECTOR,
        IRQ_TIMER_7 = _TIMER_7_VECTOR,
        IRQ_INPUT_CAPTURE_7_ERROR = _INPUT_CAPTURE_7_ERROR_VECTOR,
        IRQ_INPUT_CAPTURE_7 = _INPUT_CAPTURE_7_VECTOR,
        IRQ_OUTPUT_COMPARE_7 = _OUTPUT_COMPARE_7_VECTOR,
        IRQ_TIMER_8 = _TIMER_8_VECTOR,
        IRQ_INPUT_CAPTURE_8_ERROR = _INPUT_CAPTURE_8_ERROR_VECTOR,
        IRQ_INPUT_CAPTURE_8 = _INPUT_CAPTURE_8_VECTOR,
        IRQ_OUTPUT_COMPARE_8 = _OUTPUT_COMPARE_8_VECTOR,
        IRQ_TIMER_9 = _TIMER_9_VECTOR,
        IRQ_INPUT_CAPTURE_9_ERROR = _INPUT_CAPTURE_9_ERROR_VECTOR,
        IRQ_INPUT_CAPTURE_9 = _INPUT_CAPTURE_9_VECTOR,
        IRQ_OUTPUT_COMPARE_9 = _OUTPUT_COMPARE_9_VECTOR,
        IRQ_ADC = _ADC_VECTOR,
        IRQ_ADC_FIFO = _ADC_FIFO_VECTOR,
        IRQ_ADC_DC1 = _ADC_DC1_VECTOR,
        IRQ_ADC_DC2 = _ADC_DC2_VECTOR,
        IRQ_ADC_DC3 = _ADC_DC3_VECTOR,
        IRQ_ADC_DC4 = _ADC_DC4_VECTOR,
        IRQ_ADC_DC5 = _ADC_DC5_VECTOR,
        IRQ_ADC_DC6 = _ADC_DC6_VECTOR,
        IRQ_ADC_DF1 = _ADC_DF1_VECTOR,
        IRQ_ADC_DF2 = _ADC_DF2_VECTOR,
        IRQ_ADC_DF3 = _ADC_DF3_VECTOR,
        IRQ_ADC_DF4 = _ADC_DF4_VECTOR,
        IRQ_ADC_DF5 = _ADC_DF5_VECTOR,
        IRQ_ADC_DF6 = _ADC_DF6_VECTOR,
        IRQ_ADC_FAULT = _ADC_FAULT_VECTOR,
        IRQ_ADC_DATA0 = _ADC_DATA0_VECTOR,
        IRQ_ADC_DATA1 = _ADC_DATA1_VECTOR,
        IRQ_ADC_DATA2 = _ADC_DATA2_VECTOR,
        IRQ_ADC_DATA3 = _ADC_DATA3_VECTOR,
        IRQ_ADC_DATA4 = _ADC_DATA4_VECTOR,
        IRQ_ADC_DATA5 = _ADC_DATA5_VECTOR,
        IRQ_ADC_DATA6 = _ADC_DATA6_VECTOR,
        IRQ_ADC_DATA7 = _ADC_DATA7_VECTOR,
        IRQ_ADC_DATA8 = _ADC_DATA8_VECTOR,
        IRQ_ADC_DATA9 = _ADC_DATA9_VECTOR,
        IRQ_ADC_DATA10 = _ADC_DATA10_VECTOR,
        IRQ_ADC_DATA11 = _ADC_DATA11_VECTOR,
        IRQ_ADC_DATA12 = _ADC_DATA12_VECTOR,
        IRQ_ADC_DATA13 = _ADC_DATA13_VECTOR,
        IRQ_ADC_DATA14 = _ADC_DATA14_VECTOR,
        IRQ_ADC_DATA15 = _ADC_DATA15_VECTOR,
        IRQ_ADC_DATA16 = _ADC_DATA16_VECTOR,
        IRQ_ADC_DATA17 = _ADC_DATA17_VECTOR,
        IRQ_ADC_DATA18 = _ADC_DATA18_VECTOR,
        IRQ_ADC_DATA19 = _ADC_DATA19_VECTOR,
        IRQ_ADC_DATA20 = _ADC_DATA20_VECTOR,
        IRQ_ADC_DATA21 = _ADC_DATA21_VECTOR,
        IRQ_ADC_DATA22 = _ADC_DATA22_VECTOR,
        IRQ_ADC_DATA23 = _ADC_DATA23_VECTOR,
        IRQ_ADC_DATA24 = _ADC_DATA24_VECTOR,
        IRQ_ADC_DATA25 = _ADC_DATA25_VECTOR,
        IRQ_ADC_DATA26 = _ADC_DATA26_VECTOR,
        IRQ_ADC_DATA27 = _ADC_DATA27_VECTOR,
        IRQ_ADC_DATA28 = _ADC_DATA28_VECTOR,
        IRQ_ADC_DATA29 = _ADC_DATA29_VECTOR,
        IRQ_ADC_DATA30 = _ADC_DATA30_VECTOR,
        IRQ_ADC_DATA31 = _ADC_DATA31_VECTOR,
        IRQ_ADC_DATA32 = _ADC_DATA32_VECTOR,
        IRQ_ADC_DATA33 = _ADC_DATA33_VECTOR,
        IRQ_ADC_DATA34 = _ADC_DATA34_VECTOR,
        IRQ_ADC_DATA35 = _ADC_DATA35_VECTOR,
        IRQ_ADC_DATA36 = _ADC_DATA36_VECTOR,
        IRQ_ADC_DATA37 = _ADC_DATA37_VECTOR,
        IRQ_ADC_DATA38 = _ADC_DATA38_VECTOR,
        IRQ_ADC_DATA39 = _ADC_DATA39_VECTOR,
        IRQ_ADC_DATA40 = _ADC_DATA40_VECTOR,
        IRQ_ADC_DATA41 = _ADC_DATA41_VECTOR,
        IRQ_ADC_DATA42 = _ADC_DATA42_VECTOR,
        IRQ_ADC_DATA43 = _ADC_DATA43_VECTOR,
        IRQ_ADC_DATA44 = _ADC_DATA44_VECTOR,
        IRQ_CORE_PERF_COUNT = _CORE_PERF_COUNT_VECTOR,
        IRQ_CORE_FAST_DEBUG_CHAN = _CORE_FAST_DEBUG_CHAN_VECTOR,
        IRQ_SYSTEM_BUS_PROTECTION = _SYSTEM_BUS_PROTECTION_VECTOR,
        IRQ_SPI1_FAULT = _SPI1_FAULT_VECTOR,
        IRQ_SPI1_RX = _SPI1_RX_VECTOR,
        IRQ_SPI1_TX = _SPI1_TX_VECTOR,
        IRQ_UART1_FAULT = _UART1_FAULT_VECTOR,
        IRQ_UART1_RX = _UART1_RX_VECTOR,
        IRQ_UART1_TX = _UART1_TX_VECTOR,
        IRQ_I2C1_BUS = _I2C1_BUS_VECTOR,
        IRQ_I2C1_SLAVE = _I2C1_SLAVE_VECTOR,
        IRQ_I2C1_MASTER = _I2C1_MASTER_VECTOR,
        IRQ_CHANGE_NOTICE_A = _CHANGE_NOTICE_A_VECTOR,
        IRQ_CHANGE_NOTICE_B = _CHANGE_NOTICE_B_VECTOR,
        IRQ_CHANGE_NOTICE_C = _CHANGE_NOTICE_C_VECTOR,
        IRQ_CHANGE_NOTICE_D = _CHANGE_NOTICE_D_VECTOR,
        IRQ_CHANGE_NOTICE_E = _CHANGE_NOTICE_E_VECTOR,
        IRQ_CHANGE_NOTICE_F = _CHANGE_NOTICE_F_VECTOR,
        IRQ_CHANGE_NOTICE_G = _CHANGE_NOTICE_G_VECTOR,
        ////IRQ_CHANGE_NOTICE_H = _CHANGE_NOTICE_H_VECTOR,
        ////IRQ_CHANGE_NOTICE_J = _CHANGE_NOTICE_J_VECTOR,
        ////IRQ_CHANGE_NOTICE_K = _CHANGE_NOTICE_K_VECTOR,
        IRQ_PMP = _PMP_VECTOR,
        IRQ_PMP_ERROR = _PMP_ERROR_VECTOR,
        IRQ_COMPARATOR_1 = _COMPARATOR_1_VECTOR,
        IRQ_COMPARATOR_2 = _COMPARATOR_2_VECTOR,
        IRQ_USB = _USB_VECTOR,
        IRQ_USB_DMA = _USB_DMA_VECTOR,
        IRQ_DMA0 = _DMA0_VECTOR,
        IRQ_DMA1 = _DMA1_VECTOR,
        IRQ_DMA2 = _DMA2_VECTOR,
        IRQ_DMA3 = _DMA3_VECTOR,
        IRQ_DMA4 = _DMA4_VECTOR,
        IRQ_DMA5 = _DMA5_VECTOR,
        IRQ_DMA6 = _DMA6_VECTOR,
        IRQ_DMA7 = _DMA7_VECTOR,
        IRQ_SPI2_FAULT = _SPI2_FAULT_VECTOR,
        IRQ_SPI2_RX = _SPI2_RX_VECTOR,
        IRQ_SPI2_TX = _SPI2_TX_VECTOR,
        IRQ_UART2_FAULT = _UART2_FAULT_VECTOR,
        IRQ_UART2_RX = _UART2_RX_VECTOR,
        IRQ_UART2_TX = _UART2_TX_VECTOR,
        IRQ_I2C2_BUS = _I2C2_BUS_VECTOR,
        IRQ_I2C2_SLAVE = _I2C2_SLAVE_VECTOR,
        IRQ_I2C2_MASTER = _I2C2_MASTER_VECTOR,
        IRQ_CAN1 = _CAN1_VECTOR,
        IRQ_CAN2 = _CAN2_VECTOR,
        IRQ_ETHERNET = _ETHERNET_VECTOR,
        IRQ_SPI3_FAULT = _SPI3_FAULT_VECTOR,
        IRQ_SPI3_RX = _SPI3_RX_VECTOR,
        IRQ_SPI3_TX = _SPI3_TX_VECTOR,
        IRQ_UART3_FAULT = _UART3_FAULT_VECTOR,
        IRQ_UART3_RX = _UART3_RX_VECTOR,
        IRQ_UART3_TX = _UART3_TX_VECTOR,
        IRQ_I2C3_BUS = _I2C3_BUS_VECTOR,
        IRQ_I2C3_SLAVE = _I2C3_SLAVE_VECTOR,
        IRQ_I2C3_MASTER = _I2C3_MASTER_VECTOR,
        IRQ_SPI4_FAULT = _SPI4_FAULT_VECTOR,
        IRQ_SPI4_RX = _SPI4_RX_VECTOR,
        IRQ_SPI4_TX = _SPI4_TX_VECTOR,
        IRQ_RTCC = _RTCC_VECTOR,
        IRQ_FLASH_CONTROL = _FLASH_CONTROL_VECTOR,
        IRQ_PREFETCH = _PREFETCH_VECTOR,
        IRQ_SQI1 = _SQI1_VECTOR,
        IRQ_UART4_FAULT = _UART4_FAULT_VECTOR,
        IRQ_UART4_RX = _UART4_RX_VECTOR,
        IRQ_UART4_TX = _UART4_TX_VECTOR,
        IRQ_I2C4_BUS = _I2C4_BUS_VECTOR,
        IRQ_I2C4_SLAVE = _I2C4_SLAVE_VECTOR,
        IRQ_I2C4_MASTER = _I2C4_MASTER_VECTOR,
        IRQ_SPI5_FAULT = _SPI5_FAULT_VECTOR,
        IRQ_SPI5_RX = _SPI5_RX_VECTOR,
        IRQ_SPI5_TX = _SPI5_TX_VECTOR,
        IRQ_UART5_FAULT = _UART5_FAULT_VECTOR,
        IRQ_UART5_RX = _UART5_RX_VECTOR,
        IRQ_UART5_TX = _UART5_TX_VECTOR,
        IRQ_I2C5_BUS = _I2C5_BUS_VECTOR,
        IRQ_I2C5_SLAVE = _I2C5_SLAVE_VECTOR,
        IRQ_I2C5_MASTER = _I2C5_MASTER_VECTOR,
        IRQ_SPI6_FAULT = _SPI6_FAULT_VECTOR,
        IRQ_SPI6_RX = _SPI6_RX_VECTOR,
        IRQ_SPI6_TX = _SPI6_TX_VECTOR,
        IRQ_UART6_FAULT = _UART6_FAULT_VECTOR,
        IRQ_UART6_RX = _UART6_RX_VECTOR,
        IRQ_UART6_TX = _UART6_TX_VECTOR,
        IRQ_ADC_EOS = _ADC_EOS_VECTOR,
        IRQ_ADC_ARDY = _ADC_ARDY_VECTOR,
        IRQ_ADC_URDY = _ADC_URDY_VECTOR,
        IRQ_ADC_EARLY = _ADC_EARLY_VECTOR,
        IRQ_ADC0_EARLY = _ADC0_EARLY_VECTOR,
        IRQ_ADC1_EARLY = _ADC1_EARLY_VECTOR,
        IRQ_ADC2_EARLY = _ADC2_EARLY_VECTOR,
        IRQ_ADC3_EARLY = _ADC3_EARLY_VECTOR,
        IRQ_ADC4_EARLY = _ADC4_EARLY_VECTOR,
        IRQ_ADC7_EARLY = _ADC7_EARLY_VECTOR,
        IRQ_ADC0_WARM = _ADC0_WARM_VECTOR,
        IRQ_ADC1_WARM = _ADC1_WARM_VECTOR,
        IRQ_ADC2_WARM = _ADC2_WARM_VECTOR,
        IRQ_ADC3_WARM = _ADC3_WARM_VECTOR,
        IRQ_ADC4_WARM = _ADC4_WARM_VECTOR,
        IRQ_ADC7_WARM = _ADC7_WARM_VECTOR,
    } IRQ_E;

    ///////////////////////////////////////////////////////////////////////////

    static __inline__ void __attribute__((always_inline)) IRQ_Init(void)
    {
        PRISS = 0x76543210; // Configure Shadow Register Set
        while (PRISS != 0x76543210)
            continue;
    }

    static __inline__ void __attribute__((always_inline)) IRQ_Enable(IRQ_E source)
    {
        volatile uint32_t *IECx = (volatile uint32_t *)(&IEC0 + ((0x10 * (source / 32)) / 4));
        volatile uint32_t *IECxSET = (volatile uint32_t *)(IECx + 2);
        *IECxSET = 1 << (source & 0x1f);
    }

    static __inline__ void __attribute__((always_inline)) IRQ_Disable(IRQ_E source)
    {
        volatile uint32_t *IECx = (volatile uint32_t *)(&IEC0 + ((0x10 * (source / 32)) / 4));
        volatile uint32_t *IECxCLR = (volatile uint32_t *)(IECx + 1);
        *IECxCLR = 1 << (source & 0x1f);
    }

    static __inline__ bool __attribute__((always_inline)) IRQ_IsEnabled(IRQ_E source)
    {
        return (bool)((*(volatile uint32_t *)(&IEC0 + ((0x10 * (source / 32)) / 4)) >> (source & 0x1f)) & 0x01);
    }

    static __inline__ bool __attribute__((always_inline)) IRQ_Get(IRQ_E source)
    {
        return (bool)((*(volatile uint32_t *)(&IFS0 + ((0x10 * (source / 32)) / 4)) >> (source & 0x1f)) & 0x1);
    }

    static __inline__ void __attribute__((always_inline)) IRQ_Set(IRQ_E source, bool value)
    {
        volatile uint32_t *IFSx = (volatile uint32_t *)(&IFS0 + ((0x10 * (source / 32)) / 4));
        *(volatile uint32_t *)(IFSx + 1 + value) = 1 << (source & 0x1F);
    }

    static __inline__ void __attribute__((always_inline)) IRQ_Clear(IRQ_E source)
    {
        volatile uint32_t *IFSx = (volatile uint32_t *)(&IFS0 + ((0x10 * (source / 32)) / 4));
        *(volatile uint32_t *)(IFSx + 1 + 0) = 1 << (source & 0x1F);
    }

    static __inline__ void __attribute__((always_inline)) __irq_set(volatile unsigned int *reg, uint8_t lsb, uint8_t msb, unsigned int value)
    {
        unsigned int mask = (unsigned int)(1 << (msb - lsb + 1)) - 1;
        *reg = (*reg & ~(mask << lsb)) | ((value & mask) << lsb);
    }

    static __inline__ void __attribute__((always_inline)) IRQ_Priority(IRQ_E source, unsigned char pri, unsigned char sub)
    {
        volatile unsigned int *reg = (&IPC0 + (&IPC1 - &IPC0) * (source / 4));
        __irq_set(reg, source * 8 % 32 + 2, source * 8 % 32 + 4, pri & 7);
        __irq_set(reg, source * 8 % 32 + 0, source * 8 % 32 + 1, sub & 7);
    }

    ///////////////////////////////////////////////////////////////////////////

    static __inline__ void __attribute__((always_inline)) IRQ_INT_Enable(void)
    {
        __builtin_enable_interrupts();
    }

    static __inline__ bool __attribute__((always_inline)) IRQ_INT_Disable(void)
    {
        return (bool)(__builtin_disable_interrupts() & 0x01); // return the interrupt status
    }

    static __inline__ void __attribute__((always_inline)) IRQ_INT_Restore(bool state)
    {
        if (state)
        {
            __builtin_enable_interrupts(); // restore the state of CP0 Status register before the disable occurred
        }
    }

    ///////////////////////////////////////////////////////////////////////////

    // clang-format off
#define mCTClearIntFlag()                   (IFS0CLR = _IFS0_CTIF_MASK)
#define mCTGetIntFlag()                     (IFS0bits.CTIF)
#define mCTIntEnable(enable)                (IEC0CLR = _IEC0_CTIE_MASK, IEC0SET = ((enable) << _IEC0_CTIE_POSITION))
#define mCTGetIntEnable()                   (IEC0bits.CTIE)
#define mCTSetIntPriority(priority)         (IPC0CLR = _IPC0_CTIP_MASK, IPC0SET = ((priority) << _IPC0_CTIP_POSITION))
#define mCTGetIntPriority()                 (IPC0bits.CTIP)
#define mCTSetIntSubPriority(subPriority)   (IPC0CLR = _IPC0_CTIS_MASK, IPC0SET = ((subPriority) << _IPC0_CTIS_POSITION))
#define mCTGetIntSubPriority()              (IPC0bits.CTIS)
    // clang-format on

    ///////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif
#endif /* HAL_IRQ_H_ */