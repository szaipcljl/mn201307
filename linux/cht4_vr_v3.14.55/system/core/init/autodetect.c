#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

#include <unistd.h>
#include <string.h>
#include <ctype.h>


#include <stdint.h>
#include <linux/types.h>
#include <linux/fb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "log.h"
#include "property_service.h"
#include <sys/system_properties.h>


#define u8 __u8
#define u32 __u32

#define VOID2U64(x) ((uint64_t)(unsigned long)(x))
#define U642VOID(x) ((void *)(unsigned long)(x))

#include "drm_edid.h"
#include "xf86drm.h"
#include "xf86drmMode.h"

static int pixels_x, pixels_y;
/*
 * If display density autodetect is not enabled, ro.sf.lcd_density will be configured in default.prop.
 * Before default.prop is loading, drm device node has not been created, so ro.sf.lcd_density will be
 * set while loading default.prop.
 * If display density autodetect is enabled, ro.sf.lcd_density will not be configured in default.prop.
 * ro.sf.lcd_density will be set here. If the property cannot be set successfully here, SurfaceFlinger
 * will use the density value got from HWComposer.
 */
static int mm_x = 0, mm_y = 0;
static int mm_set = 0;
static int current_rank = 100;
static char *origin = NULL;

static int dpi = 0, rawdpi = 0;

static int verbose;


static int get_rank(char *file)
{
	if (strstr(file, "eDP"))
		return 10;
	if (strstr(file, "LVDS"))
		return 10;
	if (strstr(file, "DSI"))
		return 10;

	if (strstr(file, "HDMI"))
		return 200;

	return 50;
}

static void push_resolution(int x, int y, int m_x, int m_y, int rank, char *__origin)
{

	if (x < pixels_x || y < pixels_y)
		return;

	if (rank > current_rank && mm_set > 0)
		return;

	if (m_x > 0 && m_y > 0) {
		mm_x = m_x;
		mm_y = m_y;
		mm_set = 1;
	}

	if (rank > current_rank)
		return;

	pixels_x = x;
	pixels_y = y;

	if (m_x > 0 && m_y > 0) {
		mm_x = m_x;
		mm_y = m_y;
		mm_set = 1;
	}

	origin = __origin;
}

static int valid_edid_header(struct edid *edid)
{
	int i;
	unsigned char sum = 0, *ptr;
	if (edid->header[0] != 0)	return 0;
	if (edid->header[1] != 0xff)	return 0;
	if (edid->header[2] != 0xff)	return 0;
	if (edid->header[3] != 0xff)	return 0;
	if (edid->header[4] != 0xff)	return 0;
	if (edid->header[5] != 0xff)	return 0;
	if (edid->header[6] != 0xff)	return 0;
	if (edid->header[7] != 0)	return 0;

	ptr = (unsigned char *)edid;
	for (i = 0; i < 128; i++)
		sum += ptr[i];

	if (sum != 0)
		ERROR("edid: Invalid checksum\n");

	return 1;
}

static int vsize(int x, int vfreq_aspect)
{
	int y;
	y = x;
	vfreq_aspect = vfreq_aspect >> 6;
	if (vfreq_aspect == 0)
		y = x * 10 / 16;
	if (vfreq_aspect == 1)
		y = x * 3 / 4;
	if (vfreq_aspect == 2)
		y = x * 4 / 5;
	if (vfreq_aspect == 3)
		y = x * 9 / 16;

	return y;
}

/*
 * Device implementations SHOULD define the standard Android framework
 * density that is numerically closest to the physical density of the
 * screen */
static int snap_dpi(double dpi) {
	if (dpi < 140)
		return 120;
	if (dpi < 187)
		return 160;
	if (dpi < 227)
		return 213;
	if (dpi < 280)
		return 240;
	if (dpi < 360)
		return 320;
	if (dpi < 440)
		return 400;
	if (dpi < 560)
		return 480;

	return 640;
}

