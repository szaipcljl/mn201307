#ifndef _MM_CAMERA_INTERFACE_H
#define _MM_CAMERA_INTERFACE_H
#include <stdint.h>



/** mm_camera_buf_notify_t: function definition for frame notify
*   handling
*    @mm_camera_super_buf_t : received frame buffers
*    @user_data: user data pointer
**/
//typedef void (*mm_camera_buf_notify_t) (mm_camera_super_buf_t *bufs,
                                        //void *user_data);

/** mm_camera_super_buf_priority_t: enum for super buffer
*                                   matching priority
*    @MM_CAMERA_SUPER_BUF_PRIORITY_NORMAL :
*       Save the frame no matter focused or not. Currently only
*       this type is supported.
*    @MM_CAMERA_SUPER_BUF_PRIORITY_FOCUS :
*       only queue the frame that is focused. Will enable meta
*       data header to carry focus info
*    @MM_CAMERA_SUPER_BUF_PRIORITY_EXPOSURE_BRACKETING :
*       after shutter, only queue matched exposure index
**/
typedef enum {
    MM_CAMERA_SUPER_BUF_PRIORITY_NORMAL = 0,
    MM_CAMERA_SUPER_BUF_PRIORITY_FOCUS,
    MM_CAMERA_SUPER_BUF_PRIORITY_EXPOSURE_BRACKETING,
    MM_CAMERA_SUPER_BUF_PRIORITY_MAX
} mm_camera_super_buf_priority_t;


