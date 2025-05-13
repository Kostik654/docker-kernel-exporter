#ifndef COLLECTOR_H
#define COLLECTOR_H
#include "additional_funcs.h"
#include "metric_calc_funcs.h"

class Collector
{

private:
    const std::string collector_version_info{"Collector version 1.0.0, using cgroups2"};

    const std::string cgroup_base_path{"/sys/fs/cgroup/"};
    const std::string proc_base_path{"/proc/"};
    const std::string net_dev_file_rel_path{"/net/dev"};
    const std::string host_meminfo_file_path{"/proc/meminfo"};
    const std::string host_cpu_stats_file_path{"/proc/stat"};

    config_data cfg_data;

    StaticHostData static_host_data;

    std::vector<std::string> actual_containers_list;

public:
    static bool exit_flag;
    static unsigned int exit_code;

    Collector(config_data cfg);

    Cgroup2StatsData collect_host_data(); // host dynamic stats


    NetworkStatsData collect_process_network_data(size_t pid_, std::string if_name); // any proccess
    Cgroup2StatsData collect_cgroup2_data(std::string base_path); // host OR container
    ContainerDockerdData collect_container_data(std::string cfid_); // from dockerd jsons

    std::string get_container_dockerd_full_path(std::string cfid_);
    std::string get_container_cgroup2_full_path(std::string cfid_);
    std::string get_pid_netdev_full_path(std::string cfid_);

    float get_container_cpu_usage();

    void startCollecting();
    bool check_paths();
    bool set_static_host_info(StaticHostData *host_stats);
    void printConfig();
    ~Collector();
};

#endif