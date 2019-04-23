#ifndef _BRG_INIT_H
#define _BRG_INIT_H

#define DS90UB914_ADDR  0x61
#define DS90UB913_ADDR  0x5D  //write it into 914 reg addr 0x07

int program_register_for_913_ar0144(int bus_id);
#endif
