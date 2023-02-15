#ifndef __CC_H__
#define __CC_H__

#include <stdint.h> 
#include <stdio.h> 
#include <string.h>
#include <stdlib.h> 
#include <sys/time.h>

#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT              __attribute__ ((__packed__))
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x)            x

#define LWIP_RAND                       rand 
#define LWIP_PROVIDE_ERRNO
#define LWIP_NETIF_STATUS_CALLBACK      1

#define BYTE_ORDER                      LITTLE_ENDIAN
#define LWIP_PLATFORM_BYTESWAP          0   
#define LWIP_DONT_PROVIDE_BYTEORDER_FUNCTIONS // htonl -> lega-c

typedef uint8_t                         u8_t;
typedef int8_t                          s8_t;
typedef uint16_t                        u16_t;
typedef int16_t                         s16_t;
typedef uint32_t                        u32_t;
typedef int32_t                         s32_t;
typedef uintptr_t                       mem_ptr_t;
typedef u32_t                           sys_prot_t; 

#define SSIZE_MAX                       INT_MAX

#define SZT_F                           "lu"

#define SYS_ARCH_DECL_PROTECT(lev)
#define SYS_ARCH_PROTECT(lev)			portENTER_CRITICAL()
#define SYS_ARCH_UNPROTECT(lev)			portEXIT_CRITICAL()

#define LWIP_LOGE(fmt, ...)             printf(fmt, ##__VA_ARGS__)
#define LWIP_LOGW(fmt, ...)             printf(fmt, ##__VA_ARGS__)
#define LWIP_LOGI(fmt, ...)             printf(fmt, ##__VA_ARGS__)

#define LWIP_PLATFORM_DIAG(x) \
    do                        \
    {                         \
        LWIP_LOGI x;          \
    } while (0)

#define LWIP_PLATFORM_ASSERT(x)                                                    \
    do                                                                             \
    {                                                                              \
        printf("[LWIP] ASSERT < %s > LINE: %d FILE: %s\n", x, __LINE__, __FILE__); \
        while (1)                                                                  \
            ;                                                                      \
    } while (0)

#define LWIP_ERROR(message, expression, handler)                                        \
    do                                                                                  \
    {                                                                                   \
        if (!(expression))                                                              \
        {                                                                               \
            printf("[ASSERT] < %s > LINE: %d FILE: %s\n", message, __LINE__, __FILE__); \
            handler;                                                                    \
        }                                                                               \
    } while (0)

#endif /* __CC_H__ */
