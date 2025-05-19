#ifndef METRIC_FORM_FUNCS_H
#define METRIC_FORM_FUNCS_H
#include "additional_types.h"
#include "metric_calc_funcs.h"

// for output file
std::string get_host_static_metric_fields(StaticHostData host_data); // static info for collector

std::string get_stat_metric_field(MetricArgs base_args);
// aggregation method of host metrics
std::string get_host_stats_fields(HostStatsData host_data);  // dynamic info for collector

// aggregation method of container metrics
std::string get_container_stats_fields(ContainerStatsData c_data, std::string c_id);  // dynamic container info for collector

#endif