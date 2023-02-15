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

#ifndef LWIP
#error "Missing LWIP"
#endif

///////////////////////////////////////////////////////////////////////////////

#include <interface.h>
#include "wdrv_mrf24wn_api.h"
#include "wdrv_mrf24wn.h"
#include <netif/ethernet.h>
#include <lwip/init.h>
#include <lwip/tcpip.h>
#include <lwip/err.h>
#include <lwip/netif.h>
#include <lwip/etharp.h>
#include <lwip/inet.h>
#include <lwip/dhcp.h>

///////////////////////////////////////////////////////////////////////////////

void WDRV_HARD_Init(void);

struct netif WLAN;
ip4_addr_t wifi_ip = {.addr = 0};
ip4_addr_t wifi_mk = {.addr = 0};
ip4_addr_t wifi_gw = {.addr = 0};

// LIB
OSAL_MUTEX_HANDLE_TYPE g_debugConsoleLock;

static WDRV_CONFIG s_wdrv_cfg;

static bool WDRV_Config_Load(void);
static void WDRV_Connect(void);

static void wifi_mac_init(uint8_t const *const addr);
static void wifi_receive_CB(uint32_t len, uint8_t const *const frame);
static void wifi_set_state(bool connected);

void SYS_CMD_PRINT(const char *f, ...);
#define SYS_CONSOLE_PRINT(fmt, ...) SYS_CMD_PRINT(fmt, ##__VA_ARGS__)

#ifndef MRF_MAX_MULTICAST_FILTER_SIZE
#define MRF_MAX_MULTICAST_FILTER_SIZE 4 /* ? */
#endif

///////////////////////////////////////////////////////////////////////////////

/* WDRV_CALLBACKS */

static void ScanDone_CB(uint32_t status) /* Not Implemented */
{
}

static void WPSDone_CB(void) /* Not Implemented */
{
}

