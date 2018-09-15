/*discrete sensor hub IPC protocol*/

/*
1. host2dsh command with priority (high to low)
	1.1 CMD_CFG_STREAM
	1.2 CMD_STOP_STREAM
	1.3 CMD_GET_STATUS
	1.4 CMD_IA_NOTIFY 
2. dsh2host response
	2.1 RESP_CMD_ACK
	2.2 RESP_STREAMING
	2.3 PREP_GET_STATUS 
*/

//host2dsh IPC CMD: 
#define CMD_CFG_STREAM 3 
#define CMD_STOP_STREAM 4
#define CMD_GET_STATUS 11
#define CMD_IA_NOFITY 15 

/*
general format: [u8 tran_id][u8 cmd_id][u8 sensor_id][u8 param[] ]
*/
#define CMD_PARAM_MAX_SIZE ((u16)128)
struct ia_cmd {
        u8 tran_id;   // normally 0 
        u8 cmd_id;    // 3 for CMD_CFG_STREAM
        u8 sensor_id; // refer to 2.3 sensor_id definition 
        char param[CMD_PARAM_MAX_SIZE];
} __attribute__ ((packed));

/*1.1 CMD_CFG_STREAM 
format: [u8 tran_id][u8 cmd_id = 3][u8 sensor_id][struct sensor_cfg]
usage: host side send this command to DSH to configure sensor stream behavior
*/

struct sensor_cfg_param {
	u16 sample_freq; /* HZ */   // maximum sensor frequency 
	u16 buff_delay; /* max time(ms) for data bufferring */  // normally 0 :for VR
	u16 bit_cfg;    // keep blank 
}__attribute__ ((packed));

//Note: DSH needs ACK to host after process CMD_CFG_STREAM, refer to DSH2HOST IPC format RESP_CMD_ACK

/*1.2 CMD_STOP_STREAM 
format: [u8 tran_id][u8 4][u8 sensor_id]
usage: host send this command to DSH to stop a running sensor. 
*/

//Note: DSH needs ACK to host after process CMD_STOP_STREAM, refer to DSH2HOST IPC format RESP_CMD_ACK 

/*1.3 CMD_GET_STATUS 
format: [u8 tran_id][u8 11][struct get_status_param]
usage: host send this command to query status sensor table from DSH, 
	   including sensor name, sensor ID, max frequency, running status etc
	   each bit of snr_bitmask is corresponding to a senosr
	   0xFFFFFFFFFFFFFFFF will cover all possible sensors in the system
The get status result packet format refer to following dsh2host protocol  
*/
struct get_status_param {
	u32 snr_bitmask;
} __packed;

/*1.4 CMD_IA_NOTIFY 
format: [u8 tran_id][u8 cmd_id=15][sensor_id=0][struct cmd_ia_notify_param]
usage: host send this command to DSH to sync timestamp
*/

#define IA_NOTIFY_TIMESTAMP_SYNC  ((u8)0x3)

typedef s64 timestamp_t;
struct cmd_ia_notify_param {
	u8 id;  // IA_NOTIFY_TIMESTAMP_SYNC 0x3 
	s64 linux_base_ns; 
} __attribute__ ((packed));


//dsh2host IPC CMD: 
//[u16 sign][u16 packet_len][u8 trans_id][u8 cmd_type][u8 sensor_id][u16 sensor_data_len][n byte data]
#define RESP_CMD_ACK    0
#define RESP_STREAMING  3
#define RESP_GET_STATUS 11

#define LBUF_CELL_SIGN ((u16)0x4853)
#define LBUF_DISCARD_SIGN ((u16)0x4944)

struct frame_head {
	u16 sign;    //LBUF_CELL_SIGN for valid packet and LBUF_DISCARD_SIGN for invalid packet
	u16 length;  // total packet length including frame_head and cmd_resp
};

struct cmd_resp {
        u8 tran_id;  // leave 0
        u8 type;     // cmd_type, 0=RESP_CMD_ACK, 3=RESP_STREAMING, 11=RESP_GET_STATUS 
        u8 sensor_id; // refer to 2.4 sensor_id definition 
        u16 data_len; // length of [n byte data]
        char buf[0];
} __attribute__ ((packed));

/*2.1 RESP_CMD_ACK 
format: [u16 sign][u16 packet_len][u8 trans_id][u8 cmd_type=0][u8 sensor_id=0][u16 sensor_data_len=5][struct resp_cmd_ack]
usage: dsh send this ACK to confirm received and whether the command parsing is correct or not 
note: All commands except "CMD_IA_NOTIFY" need send ACK from dsh to host 
*/

struct resp_cmd_ack {
 u8 cmd_id; // which cmd ACKed in this packet
 s32 ret;   // ret code
 u8 extra[0];  // leave 0
} __attribute__ ((packed));

// ret code 
#define E_SUCCESS       ((int)(0))
#define E_GENERAL		((int)(-1))
#define E_NOMEM			((int)(-2))
#define E_PARAM			((int)(-3))
#define E_BUSY			((int)(-4))
#define E_HW			((int)(-5))
#define E_NOSUPPORT		((int)(-6))
#define E_RPC_COMM		((int)(-7))
#define E_LPE_COMM		((int)(-8))
#define E_CMD_ASYNC		((int)(-9))
#define E_CMD_NOACK		((int)(-10))
#define E_LBUF_COMM		((int)(-11))

/*2.2 RESP_STREAMING 
format: [u16 sign][u16 packet_len][u8 trans_id][u8 cmd_type=3][u8 sensor_id][u16 sensor_data_len][u64 timestamp][n byte sensor data]
usage: dsh send sensor data packet to host
Note: The timestamp is the last data sample timestamp, can be blank for VR 
*/

