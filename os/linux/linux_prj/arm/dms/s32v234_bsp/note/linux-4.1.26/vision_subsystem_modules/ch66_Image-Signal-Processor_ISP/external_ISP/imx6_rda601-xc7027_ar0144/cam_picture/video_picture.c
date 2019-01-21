#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <string.h>
#include <pthread.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>

struct buffer{
  void *start;
  unsigned int length;
};


int sizes_buf[10][2];
int g_input = 1;



static int getCaptureMode(int width, int height)
{
	int i, mode = -1;

	for (i = 0; i < 10; i++) {
		if (width == sizes_buf[i][0] &&
		    height == sizes_buf[i][1]) {
			mode = i;
			break;
		}
	}

	return mode;
}



int main(int argc, char *argv[])
{
	unsigned int i = 0;
	int ret = -1, mode = -1;
	unsigned int yuv_w = 0, yuv_h = 0, frame = 0;
	unsigned long file_length;
	char yuv_file[20] = {'\0'};



	struct v4l2_capability cap;
	struct v4l2_fmtdesc fmtdesc;
	struct v4l2_format fmt;
	struct v4l2_requestbuffers req;
	enum   v4l2_buf_type type;
	struct v4l2_buffer buf;
	struct v4l2_frmsizeenum frmsize;


	struct buffer *buffers = NULL;
	unsigned int n_buffers = 0;

//	if(argc < 3)
//	{
//		printf("input format:./%s width height frame\n", argv[0]);
//		return 0;
//	}

	yuv_w = 1280; //atoi(argv[1]);
	yuv_h = 720;  //atoi(argv[2]);
	frame = 20;   //atoi(argv[3]);
	//if((frame != 15) && (frame != 30))
	//{
	//	printf("the set frame is not spport, frame=%d\n", frame);
	//	return -1;
	//}

	if((yuv_w > 2592) && (yuv_h > 1944))
	{
		printf("The input data is out of the scale\n");
		return -1;
	}


	printf("yuv_w=%d yuv_h=%d frame=%d\n", yuv_w, yuv_h, frame);


	//snprintf(yuv_file, strlen(argv[1])+strlen(argv[2])+9, "%s_%s_%d.YUV", argv[1], argv[2], frame);
	snprintf(yuv_file, strlen("1280_720_20.YUV")+1, "1280_720_20.YUV");

	printf("the picture name : %s\n", yuv_file);

	int fd = open("/dev/video0",O_RDWR);
	if(fd < 0)
	{
		printf("Open /dev/video0 failed\n");
		return -1;
	}
	printf("video device fd : %d\n",fd);

	//========================
	//struct v4l2_dbg_chip_ident chip;
	//if(ioctl(fd, VIDIOC_DBG_G_CHIP_IDENT, &chip))
	//{
	//	printf("VIDIOC_DBG_G_CHIP_IDENT failed.\n");
	//	return -1;
	//}
	//printf("sensor chip is %s\n", chip.match.name);

	struct v4l2_control ctl;
	ctl.id = V4L2_CID_PRIVATE_BASE;
	ctl.value = 0;
	if (ioctl(fd, VIDIOC_S_CTRL, &ctl) < 0)
	{
		printf("set control failed\n");
		return -1;
	}

	for(i = 0; i < 10; i++)
	{
		frmsize.index = i;
		if (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize))
			break;
		else {
			sizes_buf[i][0] = frmsize.discrete.width;
			sizes_buf[i][1] = frmsize.discrete.height;
		}
	}

	mode = getCaptureMode(yuv_w, yuv_h);
	if (mode == -1) {
		printf("Not support the resolution in camera\n");
		return -1;
	}
	printf("sensor mode is %d frame size is %dx%d\n", mode, sizes_buf[mode][0], sizes_buf[mode][1]);

#if 1
	struct v4l2_streamparm stream_param;
	//GET PARAM
	memset(&stream_param, 0, sizeof(stream_param));
	stream_param.type =V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl (fd, VIDIOC_G_PARM, &stream_param);
	if(ret < 0)
		printf("ERROR: 1 VIDIOC_G_PARM failed\n;");

	printf("capture.capturemode=%d\n",stream_param.parm.capture.capturemode);
	printf("denominator=%d\n",stream_param.parm.capture.timeperframe.denominator);
	printf("numerator=%d\n",stream_param.parm.capture.timeperframe.numerator);

	//SET PARAM
	stream_param.parm.capture.capturemode = mode;
	stream_param.parm.capture.timeperframe.denominator = frame;
	stream_param.parm.capture.timeperframe.numerator   = 1;
	ret = ioctl (fd, VIDIOC_S_PARM, &stream_param);
	if(ret < 0)
		printf("ERROR: 2 VIDIOC_S_PARM failed\n;");

	//GET PARAM
	memset(&stream_param, 0, sizeof(stream_param));
	stream_param.type =V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl (fd, VIDIOC_G_PARM, &stream_param);
	if(ret < 0)
		printf("ERROR: 3 VIDIOC_G_PARM failed\n;");

	printf("capture.capturemode=%d\n",stream_param.parm.capture.capturemode);
	printf("denominator=%d\n",stream_param.parm.capture.timeperframe.denominator);
	printf("numerator=%d\n",stream_param.parm.capture.timeperframe.numerator);
