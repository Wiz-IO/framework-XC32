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
//      BOARD: Microchip Curiosity 1.0 PIC32MZ2048EFM100
//
///////////////////////////////////////////////////////////////////////////////

#include "variant.h"

///////////////////////////////////////////////////////////////////////////////

static periphery_t peri[] = {
    {
        // [00] ADC
        .adc.channel = 4,
        .adc.an_pin = 4,
    },
    {
        //  [01] ADC
        .adc.channel = 7,
        .adc.an_pin = 29,
    },
    {
        // [02] PWM1
        .pwm.OCxCON = &OC1CON,
        .pwm.TxCON = &T2CON,
        .pwm.TxCON_ALT = &T4CON,
        .pwm.RPin = &RPE8R,
        .pwm.RVal = 12,
    },
    {
        //[03] PWM2
        .pwm.OCxCON = &OC2CON,
        .pwm.TxCON = &T2CON,
        .pwm.TxCON_ALT = &T4CON,
        .pwm.RPin = &RPF2R,
        .pwm.RVal = 13,
    },
    {
        //[04] PWM_LED_GREEN
        .pwm.OCxCON = &OC4CON,
        .pwm.TxCON = &T2CON,
        .pwm.TxCON_ALT = &T2CON,
        .pwm.RPin = &RPB1R,
        .pwm.RVal = 11,
    },
    {
        //[05] RPB5 - RED
        .pwm.OCxCON = &OC3CON,
        .pwm.TxCON = &T2CON,
        .pwm.TxCON_ALT = &T4CON,
        .pwm.RPin = &RPB5R,
        .pwm.RVal = 11,
    },
    {
        //[06] PWM_LED_BLUE
        .pwm.OCxCON = &OC5CON,
        .pwm.TxCON = &T2CON,
        .pwm.TxCON_ALT = &T2CON,
        .pwm.RPin = &RPB0R,
        .pwm.RVal = 11,
    },

    {
        //[07] INT1
        .eint.irq = IRQ_EXTERNAL_1,
        .eint.pri = INT1_PRI,
        .eint.sub = INT1_SUB,
        .eint.RPin = &INT1R, // F13
        .eint.RVal = 9,
    },
    {
        //[08] INT2
        .eint.irq = IRQ_EXTERNAL_2,
        .eint.pri = INT2_PRI,
        .eint.sub = INT2_SUB,
        .eint.RPin = &INT2R, // F12
        .eint.RVal = 9,
    },

};

///////////////////////////////////////////////////////////////////////////////
// clang-format off

#define PERI(N) &peri[N]