typedef struct {
    /** query_capability: fucntion definition for querying static
     *                    camera capabilities
     *    @camera_handle : camer handler
     *  Return value: 0 -- success
     *                -1 -- failure
     *  Note: would assume cam_capability_t is already mapped
     **/
    int32_t (*query_capability) (uint32_t camera_handle);

    /** register_event_notify: fucntion definition for registering
     *                         for event notification
     *    @camera_handle : camer handler
     *    @evt_cb : callback for event notify
     *    @user_data : user data poiner
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    //int32_t (*register_event_notify) (uint32_t camera_handle,
                                      //mm_camera_event_notify_t evt_cb,
                                      //void *user_data);

    /** close_camera: fucntion definition for closing a camera
     *    @camera_handle : camer handler
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*close_camera) (uint32_t camera_handle);

    /** map_buf: fucntion definition for mapping a camera buffer
     *           via domain socket
     *    @camera_handle : camer handler
     *    @buf_type : type of mapping buffers, can be value of
     *                CAM_MAPPING_BUF_TYPE_CAPABILITY
     *                CAM_MAPPING_BUF_TYPE_SETPARM_BUF
     *                CAM_MAPPING_BUF_TYPE_GETPARM_BUF
     *    @fd : file descriptor of the stream buffer
     *    @size :  size of the stream buffer
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
	int32_t (*map_buf) (uint32_t camera_handle,
						uint8_t buf_type,
						int fd,
						uint32_t size);

    /** unmap_buf: fucntion definition for unmapping a camera buffer
     *           via domain socket
     *    @camera_handle : camer handler
     *    @buf_type : type of mapping buffers, can be value of
     *                CAM_MAPPING_BUF_TYPE_CAPABILITY
     *                CAM_MAPPING_BUF_TYPE_SETPARM_BUF
     *                CAM_MAPPING_BUF_TYPE_GETPARM_BUF
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*unmap_buf) (uint32_t camera_handle,
                          uint8_t buf_type);

    /** set_parms: fucntion definition for setting camera
     *             based parameters to server
     *    @camera_handle : camer handler
     *    @parms : batch for parameters to be set, stored in
     *               parm_buffer_t
     *  Return value: 0 -- success
     *                -1 -- failure
     *  Note: would assume parm_buffer_t is already mapped, and
     *       according parameter entries to be set are filled in the
     *       buf before this call
     **/
    //int32_t (*set_parms) (uint32_t camera_handle,
                          //parm_buffer_t *parms);

    /** get_parms: fucntion definition for querying camera
     *             based parameters from server
     *    @camera_handle : camer handler
     *    @parms : batch for parameters to be queried, stored in
     *               parm_buffer_t
     *  Return value: 0 -- success
     *                -1 -- failure
     *  Note: would assume parm_buffer_t is already mapped, and
     *       according parameter entries to be queried are filled in
     *       the buf before this call
     **/
    //int32_t (*get_parms) (uint32_t camera_handle,
                          //parm_buffer_t *parms);

    /** do_auto_focus: fucntion definition for performing auto focus
     *    @camera_handle : camer handler
     *  Return value: 0 -- success
     *                -1 -- failure
     *  Note: if this call success, we will always assume there will
     *        be an auto_focus event following up.
     **/
    int32_t (*do_auto_focus) (uint32_t camera_handle);

    /** cancel_auto_focus: fucntion definition for cancelling
     *                     previous auto focus request
     *    @camera_handle : camer handler
    *  Return value: 0 -- success
    *                -1 -- failure
     **/
    int32_t (*cancel_auto_focus) (uint32_t camera_handle);

    /** prepare_snapshot: fucntion definition for preparing hardware
     *                    for snapshot.
     *    @camera_handle : camer handler
     *    @do_af_flag    : flag indicating if AF needs to be done
     *                     0 -- no AF needed
     *                     1 -- AF needed
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*prepare_snapshot) (uint32_t camera_handle,
                                 int32_t do_af_flag);

    /** start_zsl_snapshot: function definition for starting
     *                    zsl snapshot.
     *    @camera_handle : camer handler
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*start_zsl_snapshot) (uint32_t camera_handle);

    /** stop_zsl_snapshot: function definition for stopping
     *                    zsl snapshot.
     *    @camera_handle : camer handler
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*stop_zsl_snapshot) (uint32_t camera_handle);

    /** add_channel: fucntion definition for adding a channel
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *    @attr : pointer to channel attribute structure
     *    @channel_cb : callbak to handle bundled super buffer
     *    @userdata : user data pointer
     *  Return value: channel id, zero is invalid ch_id
     * Note: attr, channel_cb, and userdata can be NULL if no
     *       superbufCB is needed
     **/
    //uint32_t (*add_channel) (uint32_t camera_handle,
                             //mm_camera_channel_attr_t *attr,
                             //mm_camera_buf_notify_t channel_cb,
                             //void *userdata);

    /** delete_channel: fucntion definition for deleting a channel
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    //int32_t (*delete_channel) (uint32_t camera_handle,
                               //uint32_t ch_id);

    /** get_bundle_info: function definition for querying bundle
     *  info of the channel
     *    @camera_handle : camera handler
     *    @ch_id         : channel handler
     *    @bundle_info   : bundle info to be filled in
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    //int32_t (*get_bundle_info) (uint32_t camera_handle,
                                //uint32_t ch_id,
                                //cam_bundle_config_t *bundle_info);

    /** add_stream: fucntion definition for adding a stream
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *  Return value: stream_id. zero is invalid stream_id
     **/
    uint32_t (*add_stream) (uint32_t camera_handle,
                            uint32_t ch_id);

    /** delete_stream: fucntion definition for deleting a stream
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *    @stream_id : stream handler
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*delete_stream) (uint32_t camera_handle,
                              uint32_t ch_id,
                              uint32_t stream_id);

    /** config_stream: fucntion definition for configuring a stream
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *    @stream_id : stream handler
     *    @confid : pointer to a stream configuration structure
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    //int32_t (*config_stream) (uint32_t camera_handle,
                              //uint32_t ch_id,
                              //uint32_t stream_id,
                              //mm_camera_stream_config_t *config);

    /** map_stream_buf: fucntion definition for mapping
     *                 stream buffer via domain socket
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *    @stream_id : stream handler
     *    @buf_type : type of mapping buffers, can be value of
     *             CAM_MAPPING_BUF_TYPE_STREAM_BUF
     *             CAM_MAPPING_BUF_TYPE_STREAM_INFO
     *             CAM_MAPPING_BUF_TYPE_OFFLINE_INPUT_BUF
     *    @buf_idx : buffer index within the stream buffers
     *    @plane_idx : plane index. If all planes share the same fd,
     *               plane_idx = -1; otherwise, plean_idx is the
     *               index to plane (0..num_of_planes)
     *    @fd : file descriptor of the stream buffer
     *    @size :  size of the stream buffer
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*map_stream_buf) (uint32_t camera_handle,
                               uint32_t ch_id,
                               uint32_t stream_id,
                               uint8_t buf_type,
                               uint32_t buf_idx,
                               int32_t plane_idx,
                               int fd,
                               uint32_t size);

    /** unmap_stream_buf: fucntion definition for unmapping
     *                 stream buffer via domain socket
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *    @stream_id : stream handler
     *    @buf_type : type of mapping buffers, can be value of
     *             CAM_MAPPING_BUF_TYPE_STREAM_BUF
     *             CAM_MAPPING_BUF_TYPE_STREAM_INFO
     *             CAM_MAPPING_BUF_TYPE_OFFLINE_INPUT_BUF
     *    @buf_idx : buffer index within the stream buffers
     *    @plane_idx : plane index. If all planes share the same fd,
     *               plane_idx = -1; otherwise, plean_idx is the
     *               index to plane (0..num_of_planes)
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*unmap_stream_buf) (uint32_t camera_handle,
                                 uint32_t ch_id,
                                 uint32_t stream_id,
                                 uint8_t buf_type,
                                 uint32_t buf_idx,
                                 int32_t plane_idx);

    /** set_stream_parms: fucntion definition for setting stream
     *                    specific parameters to server
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *    @stream_id : stream handler
     *    @parms : batch for parameters to be set
     *  Return value: 0 -- success
     *                -1 -- failure
     *  Note: would assume parm buffer is already mapped, and
     *       according parameter entries to be set are filled in the
     *       buf before this call
     **/
    //int32_t (*set_stream_parms) (uint32_t camera_handle,
                                 //uint32_t ch_id,
                                 //uint32_t s_id,
                                 //cam_stream_parm_buffer_t *parms);

    /** get_stream_parms: fucntion definition for querying stream
     *                    specific parameters from server
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *    @stream_id : stream handler
     *    @parms : batch for parameters to be queried
     *  Return value: 0 -- success
     *                -1 -- failure
     *  Note: would assume parm buffer is already mapped, and
     *       according parameter entries to be queried are filled in
     *       the buf before this call
     **/
    //int32_t (*get_stream_parms) (uint32_t camera_handle,
                                 //uint32_t ch_id,
                                 //uint32_t s_id,
                                 //cam_stream_parm_buffer_t *parms);

    /** start_channel: fucntion definition for starting a channel
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *  Return value: 0 -- success
     *                -1 -- failure
     * This call will start all streams belongs to the channel
     **/
    int32_t (*start_channel) (uint32_t camera_handle,
                              uint32_t ch_id);

    /** stop_channel: fucntion definition for stopping a channel
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *  Return value: 0 -- success
     *                -1 -- failure
     * This call will stop all streams belongs to the channel
     **/
    int32_t (*stop_channel) (uint32_t camera_handle,
                             uint32_t ch_id);

    /** qbuf: fucntion definition for queuing a frame buffer back to
     *        kernel for reuse
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *    @buf : a frame buffer to be queued back to kernel
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    //int32_t (*qbuf) (uint32_t camera_handle,
                     //uint32_t ch_id,
                     //mm_camera_buf_def_t *buf);

    /** request_super_buf: fucntion definition for requesting frames
     *                     from superbuf queue in burst mode
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *    @num_buf_requested : number of super buffers requested
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*request_super_buf) (uint32_t camera_handle,
                                  uint32_t ch_id,
                                  uint32_t num_buf_requested);

    /** cancel_super_buf_request: fucntion definition for canceling
     *                     frames dispatched from superbuf queue in
     *                     burst mode
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*cancel_super_buf_request) (uint32_t camera_handle,
                                         uint32_t ch_id);

    /** flush_super_buf_queue: function definition for flushing out
     *                     all frames in the superbuf queue up to frame_idx,
     *                     even if frames with frame_idx come in later than
     *                     this call.
     *    @camera_handle : camer handler
     *    @ch_id : channel handler
     *    @frame_idx : frame index up until which all superbufs are flushed
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    int32_t (*flush_super_buf_queue) (uint32_t camera_handle,
                                      uint32_t ch_id, uint32_t frame_idx);

    /** configure_notify_mode: function definition for configuring the
     *                         notification mode of channel
     *    @camera_handle : camera handler
     *    @ch_id : channel handler
     *    @notify_mode : notification mode
     *  Return value: 0 -- success
     *                -1 -- failure
     **/
    //int32_t (*configure_notify_mode) (uint32_t camera_handle,
                                      //uint32_t ch_id,
                                      //mm_camera_super_buf_notify_mode_t notify_mode);
} mm_camera_ops_t;

/** mm_camera_vtbl_t: virtual table for camera operations
*    @camera_handle : camera handler which uniquely identifies a
*                   camera object
*    @ops : API call table
**/
typedef struct {
    uint32_t camera_handle;
    mm_camera_ops_t *ops;
} mm_camera_vtbl_t;

#endif
