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

#ifndef MRF_INTERFACE_H_
#define MRF_INTERFACE_H_
#ifdef __cplusplus
extern "C"
{
#endif

/* USER CONFIG */
#include <user_config.h>

#ifndef WDRV_DEFAULT_CHANNEL
#define WDRV_DEFAULT_CHANNEL            5 
#endif

#ifndef WDRV_DEFAULT_NETWORK_TYPE
#define WDRV_DEFAULT_NETWORK_TYPE       WDRV_NETWORK_TYPE_INFRASTRUCTURE
#endif

#ifndef WDRV_DEFAULT_SECURITY_MODE
#define WDRV_DEFAULT_SECURITY_MODE      WDRV_SECURITY_OPEN
#endif

#ifndef WDRV_DEFAULT_SSID
#define WDRV_DEFAULT_SSID               "THE WIZARD"
#endif

#ifndef WDRV_DEFAULT_WEP_KEYS_40
#define WDRV_DEFAULT_WEP_KEYS_40        "5AFB6C8E77"                    // default WEP40 key
#endif

#ifndef WDRV_DEFAULT_WEP_KEYS_104
#define WDRV_DEFAULT_WEP_KEYS_104       "90E96780C739409DA50034FCAA"    // default WEP104 key
#endif

#ifndef WDRV_DEFAULT_PSK_PHRASE
#define WDRV_DEFAULT_PSK_PHRASE         "password"                      // default WPA-PSK or WPA2-PSK passphrase
#endif

#ifndef WDRV_DEFAULT_WPS_PIN
#define WDRV_DEFAULT_WPS_PIN            "12345678"                      // default WPS PIN
#endif

#ifdef __cplusplus
}
#endif
#endif /* MRF_INTERFACE_H_ */