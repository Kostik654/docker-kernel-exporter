#ifndef COLLECTOR_H
#define COLLECTOR_H
#include "additional_funcs.h"
#include <string>
#include <vector>

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
    Collector(config_data cfg);

    struct ContainerData
    {
    
    };

    // host OR container
    struct Cgroup2Data
    {
        size_t cpu_usage_usec;
        size_t cpu_user_usec;
        size_t cpu_system_usec;

        size_t memory_current;

        size_t io_rbytes;
        size_t io_wbytes;

        std::vector<size_t> pid_list;
    };

    // one process
    struct NetworkData
    {
        size_t rx_bytes;
        size_t tx_bytes;
    };

    // just host
    struct HostData
    {
        size_t cores_count;
        size_t memory_max;
        Cgroup2Data cgroup2data;
    };

    HostData collect_host_data();
    NetworkData collect_process_network_data(size_t pid_);
    Cgroup2Data collect_cgroup2_data(std::string base_path); // host OR container
    ContainerData collect_container_data(std::string cfid_);

    std::string get_container_dockerd_full_path(std::string cfid_);
    std::string get_container_cgroup2_full_path(std::string cfid_);
    std::string get_pid_netdev_full_path(std::string cfid_);

    void startCollecting();
    bool check_paths();
    void printConfig();
    ~Collector();
};

#endif