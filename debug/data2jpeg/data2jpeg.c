#include <stdio.h>
#include <jpeglib.h>
#include <jerror.h>

#define TRUE 1

/* The following declarations and 5 functions are jpeg related 
 * functions used by put_jpeg_grey_memory and put_jpeg_yuv420p_memory
 */
typedef struct {
        struct jpeg_destination_mgr pub;
        JOCTET *buf;
        size_t bufsize;
        size_t jpegsize;
} mem_destination_mgr;

typedef mem_destination_mgr *mem_dest_ptr;

#if 0
METHODDEF(void) init_destination(j_compress_ptr cinfo)
{
        mem_dest_ptr dest = (mem_dest_ptr) cinfo->dest;

        dest->pub.next_output_byte = dest->buf;
        dest->pub.free_in_buffer = dest->bufsize;
        dest->jpegsize = 0;
}

METHODDEF(boolean) empty_output_buffer(j_compress_ptr cinfo)
{
        mem_dest_ptr dest = (mem_dest_ptr) cinfo->dest;

        dest->pub.next_output_byte = dest->buf;
        dest->pub.free_in_buffer = dest->bufsize;

        return FALSE;
        ERREXIT(cinfo, JERR_BUFFER_SIZE);
}

METHODDEF(void) term_destination(j_compress_ptr cinfo)
{
        mem_dest_ptr dest = (mem_dest_ptr) cinfo->dest;
        dest->jpegsize = dest->bufsize - dest->pub.free_in_buffer;
}
#if 0
static GLOBAL(void) jpeg_mem_dest(j_compress_ptr cinfo, JOCTET* buf, size_t bufsize)
{
        mem_dest_ptr dest;

        if (cinfo->dest == NULL) {
                cinfo->dest = (struct jpeg_destination_mgr *)
                    (*cinfo->mem->alloc_small)((j_common_ptr)cinfo, JPOOL_PERMANENT,
                    sizeof(mem_destination_mgr));
        }

        dest = (mem_dest_ptr) cinfo->dest;

        dest->pub.init_destination    = init_destination;
        dest->pub.empty_output_buffer = empty_output_buffer;
        dest->pub.term_destination    = term_destination;

        dest->buf      = buf;
        dest->bufsize  = bufsize;
        dest->jpegsize = 0;
}
#endif

static GLOBAL(int) jpeg_mem_size(j_compress_ptr cinfo)
{
        mem_dest_ptr dest = (mem_dest_ptr) cinfo->dest;
        return dest->jpegsize;
}

/* put_jpeg_yuv420p_memory converts an input image in the YUV420P format into a jpeg image and puts
 * it in a memory buffer.
 * Inputs:
 * - image_size is the size of the input image buffer.
 * - input_image is the image in YUV420P format.
 * - width and height are the dimensions of the image
 * - quality is the jpeg encoding quality 0-100%
 * Output:
 * - dest_image is a pointer to the jpeg image buffer
 * Returns buffer size of jpeg image     
 */