static void RFReady_CB(uint8_t const *const addr)
{
  PRINT("[MRF] MAC: %02X:%02X:%02X:%02X:%02X:%02X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
  wifi_mac_init(addr);
}

static void InitDone_CB(void) { WDRV_Connect(); }

static void Connect_CB(uint32_t connected, uint8_t devID, uint8_t *mac, bool macConn, uint8_t reason)
{
  // ClientCacheUpdate(connected, mac);
  wifi_set_state(connected);
}

/* COMMON */

// LIB
void wc_ErrorString(int error, char *buffer) { sprintf(buffer, "[MRF] ERROR( %d ) %s", error, buffer); }

// LIB DEBUG
void SYS_CMD_PRINT(const char *f, ...)
{
#if WDRV_DBG_LEVEL != WDRV_DBG_NONE
  va_list a;
  va_start(a, f);
  vprintf(f, a);
  va_end(a);
#endif
}

static bool LoadConfigFromMemory(void) /* Not Implemented */
{
  return false;
}

static void NetModeSet(uint8_t networkType)
{
  switch (networkType)
  {
  case WDRV_NETWORK_TYPE_INFRASTRUCTURE:
    WDRV_EXT_CmdNetModeBSSSet();
    break;
  case WDRV_NETWORK_TYPE_SOFT_AP:
    WDRV_EXT_CmdNetModeAPSet();
    break;
  default:
    WDRV_ASSERT(false, "Undefined / NotSupported  network type");
    break;
  }
}

static void SecuritySet(uint8_t securityMode)
{
  bool pinMode;
  switch (securityMode)
  {
  case WDRV_SECURITY_OPEN:
    WDRV_EXT_CmdSecNoneSet();
    break;
  case WDRV_SECURITY_WEP_40:
  case WDRV_SECURITY_WEP_104:
    if ((s_wdrv_cfg.securityKeyLen == 5) || (s_wdrv_cfg.securityKeyLen == 13))
    {
      // coverting security key to ASCII code
      uint8_t i, originalKeyChar;
      uint8_t asciiKey[26] = {0};
      for (i = 0; i < s_wdrv_cfg.securityKeyLen * 2; ++i)
      {
        originalKeyChar = (i % 2 == 0) ? (s_wdrv_cfg.securityKey[i / 2] >> 4) : (s_wdrv_cfg.securityKey[i / 2] & 0x0F);
        asciiKey[i] = (originalKeyChar > 9) ? ('A' + originalKeyChar - 0x0A) : ('0' + originalKeyChar - 0x00);
      }
      s_wdrv_cfg.securityKeyLen *= 2;
      memcpy(s_wdrv_cfg.securityKey, asciiKey, s_wdrv_cfg.securityKeyLen);
      s_wdrv_cfg.securityKey[s_wdrv_cfg.securityKeyLen] = 0x00;
    }
    WDRV_EXT_CmdSecWEPSet(s_wdrv_cfg.securityKey, s_wdrv_cfg.securityKeyLen);
    break;
  case WDRV_SECURITY_WPA_WITH_PASS_PHRASE:
    WDRV_EXT_CmdSecWPASet(s_wdrv_cfg.securityKey, s_wdrv_cfg.securityKeyLen);
    break;
  case WDRV_SECURITY_WPA2_WITH_PASS_PHRASE:
    WDRV_EXT_CmdSecWPA2Set(s_wdrv_cfg.securityKey, s_wdrv_cfg.securityKeyLen);
    break;
  case WDRV_SECURITY_WPS_PIN:
  case WDRV_SECURITY_WPS_PUSH_BUTTON:
    pinMode = s_wdrv_cfg.securityMode == WDRV_SECURITY_WPS_PIN ? true : false;
    WDRV_EXT_CmdSecWpsSet(pinMode, s_wdrv_cfg.securityKey, s_wdrv_cfg.securityKeyLen);
    break;
  default:
    WDRV_ASSERT(false, "Undefined security mode");
    break;
  }
}

static bool WDRV_Config_Load(void)
{
  bool res = LoadConfigFromMemory();

  // if driver is open, simply return the result of the load operation
  // if driver is not open yet, there are two options:
  //    1. if NVM or s_wdrv_cfgBackup holds valid Wi-Fi configuration, use it
  //    2. otherwise use hard-coded Wi-Fi configuration in system_config.h
  if (!res)
  {
    if (sizeof(WDRV_DEFAULT_SSID) - 1 > WDRV_MAX_SSID_LENGTH)
    {
      WDRV_DBG_ERROR_PRINT("WDRV_DEFAULT_SSID could not contain more than %u characters\n", WDRV_MAX_SSID_LENGTH);
      return false;
    }

    WDRV_DBG_INFORM_PRINT("Using default Wi-Fi configuration\n");

    memcpy(s_wdrv_cfg.ssid, (const void *)WDRV_DEFAULT_SSID, sizeof(WDRV_DEFAULT_SSID));
    s_wdrv_cfg.ssidLen = sizeof(WDRV_DEFAULT_SSID) - 1;
    s_wdrv_cfg.networkType = WDRV_DEFAULT_NETWORK_TYPE;
    s_wdrv_cfg.securityMode = WDRV_DEFAULT_SECURITY_MODE;

    switch (s_wdrv_cfg.securityMode)
    {
    case WDRV_SECURITY_OPEN:
      memset(s_wdrv_cfg.securityKey, 0x00, sizeof(s_wdrv_cfg.securityKey));
      s_wdrv_cfg.securityKeyLen = 0;
      break;
    case WDRV_SECURITY_WEP_40:
      memcpy(s_wdrv_cfg.securityKey, (const void *)WDRV_DEFAULT_WEP_KEYS_40, sizeof(WDRV_DEFAULT_WEP_KEYS_40) - 1);
      s_wdrv_cfg.securityKeyLen = sizeof(WDRV_DEFAULT_WEP_KEYS_40) - 1;
      break;
    case WDRV_SECURITY_WEP_104:
      memcpy(s_wdrv_cfg.securityKey, (const void *)WDRV_DEFAULT_WEP_KEYS_104, sizeof(WDRV_DEFAULT_WEP_KEYS_104) - 1);
      s_wdrv_cfg.securityKeyLen = sizeof(WDRV_DEFAULT_WEP_KEYS_104) - 1;
      break;
    case WDRV_SECURITY_WPA_WITH_PASS_PHRASE:
    case WDRV_SECURITY_WPA2_WITH_PASS_PHRASE:
      memcpy(s_wdrv_cfg.securityKey, (const void *)WDRV_DEFAULT_PSK_PHRASE, sizeof(WDRV_DEFAULT_PSK_PHRASE) - 1);
      s_wdrv_cfg.securityKeyLen = sizeof(WDRV_DEFAULT_PSK_PHRASE) - 1;
      break;
    case WDRV_SECURITY_WPS_PUSH_BUTTON:
      memset(s_wdrv_cfg.securityKey, 0x00, sizeof(s_wdrv_cfg.securityKey));
      s_wdrv_cfg.securityKeyLen = 0;
      break;
    case WDRV_SECURITY_WPS_PIN:
      memcpy(s_wdrv_cfg.securityKey, (const void *)WDRV_DEFAULT_WPS_PIN, sizeof(WDRV_DEFAULT_WPS_PIN));
      s_wdrv_cfg.securityKeyLen = sizeof(WDRV_DEFAULT_WPS_PIN);
      break;
    default:
      WDRV_ASSERT(false, "Unsupported security mode");
      break;
    }
  }

  return true;
}

static void WDRV_Connect(void)
{
  NetModeSet(s_wdrv_cfg.networkType);

  if (s_wdrv_cfg.networkType == WDRV_NETWORK_TYPE_SOFT_AP)
    WDRV_EXT_CmdChannelSet(WDRV_DEFAULT_CHANNEL);

  WDRV_EXT_CmdSSIDSet(s_wdrv_cfg.ssid, s_wdrv_cfg.ssidLen);

  SecuritySet(s_wdrv_cfg.securityMode);

  WDRV_EXT_CmdPowerSavePut(false);

  if ((s_wdrv_cfg.securityMode != WDRV_SECURITY_WPS_PIN) && (s_wdrv_cfg.securityMode != WDRV_SECURITY_WPS_PUSH_BUTTON))
  {
    WDRV_DBG_INFORM_PRINT("Start Wi-Fi Connection . . .\n");
    WDRV_EXT_CmdConnect();
  }
}

static void WDRV_Disconnect(void) { WDRV_EXT_CmdDisconnect(); }

static int WDRV_Init(void)
{
  bool once = 0;
  if (once)
    return 0; // ok
  once = 1;

  int res = WDRV_MUTEX_CREATE(&g_debugConsoleLock);

  WDRV_HARD_Init();

  uint32_t s_wdrvext_priv_default[] = {
      0u,                   // MODULE_EVENT_PRINT,
      TASK_PRIORITY_LOW,    // WDRV_EXT_RTOS_INIT_TASK_PRIORITY = 1,
      TASK_PRIORITY_NORMAL, // WDRV_EXT_RTOS_MAIN_TASK_PRIORITY = 2,
      512u,                 // WDRV_EXT_RTOS_INIT_TASK_SIZE = 512,
      2048u,                // WDRV_EXT_RTOS_MAIN_TASK_SIZE = 2048,
      5u,                   // WDRV_BOARD_TYPE 5=DRV_BD_TYPE_CUSTOM
  };
  WDRV_EXT_PrivConfig(s_wdrvext_priv_default);

  WDRV_Config_Load();

  WDRV_CALLBACKS MRF_CB = {
      .CopyFrameToStackPacketBuffer_CB = wifi_receive_CB,
      .ProceedConnectEvent_CB = Connect_CB,
      .RFReady_CB = RFReady_CB,
      .ScanDone_CB = ScanDone_CB,
      .InitDone_CB = InitDone_CB,
      .WPSDone_CB = WPSDone_CB,
  };
  return WDRV_EXT_Initialize(&MRF_CB);
}

// LIB
void WDRV_Assert(int condition, const char *msg, const char *file, int line)
{
  if (!condition)
  {
    if (*msg)
      WDRV_DBG_ERROR_PRINT("\n[ASSERT] %s\n%s, line %u\n", msg, file, line);
    else
      WDRV_DBG_ERROR_PRINT("\n[ASSERT] \n%s, line %u\n", file, line);
    while (1)
      continue;
  }
  return;
}

typedef struct __attribute__((__packed__))
{
  uint8_t v[6];
} TCPIP_MAC_ADDR;

static TCPIP_MAC_ADDR s_MulticastFilter[MRF_MAX_MULTICAST_FILTER_SIZE];

static bool isBroadcastPacket(const uint8_t *addr)
{
  int i;
  for (i = 0; i < 6; i++)
  {
    if (addr[i] != 0xff)
      return false;
  }
  return true;
}

static bool isMulticastPacket(uint8_t const *const frame)
{
  if (((frame[0] & 0x01) == 0x01) && (!isBroadcastPacket(&frame[0])))
    return true;
  else
    return false;
}

static bool isMulticastAddrSet(const TCPIP_MAC_ADDR *addr)
{
  int i;
  for (i = 0; i < MRF_MAX_MULTICAST_FILTER_SIZE; i++)
  {
    if (memcmp(addr, &s_MulticastFilter[i], 6) == 0)
      return true;
  }
  return false;
}

static bool isSolicitedMulticastPacket(uint8_t const *const frame)
{
  return isMulticastAddrSet((TCPIP_MAC_ADDR *)frame);
}

static bool isPacketValid(uint8_t const *const frame)
{
  if (isMulticastPacket(frame))
  {
    if (isSolicitedMulticastPacket(frame))
      return true;
    else
      return false;
  }
  else
  {
    return true;
  }
  return false;
}

/* LWIP NETIF */

// #define MRF_MAX_IP_PACKET_SIZE 1564 // including header

#define MRF_MAX_PACKET_SIZE 1518

static TaskHandle_t waiting_task = 0;

static err_t wifi_send(struct netif *interface, struct pbuf *pb)
{
  err_t res = ERR_ARG;
  if (pb)
  {
    struct netif *netif;
    for (netif = netif_list; (netif != NULL) && (netif->state != (void *)interface); netif = netif->next)
      ;
    if (netif == NULL)
    {
      pbuf_free(pb);
      pb = NULL;
      PRINT("[ERROR] wifi_send( NULL ) \n");
      return ERR_IF;
    }
    if (WDRV_EXT_DataSend(pb->len, pb->payload))
    {
      PRINT("[ERROR] wifi_send( RF )\n");
      LWIP_ASSERT("wifi_send()", 1);
      return ERR_IF;
    }
    res = ERR_OK;
  }
  else
  {
    PRINT("[ERROR] wifi_send( PB )\n");
  }
  return res;
}

static void wifi_receive(struct netif *interface, struct pbuf *pb)
{
  if (pb)
  {
    struct netif *netif;
    for (netif = netif_list; (netif != NULL) && (netif->state != (void *)interface); netif = netif->next)
      ;
    if (netif == NULL)
    {
      pbuf_free(pb);
      pb = NULL;
      LINK_STATS_INC(link.drop);
      PRINT("[ERROR] wifi_receive( DROP )\n");
      return;
    }
    LINK_STATS_INC(link.recv);
    int err = netif->input(pb, netif);
    if (err)
    {
      pbuf_free(pb);
      pb = NULL;
      PRINT("[ERROR] wifi_receive( %d )\n", (int)err);
    }
  }
  else
  {
    PRINT("[ERROR] wifi_receive( PB )\n");
  }
}

static void wifi_dhcp_ip_ready_callback(struct netif *netif)
{
  if (netif)
  {
    if (!ip4_addr_isany_val(netif->ip_addr))
    {
      if (0 != inet_ntoa(netif->ip_addr))
      {
        PRINT("[DHCP] READY: %s\n", ip4addr_ntoa(&netif->ip_addr));
        if (waiting_task)
          xTaskNotify(waiting_task, 1, eSetBits);
      }
      else
      {
        PRINT("[ERROR] DHCP ip\n");
      }
    }
  }
  else
  {
    PRINT("[ERROR] DHCP netif\n");
  }
}

static void wifi_netif_status_callback(struct netif *netif)
{
  PRINT("[NETIF] Status Changed %s\n", ip4addr_ntoa(netif_ip4_addr(netif)));
}

static err_t wifi_low_level_init(struct netif *netif)
{
  err_t res = ERR_ARG;
  if (netif)
  {
    netif->name[0] = 'w';
    netif->name[1] = '_';
    netif->output = etharp_output;
#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif
    netif->linkoutput = wifi_send;
    netif->mtu = MRF_MAX_PACKET_SIZE;
    netif->hwaddr_len = ETHARP_HWADDR_LEN;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;
    res = WDRV_Init();
  }
  return res;
}

static int wifi_netif_init(const char *IP, const char *GATEWAY, const char *MASK)
{
  if (IP)
    inet_aton(IP, &wifi_ip);
  if (GATEWAY)
    inet_aton(GATEWAY, &wifi_gw);
  if (MASK)
    inet_aton(MASK, &wifi_mk);
  if (NULL == netif_add(&WLAN, &wifi_ip, &wifi_mk, &wifi_gw, &WLAN, wifi_low_level_init, ethernet_input))
  {
    ERROR_FUNC;
    return -1;
  }
  return 0;
}

static void wifi_set_state(bool connected)
{
  if (connected)
  {
    netif_set_status_callback(&WLAN, wifi_dhcp_ip_ready_callback);
    netif_set_link_up(&WLAN);
    netif_set_up(&WLAN);
    netif_set_default(&WLAN);
    if (0 == WLAN.ip_addr.addr)
    {
      err_t dhcp_start(struct netif * netif);
      if (dhcp_start(&WLAN) != ERR_OK)
      {
        PRINT("[ERROR] DHCP\n");
      }
    }
    PRINT("[WIFI] UP\n");
  }
  else
  {
    netif_set_down(&WLAN);
    dhcp_stop(&WLAN);
    WLAN.ip_addr.addr = 0;
    WDRV_Disconnect();
    PRINT("[WIFI] DOWN\n");
  }
}

static void wifi_mac_init(uint8_t const *const addr)
{
  memcpy(WLAN.hwaddr, addr, ETHARP_HWADDR_LEN);
}

static void wifi_receive_CB(uint32_t len, uint8_t const *const frame)
{
  if (isPacketValid(frame))
  {
    struct pbuf *pb = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    if (pb)
    {
      digitalToggle(1);
      err_t err = pbuf_take(pb, frame, len);
      if (ERR_OK == err)
      {
        wifi_receive(&WLAN, pb);
      }
      else
      {
        PRINT("[ERROR] pbuf_take( %d )\n", (int)err);
      }
    }
    else
    {
      PRINT("[ERROR] pbuf_alloc()\n");
    }
  }
}

/// @brief Init interface
/// @param timeout Ticks blocked
/// @return 0 = OK
int WIFI_Init(uint32_t timeout)
{
  static bool once = 0;
  if (once)
    return 1;
  once = 1;

  int res = wifi_netif_init(0, 0, 0);
  if (res)
    return res;

  if (timeout)
  {
    EventBits_t evBits;
    waiting_task = xTaskGetCurrentTaskHandle();
    if (pdFALSE == xTaskNotifyWait(1, 1, &evBits, timeout))
      res = ERR_TIMEOUT;
  }
  else
  {
    waiting_task = 0;
  }

  return res;
}
