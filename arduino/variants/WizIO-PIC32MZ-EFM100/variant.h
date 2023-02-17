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

#pragma once

#include <interface.h>

#ifdef __cplusplus
extern "C"
{
#endif
    /* C ware */

    // clang-format off

#define LED_GREEN       0  
#define LED_RED         1
#define LED_ORANGE      2
#define LED             LED_GREEN

#define LED_R           3
#define LED_G           4
#define LED_B           5

#define BUTTON          6

#define A0              7
#define A1              19

#define MRF24WN         1

#define PIN_MAX         (31)
    extern pin_t pins[PIN_MAX];

    // clang-format on

    ///////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
/* CPP ware */

#define SERIAL_MAX 2
#include <SerialUart.h>

#define SOFTWARE_SERIAL_MAX 2
#include <SoftwareSerial.h>

#endif //__cplusplus
