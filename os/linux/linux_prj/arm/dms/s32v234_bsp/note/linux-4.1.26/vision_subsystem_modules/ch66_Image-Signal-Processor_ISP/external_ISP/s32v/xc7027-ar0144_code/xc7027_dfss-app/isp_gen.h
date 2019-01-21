/*
 * ISP call include
 *
 */
#ifndef	__ISP_GEN_H
#define	__ISP_GEN_H

#define ISP_CALL()

#include "typedefs.h"
#include "sdi.hpp" 

void camera_config(void);
void io_config(sdi_grabber *lpGrabber);

#endif // __ISP_GEN_H
