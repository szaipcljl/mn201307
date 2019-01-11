/*
 * kernel/private/drivers/vout/ds90ub925/ds90ub925.h
 *
 * History:
 *	2019/01/07 - [Ning Ma]
 *
 * Copyright (C) 2018  Roadefend, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef _DS90UB925_H
#define _DS90UB925_H

//gpio pins
#define UB925_INT 18
#define UB925_PDB 19

//addr
//ds90ub925 address 7'b:0x0C, 8'b: 0x18
#define UB925_ADDR_7BIT 0x0C
//926 i2c addr 7'bit: 0x2c, 8'b: 0x58
#define UB926_ADDR_7BIT 0x2C
#define UB926_ADDR 0x58

// slave:IT66121FN, 7'b:0x4c, 8'b: 0x98
#define SLAVE_ADDR_7BIT 0x4C
#define SLAVE_ADDR 0x98

//ub925 regs
#define UB925_I2C_PASS_THROUGH_REG 0X03
#define UB925_SER_ID_REG 0x06
#define UB925_SLAVE_ID_REG 0x07
#define UB925_SLAVE_ALIAS_REG 0x08

//ub926 regs
// reset IT66121FN
// 926.GPO_REG6(0x20) -> IT66121FN.SYSRSTN: 0->1
#define UB926_GPO_REG6 0x20

#endif
