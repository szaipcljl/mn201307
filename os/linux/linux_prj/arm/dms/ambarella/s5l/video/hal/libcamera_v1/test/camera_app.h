#ifndef _CAMERA_APP_H
#define _CAMERA_APP_H
#include "mm_camera_interface.h"

typedef enum {
    MM_CAMERA_OK,
    MM_CAMERA_E_GENERAL,
    MM_CAMERA_E_NO_MEMORY,
    MM_CAMERA_E_NOT_SUPPORTED,
    MM_CAMERA_E_INVALID_INPUT,
    MM_CAMERA_E_INVALID_OPERATION, /* 5 */
    MM_CAMERA_E_ENCODE,
    MM_CAMERA_E_BUFFER_REG,
    MM_CAMERA_E_PMEM_ALLOC,
    MM_CAMERA_E_CAPTURE_FAILED,
    MM_CAMERA_E_CAPTURE_TIMEOUT, /* 10 */
} mm_camera_status_type_t;

typedef struct {
  void *ptr;
  void* ptr_jpeg;

  uint8_t (*get_num_of_cameras) ();
  mm_camera_vtbl_t *(*mm_camera_open) (uint8_t camera_idx);
  //uint32_t (*jpeg_open) (mm_jpeg_ops_t *ops);
} hal_interface_lib_t;

typedef struct {
    uint8_t num_cameras;
    hal_interface_lib_t hal_lib;
} mm_camera_app_t;
#endif
