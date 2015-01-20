
#ifndef __AC_LOGGER_H__
#define __AC_LOGGER_H__

#include <stdio.h>
#define ac_log_error(fmt, arg...) printf("ERROR:"fmt"\n", ## arg)
#define ac_log_info(fmt, arg...) printf("INFO:"fmt"\n", ## arg)
#define ac_log_warn(fmt, arg...) printf("WARN:"fmt"\n", ## arg)
#define ac_log_debug(fmt, arg...) printf("DEBUG:"fmt"\n", ## arg)
#define ac_log_fatal(fmt, arg...) printf("FATAL:"fmt"\n", ## arg)

#endif //__AC_LOGGER_H__