static void dpi_math(void)
{
	double xdpi, ydpi, adpi;

	/* detect misrotated dimensions */
	if (((mm_x > mm_y) && (pixels_x < pixels_y)) ||
			((mm_x < mm_y) && (pixels_x > pixels_y))) {
		int t;
		t = mm_x;
		mm_x = mm_y;
		mm_y = t;
	}

	ERROR("edid: Final screen info:   %ix%i pixels, %ix%i mm\n", pixels_x, pixels_y, mm_x, mm_y);

	if (mm_x == 0)
		return;
	if (mm_y == 0)
		return;

	xdpi = 1.0 * pixels_x / (mm_x / 25.4);
	ydpi = 1.0 * pixels_y / (mm_y / 25.4);

	adpi = xdpi;
	if (ydpi > adpi)
		adpi = ydpi;

	ERROR("edid: dpi   %5.2f, %5.2f for an converged dpi of %5.2f  \n", xdpi, ydpi, adpi);
	rawdpi = adpi;
	dpi = snap_dpi(adpi);
	ERROR("edid: Final DPI is %i  \n", dpi);
}

static void parse_edid(char *filename)
{
	FILE *file;
	struct edid edid;
	int ret;
	int i;

	file = fopen(filename, "r");
	if (!file) {
		ERROR("edid: Cannot open %s\n", filename);
		return;
	}
	ret = fread(&edid, 128, 1, file);
	if (ret != 1) {
		fclose(file);
		ERROR("edid: Edid read failed: %i (%s)\n", ret, filename);
		return;
	}
	fclose(file);

	if (!valid_edid_header(&edid)) {
		ERROR("edid: Invalid EDID header  : %02x%02x%02x%02x%02x%02x%02x%02x\n", edid.header[0], edid.header[1], edid.header[2], edid.header[3], edid.header[4], edid.header[5], edid.header[6], edid.header[7]);
		return;
	}

	ERROR("edid: Edid version : %i.%i\n", edid.version, edid.revision);

	for (i = 0; i < 4; i++)
		if (edid.detailed_timings[i].pixel_clock) {
			push_resolution(
				edid.detailed_timings[i].data.pixel_data.hactive_lo + ((edid.detailed_timings[i].data.pixel_data.hactive_hblank_hi >> 4) << 8),
				edid.detailed_timings[i].data.pixel_data.vactive_lo + ((edid.detailed_timings[i].data.pixel_data.vactive_vblank_hi >> 4) << 8),
				edid.detailed_timings[i].data.pixel_data.width_mm_lo + ((edid.detailed_timings[i].data.pixel_data.width_height_mm_hi >> 4) << 8),
				edid.detailed_timings[i].data.pixel_data.height_mm_lo + ((edid.detailed_timings[i].data.pixel_data.width_height_mm_hi & 15) << 8),
				get_rank(filename),
				"EDID detailed timings"
				       );

			ERROR("edid:     %i x %i pixels  \n",  edid.detailed_timings[i].data.pixel_data.hactive_lo + ((edid.detailed_timings[i].data.pixel_data.hactive_hblank_hi >> 4) << 8),
				   			   edid.detailed_timings[i].data.pixel_data.vactive_lo + ((edid.detailed_timings[i].data.pixel_data.vactive_vblank_hi >> 4) << 8));

			ERROR("edid:     %imm x %imm\n", 	   edid.detailed_timings[i].data.pixel_data.width_mm_lo + ((edid.detailed_timings[i].data.pixel_data.width_height_mm_hi >> 4) << 8),
					 edid.detailed_timings[i].data.pixel_data.height_mm_lo + ((edid.detailed_timings[i].data.pixel_data.width_height_mm_hi & 15) << 8));
		}

	for (i = 0; i < 8; i++)
		if (edid.standard_timings[i].hsize && (edid.standard_timings[i].hsize != 1 || edid.standard_timings[i].vfreq_aspect != 1) ) {
			ERROR("edid:     %i x %i\n", (edid.standard_timings[i].hsize + 31) * 8, vsize((edid.standard_timings[i].hsize + 31) * 8, edid.standard_timings[i].vfreq_aspect) );
			ERROR("edid:     %imm x %imm \n", edid.width_cm * 10, edid.height_cm * 10);

			push_resolution( (edid.standard_timings[i].hsize + 31) * 8, vsize((edid.standard_timings[i].hsize + 31) * 8, edid.standard_timings[i].vfreq_aspect),
						edid.width_cm * 10, edid.height_cm * 10,
						get_rank(filename) + 5, "EDID legacy timings");
		}
}

