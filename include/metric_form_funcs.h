#ifndef METRIC_FORM_FUNCS_H
#define METRIC_FORM_FUNCS_H
#include "additional_types.h"
#include "metric_calc_funcs.h"

// for output file
std::string get_host_static_metric_fields(StaticHostData host_data);

std::string get_host_cpu_stats_metric_field(HostCPUStats cpustats);
std::string get_host_memory_stats_metric_field(MemInfoData meminfo);
std::string get_host_x_stat_metric_field(MetricArgs base_args);
// aggregation method of host metrics
std::string get_host_stats_fields(HostStatsData host_data);

std::string get_container_metric_field(MetricArgs base_args, std::string c_name, std::string c_id, std::string c_state);
// aggregation method of container metrics
std::string get_container_stats_fields(ContainerStatsData c_data, std::string c_id);

#endif