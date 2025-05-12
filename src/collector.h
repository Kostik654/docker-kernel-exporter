#ifndef COLLECTOR_H
#define COLLECTOR_H
#include "additional_funcs.h"

class Collector
{

private:
    const std::string collector_version_info{"Collector version 1.0.0, using cgroups2"};

    const std::string cgroup_base_path{"/sys/fs/cgroup/system.slice/"};
    const std::string net_dev_stat_base_path{"/proc/"};
    const std::string net_dev_file_rel_path{"/net/dev"};

    config_data cfg_data;

    std::vector<std::string> actual_containers_list;

public:

    static bool exit_flag;

    Collector(config_data cfg);

    HostData collect_host_data();
    NetworkData collect_process_network_data(size_t pid_);
    Cgroup2Data collect_cgroup2_data(std::string base_path); // host OR container
    ContainerData collect_container_data(std::string cfid_);

    std::string get_container_dockerd_full_path(std::string cfid_);
    std::string get_container_cgroup2_full_path(std::string cfid_);
    std::string get_pid_netdev_full_path(std::string cfid_);

    std::string get_container_metric_field(std::string m_name, std::string m_desc, std::string m_unit, std::string c_name, std::string c_id, std::string c_state);

    float get_container_cpu_usage();

    void startCollecting();
    bool check_paths();
    void printConfig();
    ~Collector();
};

#endif