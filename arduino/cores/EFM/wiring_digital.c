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

void pinMode(uint32_t pin, uint32_t mode)
{
    if (pin < PIN_MAX)
    {
        pin_t *p = &pins[pin];

        if (mode & (PULLUP & PULLDOWN))
        {
            return;
        }

        *(p->gpio + ANSEL_CLR) = p->mask;
        *(p->gpio + CNPU_CLR) = p->mask;
        *(p->gpio + CNPD_CLR) = p->mask;
        if (mode == OUTPUT)
        {
            *(p->gpio + TRIS_CLR) = p->mask;
        }
        else if (mode == OUTPUT_HIGH)
        {
            *(p->gpio + LAT_SET) = p->mask;
            *(p->gpio + TRIS_CLR) = p->mask;
        }
        else if (mode == OUTPUT_LOW)
        {
            *(p->gpio + LAT_CLR) = p->mask;
            *(p->gpio + TRIS_CLR) = p->mask;
        }
        else if (mode & INPUT_PULLUP)
        {
            *(p->gpio + CNPU_SET) = p->mask;
        }
        else if (mode & INPUT_PULLDOWN)
        {
            *(p->gpio + CNPD_SET) = p->mask;
        }
        else
        {
            *(p->gpio + TRIS_SET) = p->mask;
        }
    }
}

int digitalRead(uint32_t pin)
{
    if (pin < PIN_MAX)
    {
        pin_t *p = &pins[pin];
        return (bool)(*(p->gpio + PORT) & p->mask);
    }
    return -1;
}

void digitalWrite(uint32_t pin, uint32_t val)
{
    if (pin < PIN_MAX)
    {
        pin_t *p = &pins[pin];
        *(p->gpio + LAT_CLR + (bool)val) = p->mask;
    }
}

void digitalToggle(uint32_t pin)
{
    if (pin < PIN_MAX)
    {
        pin_t *p = &pins[pin];
        *(p->gpio + LAT_INV) = p->mask;
    }
}

///////////////////////////////////////////////////////////////////////////////

static uint8_t cn_counter[GPIO_MAX] = {0};

static struct
{
    uint8_t pri;
    uint8_t sub;
} cn_priority[GPIO_MAX] = {
    {CNA_PRI, CNA_SUB},
    {CNB_PRI, CNB_SUB},
    {CNC_PRI, CNC_SUB},
    {CND_PRI, CND_SUB},
    {CNE_PRI, CNE_SUB},
    {CNF_PRI, CNF_SUB},
    {CNG_PRI, CNG_SUB},
};

void attachInterrupt(uint32_t pin, void (*func)(void), int mode)
{
    if (pin < PIN_MAX && func)
    {
        IRQ_E irq;
        int id;
        pin_t *p = &pins[pin];

        detachInterrupt(pin);

        pinMode(pin, mode);

        if (mode & EXTERNAL) // INTx
        {
            if (p->eint)
            {
                if (p->eint->eint.RPin)
                    pinMux(p->eint->eint.RPin, p->eint->eint.RVal);
                id = (p->eint->eint.irq - IRQ_EXTERNAL_0) / 5; // INTx index
                irq = p->eint->eint.irq;
                IRQ_Priority(irq, p->eint->eint.pri, p->eint->eint.sub);
                p->eint->eint.mode = mode;
                INTCON |= (bool)(mode & RISING) << id;
            }
            else
            {
                return;
            }
        }
        else // CNx
        {
            id = (p->gpio - &ANSELA) / GPIO_W_SIZE; // GPIOx index
            irq = IRQ_CHANGE_NOTICE_A + id;
            if (mode & CHANGE)
            {
                *(p->gpio + CNCON_CLR) = _CNCONA_EDGEDETECT_MASK;
            }
            else
            {
                *(p->gpio + CNCON_SET) = _CNCONA_EDGEDETECT_MASK;
                if (mode & FALLING)
                {
                    *(p->gpio + CNNE_SET) = p->mask;
                }
                else
                {
                    *(p->gpio + CNNE_CLR) = p->mask;
                }
            }
            *(p->gpio + CNCON_SET) = _CNCONA_ON_MASK; // enable CN
            *(p->gpio + CNF_CLR) = p->mask;           // clear  CN IRQ flag
            *(p->gpio + CNEN_SET) = p->mask;          // enable CN IRQ
            cn_counter[id] += 1;
            IRQ_Priority(irq, cn_priority[id].pri, cn_priority[id].sub);
        }

        p->callback = func;
        IRQ_Clear(irq);
        IRQ_Enable(irq);
    }
}