static void parse_display_info(char *filename)
{
	FILE *file;
	file = fopen(filename, "r");
	if (!file) {
		ERROR("edid: cannot open %s\n", filename);
		return;
	}
	while (!feof(file)) {
		char *c;
		char line[PATH_MAX];
		memset(line, 0, PATH_MAX);
		if (fgets(line, PATH_MAX, file) == NULL)
			break;
		c = strchr(line, ':');
		if (c && strstr(line, "physical dimensions")) {
			int x, y;
			while (*c == ' ' || *c == ':') c++;

			x = strtoull(c, NULL, 10);
			if (!x)
				break;
			c = strchr(c, 'x');
			if (!c)
				break;
			c++;
			y = strtoull(c, NULL, 10);
			if (x && y && current_rank == 100) {
				mm_x = x;
				mm_y = y;
				mm_set = 1;
				origin = "i915_display_info";
			}
		}
		if (c && strstr(line, "hdisp")) {
			int x, y;
			while ((*c == ' ') || (*c == ':') || (*c == '"')) c++;

			x = strtoull(c, NULL, 10);
			if (!x)
				break;
			c = strchr(c, 'x');
			if (!c)
				break;
			c++;
			y = strtoull(c, NULL, 10);
			if (x && y && current_rank == 100) {
				pixels_x = x;
				pixels_y = y;
				origin = "i915_display_info";
			}
		}
	}
	fclose(file);
}

void *drmMalloc(int size)
{
	void *pt;

	if ((pt = malloc(size)))
		memset(pt, 0, size);

	return pt;
}

void *drmAllocCpy(void *array, int count, int entry_size)
{
	char *r;
	int i;

	if (!count || !array || !entry_size)
		return 0;

	if (!(r = drmMalloc(count*entry_size)))
		return 0;

	for (i = 0; i < count; i++)
		memcpy(r+(entry_size*i), array+(entry_size*i), entry_size);

	return r;
}

static void parse_drm_connector_info(void)
{
	struct drm_mode_card_res res;
	struct drm_mode_get_connector conn;
	uint32_t *connectors;
	int gfx_fd;
	int ret;

	gfx_fd = open("/dev/dri/card0", O_RDWR | O_CLOEXEC);
	if (gfx_fd < 0) {
		ERROR("edid: cannot open /dev/dri/card0: %s\n", strerror(errno));
		return;
	}

	memset(&res, 0, sizeof(struct drm_mode_card_res));
	if (ret = ioctl(gfx_fd, DRM_IOCTL_MODE_GETRESOURCES, &res)) {
		ERROR("edid: DRM_IOCTL_MODE_GETRESOURCES failed: %s\n", strerror(errno));
		goto close_fd;
	}

	if (res.count_fbs) {
		res.fb_id_ptr = VOID2U64(drmMalloc(res.count_fbs*sizeof(uint32_t)));
		if (!res.fb_id_ptr)
			goto err_allocs3;
	}
	if (res.count_crtcs) {
		res.crtc_id_ptr = VOID2U64(drmMalloc(res.count_crtcs*sizeof(uint32_t)));
		if (!res.crtc_id_ptr)
			goto err_allocs2;
	}
	if (res.count_connectors) {
		res.connector_id_ptr = VOID2U64(drmMalloc(res.count_connectors*sizeof(uint32_t)));
		if (!res.connector_id_ptr) {
			goto err_allocs1;
		}
	}
	if (res.count_encoders) {
		res.encoder_id_ptr = VOID2U64(drmMalloc(res.count_encoders*sizeof(uint32_t)));
		if (!res.encoder_id_ptr)
			goto err_allocs;
	}

	if (ret = ioctl(gfx_fd, DRM_IOCTL_MODE_GETRESOURCES, &res)) {
		ERROR("edid: DRM_IOCTL_MODE_GETRESOURCES failed: %s\n", strerror(errno));
		goto err_allocs;
	}

	memset(&conn, 0, sizeof(struct drm_mode_get_connector));
	connectors = drmAllocCpy(U642VOID(res.connector_id_ptr), res.count_connectors, sizeof(uint32_t));
	if (connectors) {
		/*
		 * Lcd density property is set based on the primary display. The
		 * drm connector id of the primary display is allocated first in
		 * drm driver. connectors[0] is the primary display connector id.
		 * We do not need to get other drm connectors' info here.
		 */
		conn.connector_id = connectors[0];
		if (ret = ioctl(gfx_fd, DRM_IOCTL_MODE_GETCONNECTOR, &conn))
		{
			ERROR("edid: DRM_IOCTL_MODE_GETCONNECTOR failed: %s\n", strerror(errno));
			goto err_allocs;
		}
	}
	close(gfx_fd);

	if ((int)conn.mm_width > 0 && (int)conn.mm_height > 0) {
		mm_x = conn.mm_width;
		mm_y = conn.mm_height;
		mm_set = 1;
		origin = "drm connector info";
	}

err_allocs:
	free(U642VOID(res.encoder_id_ptr));
err_allocs1:
	free(U642VOID(res.connector_id_ptr));
err_allocs2:
	free(U642VOID(res.crtc_id_ptr));
err_allocs3:
	free(U642VOID(res.fb_id_ptr));
close_fd:
	close(gfx_fd);
	return;
}

