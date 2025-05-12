#ifndef ADDITIONAL_FUNCS_H
#define ADDITIONAL_FUNCS_H
#include "additional_types.h"

config_data upload_config_data(std::string filepath = "./kernel-de.conf");

bool output_metrics(std::string content, std::string filepath);
bool update_containers_list(std::string base_path, std::vector<std::string> *list);

#endif