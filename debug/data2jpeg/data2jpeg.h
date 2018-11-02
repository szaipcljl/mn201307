#ifndef    _DATA_2_JPEG_H_
#define    _DATA_2_JPEG_H_

#ifdef __cplusplus    //__cplusplus是cpp中自定义的一个宏
extern "C" {          //告诉编译器，这部分代码按C语言的格式进行编译，而不是C++的
#endif
void put_jpeg_grey_file(FILE *picture, unsigned char *image, int width, int height, int quality);
int put_jpeg_grey_memory(unsigned char *dest_image, int image_size, unsigned char *input_image, int width, int height, int quality);
void put_jpeg_yuv420p_file(FILE *fp, unsigned char *image, int width, int height, int quality);
int put_jpeg_yuv420p_memory(unsigned char *dest_image, int image_size, unsigned char *input_image, int width, int height, int quality);

#ifdef __cplusplus    //__cplusplus是cpp中自定义的一个宏
}
#endif
#endif

