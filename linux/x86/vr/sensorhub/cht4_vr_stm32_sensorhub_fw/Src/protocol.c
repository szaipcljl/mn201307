#include "protocol.h"
#include "common.h"
#include <string.h>

extern io_t io_state;
extern ipc_state_t ipc_state;
extern pdsh_handle pDsh_handle;

static int ts_counter = 0;

	static void dump_buffer(u8* lp_buffer, int size)
	{
		int loop_end = size / 8;
		int remain_end = size % 8;
		int index = 0;
	
		for (int loop_i = 0; loop_i < loop_end; ++loop_i)
		{
			printf("[%0.2d]:", index);
			for (int loop_j = 0; loop_j < 8; ++loop_j)
			{
				printf(" 0x%0.2x,", lp_buffer[index++]);
			}
			printf("\n", index);
		}
	
		if (0 != remain_end)
		{
			printf("[%0.2d]:", index);
			for (int loop_i = 0; loop_i < remain_end; ++loop_i)
			{
				printf(" 0x%0.2x,", lp_buffer[index++]);
			}
			printf("\n", index);
		}
	
		return;
	}

int init_ipc(ipc_state_t* ipc_data)
{
	int index = 0;
	
	ipc_data->rx_cmd_state = RX_CMD_EMPTY;
	memset(ipc_data->rx_buf, 0, BUFF_SIZE);
	
	ipc_data->tx_pre_buf_index = -1;

	ipc_data->tx_sending_buffer = NULL;
	ipc_data->tx_sending_index = 0;
	ipc_data->tx_sending_buff_index = -1;
	ipc_data->tx_sending_length = 0;

	ipc_data->tx_updating.tx_buff_index = -1;
	ipc_data->tx_updating.tx_updating_index = 0;
	
	for(index = 0; index < TX_DATA_ITEM_SIZE; index++)
	{
		ipc_data->tx_buf[index].tx_buff_length = 0;
		ipc_data->tx_buf[index].tx_buff_status = TX_DATA_EMPTY;
		memset(ipc_data->tx_buf[index].tx_buf, 0, BUFF_SIZE);
	}
		
	return ER_SUCCESS;
}

s8 find_updating_buf(ipc_state_t* ipc_data)
{
	int i = 0;

	for(i=0; i< TX_DATA_ITEM_SIZE; i++)
          if(IS_TX_BUFF_FREE(ipc_data->tx_buf[i].tx_buff_status)){
            update_tx_buf_state(ipc_data,i, TX_DATA_UPDATING);
			//printf("found updating index %d\n", i);
			return i;
          }

	return -1;
}

s8 find_sending_buf(ipc_state_t* ipc_data)
{
	int i = 0;

	for (i = 0; i < TX_DATA_ITEM_SIZE; i++)
		if(IS_TX_BUFF_LOCKED(ipc_data->tx_buf[i].tx_buff_status))
			{
				printf("found locking buff index %d\n",i);
				ipc_data->tx_sending_buffer = ipc_data->tx_buf[i].tx_buf;
			ipc_data->tx_sending_index = 0;
			ipc_data->tx_sending_length = ipc_data->tx_buf[i].tx_buff_length;
			ipc_data->tx_sending_buff_index = i;
			update_tx_buf_state(ipc_data, i, TX_DATA_SENDING);
				return i;
			}
		
	for(i=0; i< TX_DATA_ITEM_SIZE; i++)
		if(IS_TX_BUFF_FULL(ipc_data->tx_buf[i].tx_buff_status))
			{
			update_tx_buf_state(ipc_data,i, TX_DATA_SENDING);
			//printf("found sending index %d\n", i);
			ipc_data->tx_sending_buffer = ipc_data->tx_buf[i].tx_buf;
			ipc_data->tx_sending_index = 0;
			ipc_data->tx_sending_length = ipc_data->tx_buf[i].tx_buff_length;
			ipc_data->tx_sending_buff_index = i;
			return i;
			}

	return -1;
}


int update_tx_buf_state(ipc_state_t* ipc_data, u8 index, u8 state)
{
	ipc_data->tx_buf[index].tx_buff_status = state;

	return ER_SUCCESS;
}


