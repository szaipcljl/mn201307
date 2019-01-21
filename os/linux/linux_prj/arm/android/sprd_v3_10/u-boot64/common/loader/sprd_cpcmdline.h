#ifndef _MODEM_COMMON_H_
#define _MODEM_COMMON_H_

#include <config.h>
#include <common.h>
#include <linux/string.h>


#define MAX_CP_CMDLINE_LEN  (200)

/*cp cmd define */

#define BOOT_MODE        "androidboot.mode"
#define CALIBRATION_MODE "calibration"
#define LTE_MODE         "ltemode"
#define AP_VERSION       "apv"
#define RF_BOARD_ID      "rfboard.id"
#define K32_LESS		"32k.less"

void cmdline_add_cp_cmdline(char *cmd, char* value);
void cp_cmline_fixup(void);
char* cp_getcmdline(void);

#endif // _MODEM_COMMON_H_