pin_t pins[PIN_MAX] = { //          EINT       ADC         PWM            ARDUINO   
    {&ANSELE, _LATE_LATE4_MASK,     NULL,     NULL,     NULL     }, //  0 - GREEN  
    {&ANSELE, _LATE_LATE3_MASK,     NULL,     NULL,     NULL     }, //  1 - RED
    {&ANSELE, _LATE_LATE6_MASK,     NULL,     NULL,     NULL     }, //  2 - ORANGE
    {&ANSELB, _LATB_LATB5_MASK,     NULL,     NULL,     PERI(5)  }, //  3 - R 
    {&ANSELB, _LATB_LATB1_MASK,     NULL,     NULL,     PERI(4)  }, //  4 - G 
    {&ANSELB, _LATB_LATB0_MASK,     NULL,     NULL,     PERI(6)  }, //  5 - B
    {&ANSELG, _LATG_LATG12_MASK,    NULL,     NULL,     NULL     }, //  6 - BUTTON

    /*** microBUS 1 ***/
    {&ANSELB, _LATB_LATB4_MASK,     NULL,     PERI(0),  NULL     }, //  7 - ADC   RPB4 / AN4_ADC4
    {&ANSELA, _LATA_LATA9_MASK,     NULL,     NULL,     NULL     }, //  8 - RST   A9/RST
    {&ANSELD, _LATD_LATD4_MASK,     NULL,     NULL,     NULL     }, //  9 - CS1   RPD4    (RS)
    {&ANSELD, _LATD_LATD1_MASK,     NULL,     NULL,     NULL     }, // 10 - SCK1  RPD1
    {&ANSELD, _LATD_LATD14_MASK,    NULL,     NULL,     NULL     }, // 11 - MISO1 RPD14   (DC)
    {&ANSELD, _LATD_LATD3_MASK,     NULL,     NULL,     NULL     }, // 12 - MOSI1 RPD3
    {&ANSELE, _LATE_LATE8_MASK,     NULL,     NULL,     PERI(2)  }, // 13 - PWM   RPE8
    {&ANSELF, _LATF_LATF13_MASK,    PERI(7),  NULL,     NULL     }, // 14 - INT   F13 / INT1
    {&ANSELD, _LATD_LATD10_MASK,    NULL,     NULL,     NULL     }, // 15 - RX    RPD10
    {&ANSELD, _LATD_LATD15_MASK,    NULL,     NULL,     NULL     }, // 16 - TX    RPD15
    {&ANSELA, _LATA_LATA14_MASK,    NULL,     NULL,     NULL     }, // 17 - SCL1  A14
    {&ANSELA, _LATA_LATA15_MASK,    NULL,     NULL,     NULL     }, // 18 - SDA1  A15

    /*** microBUS 2 ***/
    {&ANSELA, _LATA_LATA1_MASK,     NULL,     PERI(1),  NULL     }, // 19 - ADC   RA1 / AN29_ADC7
    {&ANSELA, _LATA_LATA5_MASK,     NULL,     NULL,     NULL     }, // 20 - RST   A5
    {&ANSELD, _LATD_LATD5_MASK,     NULL,     NULL,     NULL     }, // 21 - CS    RPD5
    {&ANSELG, _LATG_LATG6_MASK,     NULL,     NULL,     NULL     }, // 22 - SCK   RG6
    {&ANSELG, _LATG_LATG0_MASK,     NULL,     NULL,     NULL     }, // 23 - MISO  RPG0
    {&ANSELG, _LATG_LATG7_MASK,     NULL,     NULL,     NULL     }, // 24 - MOSI  RG7
    {&ANSELF, _LATF_LATF2_MASK,     NULL,     NULL,     PERI(3)  }, // 25 - PWM   F2
    {&ANSELF, _LATF_LATF12_MASK,    PERI(8),  NULL,     NULL     }, // 26 - INT   F12 / INT2
    {&ANSELC, _LATC_LATC3_MASK,     NULL,     NULL,     NULL     }, // 27 - RX    RPC3
    {&ANSELC, _LATC_LATC2_MASK,     NULL,     NULL,     NULL     }, // 28 - TX    RPC2
    {&ANSELA, _LATA_LATA2_MASK,     NULL,     NULL,     NULL     }, // 29 - SCL2  A2
    {&ANSELA, _LATA_LATA3_MASK,     NULL,     NULL,     NULL     }, // 30 - SDA   A3

    /*** OTHER ***/
};

///////////////////////////////////////////////////////////////////////////////

uart_context_t uart_context[UART_CONTEXT_MAX] = {
    {UART_BASE + 0x000, IRQ_UART1_RX, SERIAL_PRI,  SERIAL_SUB,  &RPD15R, 1, &U1RXR, 3},
    {UART_BASE + 0x200, IRQ_UART2_RX, SERIAL1_PRI, SERIAL1_SUB, &RPC2R,  2, &U2RXR, 8},
    {UART_BASE + 0x400, IRQ_UART3_RX, SERIAL2_PRI, SERIAL2_SUB, NULL,    0, NULL,   0},
    {UART_BASE + 0x600, IRQ_UART4_RX, SERIAL3_PRI, SERIAL3_SUB, NULL,    0, NULL,   0},
    {UART_BASE + 0x800, IRQ_UART5_RX, SERIAL4_PRI, SERIAL4_SUB, NULL,    0, NULL,   0},
    {UART_BASE + 0xA00, IRQ_UART6_RX, SERIAL5_PRI, SERIAL5_SUB, NULL,    0, NULL,   0},
};

