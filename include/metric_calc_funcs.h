#ifndef METRIC_CALC_FUNCS_H
#define METRIC_CALC_FUNCS_H
#include "additional_types.h"

// for calculation
float count_host_cpu_load(HostCPUStats cpu_stats); // host
float get_container_cpu_usage(ContainerCPUStats cpu_stats); // container

#endif