static void parse_framebuffer_data(void)
{
	int fd;
	struct fb_var_screeninfo vinfo;


	static const char *name = "/dev/__fb0__";
	mknod(name, S_IFCHR | 0600, (29 << 8) | 0);

	fd = open("/dev/graphics/fb0", O_RDWR);

	if (fd < 1)
		fd = open("/dev/fb0", O_RDWR);
	if (fd < 1)
		fd = open("/dev/__fb0__", O_RDWR);

	unlink("/dev/__fb0__");
	if (fd < 0) {
		ERROR("edid: cannot open framebuffer device\n");
		return;
	}

	if (ioctl(fd, FBIOGET_VSCREENINFO, &vinfo) < 0)
	{
		close(fd);
		ERROR("edid: FBIOGET_VSCREENINFO failed\n");
		return;
	}
	close(fd);

	if (vinfo.xres > 0 && vinfo.yres > 0) {
		ERROR("edid: setting resolution based on framebuffer data\n");
		pixels_x = vinfo.xres;
		pixels_y = vinfo.yres;
		origin = "framebuffer";
		if ((int)vinfo.width > 0 && (int)vinfo.height > 0) {
			mm_x = vinfo.width;
			mm_y = vinfo.height;
			mm_set = 1;
			origin = "framebuffer physical dimensions";
		} else {
			parse_drm_connector_info();
		}
	}
}

static int get_edid_dpi(void)
{
	DIR *dir;
	char path[PATH_MAX];
	struct dirent *entry;

	ERROR("edid: start get_edid_dpi\n");

	parse_framebuffer_data();
	parse_display_info("/sys/kernel/debug/dri/0/i915_display_info");


	dir = opendir("/sys/class/drm/");
	if (!dir)
		return dpi;
	while (1) {
		entry = readdir(dir);
		if (!entry)
			break;

		if (entry->d_name[0] == '.')
			continue;

		sprintf(path, "/sys/class/drm/%s/edid", entry->d_name);

		parse_edid(path);

	}
	closedir(dir);

	dpi_math();

	if (!dpi) {
		ERROR("edid: Fail to determine screen density. Default to 160dpi\n");
		rawdpi = 160;
		dpi = 160;
		mm_x = pixels_x * 25.4 / dpi;
		mm_y = pixels_y * 25.4 / dpi;
		origin = "default value";
	}

	sprintf(path, "%i", dpi);
	ERROR("edid: Setting DPI property to %s\n", path);
	property_set("ro.sf.lcd_density", path);
	if (origin)
		property_set("ro.sf.lcd_density_origin", origin);
	sprintf(path, "%i x %ipx %imm x %imm  %i dpi => density: %i", pixels_x, pixels_y, mm_x, mm_y, rawdpi, dpi);
	property_set("ro.sf.lcd_density_info", path);

	return dpi;
}

