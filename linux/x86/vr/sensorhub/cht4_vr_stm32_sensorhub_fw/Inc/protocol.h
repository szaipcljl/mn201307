#include "sensor_protocol.h"
#include "transport.h"

#define TX_DATA_ITEM_SIZE 3

#define RX_CMD_EMPTY	0x0
#define RX_CMD_READY 	0x1
#define RX_CMD_PARSED 	0x2

#define TX_DATA_EMPTY		0x0
#define TX_DATA_UPDATING 	0x1
#define TX_DATA_FULL		0x2
#define TX_DATA_SENDING		0x4
#define TX_DATA_LOCKED		0x8

#define IS_RX_CMD_EMPTY(x)  ((x) == RX_CMD_EMPTY)
#define IS_TX_DATA_EMPTY(x)  ((x) == TX_DATA_EMPTY)
#define IS_TX_BUFF_FREE(x)	(((x) == TX_DATA_EMPTY )||((x) == TX_DATA_FULL))
#define IS_TX_BUFF_FULL(x)	((x) == TX_DATA_FULL)
#define IS_TX_BUFF_LOCKED(x) ((x) == TX_DATA_LOCKED)

typedef struct _tx_buff
{
	u8 tx_buf[BUFF_SIZE];
	u8 tx_buff_status;
	u8 tx_buff_length;
}tx_buff_t;

typedef struct _tx_sending
{
	s8 tx_buff_index;
	u8 tx_sending_index;
}tx_sending_t;

typedef struct _tx_updating
{
	s8 tx_buff_index;
	u8 tx_updating_index;
}tx_updating_t;

typedef struct _ia_cmd_head
{
	u8 tran_id;
	u8 cmd_id;
	u8 sensor_id;
}ia_cmd_head_t;

typedef struct _ipc_state
{
	u8 rx_buf[BUFF_SIZE];
	ia_cmd_head_t ia_cmd_head;
	u8 rx_cmd_state;
	u8* tx_sending_buffer;
	u8 tx_sending_index;
	u8 tx_sending_length;
	s8 tx_sending_buff_index;
	u8 sending_locked;
	
	tx_buff_t tx_buf[TX_DATA_ITEM_SIZE];
	//tx_sending_t tx_sending;
	tx_updating_t tx_updating;
	//test propose
	s8 tx_pre_buf_index;
}ipc_state_t;

int init_ipc(ipc_state_t* ipc_data);
s8 find_updating_buf(ipc_state_t* ipc_data);
s8 find_sending_buf(ipc_state_t* ipc_data);
int update_tx_buf_state(ipc_state_t* ipc_data, u8 index, u8 state);

int rx_cplt_notify(io_t* io_data);
int tx_cplt_notify(io_t* io_data);



typedef struct _dsh 
{
  ipc_state_t* ipc_data;  
  status_t status_;
  //status_t tx_cplt_status_;
  
  psh_sensor_t single_sensor;
  
  uint8_t streaming_mode;
  uint8_t ret_err;
  //TODO: Change to list?
  stream_cfg_t stream_sensor[SENSOR_ID_MAX];
  
  uint32_t snr_bitmask;
  int64_t host_base_ns;
  uint16_t max_freq;  

  struct accel_data accel;
  struct gyro_raw_data gyro;
  struct compass_raw_data magn;
  struct rot_raw_data rot;

  struct accel_data accel_raw;
  struct gyro_raw_data gyro_raw;
  struct compass_raw_data magn_raw;
}dsh, *pdsh_handle;

void fill_frame_head(void* buf, uint16_t length);

int init_dsh(pdsh_handle pDsh_data);
int sensor_cmd_parse(ipc_state_t* ipc_data, u8* cmd);
int sensor_data_prepare(pdsh_handle pDsh_handle);
int create_resp(u8* pBuff);
int create_data(u8* pBuff);



