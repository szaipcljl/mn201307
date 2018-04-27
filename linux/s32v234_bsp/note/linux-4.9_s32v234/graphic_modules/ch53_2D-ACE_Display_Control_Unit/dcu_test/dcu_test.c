#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>

int main(int argc, char *argv[])
{
	int fbfd = 0;
	struct fb_var_screeninfo vinfo;
	unsigned long screensize = 0;
	unsigned long location = 0;
	char *fbp = 0;
	int x = 0, y = 0;
	int seg_len = 0;
	int tmp_seg_len = 0;
	int seg_num = 0;
	//unsigned short rgb = 0;
	unsigned int rgb = 0;
	unsigned int r = 0, g = 0, b = 0;
	unsigned int R8 = 0, G8 = 0, B8 = 0;

	// Open the file for reading and writing
	fbfd = open("/dev/fb0", O_RDWR);
	if (!fbfd) {
		printf("Error: cannot open framebuffer device.\n");
		exit(1);
	}
	printf("The framebuffer device was opened successfully.\n");

	// Get variable screen information
	if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
		printf("Error reading variable information.\n");
		exit(1);
	}

	printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
	if (vinfo.bits_per_pixel != 16) {
		/*printf("Error: not supported bits_per_pixel, it only supports 16 bit color\n"); //RGB565:16bpp*/
		/*exit(1);*/
	}

	// Figure out the size of the screen in bytes
	screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

	// Map the device to memory
	fbp = (unsigned char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED,
			fbfd, 0);
	if ((int)fbp == -1) {
		printf("Error: failed to map framebuffer device to memory.\n");
		exit(4);
	}
	printf("The framebuffer device was mapped to memory successfully.\n");

	seg_len = vinfo.yres/6;
#if 1
	seg_len = vinfo.yres/6;
	for (seg_num = 0; seg_num < 6; seg_num++) {
		if (seg_num == 5)
			tmp_seg_len = vinfo.yres - seg_len*5;
		else
			tmp_seg_len = seg_len;

		for (y = 0; y < tmp_seg_len; y++) {
			for (x = 0; x < vinfo.xres; x++) {
				location = (seg_num * seg_len * vinfo.xres + (y * vinfo.xres + x)) * (vinfo.bits_per_pixel/8);
				switch (seg_num) {
				case 0:
					r = 0xff;
					g = (0xff/seg_len)*y;
					b = 0;
					break;
				case 1:
					r = (0xff/seg_len)*(seg_len-y);
					g = 0xff;
					b = 0;
					break;
				case 2:
					r = 0;
					g = 0xff;
					b = (0xff/seg_len)*y;
					break;
				case 3:
					r = 0;
					g = (0xff/seg_len)*(seg_len-y);
					b = 0xff;
					break;
				case 4:
					r =  (0xff/seg_len)*y;
					g = 0;
					b = 0xff;
					break;
				case 5:
					r = 0xff;
					g = 0;
					b = (0xff/seg_len)*(seg_len-y);
					break;
				default:
					printf("%s--%d:unknown seg_num %d\n", __FILE__, __LINE__);
					break;
				}

				//r = (r*0x1f)/0xff;
				//g = (g*0x3f)/0xff;
				//b = (b*0x1f)/0xff;
				//rgb = (r << 11) | (g << 5) | b; //RGB565
				//*((unsigned short*)(fbp + location)) = rgb;

				rgb = (0xff << 24) | (r << 16) | (g << 8) | b; //RGB888
				*((unsigned int*)(fbp + location)) = rgb;
			}
		}
	}

	//return 0;
	sleep(2);

	seg_len = vinfo.yres/6;
	for (seg_num = 0; seg_num < 6; seg_num++) {
		if (seg_num == 5)
			tmp_seg_len = vinfo.yres - seg_len*5;
		else
			tmp_seg_len = seg_len;

		for (y = 0; y < tmp_seg_len; y++) {
			for (x = 0; x < vinfo.xres; x++) {
				location = (seg_num*seg_len*vinfo.xres + (y*vinfo.xres+ x)) * (vinfo.bits_per_pixel/8);
				switch (seg_num) {
				case 0://grey
					r = 100;
					g = 100;
					b = 100;
					break;
				case 1: //black
					r = 0x00;
					g = 0x00;
					b = 0x00;
					break;
				case 2://white
					r = 0xff;
					g = 0xff;
					b = 0xff;
					break;
				case 3://red
					r = 0xff;
					g = 0;
					b = 0;
					break;
				case 4: //green
					r =  0;
					g = 0xff;
					b = 0;
					break;
				case 5: //blue
					r = 0;
					g = 0;
					b = 0xff;
					break;
				default:
					printf("%s--%d:unknown seg_num %d\n", __FILE__, __LINE__);
					break;
				}

				//r = (r*0x1f)/0xff;
				//g = (g*0x3f)/0xff;
				//b = (b*0x1f)/0xff;
				//rgb = (r << 11) | (g << 5) | b; //rgb565

				rgb = (0xff << 24) | (r << 16) | (g << 8) | b; //rgb888
				*((unsigned int*)(fbp + location)) = rgb;
			}
		}
	}