/* CDD says: $(BRAND)/$(PRODUCT)/$(DEVICE):$(VERSION.RELEASE)/$(ID)/$(VERSION.INCREMENTAL):$(TYPE)/$(TAGS)
 * from /system/build.prop we get strings like:
 *      ro.build.fingerprint=generic/starpeak/starpeak:4.4.2/768/eng.arjan.20140506.154354:userdebug/test-keys
 *      ro.build.fingerprint=Android/irda/irda:4.4.4/KTU84P/IRDA00150:userdebug/test-keys
 * Cut out until the ':', replacing with brand/name/device from bootloader,
 * then set the property.  */
static void create_fingerprint(char *brand, char *name, char *device,
			       char *model)
{
	char fingerprint[4 * PROP_VALUE_MAX];
	char client_id_base[PROP_VALUE_MAX];
	char *c;
	int i;
	FILE *file;
	char line[PATH_MAX];
	char original[PATH_MAX];
	char *clientid = NULL;
	original[0] = 0;

	/* we are usually in the Android OS */
	file = fopen("/system/build.prop", "r");
	if (!file) {
		/* then we are likely in the Recovery Console */
		file = fopen("/default.prop", "r");
		if (!file)
			return;
	}

	while (!feof(file)) {
		memset(line, 0, PATH_MAX);
		if (fgets(line, PATH_MAX, file) == 0)
			break;

		c = strchr(line, '=');
		if (strstr(line, "ro.build.fingerprint=") && c) {
			c++;
			strcpy(original, c);
			c = strchr(original, '\n');
			if (c)
				*c = 0;
		}
	}
	fclose(file);

	c = strchr(original, ':');
	if (!c)
		return;

	if (strlen(brand) + strlen(name) + strlen(device) +
	    strlen(c) + 3 > (PROP_VALUE_MAX - 1)) {
		brand = "ERROR";
		name = "ERROR";
		device = "ERROR";
	}
	property_set("ro.product.brand", brand);

	property_set("ro.product.name", name);

	property_set("ro.product.device", device);
	property_set("ro.build.product", device);
	property_set("ro.product.board", device);

	snprintf(fingerprint, sizeof(fingerprint), "%s/%s/%s%s", brand, name,
		 device, c);
	property_set("ro.build.fingerprint", fingerprint);

	property_set("ro.product.model", model);

	snprintf(client_id_base, PROP_VALUE_MAX, "android-%s", brand);
	property_set("ro.com.google.clientidbase", client_id_base);
}

static void property_get_check(const char *prop, char *buf)
{
	buf[0] = 0;
	property_get(prop, buf);
	if (strlen(buf) == 0)
		strcpy(buf, "BADBIOS");
}

/* FIXME don't hard-code this */
#define IRDA_FISH_NAME "_coho"
void autodetect_properties(bool get_dpi)
{
	char brand[PROP_VALUE_MAX];
	char name[PROP_VALUE_MAX];
	char device[PROP_VALUE_MAX];
	char model[PROP_VALUE_MAX];

	property_get_check("ro.boot.brand", brand);
	property_get_check("ro.boot.name", name);
	property_get_check("ro.boot.device", device);
	strncat(device, IRDA_FISH_NAME, PROP_VALUE_MAX - strlen(device) - 1);
	property_get_check("ro.boot.model", model);
	create_fingerprint(brand, name, device, model);

	if (get_dpi)
		get_edid_dpi();
}


/* sysfs helpers */
static void write_int_to_file(char *filename, int value)
{
	FILE *file;

	file = fopen(filename, "w");
	if (!file) {
		ERROR("Cannot write %i to %s: %s\n",
		      value, filename, strerror(errno));
		return;
	}

	fprintf(file, "%i\n", value);
	fclose(file);
}


static void write_string_to_file(char *filename, char *string)
{
	FILE *file;

	file = fopen(filename, "w");
	if (!file) {
		ERROR("INIT Cannot write %s to %s: %s\n",
		      string, filename, strerror(errno));
		return;
	}

	fprintf(file, "%s\n", string);
	fclose(file);
}

