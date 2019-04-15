//camera_app.c
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>

#include "camera_app.h"

int mm_app_load_hal(mm_camera_app_t *my_cam_app)
{
    memset(&my_cam_app->hal_lib, 0, sizeof(hal_interface_lib_t));
    my_cam_app->hal_lib.ptr = dlopen("libmmcamera_interface.so", RTLD_NOW);
    /*my_cam_app->hal_lib.ptr_jpeg = dlopen("libmmjpeg_interface.so", RTLD_NOW);*/
    if (!my_cam_app->hal_lib.ptr /*|| !my_cam_app->hal_lib.ptr_jpeg*/) {
        printf("%s Error opening HAL library %s\n", __func__, dlerror());
        return -MM_CAMERA_E_GENERAL;
    }
    *(void **)&(my_cam_app->hal_lib.get_num_of_cameras) =
        dlsym(my_cam_app->hal_lib.ptr, "get_num_of_cameras");
    *(void **)&(my_cam_app->hal_lib.mm_camera_open) =
        dlsym(my_cam_app->hal_lib.ptr, "camera_open");
    /**(void **)&(my_cam_app->hal_lib.jpeg_open) =*/
        /*dlsym(my_cam_app->hal_lib.ptr_jpeg, "jpeg_open");*/

    if (my_cam_app->hal_lib.get_num_of_cameras == NULL ||
        my_cam_app->hal_lib.mm_camera_open == NULL/* ||*/
        /*my_cam_app->hal_lib.jpeg_open == NULL*/) {
        printf("%s Error loading HAL sym %s\n", __func__, dlerror());
        return -MM_CAMERA_E_GENERAL;
    }

    my_cam_app->num_cameras = my_cam_app->hal_lib.get_num_of_cameras();
    printf("%s: num_cameras = %d\n", __func__, my_cam_app->num_cameras);

    return MM_CAMERA_OK;
}

int main(int argc, const char *argv[])
{
	int c;
    int run_tc = 0;
    int run_dual_tc = 0;
    mm_camera_app_t my_cam_app;

    printf("\nCamera Test Application\n");

    while ((c = getopt(argc, argv, "tdh")) != -1) {
        switch (c) {
           case 't':
               run_tc = 1;
               break;
           case 'd':
               run_dual_tc = 1;
               break;
           case 'h':
           default:
               printf("usage: %s [-t] [-d] \n", argv[0]);
               printf("-t:   Unit test        \n");
               printf("-d:   Dual camera test \n");
               return 0;
        }
    }

    memset(&my_cam_app, 0, sizeof(mm_camera_app_t));
    if((mm_app_load_hal(&my_cam_app) != MM_CAMERA_OK)) {
        printf("%s:mm_app_init err\n", __func__);
        return -1;
    }

	return 0;
}
