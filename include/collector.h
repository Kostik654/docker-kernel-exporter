#ifndef COLLECTOR_H
#define COLLECTOR_H
#include "filesystem_funcs.h"
#include "metric_form_funcs.h"
#include "collecting_funcs.h"

class Collector
{

private:
    const std::string collector_version_info{"Collector version 1.0.0, using cgroups2"};

    const ConstPaths const_paths;

    config_data cfg_data;

    std::vector<std::string> actual_containers_list;

public:
    static bool exit_flag;
    static unsigned int exit_code;

    explicit Collector(config_data cfg);

    [[nodiscard]] HostStatsData collect_host_data() const; // host dynamic stats
    [[nodiscard]] ContainerStatsData collect_container_data(std::string c_id) const; // container dynamic stats + json static

    [[nodiscard]] std::string get_container_dockerd_full_path(std::string cfid_) const;
    [[nodiscard]] std::string get_container_cgroup2_full_path(std::string cfid_) const;

    void startCollecting();

    bool check_paths();

    bool set_static_host_info(StaticHostData &host_stats) const;

    std::string collected_data;

    std::atomic<bool> lock_data = false;

    void printConfig() const;

    ~Collector();
};

#endif