#include <xc.h>

#define TRAP_DEBUG

#ifdef TRAP_DEBUG
#include <stdio.h>
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
    "Reserved"};
#endif

void _general_exception_handler(void)
{
#ifdef TRAP_DEBUG
    _excep_code = (_CP0_GET_CAUSE() & 0x0000007C) >> 2;
    _excep_addr = _CP0_GET_EPC();
    _cause_str = cause[_excep_code];
    printf("[TRAP] General Exception '%s' (cause = %d, addr = 0x%X)\n", _cause_str, _excep_code, _excep_addr);
#endif
    while (1)
        continue;
}