#endif

#if 1
	struct v4l2_crop crop;

	memset(&crop, 0, sizeof(crop));
	crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl (fd, VIDIOC_G_CROP, &crop);
	if(ret < 0)
		printf("ERROR: VIDIOC_G_CROP failed\n;");
	printf("crop.type=%d\n",crop.type);
	printf("crop.c.left=%d\n",crop.c.left);
	printf("crop.c.top=%d\n",crop.c.top);
	printf("crop.c.width=%d\n",crop.c.width);
	printf("crop.c.height=%d\n",crop.c.height);

	crop.c.width = yuv_w;
	crop.c.height = yuv_h;
	ret = ioctl (fd, VIDIOC_S_CROP, &crop);
	if(ret < 0)
		printf("ERROR: VIDIOC_S_CROP failed\n;");

	memset(&crop, 0, sizeof(crop));
	crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl (fd, VIDIOC_G_CROP, &crop);
	if(ret < 0)
		printf("ERROR: VIDIOC_G_CROP failed\n;");
	printf("crop.type=%d\n",crop.type);
	printf("crop.c.left=%d\n",crop.c.left);
	printf("crop.c.top=%d\n",crop.c.top);
	printf("crop.c.width=%d\n",crop.c.width);
	printf("crop.c.height=%d\n",crop.c.height);
#endif

	if (ioctl(fd, VIDIOC_S_INPUT, &g_input) < 0) {
		printf("VIDIOC_S_INPUT failed\n");
		close(fd);
		return -1;
	}

	memset (&fmt, 0, sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(fd,VIDIOC_G_FMT,&fmt);
	if(ret < 0)
	{
		printf("VIDIOC_G_FMT failed\n");
		return -1;
	}
	printf("\n---------VIDIOC_G_FMT---------\n");
	printf(">>>default format\n");
	printf(">>>width : %d height : %d colorspace : %d\n",fmt.fmt.pix.width,fmt.fmt.pix.height,fmt.fmt.pix.colorspace);

	//memset (&fmt, 0, sizeof(fmt));
	fmt.type			  = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width 	  = yuv_w;
	fmt.fmt.pix.height	  = yuv_h;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.field 	  = V4L2_FIELD_INTERLACED;
	ret = ioctl (fd, VIDIOC_S_FMT, &fmt);
	if(ret < 0)
	{
		printf("VIDIOC_S_FMT failed\n");
		return -1;
	}
	printf("\n---------VIDIOC_S_FMT---------\n");
	printf(">>>set format\n");
	printf(">>>width : %d height : %d colorspace : %d\n",fmt.fmt.pix.width,fmt.fmt.pix.height,fmt.fmt.pix.colorspace);

	memset (&fmt, 0, sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(fd,VIDIOC_G_FMT,&fmt);
	if(ret < 0)
	{
		printf("VIDIOC_G_FMT failed\n");
		return -1;
	}
	printf("\n---------VIDIOC_G_FMT---------\n");
	printf(">>>get format\n");
	printf(">>>width : %d height : %d colorspace : %d\n",fmt.fmt.pix.width,fmt.fmt.pix.height,fmt.fmt.pix.colorspace);
	printf(">>>bytesperline : %d\n", fmt.fmt.pix.bytesperline);

	file_length = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height ; //* 2;


	memset (&req, 0, sizeof(req));
	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	ret = ioctl(fd,VIDIOC_REQBUFS,&req);
	if(ret < 0)
	{
		printf("VIDIOC_REQBUFS failed\n");
		return -1;
	}

	buffers = (struct buffer*)calloc (req.count, sizeof(*buffers));
	for(n_buffers = 0; n_buffers < req.count; ++n_buffers){
		memset(&buf,0,sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n_buffers;
		if(ioctl(fd,VIDIOC_QUERYBUF,&buf) == -1){
			printf("VIDIOC_QUERYBUF error\n");
			close(fd);
			exit(-1);
		}
		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start = mmap(NULL,buf.length,PROT_READ|PROT_WRITE,MAP_SHARED,fd,buf.m.offset);
		if(MAP_FAILED == buffers[n_buffers].start){
			printf("mmap error\n");
			close(fd);
			exit(-1);
		}
	}

	for(i = 0; i < 4; i++)
	{
		memset(&buf,0,sizeof(buf));
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		ret = ioctl(fd,VIDIOC_QBUF,&buf);
		if(ret < 0)
		{
			printf("VIDIOC_QBUF failed\n");
			return -1;
		}
	}

	memset(&type,0,sizeof(type));
	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(fd,VIDIOC_STREAMON,&type);
	if(ret < 0)
	{
		printf("VIDIOC_STREAMON failed\n");
		return -1;
	}

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	ret = ioctl(fd,VIDIOC_DQBUF,&buf);
	if(ret < 0)
	{
		printf("VIDIOC_DQBUF failed\n");
		return -1;
	}

	int fdyuyv = open(yuv_file, O_WRONLY|O_CREAT, 00700);
	printf("the picture fd : %d\n",fdyuyv);
	int resultyuyv = write(fdyuyv,buffers[buf.index].start, file_length);
	printf("the picture size : %d\n",resultyuyv);

	close(fdyuyv);
	close(fd);
	return 0;
}