int put_jpeg_yuv420p_memory(unsigned char *dest_image, int image_size,
                            unsigned char *input_image, int width, int height, int quality)
{
	int i, j, jpeg_image_size;

	JSAMPROW y[16],cb[16],cr[16]; // y[2][5] = color sample of row 2 and pixel column 5; (one plane)
	JSAMPARRAY data[3]; // t[0][2][5] = color sample 0 of row 2 and column 5

	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	data[0] = y;
	data[1] = cb;
	data[2] = cr;

	cinfo.err = jpeg_std_error(&jerr);  // errors get written to stderr 
	
	jpeg_create_compress(&cinfo);
	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = 3;
	jpeg_set_defaults (&cinfo);

	jpeg_set_colorspace(&cinfo, JCS_YCbCr);

	cinfo.raw_data_in = TRUE; // supply downsampled data
	cinfo.comp_info[0].h_samp_factor = 2;
	cinfo.comp_info[0].v_samp_factor = 2;
	cinfo.comp_info[1].h_samp_factor = 1;
	cinfo.comp_info[1].v_samp_factor = 1;
	cinfo.comp_info[2].h_samp_factor = 1;
	cinfo.comp_info[2].v_samp_factor = 1;

	jpeg_set_quality(&cinfo, quality, TRUE);
	cinfo.dct_method = JDCT_FASTEST;

	jpeg_mem_dest(&cinfo, dest_image, image_size);	// data written to mem
	
	jpeg_start_compress (&cinfo, TRUE);

	for (j=0; j<height; j+=16) {
		for (i=0; i<16; i++) {
			y[i] = input_image + width*(i+j);
			if (i%2 == 0) {
				cb[i/2] = input_image + width*height + width/2*((i+j)/2);
				cr[i/2] = input_image + width*height + width*height/4 + width/2*((i+j)/2);
			}
		}
		jpeg_write_raw_data(&cinfo, data, 16);
	}

	jpeg_finish_compress(&cinfo);
	jpeg_image_size = jpeg_mem_size(&cinfo);
	jpeg_destroy_compress(&cinfo);
	
	return jpeg_image_size;
}

/* put_jpeg_grey_memory converts an input image in the grayscale format into a jpeg image
 * Inputs:
 * - image_size is the size of the input image buffer.
 * - input_image is the image in grayscale format.
 * - width and height are the dimensions of the image
 * - quality is the jpeg encoding quality 0-100%
 * Output:
 * - dest_image is a pointer to the jpeg image buffer
 * Returns buffer size of jpeg image     
 */
int put_jpeg_grey_memory(unsigned char *dest_image, int image_size, unsigned char *input_image, int width, int height, int quality)
{
	int y, dest_image_size;
	JSAMPROW row_ptr[1];
	struct jpeg_compress_struct cjpeg;
	struct jpeg_error_mgr jerr;

	cjpeg.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cjpeg);
	cjpeg.image_width = width;
	cjpeg.image_height = height;
	cjpeg.input_components = 1; /* one colour component */
	cjpeg.in_color_space = JCS_GRAYSCALE;

	jpeg_set_defaults(&cjpeg);

	jpeg_set_quality(&cjpeg, quality, TRUE);
	cjpeg.dct_method = JDCT_FASTEST;
	jpeg_mem_dest(&cjpeg, dest_image, image_size);  // data written to mem

	jpeg_start_compress (&cjpeg, TRUE);

	row_ptr[0] = input_image;
	
	for (y=0; y<height; y++) {
		jpeg_write_scanlines(&cjpeg, row_ptr, 1);
		row_ptr[0] += width;
	}
	
	jpeg_finish_compress(&cjpeg);
	dest_image_size = jpeg_mem_size(&cjpeg);
	jpeg_destroy_compress(&cjpeg);

	return dest_image_size;
}

/* put_jpeg_yuv420p_file converts an YUV420P coded image to a jpeg image and writes
 * it to an already open file.
 * Inputs:
 * - image is the image in YUV420P format.
 * - width and height are the dimensions of the image
 * - quality is the jpeg encoding quality 0-100%
 * Output:
 * - The jpeg is written directly to the file given by the file pointer fp
 * Returns nothing
 */
