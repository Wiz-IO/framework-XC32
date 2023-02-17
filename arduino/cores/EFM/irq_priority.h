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

#ifndef PRIORITY_H__
#define PRIORITY_H__
#ifdef __cplusplus
extern "C"
{
#endif

#include <hal_irq.h>

#ifndef DEFAULT_PRI
#define DEFAULT_PRI 4
#endif

#ifndef DEFAULT_SUB
#define DEFAULT_SUB 0
#endif

#if 1 /* CORE TIMER */
#define DEFAULT_CT_PRI DEFAULT_PRI
#define DEFAULT_CT_SUB DEFAULT_SUB

#ifndef CT_PRI
#define CT_PRI DEFAULT_CT_PRI
#endif
#ifndef CT_SUB
#define CT_SUB DEFAULT_CT_SUB
#endif
#endif

#if 1 /* INTx */

#define DEFAULT_INT_PRI DEFAULT_PRI
#define DEFAULT_INT_SUB DEFAULT_SUB

#ifndef INT0_PRI
#define INT0_PRI DEFAULT_INT_PRI
#endif

#ifndef INT1_PRI
#define INT1_PRI DEFAULT_INT_PRI
#endif

#ifndef INT2_PRI
#define INT2_PRI DEFAULT_INT_PRI
#endif

#ifndef INT3_PRI
#define INT3_PRI DEFAULT_INT_PRI
#endif

#ifndef INT4_PRI
#define INT4_PRI DEFAULT_INT_PRI
#endif

    //////

#ifndef INT0_SUB
#define INT0_SUB DEFAULT_INT_SUB
#endif

#ifndef INT1_SUB
#define INT1_SUB DEFAULT_INT_SUB
#endif

#ifndef INT2_SUB
#define INT2_SUB DEFAULT_INT_SUB
#endif

#ifndef INT3_SUB
#define INT3_SUB DEFAULT_INT_SUB
#endif

#ifndef INT4_SUB
#define INT4_SUB DEFAULT_INT_SUB
#endif

#endif /* INTx */

#if 1 /* CNx */

#define DEFAULT_CN_PRI DEFAULT_PRI
#define DEFAULT_CN_SUB DEFAULT_SUB

#ifndef CNA_PRI
#define CNA_PRI DEFAULT_CN_PRI
#endif

#ifndef CNB_PRI
#define CNB_PRI DEFAULT_CN_PRI
#endif

#ifndef CNC_PRI
#define CNC_PRI DEFAULT_CN_PRI
#endif

#ifndef CND_PRI
#define CND_PRI DEFAULT_CN_PRI
#endif

#ifndef CNE_PRI
#define CNE_PRI DEFAULT_CN_PRI
#endif

#ifndef CNF_PRI
#define CNF_PRI DEFAULT_CN_PRI
#endif

#ifndef CNG_PRI
#define CNG_PRI DEFAULT_CN_PRI
#endif

    /////

#ifndef CNA_SUB
#define CNA_SUB DEFAULT_CN_SUB
#endif

#ifndef CNB_SUB
#define CNB_SUB DEFAULT_CN_SUB
#endif

#ifndef CNC_SUB
#define CNC_SUB DEFAULT_CN_SUB
#endif

#ifndef CND_SUB
#define CND_SUB DEFAULT_CN_SUB
#endif

#ifndef CNE_SUB
#define CNE_SUB DEFAULT_CN_SUB
#endif

#ifndef CNF_SUB
#define CNF_SUB DEFAULT_CN_SUB
#endif

#ifndef CNG_SUB
#define CNG_SUB DEFAULT_CN_SUB
#endif

#endif /* CNx */

#if 1 /* SERIALx */

#define DEFAULT_SERIAL_PRI DEFAULT_PRI
#define DEFAULT_SERIAL_SUB DEFAULT_SUB

#ifndef SERIAL_PRI
#define SERIAL_PRI DEFAULT_SERIAL_PRI
#endif

#ifndef SERIAL1_PRI
#define SERIAL1_PRI DEFAULT_SERIAL_PRI
#endif

#ifndef SERIAL2_PRI
#define SERIAL2_PRI DEFAULT_SERIAL_PRI
#endif

#ifndef SERIAL3_PRI
#define SERIAL3_PRI DEFAULT_SERIAL_PRI
#endif

#ifndef SERIAL4_PRI
#define SERIAL4_PRI DEFAULT_SERIAL_PRI
#endif

#ifndef SERIAL5_PRI
#define SERIAL5_PRI DEFAULT_SERIAL_PRI
#endif

    //////

#ifndef SERIAL_SUB
#define SERIAL_SUB DEFAULT_SERIAL_SUB
#endif

#ifndef SERIAL1_SUB
#define SERIAL1_SUB DEFAULT_SERIAL_SUB
#endif

#ifndef SERIAL2_SUB
#define SERIAL2_SUB DEFAULT_SERIAL_SUB
#endif

#ifndef SERIAL3_SUB
#define SERIAL3_SUB DEFAULT_SERIAL_SUB
#endif

#ifndef SERIAL4_SUB
#define SERIAL4_SUB DEFAULT_SERIAL_SUB
#endif

#ifndef SERIAL5_SUB
#define SERIAL5_SUB DEFAULT_SERIAL_SUB
#endif

#endif /* SERIALx */

#ifdef __cplusplus
}
#endif
#endif