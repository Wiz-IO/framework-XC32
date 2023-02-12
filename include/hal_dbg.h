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

#ifndef HAL_DBG_H_
#define HAL_DBG_H_
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

#undef PRINT
#undef PRINT_FUNC
#undef ERROR_FUNC

#ifdef DBG
#define PRINT printf
#define PRINT_FUNC PRINT("[DBG] %s()\n", __FUNCTION__)
#define ERROR_FUNC PRINT("[ERROR] %s()\n", __FUNCTION__)
#else
#define PRINT
#define PRINT_FUNC
#define ERROR_FUNC
#endif

    static inline void dump(const char *txt, uint8_t *buf, int len)
    {
        printf("[DMP] %s: ", txt);
        while (len--)
            printf("%02X ", (int)*buf++);
        printf("\n");
    }

#ifdef __cplusplus
}
#endif
#endif /* HAL_SPI_H_ */