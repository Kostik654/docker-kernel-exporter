#ifndef COLLECTING_FUNCS_H
#define COLLECTING_FUNCS_H
#include "additional_types.h"
#include "filesystem_funcs.h"

bool update_containers_list(std::string base_path, std::vector<std::string> *list);

MemInfoData get_host_meminfo_data(std::string filepath, bool just_total = false);
HostCPUStats get_host_cpu_data(std::string filepath);

ContainerDockerdData get_container_json_data(std::string filepath);
Cgroup2StatsData get_container_cgroup_data(std::string filepath);

NetworkStatsData get_process_network_data(std::string filepath, size_t pid_, std::string if_name); // any proccess

#endif