void detachInterrupt(uint32_t pin)
{
    if (pin < PIN_MAX)
    {
        pin_t *p = &pins[pin];

        // close CNx
        int id = (p->gpio - &ANSELA) / GPIO_W_SIZE;
        IRQ_E irq = IRQ_CHANGE_NOTICE_A + id;
        *(p->gpio + CNEN_CLR) = p->mask;

        if (--cn_counter[id] == 0)
        {
            IRQ_Disable(irq);
        }

        // close INTx
        if (p->eint)
        {    
            IRQ_Disable(p->eint->eint.irq);
        }

        p->callback = NULL;
    }
}

static void c_handler(int irq)
{
    TREG gpio = (TREG)&ANSELA + (irq - IRQ_CHANGE_NOTICE_A) * GPIO_W_SIZE;
    TREG s = (TREG)gpio + CNSTAT; // status change
    TREG f = (TREG)gpio + CNF;    // status fall / rise
    int event = *s | *f;
    pin_t *p = &pins[0];
    for (int i = 0; i < PIN_MAX; i++, p++)
    {
        if (p->callback && (p->gpio == gpio) && (p->mask & event))
        {
            p->callback();
            *(f + REG_CLR) = p->mask;
            *(s + REG_CLR) = p->mask;
        }
    }
    IRQ_Clear(irq);
}

static void i_handler(int irq)
{
    pin_t *p = &pins[0];
    for (int i = 0; i < PIN_MAX; i++, p++)
    {
        if (p->callback && p->eint && p->eint->eint.irq == irq)
        {
            p->callback();
            if (p->eint->eint.mode & CHANGE)
            {    
                INTCONINV = 1 << ((p->eint->eint.irq - IRQ_EXTERNAL_0) / 5);
            }
            break;
        }
    }
    IRQ_Clear(irq);
}

WEAK void __ISR(_EXTERNAL_0_VECTOR,      CONCAT(IPL, INT0_PRI, AUTO)) ISR_INT0(void) { i_handler(IRQ_EXTERNAL_0); }
WEAK void __ISR(_EXTERNAL_1_VECTOR,      CONCAT(IPL, INT1_PRI, AUTO)) ISR_INT1(void) { i_handler(IRQ_EXTERNAL_1); }
WEAK void __ISR(_EXTERNAL_2_VECTOR,      CONCAT(IPL, INT2_PRI, AUTO)) ISR_INT2(void) { i_handler(IRQ_EXTERNAL_2); }
WEAK void __ISR(_EXTERNAL_4_VECTOR,      CONCAT(IPL, INT4_PRI, AUTO)) ISR_INT4(void) { i_handler(IRQ_EXTERNAL_4); }

#if MRF24WN != 1
WEAK void __ISR(_EXTERNAL_3_VECTOR,      CONCAT(IPL, INT3_PRI, AUTO)) ISR_INT3(void) { i_handler(IRQ_EXTERNAL_3); }
#endif

WEAK void __ISR(_CHANGE_NOTICE_A_VECTOR, CONCAT(IPL, CNA_PRI, AUTO))  ISR_CNA(void)  { c_handler(IRQ_CHANGE_NOTICE_A); }
WEAK void __ISR(_CHANGE_NOTICE_B_VECTOR, CONCAT(IPL, CNB_PRI, AUTO))  ISR_CNB(void)  { c_handler(IRQ_CHANGE_NOTICE_B); }
WEAK void __ISR(_CHANGE_NOTICE_C_VECTOR, CONCAT(IPL, CNC_PRI, AUTO))  ISR_CNC(void)  { c_handler(IRQ_CHANGE_NOTICE_C); }
WEAK void __ISR(_CHANGE_NOTICE_D_VECTOR, CONCAT(IPL, CND_PRI, AUTO))  ISR_CND(void)  { c_handler(IRQ_CHANGE_NOTICE_D); }
WEAK void __ISR(_CHANGE_NOTICE_E_VECTOR, CONCAT(IPL, CNE_PRI, AUTO))  ISR_CNE(void)  { c_handler(IRQ_CHANGE_NOTICE_E); }
WEAK void __ISR(_CHANGE_NOTICE_F_VECTOR, CONCAT(IPL, CNF_PRI, AUTO))  ISR_CNF(void)  { c_handler(IRQ_CHANGE_NOTICE_F); }
WEAK void __ISR(_CHANGE_NOTICE_G_VECTOR, CONCAT(IPL, CNG_PRI, AUTO))  ISR_CNG(void)  { c_handler(IRQ_CHANGE_NOTICE_G); }
