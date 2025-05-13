#ifndef ADDITIONAL_FUNCS_H
#define ADDITIONAL_FUNCS_H
#include "additional_types.h"

config_data upload_config_data(std::string filepath = "./kernel-de.conf");

bool output_metrics(std::string content, std::string filepath);
bool update_containers_list(std::string base_path, std::vector<std::string> *list);
bool check_object_path(std::string obj_path, std::string obj_name, bool is_dir); // check if file or dir exists

unsigned int get_meminfo_value(std::string line, std::string var_name);

MemInfoData get_meminfo_data(std::string filepath, bool just_total = false);
HostCPUStats get_host_cpu_data(std::string filepath);

float get_container_cpu_usage();

#endif