///////////////////////////////////////////////////////////////////////////////

spi_context_t spi_context[SPI_CONTEXT_MAX] = {
    {SPI_BASE + 0x000, &SDI1R, 11, &RPD3R, 5, &RPD4R, 9},
    {SPI_BASE + 0x200, &SDI2R, 12, &RPG7R, 6, &RPD5R, 6},
    {SPI_BASE + 0x400, NULL,    0, NULL,   0, NULL,   0}, // WiFi
    {SPI_BASE + 0x600, NULL,    0, NULL,   0, NULL,   0},
    {SPI_BASE + 0x800, NULL,    0, NULL,   0, NULL,   0},
    {SPI_BASE + 0xA00, NULL,    0, NULL,   0, NULL,   0},
};

// clang-format on
///////////////////////////////////////////////////////////////////////////////

void debug_begin(void)
{
    // printf for UART1 8n1 115200
    if ((0 == (U1MODE & _U1MODE_ON_MASK)) && (__XC_UART != 1))
    {
        pinMux(&RPD15R, 1);
        U1STA = U1MODE = 0;
        U1BRG = (((SYS_CLK_BUS_PERIPHERAL_1 >> 4) + (115200 >> 1)) / 115200) - 1;
        U1STA = _U1STA_UTXEN_MASK;
        U1MODE = _U1MODE_ON_MASK;
        __XC_UART = 1;
    }
}

void debug_end(void)
{
    if ((U1MODE & _U1MODE_ON_MASK) && (__XC_UART == 1))
    {
        __XC_UART = 0;
        U1STA = U1MODE = 0;
        pinMux(&RPD15R, 0);
    }
}

///////////////////////////////////////////////////////////////////////////////

void BoardInit(void)
{
    SystemUnlock();

    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;
    ANSELD = 0;
    ANSELE = 0;
    ANSELF = 0;

    _CP0_BIS_CAUSE(0x00800000U); // Use Multi Vectored Interrupts
    INTCONSET = _INTCON_MVEC_MASK;

    // Set up prefetch
    PRECONbits.PFMSECEN = 0;  // Flash SEC Interrupt Enable (Do not generate an interrupt when the PFMSEC bit is set)
    PRECONbits.PREFEN = 0b11; // Predictive Prefetch Enable (Enable predictive prefetch for any address)
    PRECONbits.PFMWS = 2;     // PFM Access Time Defined in Terms of SYSCLK Wait States (Two wait states)

    SYS_DEVCON_CacheCoherencySet(SYS_CACHE_WRITEBACK_WRITEALLOCATE);

    PB1DIVbits.PBDIV = 1; // Peripheral Bus 1 Clock Divisor Control (PBCLK1 is SYSCLK divided by 2)

    // CORE TIMER 1 ms
    int ticks = CORE_TIMER_INTERRUPT_TICKS;
    asm volatile("mtc0   $0,$9");
    asm volatile("mtc0   %0,$11"
                 : "+r"(ticks));
    _CP0_SET_COMPARE(ticks);
    mCTSetIntPriority(4);
    mCTSetIntSubPriority(0);
    mCTIntEnable(1);

    SystemLock();

    RNGCON = _RNGCON_TRNGMODE_MASK | _RNGCON_TRNGEN_MASK;
    while (RNGCNT < 64)
        continue;
    srand(RNGSEED2 ^ RNGSEED1);
    RNGCON = 0;

#ifdef LWIP
#if NO_SYS
    lwip_init();
#else
    tcpip_init(0, 0);
#endif
#endif
}

///////////////////////////////////////////////////////////////////////////////