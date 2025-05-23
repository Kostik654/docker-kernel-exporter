#ifndef FILESYSTEM_FUNCS_H
#define FILESYSTEM_FUNCS_H
#include "additional_types.h"

config_data upload_config_data(std::string filepath = "./kernel-de.conf");

// bool output_metrics(std::string content, std::string filepath);
[[nodiscard]] bool check_object_path(std::string obj_path, std::string obj_name, bool is_dir); // check if file or dir exists

[[nodiscard]] std::string read_file_line(std::string filepath); // just one line
[[nodiscard]] std::vector<std::string> read_file_lines(std::string filepath);

#endif