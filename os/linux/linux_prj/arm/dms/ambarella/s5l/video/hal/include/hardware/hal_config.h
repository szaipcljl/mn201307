#ifndef _HAL_CONFIG_H_
#define _HAL_CONFIG_H_

/* Include variant definitions */
#include "hal_variants.h"

/* Select the correct configuration file */
#include "hal_config_a1.h"
/*
#ifdef VS_HW_PROFILE
#   if defined(VS_R013) && (VS_HW_PROFILE == VS_R013)
#       ifdef VS_HW_SAMPLE
#           if defined(VS_A1) && (VS_HW_SAMPLE == VS_A1)
#               include "hal_config_a1.h"
#           elif defined(VS_A2) && (VS_HW_SAMPLE == VS_A2)
#               include "hal_config_a2.h"
#           else
#               error Value of VS_HW_SAMPLE not recognised or not defined
#           endif
#       else
#           error VS_HW_SAMPLE not defined
#       endif
#   else
#       error Value of VS_HW_PROFILE not recognised or not defined
#   endif
#else
#   error VS_HW_PROFILE not defined
#endif
*/

#endif


