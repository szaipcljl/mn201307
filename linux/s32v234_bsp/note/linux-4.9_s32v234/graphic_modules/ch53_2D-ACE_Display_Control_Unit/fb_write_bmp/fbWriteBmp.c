#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <string.h>

int display(char *fb_name, char *pic_name, int transparency_level)
{
	int fd;

	struct fb_var_screeninfo vinfo;
	struct fb_fix_screeninfo finfo;

	int x, y, bpp;
	long ssize = 0;

	unsigned char * sstart;

	int bmpfd;
	unsigned char *buf;
	int off;
	//unsigned short int tmp;
	unsigned int tmp;
	int location;

	fd = open(fb_name, O_RDWR);
	if(fd < 0) {
		printf("Error: open %s failed\n", fb_name);
		return 1;
	}

	ioctl(fd, FBIOGET_FSCREENINFO, &finfo);
	ioctl(fd, FBIOGET_VSCREENINFO, &vinfo);

	x = vinfo.xres;
	printf("xres: %d\n", x);

	y = vinfo.yres;
	printf("yres: %d\n", y);


	bpp = vinfo.bits_per_pixel;
	printf("bits_per_pixel: %d\n", bpp);


	ssize = x*y*bpp/8;
	printf("screen size: %d\n", ssize);


	sstart = (unsigned char *)mmap(0, ssize, PROT_WRITE, MAP_SHARED, fd, 0);
	if( sstart == MAP_FAILED)
		return 2;

	buf = (unsigned char *)malloc(x*y*3);
	memset(buf, 0, x*y*3);


	bmpfd = open(pic_name, O_RDONLY);
	if(bmpfd < 0) {
		printf("Error:open %s failed\n", pic_name);
		return 3;
	}

	read(bmpfd, buf, 14);

	off = buf[10]+buf[11]*256+buf[12]*256*256+buf[13]*256*256*256;
	lseek(bmpfd, off, SEEK_SET);

	memset(buf, 0, 14);
	off = read(bmpfd, buf, x*y*3);
	if(off < x*y*3) {
		printf("Error: read failed\n");
		return 4;
	}

	int i;
	for(i = 0; i < x*y; i++)
	{
		tmp = 0;
		//tmp |= ((buf[i*3+2]&0xf8)*256);
		//tmp |= ((buf[i*3+1]&0xfc)*8);
		//tmp |= ((buf[i*3]&0xf8)/8);
		location = (y-i/x-1)*x+i%x;

		tmp = ((0xff * (100 - transparency_level) /100) << 24) | (buf[i*3+2] << 16) | (buf[i*3+1] << 8) | buf[i*3]; //RGB888

		//*((unsigned short int *)(sstart+location*2)) = tmp;
		*((unsigned int *)(sstart+location*4)) = tmp; //RGB888
	}

}

int main(int argc ,char **argv)
{
	char *fb0_dev = "/dev/fb0";
	char *fb1_dev = "/dev/fb1";
	char *tree_pic = "tree.bmp";
	char *fish_pic = "fish.bmp";
	int transparency_level; //0~100, 

	display(fb0_dev, tree_pic, 100); //fb0 is up layer
	display(fb1_dev, fish_pic, 50);
}
