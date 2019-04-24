#include <stdio.h>
#include <unistd.h>

#include "cmr_interface.h"
#include "cmr_dev_info.h"


int main(int argc, const char *argv[])
{
	#define TIME 1
	struct i2c_dev *cmr_dev;
	int count = 5;

	// get the initialized and opened camera device
	cmr_dev = i2c_dev_init();

	// get the ir led status, this will save the origin led ctl value when ir
	// led is on.
	if (cmr_dev->ops->get_sensor_ir_led_status) {
		cmr_dev->ops->get_sensor_ir_led_status(cmr_dev);
	}

	printf("========\nchange the ir led status:\n");
	// change the ir led status
	do {
		printf("\n[%d] set ir led off:\n", count);
		if (cmr_dev->ops->set_sensor_ir_led_off) {
			cmr_dev->ops->set_sensor_ir_led_off(cmr_dev);
		}
		sleep(TIME);

		printf("\n[%d]set ir led on:\n", count);
		if (cmr_dev->ops->set_sensor_ir_led_on) {
			cmr_dev->ops->set_sensor_ir_led_on(cmr_dev);
		}
		printf("--------\n\n");
		sleep(TIME);

	} while (count--);

	return 0;
}

