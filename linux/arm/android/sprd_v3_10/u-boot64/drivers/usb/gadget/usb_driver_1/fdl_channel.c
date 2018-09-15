#include <usb/dl_channel.h>
#include <usb/usb_boot.h>

#define BOOT_FLAG_USB                   (0x5A)
#define BOOT_FLAG_UART1                 (0x6A)
#define BOOT_FLAG_UART0                 (0x7A)

//extern uint32 FDL_GetBootMode (void);
//#define FDL_UART0_CHANNEL_USE

/******************************************************************************/
//  Description:    find a useable channel
//  Global resource dependence:
//  Author:         junqiang.wang
//  Note:
/******************************************************************************/
extern struct FDL_ChannelHandler gUart0Channel;
extern struct FDL_ChannelHandler gUart1Channel;
struct FDL_ChannelHandler *FDL_ChannelGet()
{
    uint32_t bootMode = 0;
    char ch;

    struct FDL_ChannelHandler *channel;
   // bootMode = FDL_GetBootMode();
	bootMode = BOOT_FLAG_USB;
    switch (bootMode)
    {
        case BOOT_FLAG_UART1:
            channel = &gUart1Channel;
            break;
        case BOOT_FLAG_UART0:
            channel = &gUart0Channel;
            break;
        case BOOT_FLAG_USB:
            channel = &gUSBChannel;
            break;
        default:
            channel = &gUSBChannel;
            break;
    }
    return channel;
}