int rx_cplt_notify(io_t* io_data)
{
	int i = 0;
	debug("got io_data string:\t");
/*
	for(i = 0; i< io_data->rx_len; i++)
	{
		printf(" 0x%x", io_data->rx_buff[i]);
		io_data->rx_buff[i] = '\0';
	}
	printf("\n");
*/
	io_data->rx_counter = io_data->rx_valid_counts;
	io_data->rx_valid_counts = 0;

	ipc_state.rx_cmd_state = RX_CMD_READY;
}

int tx_cplt_notify(io_t* io_data)
{
	int ret_val;

	ret_val =  update_tx_buf_state(&ipc_state, ipc_state.tx_sending_buff_index, TX_DATA_EMPTY);
	ipc_state.tx_pre_buf_index = ipc_state.tx_sending_buff_index;
	io_data->tx_counter = ipc_state.tx_sending_length;
	
	ipc_state.tx_buf[ipc_state.tx_sending_buff_index].tx_buff_length = 0;
	ipc_state.tx_sending_buffer = NULL;
	ipc_state.tx_sending_index = 0;
	ipc_state.tx_sending_buff_index = -1;
	ipc_state.tx_sending_length = 0;

	debug("TX cplt");
}

int init_dsh(pdsh_handle pDsh_data)
{
	pDsh_data->ipc_data = &ipc_state;
	pDsh_data->status_ = STA_IDLE;
}

int sensor_data_parse(ipc_state_t* ipc_data, u8* cmd)
{
	printf("SH RX Buffer:\n");
	dump_buffer(ipc_state.rx_buf, io_state.rx_counter);
	debug("Nothing");
	ipc_data->rx_cmd_state = RX_CMD_PARSED;

	struct ia_cmd* ia_cmd_handle = NULL;
	//parse cmd packet
		uint8_t i = 0;
		pDsh_handle->status_ = STA_RESP_ACK;
		pDsh_handle->ret_err = E_SUCCESS;
		struct sensor_cfg_param *cfg_para = NULL;
		stream_cfg_t *pcfg_t = NULL;

		ia_cmd_handle = (struct ia_cmd*)&ipc_state.rx_buf[SIZE_OF_FRAME_HEAD];

		ipc_state.ia_cmd_head.cmd_id = ia_cmd_handle->cmd_id;
		ipc_state.ia_cmd_head.sensor_id = ia_cmd_handle->sensor_id;
		ipc_state.ia_cmd_head.tran_id = ia_cmd_handle->tran_id;
		
		switch(pDsh_handle->ipc_data->ia_cmd_head.cmd_id){
		case CMD_CFG_STREAM:
			//cfg_para = (struct sensor_cfg_param *)pDsh_handle->ia_buff.param;
			//debug("CMD_COFNIG_STREAM for sensor %d", pDsh_handle->ia_buff.sensor_id);
			pcfg_t = 
			&pDsh_handle->stream_sensor[pDsh_handle->ipc_data->ia_cmd_head.sensor_id];
			pcfg_t->isStream = 1;
			pcfg_t->cfg.bit_cfg = cfg_para->bit_cfg;
			pcfg_t->cfg.buff_delay = cfg_para->buff_delay;
			pcfg_t->cfg.sample_freq = cfg_para->sample_freq;
			
			pDsh_handle->streaming_mode = 1;
			//for(i = 1; i < 8; i++ ) 
			//	pDsh_handle->stream_sensor[i].isStream = 1;		
		break;
	
		case CMD_STOP_STREAM:
			debug("CMD_STOP_STREAM");
			for(i = 0; i < SENSOR_ID_MAX; i++)
				pDsh_handle->stream_sensor[i].isStream = 0;
			pDsh_handle->streaming_mode = 0;
			break;
		case CMD_LOW_TRANSFER_TEST:
			pDsh_handle->streaming_mode = 0;
			pDsh_handle->status_ = STA_RESP_ACK;
			break;
			
		default:
			pDsh_handle->status_ = STA_IDLE;
		}		
	
	return ER_SUCCESS;
}

void fill_frame_head(void* buf, uint16_t length)
{
	debug("fill FRAME_HEAD");
	
	struct frame_head *pHEAD = (struct frame_head*)buf;
	pHEAD->length = length;
	pHEAD->sign = LBUF_CELL_SIGN;
}


