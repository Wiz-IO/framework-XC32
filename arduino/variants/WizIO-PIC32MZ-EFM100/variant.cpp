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

#include <variant.h>

Uart Serial(0);
extern "C" void __ISR(_UART1_RX_VECTOR, CONCAT(IPL, SERIAL_PRI, AUTO)) UART1_RX_Handler(void)
{
    Serial.isr_callback();
}

Uart Serial1(1);
extern "C" void __ISR(_UART2_RX_VECTOR, CONCAT(IPL, SERIAL1_PRI, AUTO)) UART2_RX_Handler(void)
{
    Serial1.isr_callback();
}

///////////////////////////////////////////////////////////////////////////////

extern ss_context_t ss_context[];

static void SOFTWARE_SERIAL_RX_0() { ss_context[0].serial->rxRead(); };
static void SOFTWARE_SERIAL_RX_1() { ss_context[1].serial->rxRead(); };

ss_context_t ss_context[SOFTWARE_SERIAL_MAX] = {
    {NULL, SOFTWARE_SERIAL_RX_0},
    {NULL, SOFTWARE_SERIAL_RX_1},
};

///////////////////////////////////////////////////////////////////////////////