/* data format of each sensor type */
struct accel_data {
	s64 ts;  // timestamp for this sample
	s16 x;     // x-axis value
	s16 y;     // y-axis value
	s16 z;     // z-axis value 
} __attribute__ ((packed));

struct gyro_raw_data {
	s64 ts;
	s16 x;
	s16 y;
	s16 z;
} __attribute__ ((packed));

struct compass_raw_data {
	s64 ts;
	s16 x;
	s16 y;
	s16 z;
} __attribute__ ((packed));


/*2.3 RESP_GET_STATUS 
format: [u16 packet_len][u8 trans_id][u8 cmd_type=11][u8 sensor_id][u16 sensor_data_len][n byte sensor data]
usage: dsh send this sensors description to host for sensor enumeration 
Note: This is not a single response – there would be N packets and each for a dedicated sensor. 
*/

#define LINK_AS_CLIENT		(0)
#define LINK_AS_MONITOR		(1)
#define LINK_AS_REPORTER	(2)
struct link_info {
	u8 sid;
	u8 ltype;
	u16 rpt_freq;
} __attribute__ ((packed));

struct snr_info {
	u8 sid;  // must be filled
	u8 status; //0
	u16 freq; // must be filled
	u16 data_cnt;  // 0
	u16 bit_cfg;   // 0
	u16 priv;      // 0
	u16 attri;     // 0

	u16 freq_max;   // must be filled
	char name[SNR_NAME_MAX_LEN];  // must be filled 

	u8 health;     // 0
	u8 link_num;   // fill with 0
	struct link_info linfo[0];  // leave blank  
} __attribute__ ((packed));
#define SNR_INFO_SIZE(sinfo) (sizeof(struct snr_info) \
		+ sinfo->link_num * sizeof(struct link_info))
#define SNR_INFO_MAX_SIZE 256


// sensor_id definition 
typedef enum {
    SENSOR_INVALID = -1,
	SENSOR_ACCELEROMETER = 0,
	SENSOR_GYRO,
	SENSOR_COMP,
	SENSOR_BARO,
	SENSOR_ALS,
	SENSOR_PROXIMITY,
	SENSOR_TC,
	SENSOR_LPE,
	SENSOR_ACCELEROMETER_SEC,
	SENSOR_GYRO_SEC,
	SENSOR_COMP_SEC,
	SENSOR_ALS_SEC,
	SENSOR_PROXIMITY_SEC,
	SENSOR_BARO_SEC,

	SENSOR_ACTIVITY,
	SENSOR_GS,
	SENSOR_GESTURE_FLICK,

	SENSOR_ROTATION_VECTOR,
	SENSOR_GRAVITY,
	SENSOR_LINEAR_ACCEL,
	SENSOR_ORIENTATION,
	SENSOR_CALIBRATION_COMP,
	SENSOR_CALIBRATION_GYRO,
	SENSOR_9DOF,
	SENSOR_PEDOMETER,
	SENSOR_MAG_HEADING,
	SENSOR_SHAKING,
	SENSOR_MOVE_DETECT,
	SENSOR_STAP,
	SENSOR_PAN_TILT_ZOOM,
	SENSOR_LIFT_VERTICAL, /*invalid sensor, leave for furture using*/
	SENSOR_DEVICE_POSITION,
	SENSOR_STEPCOUNTER,
	SENSOR_STEPDETECTOR,
	SENSOR_SIGNIFICANT_MOTION,
	SENSOR_GAME_ROTATION_VECTOR,
	SENSOR_GEOMAGNETIC_ROTATION_VECTOR,
	SENSOR_6DOFAG,
	SENSOR_6DOFAM,
	SENSOR_LIFT,
	SENSOR_DTWGS,
	SENSOR_GESTURE_HMM,
	SENSOR_GESTURE_EARTOUCH,
	SENSOR_PEDESTRIAN_DEAD_RECKONING,
	SENSOR_INSTANT_ACTIVITY,
	SENSOR_DIRECTIONAL_SHAKING,
	SENSOR_GESTURE_TILT,
	SENSOR_GESTURE_SNAP,
	SENSOR_PICKUP,
	SENSOR_TILT_DETECTOR,

	SENSOR_BIST,

	SENSOR_EVENT,
	SENSOR_MAX
} psh_sensor_t;

// sensor name map table
struct sensor_name {
        char name[SNR_NAME_MAX_LEN + 1];
};

struct sensor_name sensor_type_to_name_str[SENSOR_MAX] = {
	{"ACCEL"}, {"GYRO"}, {"COMPS"}, {"BARO"}, {"ALS_P"}, {"PS_P"}, {"TERMC"}, {"LPE_P"},
	{"ACC1"}, {"GYRO1"}, {"COMP1"}, {"ALS1"}, {"PS1"}, {"BARO1"}, {"PHYAC"}, {"GSSPT"},
	{"GSFLK"}, {"RVECT"}, {"GRAVI"}, {"LACCL"}, {"ORIEN"}, {"COMPC"}, {"GYROC"}, {"9DOF"},
	{"PEDOM"}, {"MAGHD"}, {"SHAKI"}, {"MOVDT"}, {"STAP"}, {"PZOOM"}, {"LTVTL"}, {"DVPOS"},
	{"SCOUN"}, {"SDET"}, {"SIGMT"}, {"6AGRV"}, {"6AMRV"}, {"6DOFG"}, {"6DOFM"}, {"LIFT"},
	{"DTWGS"}, {"GSPX"}, {"GSETH"}, {"PDR"}, {"ISACT"}, {"DSHAK"}, {"GTILT"}, {"GSNAP"},
	{"PICUP"}, {"TILTD"}, {"BIST"}, {"EVENT"}
};

