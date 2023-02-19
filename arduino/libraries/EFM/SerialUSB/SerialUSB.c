#include <xc.h>
#include <interface.h>
#include <tusb.h>

#if !defined(TINYUSB_HOST_LINKED) && !defined(TINYUSB_DEVICE_LINKED)

#define USBD_VID (0x04D8)
#define USBD_PID (0x000A)
// https://www.the-sz.com/products/usbid/index.php?v=04d8&p=&n=

#define USBD_DESC_LEN (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN)
#define USBD_MAX_POWER_MA (250)

#define USBD_ITF_CDC (0) // needs 2 interfaces
#define USBD_ITF_MAX (2)

#define USBD_CDC_EP_CMD (0x81)
#define USBD_CDC_EP_OUT (0x02)
#define USBD_CDC_EP_IN (0x82)
#define USBD_CDC_CMD_MAX_SIZE (8)
#define USBD_CDC_IN_OUT_MAX_SIZE (64)

#define USBD_STR_0 (0x00)
#define USBD_STR_MANUF (0x01)
#define USBD_STR_PRODUCT (0x02)
#define USBD_STR_SERIAL (0x03)
#define USBD_STR_CDC (0x04)

static const tusb_desc_device_t usbd_desc_device = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = USBD_VID,
    .idProduct = USBD_PID,
    .bcdDevice = 0x0100,
    .iManufacturer = USBD_STR_MANUF,
    .iProduct = USBD_STR_PRODUCT,
    .iSerialNumber = USBD_STR_SERIAL,
    .bNumConfigurations = 1,
};

static const uint8_t usbd_desc_cfg[USBD_DESC_LEN] = {
    TUD_CONFIG_DESCRIPTOR(1, USBD_ITF_MAX, USBD_STR_0, USBD_DESC_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, USBD_MAX_POWER_MA),
    TUD_CDC_DESCRIPTOR(USBD_ITF_CDC, USBD_STR_CDC, USBD_CDC_EP_CMD, USBD_CDC_CMD_MAX_SIZE, USBD_CDC_EP_OUT, USBD_CDC_EP_IN, USBD_CDC_IN_OUT_MAX_SIZE),
};

static const char *const usbd_desc_str[] = {
    [USBD_STR_MANUF] = "Microchip",
    [USBD_STR_PRODUCT] = "WizIO",
    [USBD_STR_SERIAL] = "000000000000", // TODO
    [USBD_STR_CDC] = "Board CDC",
};

const uint8_t *tud_descriptor_device_cb(void)
{
    return (const uint8_t *)&usbd_desc_device;
}

const uint8_t *tud_descriptor_configuration_cb(uint8_t index)
{
    (void)index;
    return usbd_desc_cfg;
}

const uint16_t *tud_descriptor_string_cb(uint8_t index, uint16_t langid)
{
#define DESC_STR_MAX (20)
    static uint16_t desc_str[DESC_STR_MAX];

    uint8_t len;
    if (index == 0)
    {
        desc_str[1] = 0x0409; // supported language is English
        len = 1;
    }
    else
    {
        if (index >= sizeof(usbd_desc_str) / sizeof(usbd_desc_str[0]))
        {
            return NULL;
        }
        const char *str = usbd_desc_str[index];
        for (len = 0; len < DESC_STR_MAX - 1 && str[len]; ++len)
        {
            desc_str[1 + len] = str[len];
        }
    }

    // first byte is length (including header), second byte is string type
    desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * len + 2);

    return desc_str;
}

void __attribute__((interrupt(IPL2AUTO), vector(_USB_VECTOR), no_fpu)) USBD_IRQHandler(void)
{
    IFS4CLR = _IFS4_USBIF_MASK;
    tud_int_handler(0);
}

#endif // _LINKED
