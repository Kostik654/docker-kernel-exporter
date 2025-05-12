#ifndef ADDITIONAL_FUNCS_H
#define ADDITIONAL_FUNCS_H
#include <vector>
#include <string>

struct config_data
{
    int scrape_period = 3;
    std::string default_dockerd_base_path{"/var/lib/docker/containers/"};
    std::string default_metrics_file{"./metrics.data"};
};

config_data upload_config_data(std::string filepath = "./kernel-de.conf");

bool output_metrics(std::string content, std::string filepath);
void update_containers_list(std::string base_path, std::vector<std::string> *list);

#endif