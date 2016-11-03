#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "transport.h"


void init_io_machine(io_t* io_data,u8* rx_buf, pFunc rx_func, pFunc tx_func)
{
	io_data->rx_state = DROP;
	io_data->rx_len = 0;
	io_data->rx_valid_counts = 0;
	io_data->rx_buff = rx_buf;
	io_data->rxCPLT =rx_func;
	io_data->txCPLT = tx_func;
}

int rx_reset(io_t* io_data)
{
	io_data->rx_state = DROP;
	io_data->rx_len = 0;
	io_data->rx_valid_counts = 0;
	debug("rx_reset");

	return ER_SUCCESS;
};

int get_header(io_t* io_data)
{
	io_data->rx_state = HEADER;
	debug("Transfer to HEAD_INIT");
	io_data->rx_buff[io_data->rx_valid_counts] = io_data->rx_temp;
	io_data->rx_valid_counts++;	

	return ER_SUCCESS;
};

int cplt_header(io_t* io_data)
{
	io_data->rx_state = LEN;
	io_data->rx_buff[io_data->rx_valid_counts] = io_data->rx_temp;
	io_data->rx_valid_counts++;
	debug("Transfer to HEAD_CPLT");

	return ER_SUCCESS;
};

int dump_len(io_t* io_data)
{
	int i ;
	int ret_val;
	struct frame_head* pfh;
	io_data->rx_buff[io_data->rx_valid_counts] = io_data->rx_temp;
	io_data->rx_valid_counts++;
	
	if (io_data->rx_valid_counts == SIZE_OF_FRAME_HEAD )
	{
		pfh = (struct frame_head*)io_data->rx_buff;
        io_data->rx_len = pfh->length + SIZE_OF_FRAME_HEAD;
		io_data->rx_state = PAYLOAD;
		//printf("parse len as %d\n", io_data->rx_len);

		if(io_data->rx_len > BUFF_SIZE)
		{
			io_data->rx_state = DROP;
			io_data->rx_valid_counts = 0;
			debug("FULL_DROP");
			return ER_NO_ENOUGH_BUFFER_SIZE;
		};

	}
	else
		debug("read count");
	return ER_SUCCESS;
};

int dump_payload(io_t* io_data)
{
	int i;

	io_data->rx_buff[io_data->rx_valid_counts] = io_data->rx_temp;
	io_data->rx_valid_counts++;	

	if (io_data->rx_valid_counts == io_data->rx_len )
	{
		io_data->rx_state = DROP;
		rx_cplt_notify(io_data);
	}

	return ER_SUCCESS;
};

void rx_transfer(io_t* io_data, u8 event)
{
	int i = 0;
	debug("Got event %x", event);
	int event_index = 0;
	if(event == INIT_HEADER)
		event_index = 1;
	else
		event_index = 2;

	
	io_data->rx_temp = event;

	rx_state_table[io_data->rx_state][event_index](io_data);

}
