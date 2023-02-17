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

#include <user_config.h>

#define TRAPS

#ifdef TRAPS

#include <xc.h>
#include <stdio.h>

#ifdef TRAPS_PRINT
static unsigned int _excep_code;
static unsigned int _excep_addr;
static char *_cause_str;
static char *cause[] = {
    "Interrupt",
    "Undefined",
    "Undefined",
    "Undefined",
    "Load/Fetch address error",
    "Store address error",
    "Instruction bus error",
    "Data bus error",
    "Syscall",
    "Breakpoint",
    "Reserved instruction",
    "Coprocessor unusable",
    "Arithmetic overflow",
    "Trap",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
};
#endif

extern void exception_handler(void) __attribute__((weak));

void _general_exception_handler(void)
{
#ifdef TRAPS_PRINT      
    _excep_code = (_CP0_GET_CAUSE() & 0x0000007C) >> 2;
    _excep_addr = _CP0_GET_EPC();
    _cause_str = cause[_excep_code];
    printf("[TRAP] General Exception '%s' (cause = %d, addr = 0x%X)\n", _cause_str, _excep_code, _excep_addr);
#endif    
    exception_handler();
    while (1)
        continue;
}
#endif