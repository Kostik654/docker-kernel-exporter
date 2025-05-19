#ifndef METRIC_CALC_FUNCS_H
#define METRIC_CALC_FUNCS_H
#include "additional_types.h"

// for calculation
HostCPUStats return_host_cpu_delta(HostCPUStats cpu_stats_a, HostCPUStats cpu_stats_b);
ContainerCPUStats return_container_cpu_delta(ContainerCPUStats c_cpu_stats_a, ContainerCPUStats c_cpu_stats_b);

float count_host_cpu_load(HostCPUStats cpu_stats_delta); // host
float get_container_cpu_usage(ContainerCPUStats delta, uint64_t interval_usec); // container

#endif