int create_resp(u8* pBuff)
{
	int pack_len = 0;
	u8* src_ptr = 0;
	u8* dest_ptr = 0;
	int index = 0;
	
	struct cmd_resp *presp = NULL;
	pDsh_handle->ret_err = ER_SUCCESS;

	presp = (struct cmd_resp*)&pBuff[SIZE_OF_FRAME_HEAD];
	

	//packing ACK

	//header
	printf("built ACK pack\n");
	presp->tran_id = ipc_state.ia_cmd_head.tran_id;
	presp->type = RESP_CMD_ACK;
	presp->sensor_id = ipc_state.ia_cmd_head.sensor_id;
	presp->data_len = sizeof(struct resp_cmd_ack);

	pack_len += RESP_SIZE;

	struct resp_cmd_ack *presp_ack = (struct resp_cmd_ack*)presp->buf;
	presp_ack->cmd_id = ipc_state.ia_cmd_head.cmd_id;
	presp_ack->ret = pDsh_handle->ret_err;

	pack_len += presp->data_len;
	//packing DATA for single commands
	presp = (struct cmd_resp*)&pBuff[SIZE_OF_FRAME_HEAD + pack_len];
	presp->tran_id = ipc_state.ia_cmd_head.tran_id;
	presp->sensor_id = ipc_state.ia_cmd_head.sensor_id;

	pack_len += RESP_SIZE;
	switch(ipc_state.ia_cmd_head.cmd_id)
	{
		case CMD_CFG_STREAM:
		case CMD_STOP_STREAM:
			pack_len -= RESP_SIZE;
			break;
		case CMD_LOW_TRANSFER_TEST:
			//copy rx payload
			src_ptr = &io_state.rx_buff[SIZE_OF_FRAME_HEAD];
			dest_ptr = presp->buf;

			for(index = 0; index< (io_state.rx_counter- SIZE_OF_FRAME_HEAD);index++)
			{
				dest_ptr[index] = src_ptr[index];
				
			}
			presp->type = RESP_LOW_TRANSFER_TEST;
			presp->data_len = io_state.rx_counter- SIZE_OF_FRAME_HEAD;
			printf("copy src payload 0x%x bytes\n", presp->data_len);

			pack_len += (io_state.rx_counter - SIZE_OF_FRAME_HEAD);
			break;
		default:
			break;
	}
	
	fill_frame_head(pBuff, pack_len);
	pack_len += SIZE_OF_FRAME_HEAD;

	return pack_len;
}

int create_data(u8* pBuff)
{
	int pack_len = 0;
	struct cmd_resp *presp = NULL;
	pDsh_handle->ret_err = ER_SUCCESS;
	//printf("create streaming data here\n");
	presp = (struct cmd_resp*)&pBuff[SIZE_OF_FRAME_HEAD];
	u8* buff_ptr = NULL;

	buff_ptr = (u8*) presp;

	//fill data here
	int i = 0;
    int counts = 0;

	pSENSOR_DATA raw = NULL;


	for(i = 0; i < SENSOR_ID_MAX; i++ ) {
		if(pDsh_handle->stream_sensor[i].isStream == 1){		

			presp = (struct cmd_resp*)buff_ptr;
			presp->tran_id = ipc_state.ia_cmd_head.tran_id;
			presp->type = RESP_STREAMING;
			presp->sensor_id = i;
			presp->data_len = sensor_t_size[i];
			raw = (pSENSOR_DATA)presp->buf;

			pack_len += RESP_SIZE;
			pack_len += presp->data_len;
			buff_ptr += (RESP_SIZE + sensor_t_size[i]);			

			switch(presp->sensor_id) {
					case SENSOR_ACCELEROMETER:
						//printf("prepare ACC data\n");
						raw->ts = pDsh_handle->accel.ts;
						raw->accel.x= pDsh_handle->accel.x;
						raw->accel.y = pDsh_handle->accel.y;
						raw->accel.z = pDsh_handle->accel.z;
						break;
					case SENSOR_GYRO:
						//printf("prepare GYRO data");
						raw->ts = pDsh_handle->gyro.ts;
						raw->gyro.x = pDsh_handle->gyro.x;
						raw->gyro.y = pDsh_handle->gyro.y;
						raw->gyro.z = pDsh_handle->gyro.z;
						break;
					case SENSOR_COMP:
						//printf("prepare COMPASS data");
						raw->ts = pDsh_handle->magn.ts;
						raw->compass.x = pDsh_handle->magn.x;
						raw->compass.y = pDsh_handle->magn.y;
						raw->compass.z = pDsh_handle->magn.z;
						break;
					case SENSOR_ACC_RAW:
						debug("prepare ACC_RAW data");
						raw->ts = pDsh_handle->accel_raw.ts;
						raw->accel.x= pDsh_handle->accel_raw.x;
						raw->accel.y = pDsh_handle->accel_raw.y;
						raw->accel.z = pDsh_handle->accel_raw.z;

						break;
					case SENSOR_GYRO_RAW:
						debug("prepare GYRO_RAW data");
						raw->ts = pDsh_handle->gyro_raw.ts;
						raw->gyro.x = pDsh_handle->gyro_raw.x;
						raw->gyro.y = pDsh_handle->gyro_raw.y;
						raw->gyro.z = pDsh_handle->gyro_raw.z;
						break;
					case SENSOR_COMP_RAW:
						debug("prepare COMPASS_RAW data");
						raw->ts = pDsh_handle->magn_raw.ts;
						raw->compass.x = pDsh_handle->magn_raw.x;
						raw->compass.y = pDsh_handle->magn_raw.y;
						raw->compass.z = pDsh_handle->magn_raw.z;
						break;
					case SENSOR_ROT:
						debug("prepare ROTATION matrix");
						raw->ts = pDsh_handle->accel_raw.ts;

						for(counts = 0; counts < 9; counts++)
						raw->rot[counts] = pDsh_handle->rot.matrix[counts];
						break;
    				default:
      					break;
				}
				
			}
		}
		//printf("prepare data done counts %d\n", ts_counter);
	fill_frame_head(pBuff,pack_len);
	pack_len += SIZE_OF_FRAME_HEAD;	

	return pack_len;
}

