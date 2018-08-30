#ifndef _UEVENTD_PARSER_H
#define _UEVENTD_PARSER_H

#include "ueventd.h"
#include "ueventd_keywords.h"

#define UEVENTD_PARSER_MAXARGS 5

int ueventd_parse_config_file(const char *fn);
void set_device_permission(int nargs, char **args);
struct ueventd_subsystem *ueventd_subsystem_find_by_name(const char *name);

#endif
