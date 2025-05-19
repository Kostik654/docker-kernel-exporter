#ifndef COLLECTING_FUNCS_H
#define COLLECTING_FUNCS_H
#include "additional_types.h"
#include "filesystem_funcs.h"
#include "metric_calc_funcs.h"

bool update_containers_list(std::string base_path, std::vector<std::string> *list);

std::string get_pid_netdev_full_path(std::string pid_);

MemInfoData get_host_meminfo_data(std::string filepath, bool just_total = false);
HostCPUStats get_host_cpu_data(std::string filepath);

ContainerIOStats get_container_io_stats(std::string filepath);
ContainerMemoryStats get_container_mem_stats(std::string filepath);
ContainerCPUStats get_container_cpu_stats(std::string filepath);

ContainerDockerdData get_container_json_data(std::string filepath);
Cgroup2StatsData get_container_cgroup_data(std::string filepath, unsigned int cpu_int);

NetworkStatsData get_process_network_data(std::string filepath); // any proccess
NetworkStatsData get_processes_sum_network_data(std::vector<std::string> pid_list); // aggregation of all processes network stats


#endif