int sensor_data_prepare(pdsh_handle pDsh_handle)
{
	s8 data_index = -1;
	u8 data_length = 0;

	if((ipc_state.rx_cmd_state == RX_CMD_PARSED)||
		(pDsh_handle->streaming_mode == 1))
			data_index = find_updating_buf(pDsh_handle->ipc_data);
	else
		return ER_FAILED;
	
	if(data_index == -1)
		return ER_FAILED;
	
	if(ipc_state.rx_cmd_state == RX_CMD_PARSED)
	{
		//recvd and parsed cmd
		data_length = create_resp(pDsh_handle->ipc_data->tx_buf[data_index].tx_buf);
		pDsh_handle->ipc_data->tx_buf[data_index].tx_buff_length = data_length;
		update_tx_buf_state(pDsh_handle->ipc_data, data_index, TX_DATA_LOCKED);
		ipc_state.rx_cmd_state = RX_CMD_EMPTY;

		return ER_SUCCESS;
	}
	if(pDsh_handle->streaming_mode == 1) 
	{
		data_length = create_data(pDsh_handle->ipc_data->tx_buf[data_index].tx_buf);
		pDsh_handle->ipc_data->tx_buf[data_index].tx_buff_length = data_length;
		update_tx_buf_state(pDsh_handle->ipc_data,data_index, TX_DATA_FULL);
	}
	
	//printf("prepared data :\n");
	//	dump_buffer(pDsh_handle->ipc_data->tx_buf[data_index].tx_buf,
	//		pDsh_handle->ipc_data->tx_buf[data_index].tx_buff_length);
	return ER_SUCCESS;
}


#if 0
int sensor_data_prepare(ipc_state_t* ipc_data)
{
	printf("prepare_data\n");
	struct frame_head *pfh;
	int ret_val;
	u8 data_index = TX_DATA_ITEM_SIZE;
	int i = rand()%IO_BUF_SIZE;
	int j = 0;
	debug("fake data %d", i);
	//fake data

	data_index = find_updating_buf(ipc_data);
	if(data_index == -1)
		return ER_FAILED;

	ipc_data->tx_updating.tx_buff_index = data_index;
	
	printf("create %x payload in index %d\n", i, data_index);
	pfh = (struct frame_head*)ipc_data->tx_buf[data_index].tx_buf;
	pfh->sign = 0x4853;
	pfh->length = i;

	ipc_data->tx_buf[data_index].tx_buff_length = i + SIZE_OF_FRAME_HEAD;

	for(j = SIZE_OF_FRAME_HEAD; j < i + SIZE_OF_FRAME_HEAD  ; j++)
		ipc_data->tx_buf[data_index].tx_buf[j] = j;

	return update_tx_buf_state(ipc_data,data_index, TX_DATA_FULL);
}
#endif
