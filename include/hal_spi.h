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

#ifndef HAL_SPI_H_
#define HAL_SPI_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <xc.h>
#include <hal_dbg.h>

    typedef struct
    {
        uint32_t SRXISEL : 2;
        uint32_t STXISEL : 2;
        uint32_t DISSDI : 1;
        uint32_t MSTEN : 1;
        uint32_t CKP : 1;
        uint32_t SSEN : 1;
        uint32_t CKE : 1;
        uint32_t SMP : 1;
        uint32_t MODE16 : 1;
        uint32_t MODE32 : 1;
        uint32_t DISSDO : 1;
        uint32_t SIDL : 1;
        uint32_t : 1;
        uint32_t ON : 1;
        uint32_t ENHBUF : 1;
        uint32_t SPIFE : 1;
        uint32_t : 5;
        uint32_t MCLKSEL : 1;
        uint32_t FRMCNT : 3;
        uint32_t FRMSYPW : 1;
        uint32_t MSSEN : 1;
        uint32_t FRMPOL : 1;
        uint32_t FRMSYNC : 1;
        uint32_t FRMEN : 1;
    } spicon_t;

    typedef struct
    {
        uint32_t RBF : 1;
        uint32_t TBF : 1;
        uint32_t : 1;
        uint32_t TBE : 1;
        uint32_t : 1;
        uint32_t RBE : 1;
        uint32_t ROV : 1;
        uint32_t SRMT : 1;
        uint32_t TUR : 1;
        uint32_t : 2;
        uint32_t BUSY : 1;
        uint32_t FRMERR : 1;
        uint32_t : 3;
        uint32_t TXBUFELM : 5;
        uint32_t : 3;
        uint32_t RXBUFELM : 5;
    } spistat_t;

    typedef struct SPI_S
    {
        union
        {
            volatile uint32_t CON; /* 0xBF821000 */
            volatile spicon_t con;
        };
        union
        {
            volatile uint32_t CON_CLR; /* 0xBF821004 */
            volatile spicon_t con_clr;
        };
        union
        {
            volatile uint32_t CON_SET; /* 0xBF821008 */
            volatile spicon_t con_set;
        };
        union
        {
            volatile uint32_t CON_INV; /* 0xBF82100C */
            volatile spicon_t con_inv;
        };
        union
        {
            volatile uint32_t STA; /* 0xBF821010 */
            volatile spistat_t sta;
        };
        union
        {
            volatile uint32_t STA_CLR;
            volatile spistat_t sta_clr;
        };
        union
        {
            volatile uint32_t STA_SET;
            volatile spistat_t sta_set;
        };
        union
        {
            volatile uint32_t STA_INV;
            volatile spistat_t sta_inv;
        };
        volatile uint32_t BUF;      /* 0xBF821020 */
        volatile uint32_t __nc[3];  /* ********** */
        volatile uint32_t BRG;      /* 0xBF821030 */
        volatile uint32_t BRG_CLR;  /* 0xBF821034 */
        volatile uint32_t BRG_SET;  /* 0xBF821038 */
        volatile uint32_t BRG_INV;  /* 0xBF82103C */
        volatile uint32_t CON2;     /* 0xBF821040 */
        volatile uint32_t CON2_CLR; /* 0xBF821044 */
        volatile uint32_t CON2_SET; /* 0xBF821048 */
        volatile uint32_t CON2_INV; /* 0xBF82104C */
    } SPI_T;

#define SPI_HZ_BRG(F, H) (((F / H) >> 1) - 1)

    static inline __attribute__((always_inline)) void hal_spi_init(volatile SPI_T *p, uint32_t brg, uint32_t con)
    {
        if (p)
        {
            p->CON = 0;
            p->CON2 = 0;
            p->BRG = brg;
            if (-1 == con)        // DEFAULT CONFIG
                con = 0x00018265; // ENHBUF
            p->CON = con;
        }
    }

    static inline __attribute__((always_inline)) bool hal_spi_ready(volatile SPI_T *p)
    {
        if (p->CON & _SPI1CON_ENHBUF_MASK)
            return ((p->STA & _SPI1STAT_SPIRBE_MASK) == 0);
        else
            return ((p->STA & _SPI1STAT_SPIRBF_MASK) != 0);
    }

    static inline __attribute__((always_inline)) uint32_t hal_spi_xfer(volatile SPI_T *p, uint32_t value)
    {
        p->BUF = value;
        while (!hal_spi_ready(p))
            continue;
        return p->BUF;
    }

    static volatile SPI_T *HAL_SPI1 = (volatile SPI_T *)(&SPI1CON);
    static volatile SPI_T *HAL_SPI2 = (volatile SPI_T *)(&SPI2CON);
    static volatile SPI_T *HAL_SPI3 = (volatile SPI_T *)(&SPI3CON);
    static volatile SPI_T *HAL_SPI4 = (volatile SPI_T *)(&SPI4CON);
    static volatile SPI_T *HAL_SPI5 = (volatile SPI_T *)(&SPI5CON);
    static volatile SPI_T *HAL_SPI6 = (volatile SPI_T *)(&SPI6CON);

#define SPI_ID_SFR(N) (volatile SPI_T *)((int *)&SPI1CON + (N * 128))

    static inline __attribute__((always_inline)) void spi_init(uint32_t id, uint32_t brg, uint32_t con)
    {
        hal_spi_init(SPI_ID_SFR(id), brg, con);
    }

    static inline __attribute__((always_inline)) bool spi_ready(int32_t id)
    {
        return hal_spi_ready(SPI_ID_SFR(id));
    }

    static inline __attribute__((always_inline)) uint32_t spi_xfer(int32_t id, uint32_t value)
    {
        return hal_spi_xfer(SPI_ID_SFR(id), value);
    }

#ifdef __cplusplus
}
#endif
#endif /* HAL_SPI_H_ */