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

static void ADC_init(pin_t *p)
{
    if (*(p->gpio) & p->mask) // already init
    {
        return;
    }

    static int once = 0;
    if (0 == once)
    {
        ADCCON3 = ADCCON2 = ADCCON1 = ADCANCON = 0;
        ADC0CFG = DEVADC0;
        ADC1CFG = DEVADC1;
        ADC2CFG = DEVADC2;
        ADC3CFG = DEVADC3;
        ADC4CFG = DEVADC4;
        ADC7CFG = DEVADC7;
        once = -1;
    }

    uint32_t tempVal, channel = p->adc->adc.channel;
    ADCANCONbits.WKUPCLKCNT = 5; // Wakeup exponent = 32 * TADx
    ADCCON3bits.CONCLKDIV = 1;   // Control clock frequency is half of input clock
    if (channel == 7)
    {
        tempVal = ADCCON2 & ~(0x3FF077F);
        tempVal |= 1;       // channelClockDivider
        tempVal |= 5 << 16; // sampleTimeCount
        ADCCON2 = tempVal;
        ADCCON1bits.SELRES = 3; // resolution - 12 bits
    }
    else
    {
        volatile uint32_t *ADCxTIME = (volatile uint32_t *)(0xBF84B0D4 + (4 * channel));
        tempVal = *ADCxTIME & ~(0x1C37F03FF);
        tempVal |= 5;       // sampleTimeCount
        tempVal |= 1 << 16; // channelClockDivider
        tempVal |= 3 << 24; // resolution - 12 bits
        *ADCxTIME = tempVal;
    }

    // Turn the ADC on
    if (0 == ADCCON1bits.ON)
    {
        ADCCON1bits.ON = 1;
        while (!ADCCON2bits.BGVRRDY)
            continue; // Wait voltage is ready
        while (ADCCON2bits.REFFLT)
            continue; // Wait fault
    }

    // Enable ADC the clock to analog bias
    ADCANCON |= (1 << channel);

    // Wait for ADC to be ready
    while (!(ADCANCON & (1 << (channel + 8)))) // ADCANCONbits.WKRDYx
        continue;

    // Enable the ADC module
    ADCCON3 |= (1 << (channel + 16)); // ADCCON3bits.DIGENx = 1
    *(p->gpio + REG_SET) = p->mask;   // ANSEL analog pin
}

int ADC_Read(uint32_t pin)
{
    if (pin < PIN_MAX)
    {
        pin_t *p = &pins[pin];
        if (p->adc)
        {
            ADC_init(p);
            uint8_t v = p->adc->adc.an_pin;
            if (v > 44)
            {
                v -= 45;
            }
            uint32_t save = ADCTRGMODE; // save trigger mode
            ADCCON3bits.ADINSEL = v;    // Analog Input Select bits [0..5]
            ADCCON3bits.RQCNVRT = 1;    // trigger it
            if (v < 32)
            {
                while (!(ADCDSTAT1 & (1 << v)))
                    continue;
            }
            else
            {
                while (!(ADCDSTAT2 & (1 << (v - 32))))
                    continue;
            }
            ADCTRGMODE = save;
            return ((int)((uint32_t *)&ADCDATA0)[v]);
        }
    }
    return -1;
}

int analogRead(uint32_t pin) __attribute__((alias("ADC_Read")));

///////////////////////////////////////////////////////////////////////////////

static inline uint32_t PWM_prescaler(uint32_t frequency)
{
    if (frequency)
    {
        frequency = GetPeripheralClock() / frequency;
        for (int i = 0; i < 8; i++)
        {
            if (frequency / (1u << i) < 0x10000u)
            {
                return i;
            }
        }
    }
    return 7;
}

static int PWM_timer_init(periphery_pwm_t *pwm, uint32_t frequency)
{
    if (pwm && frequency)
    {
        *pwm->TxCON = 0;
        uint32_t prescaler = PWM_prescaler(frequency);
        *(TREG)(pwm->TxCON + 8) = GetPeripheralClock() / frequency / (1 << prescaler);
        *pwm->TxCON = _T2CON_ON_MASK | (prescaler << _T2CON_TCKPS0_POSITION);
        return *(TREG)(pwm->TxCON + 8);
    }
    return -1;
}

static periphery_pwm_t *PWM_open(periphery_pwm_t *pwm, uint32_t frequency, float duty)
{
    if (0 == (*pwm->OCxCON & _OC1CON_ON_MASK)) // already open
    {
        // TIMER INIT
        if (CFGCONbits.OCACLK)
        {
            pwm->TxCON = pwm->TxCON_ALT; // alt timer
        }
        else
        {
            pwm->TxCON = &T2CON; // always is T2
        }
        if (PWM_timer_init(pwm, frequency) < 0)
        {
            return NULL;
        }
        // OCx INIT
        *pwm->OCxCON = 0;
        if (duty > 100)
        {
            duty = 100;
        }
        if (duty < 0)
        {
            duty = 0;
        }
        *(TREG)(pwm->OCxCON + 8) = *(TREG)(pwm->TxCON + 8) * (duty / 100.0); // OCxRS
        *pwm->OCxCON = _OC1CON_ON_MASK | 6;
        // RPIN INIT
        pinMux(pwm->RPin, pwm->RVal);
    }
    return pwm;
}

int PWM_Begin(uint32_t pin, uint32_t frequency, float duty)
{
    if (pin < PIN_MAX)
    {
        pin_t *p = &pins[pin];
        if (p->pwm)
        {
            periphery_pwm_t *pwm = PWM_open((periphery_pwm_t *)p->pwm, frequency, duty);
            if (pwm)
            {
                return *(pwm->TxCON + 8); // max value
            }
        }
    }
    return -1;
}

int PWM_writeDuty(uint32_t pin, float duty)
{
    if (pin < PIN_MAX)
    {
        pin_t *p = &pins[pin];
        if (p->pwm)
        {
            periphery_pwm_t *pwm = PWM_open((periphery_pwm_t *)p->pwm, 1000, duty);
            if (pwm)
            {
                if (duty > 100)
                {
                    duty = 100;
                }
                if (duty < 0)
                {
                    duty = 0;
                }
                *(pwm->OCxCON + 8) = *(pwm->TxCON + 8) * (duty / 100.0);
            }
            return 0; // ok
        }
    }
    return -1;
}

void PWM_writeValule(uint32_t pin, int val)
{

    if (pin < PIN_MAX)
    {
        pin_t *p = &pins[pin];
        if (p->pwm)
        {
            val &= 0xFFFF;
            periphery_pwm_t *pwm = PWM_open((periphery_pwm_t *)p->pwm, 1000, val); // duty
            if (pwm)
            {
                if (val > *(pwm->TxCON + 8))
                {
                    val = *(pwm->TxCON + 8);
                }
                *(pwm->OCxCON + 8) = val;
            }
        }
    }
}

void analogWrite(uint32_t pin, int val) __attribute__((alias("PWM_writeValule")));

///////////////////////////////////////////////////////////////////////////////