#endif

#if 1
	sleep(2);

	seg_len = vinfo.xres/6;
	for (seg_num = 0; seg_num < 6; seg_num++) {
		if (seg_num == 5)
			tmp_seg_len = vinfo.xres - seg_len*5;
		else
			tmp_seg_len = seg_len;

		for (x = 0; x < tmp_seg_len; x++) {
			for (y = 0; y < vinfo.yres; y++) {
				location = (y*vinfo.xres + (seg_num*seg_len + x)) * (vinfo.bits_per_pixel/8);

				switch (seg_num) {
				case 0:
					r = 0xff;
					g = (0xff/seg_len)*x;
					b = 0;
					break;
				case 1:
					r = (0xff/seg_len)*(seg_len-x);
					g = 0xff;
					b = 0;
					break;
				case 2:
					r = 0;
					g = 0xff;
					b = (0xff/seg_len)*x;
					break;
				case 3:
					r = 0;
					g = (0xff/seg_len)*(seg_len-x);
					b = 0xff;
					break;
				case 4:
					r =  (0xff/seg_len)*x;
					g = 0;
					b = 0xff;
					break;
				case 5:
					r = 0xff;
					g = 0;
					b = (0xff/seg_len)*(seg_len-x);
					break;
				default:
					printf("%s--%d:unknown seg_num %d\n", __FILE__, __LINE__);
					break;
				}

				//r = (r*0x1f)/0xff;
				//g = (g*0x3f)/0xff;
				//b = (b*0x1f)/0xff;
				//rgb = (r << 11) | (g << 5) | b; //rgb565

				rgb = (0xff << 24) | (r << 16) | (g << 8) | b; //rgb888
				*((unsigned int*)(fbp + location)) = rgb;
			}
		}
	}

	sleep(2);
#endif

#if 1
	seg_len = vinfo.xres/6;
	/* white black gray red green blue */
	for (seg_num = 0; seg_num < 6; seg_num++) {
		if (seg_num == 5)
			tmp_seg_len = vinfo.xres - seg_len*5;
		else
			tmp_seg_len = seg_len;

		for (x = 0; x < tmp_seg_len; x++) {
			for (y = 0; y < vinfo.yres * 1; y++) {
				location = (y * vinfo.xres + (seg_num * seg_len + x)) * (vinfo.bits_per_pixel/8);

				switch (seg_num) {
				case 0://grey
					r = 100;
					g = 100;
					b = 100;
					break;
				case 1://black
					r = 0;
					g = 0;
					b = 0;
					break;
				case 2: //white
					r = 0xff;
					g = 0xff;
					b = 0xff;
					break;
				case 3://red
					r = 0xff;
					g = 0;
					b = 0;
					break;
				case 4: //green
					r =  0;
					g = 0xff;
					b = 0;
					break;
				case 5: //blue
					r = 0;
					g = 0;
					b = 0xff;
					break;
				default:
					printf("%s--%d:unknown seg_num %d\n", __FILE__, __LINE__);
					break;
				}

				//r = (r*0x1f)/0xff;
				//g = (g*0x3f)/0xff;
				//b = (b*0x1f)/0xff;
				//rgb = (r << 11) | (g << 5) | b; //rgb565
				//*((unsigned short*)(fbp + location)) = rgb;

				rgb = (0xff << 24)|(r << 16) | (g << 8) | b; //rgb888
				*((unsigned int*)(fbp + location)) = rgb;
			}
		}
	}
#endif

	munmap(fbp, screensize);
	close(fbfd);
	return 0;
}