/* SATA links */
static void do_sata_links(void)
{
	DIR *dir;
	struct dirent *entry;

	dir = opendir("/sys/class/scsi_host");
	if (!dir)
		return;

	do {
		char *filename;

		entry = readdir(dir);
		if (!entry)
			break;

		if (strcmp(entry->d_name, ".") == 0)
			continue;
		if (strcmp(entry->d_name, "..") == 0)
			continue;

		if (asprintf(&filename,
			     "/sys/class/scsi_host/%s/link_power_management_policy",
			     entry->d_name) < 0)
			return;

		write_string_to_file(filename, "min_power");
		free(filename);

	} while (1);

	closedir(dir);
}

/* Virtual Memory tweaks */
static void do_vm_tweaks(void)
{
	/* synchronous dirty ratio --> 50% */
	write_int_to_file("/proc/sys/vm/dirty_ratio", 50);
	/*
	 * start IO at 30% not 10%...
	 * the FS/timeout based write generates better IO patterns
	 */
	write_int_to_file("/proc/sys/vm/dirty_background_ratio", 30);
	/*
	 * 15 seconds before the VM starts writeback,
	 * allowing the FS to deal with this better
	 */
	write_int_to_file("/proc/sys/vm/dirty_writeback_centisecs", 1500);
	write_int_to_file("/sys/kernel/mm/transparent_hugepage/khugepaged/scan_sleep_millisecs",
			  300000);

	write_int_to_file("/sys/block/sda/queue/nr_requests", 4096);

	 /* android can't cope with more than 32k */
	write_int_to_file("/proc/sys/vm/mmap_min_addr", 32 * 1024);

	/* oom less */
	write_int_to_file("/proc/sys/vm/extfrag_threshold", 100);
	write_int_to_file("/sys/kernel/mm/ksm/sleep_millisecs", 10000);
	write_int_to_file("/sys/kernel/mm/ksm/run", 1);
	write_int_to_file("/sys/kernel/mm/ksm/pages_to_scan", 1000);
}

/* NMI watch dog */
static void do_nmi_watchdog(void)
{
	write_int_to_file("/proc/sys/kernel/nmi_watchdog", 0);
}

/* Audio PM */
static void do_audio(void)
{
	write_int_to_file("/sys/module/snd_hda_intel/parameters/power_save", 1);
}

/* P-state */
static void do_pstate(void)
{
	write_string_to_file("/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor",
			     "powersave");
	/*
	 * we want at least half performance, this helps us in race-to-halt
	 * and to give us reasonable responses
	 */
	write_int_to_file("/sys/devices/system/cpu/intel_pstate/min_perf_pct", 50);
}

static int pnp_init(void)
{
	char boardname[PROP_VALUE_MAX];
	boardname[0] = 0;
	property_get("ro.boot.board", boardname);

	/* SATA link power management -- except on preproduction hardware */
	if (!strcmp(boardname, "NOTEBOOK\n"))
		do_sata_links();

	/* VM writeback timeout and dirty pages */
	do_vm_tweaks();

	/* turn off the NMI wathdog */
	do_nmi_watchdog();

	/* Audio PM */
	do_audio();

	/* P-state */
	do_pstate();

	return 0;
}

#ifndef HAL_AUTODETECT_KMSG_NAME
#define HAL_AUTODETECT_KMSG_NAME "/dev/__hal_kmsg__"
#endif

void autodetect_init(void)
{
	int rc;

	/*
	 * Create a klog node for hald
	 * Because os sepolicy constraints, hald cannot use mknod. So it's
	 * created by init and opened by hald.
	 */
	if (mknod(HAL_AUTODETECT_KMSG_NAME, S_IFCHR | 0600, (1 << 8) | 11) < 0)
		ERROR("Could not create '%s' character device: %s\n",
		      HAL_AUTODETECT_KMSG_NAME, strerror(errno));

#ifndef HAL_AUTODETECT_PNP_TWEAK_DISABLED
	pnp_init();
#endif
}
