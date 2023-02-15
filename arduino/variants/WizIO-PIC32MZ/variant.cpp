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

#include <Arduino.h>

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

#if 0
Uart Serial2(2);
extern "C" void __ISR(_UART3_RX_VECTOR, CONCAT(IPL, SERIAL2_PRI, AUTO)) UART3_RX_Handler(void)
{
    Serial2.isr_callback();
}

Uart Serial3(3);
extern "C" void __ISR(_UART4_RX_VECTOR, CONCAT(IPL, SERIAL3_PRI, AUTO)) UART4_RX_Handler(void)
{
    Serial3.isr_callback();
}

Uart Serial4(4);
extern "C" void __ISR(_UART5_RX_VECTOR, CONCAT(IPL, SERIAL4_PRI, AUTO)) UART5_RX_Handler(void)
{
    Serial4.isr_callback();
}

Uart Serial5(5);
extern "C" void __ISR(_UART6_RX_VECTOR, CONCAT(IPL, SERIAL5_PRI, AUTO)) UART6_RX_Handler(void)
{
    Serial5.isr_callback();
}
#endif

///////////////////////////////////////////////////////////////////////////////

#include <SoftwareSerial.h>

extern ssi_t ssi[];

static void SOFTWARE_SERIAL_RX_0() { ssi[0].serial->rxRead(); };
static void SOFTWARE_SERIAL_RX_1() { ssi[1].serial->rxRead(); };
static void SOFTWARE_SERIAL_RX_2() { ssi[2].serial->rxRead(); };
static void SOFTWARE_SERIAL_RX_3() { ssi[3].serial->rxRead(); };

ssi_t ssi[SOFTWARE_SERIAL_MAX] = {
    {NULL, SOFTWARE_SERIAL_RX_0},
    {NULL, SOFTWARE_SERIAL_RX_1},
    {NULL, SOFTWARE_SERIAL_RX_2},
    {NULL, SOFTWARE_SERIAL_RX_3},
};

///////////////////////////////////////////////////////////////////////////////