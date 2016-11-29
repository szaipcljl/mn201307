#ifndef __PSH_CHT_SPI_TEST_H__
#define __PSH_CHT_SPI_TEST_H__



/*
  common share function list here for sensor spi driver
 */
struct psh_ext_if;

void poll_sensor_data_debug(struct psh_ext_if *psh_if_info);

void dump_cmd_resp(char *ptr, int len);

#endif
