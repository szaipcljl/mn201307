/*
 * transport.h
 *
 *  Created on: Oct 11, 2016
 *      Author: sugq
 */

#ifndef TRANSPORT_H_
#define TRANSPORT_H_

#include <stdlib.h>
#include <string.h>
#include "sensor_protocol.h"

#define IO_BUF_SIZE 64
#define IO_LEN_SIZE 2
#define BUFF_SIZE 256
#define STATE_COUNT 6

typedef enum
{
	DROP = 0,
	HEADER,
	LEN,
	PAYLOAD,
}SPI_IO_STATE;

typedef enum
{
	ENV_ANY = 0,
	INIT_HEADER = 0x53,
	CPLT_HEADER = 0x48,
}SPI_IO_EVENT;



typedef struct _io_t
{
	SPI_IO_STATE rx_state;
	SPI_IO_STATE tx_state;
	u8* rx_buff;
	u8 rx_temp;
	int rx_len;
	int rx_valid_counts;
	int (*rxCPLT)(struct _io_t*);
	u8 tx_buff;
	int tx_len;
	int tx_counts;
	int (*txCPLT)(struct _io_t*);

	int rx_counter;
	int tx_counter;
}io_t;

typedef int (*pFunc)(io_t* io_data);

struct IO_STATE_MACHINE
{
	SPI_IO_STATE state;
	SPI_IO_EVENT event;
	pFunc		 func;
};

//rx handler
int get_header(io_t* io_data);
int cplt_header(io_t* io_data);
int dump_len(io_t* io_data);
int dump_payload(io_t* io_data);

int rx_reset(io_t* io_data);

void rx_transfer(io_t* io_data, u8 event);
void init_io_machine(io_t* io_data,u8* rx_buf, pFunc rx_func, pFunc tx_func);


static struct IO_STATE_MACHINE rx_state_machine[] = {
		{DROP, INIT_HEADER, get_header},
		{HEADER,CPLT_HEADER, cplt_header},
		{LEN,ENV_ANY, dump_len},
		{PAYLOAD,ENV_ANY, dump_payload},
		{DROP, ENV_ANY, rx_reset},
		{HEADER, ENV_ANY, rx_reset},
};

static pFunc rx_state_table[4][3] = {//ENV_ANY, INIT_HEADER, CPLT_HEADER
	{rx_reset, get_header,rx_reset},//DROP
	{rx_reset, rx_reset, cplt_header}, //HEADER
	{dump_len, dump_len, dump_len}, //LEN
	{dump_payload, dump_payload, dump_payload} //PAYLOAD
};

#endif /* TRANSPORT_H_ */
