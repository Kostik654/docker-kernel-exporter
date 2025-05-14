#ifndef FILESYSTEM_FUNCS_H
#define FILESYSTEM_FUNCS_H
#include "additional_types.h"

config_data upload_config_data(std::string filepath = "./kernel-de.conf");

bool output_metrics(std::string content, std::string filepath);
bool check_object_path(std::string obj_path, std::string obj_name, bool is_dir); // check if file or dir exists

#endif