void put_jpeg_yuv420p_file(FILE *fp, unsigned char *image, int width, int height, int quality)
{
	int i,j;

	JSAMPROW y[16],cb[16],cr[16]; // y[2][5] = color sample of row 2 and pixel column 5; (one plane)
	JSAMPARRAY data[3]; // t[0][2][5] = color sample 0 of row 2 and column 5

	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	data[0] = y;
	data[1] = cb;
	data[2] = cr;

	cinfo.err = jpeg_std_error(&jerr);  // errors get written to stderr 
	
	jpeg_create_compress(&cinfo);
	cinfo.image_width = width;
	cinfo.image_height = height;
	cinfo.input_components = 3;
	jpeg_set_defaults(&cinfo);

	jpeg_set_colorspace(&cinfo, JCS_YCbCr);

	cinfo.raw_data_in = TRUE; // supply downsampled data
	cinfo.comp_info[0].h_samp_factor = 2;
	cinfo.comp_info[0].v_samp_factor = 2;
	cinfo.comp_info[1].h_samp_factor = 1;
	cinfo.comp_info[1].v_samp_factor = 1;
	cinfo.comp_info[2].h_samp_factor = 1;
	cinfo.comp_info[2].v_samp_factor = 1;

	jpeg_set_quality(&cinfo, quality, TRUE);
	cinfo.dct_method = JDCT_FASTEST;

	jpeg_stdio_dest(&cinfo, fp);  	  // data written to file
	jpeg_start_compress(&cinfo, TRUE);

	for (j=0;j<height;j+=16) {
		for (i=0;i<16;i++) {
			y[i] = image + width*(i+j);
			if (i%2 == 0) {
				cb[i/2] = image + width*height + width/2*((i+j)/2);
				cr[i/2] = image + width*height + width*height/4 + width/2*((i+j)/2);
			}
		}
		jpeg_write_raw_data(&cinfo, data, 16);
	}

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
}
#endif

/* put_jpeg_grey_file converts an greyscale image to a jpeg image and writes
 * it to an already open file.
 * Inputs:
 * - image is the image in greyscale format.
 * - width and height are the dimensions of the image
 * - quality is the jpeg encoding quality 0-100%
 * Output:
 * - The jpeg is written directly to the file given by the file pointer fp
 * Returns nothing
 */
void put_jpeg_grey_file(FILE *picture, unsigned char *image, int width, int height, int quality)
{
	int y;
	JSAMPROW row_ptr[1];
	struct jpeg_compress_struct cjpeg;
	struct jpeg_error_mgr jerr;

	cjpeg.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cjpeg);
	cjpeg.image_width = width;
	cjpeg.image_height = height;
	cjpeg.input_components = 1; /* one colour component */
	cjpeg.in_color_space = JCS_GRAYSCALE;

	jpeg_set_defaults(&cjpeg);

	jpeg_set_quality(&cjpeg, quality, TRUE);
	cjpeg.dct_method = JDCT_FASTEST;
	jpeg_stdio_dest(&cjpeg, picture);

	jpeg_start_compress(&cjpeg, TRUE);

	row_ptr[0]=image;
	for (y=0; y<height; y++) {
		jpeg_write_scanlines(&cjpeg, row_ptr, 1);
		row_ptr[0]+=width;
	}
	jpeg_finish_compress(&cjpeg);
	jpeg_destroy_compress(&cjpeg);
}

/* put_picture_mem is used for the webcam feature. Depending on the image type
 * (colour YUV420P or greyscale) the corresponding put_jpeg_X_memory function is called.
 * Inputs:
 * - cnt is the global context struct and only cnt->imgs.type is used.
 * - image_size is the size of the input image buffer
 * - *image points to the image buffer that contains the YUV420P or Grayscale image about to be put
 * - quality is the jpeg quality setting from the config file.
 * Output:
 * - **dest_image is a pointer to a pointer that points to the destination buffer in which the
 *   converted image it put
 * Function returns the dest_image_size if successful. Otherwise 0.
 */ 
 #if 0
int put_picture_memory(struct context *cnt, unsigned char* dest_image, int image_size,
                       unsigned char *image, int quality)
{
	switch (cnt->imgs.type) {
		case VIDEO_PALETTE_YUV420P:
			return put_jpeg_yuv420p_memory(dest_image, image_size, image,
			                               cnt->imgs.width, cnt->imgs.height, quality);
		case VIDEO_PALETTE_GREY:
			return put_jpeg_grey_memory(dest_image, image_size, image,
			                            cnt->imgs.width, cnt->imgs.height, quality);
	}

	return 0;
}
#endif 
