#!/bin/bash

# "RX0 VC=0"
echo "# i2cset -y 1 0x30 0x4c 0x1 #RX0"
i2cset -y 1 0x30 0x4c 0x1

echo "# i2cset -y 1 0x30 0x71 0xEC #Map sensor A VC0 to CSI-Tx VC0"
i2cset -y 1 0x30 0x71 0xEC

# "RX1 VC=1"

# CSI_EN
echo "# i2cset -y 1 0x30 0x33 0x1 #CSI_EN & CSI0 4L"
i2cset -y 1 0x30 0x33 0x1

# "Basic_FWD"
echo "# i2cset -y 1 0x30 0x21 0x14 #Basic_FWD"
i2cset -y 1 0x30 0x21 0x14
echo "# i2cset -y 1 0x30 0x20 0x00 #FWD_PORT RX port0 to CSI0"
i2cset -y 1 0x30 0x20 0x20


# // "RX0 VC=0"
# ds90ub954_write_reg_8(ambrg, pinfo->i2c_ctrl.addr.des, UB954_FPD3_PORT_SEL_REG, 0x01); //RX0
# //ds90ub954_write_reg_8(ambrg, pinfo->i2c_ctrl.addr.des, UB954_CSI_VC_MAP_REG, 0xE8) ; //Map Sensor A VC0 to CSI-Tx VC0
# ds90ub954_write_reg_8(ambrg, pinfo->i2c_ctrl.addr.des, UB954_RAW12_ID_REG, 0xEC) ; //Map Sensor A VC0 to CSI-Tx VC0 //sure??
#
# // "RX1 VC=1"
# //ds90ub954_write_reg_8(ambrg, pinfo->i2c_ctrl.addr.des, UB954_FPD3_PORT_SEL_REG,0x12) ; //RX1
# //ds90ub954_write_reg_8(ambrg, pinfo->i2c_ctrl.addr.des, UB954_RAW10_ID_REG, 0xED) ; //Map Sensor B VC0 to CSI-Tx VC1
#
# //"CSI_EN"
# ds90ub954_write_reg_8(ambrg, pinfo->i2c_ctrl.addr.des, UB954_CSI_CTL_REG, 0x1) ; //CSI_EN & CSI0 4L
# //ds90ub954_write_reg_8(ambrg, pinfo->i2c_ctrl.addr.des, UB954_CSI_CTL_REG, 0x21) ; //CSI_EN & CSI0 2L
#
# // "Basic_FWD"
# ds90ub954_write_reg_8(ambrg, pinfo->i2c_ctrl.addr.des, UB954_FWD_CTL2_REG, 0x14) ; //Synchronized Basic_FWD
# //"FWD_PORT all RX to CSI0"
# ds90ub954_write_reg_8(ambrg, pinfo->i2c_ctrl.addr.des, UB954_FWD_CTL1_REG,0x00) ; //forwarding of all RX to CSI0

#define UB954_FPD3_PORT_SEL_REG 0x4c
#define UB954_RX_PORT_CTL_REG 0x0C
#define UB954_BCC_CONFIG_REG 0x58
#define UB954_SER_ALIAS_ID_REG 0x5C
#define UB954_SLAVE_ID0_REG 0x5D
#define UB954_SLAVE_ALIAS_ID0_REG 0x65
#define UB954_I2C_DEVICE_ID_REG 0x00

#define UB954_DEVICE_STS_REG 0x04

#//CSI control
#define UB954_FWD_CTL1_REG 0x20
#define UB954_FWD_CTL2_REG 0x21
#define UB954_CSI_CTL_REG 0x33
#define UB954_PORT_CONFIG_REG 0x6D
#define UB954_PORT_CONFIG2_REG 0x7C
#define UB954_RAW10_ID_REG 0x70   //RAW10 input mode
#define UB954_RAW12_ID_REG 0x71   //RAW12 input mode
#define UB954_CSI_VC_MAP_REG 0x72 //CSI-2 input mode

