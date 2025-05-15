#ifndef ADDITIONAL_TYPES_H
#define ADDITIONAL_TYPES_H

#include <algorithm>
#include <vector>
#include <string>
#include <cstdio>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <thread>
#include <unistd.h>
#include <limits.h>
#include <sstream>
#include <regex>

// to find numbers
const std::string num_arr{"0123456789"};

// configuration file
struct config_data
{
    int scrape_period = 3;
    std::string default_dockerd_base_path{"/var/lib/docker/containers/"};
    std::string default_metrics_file{"./metrics.data"};
};

// constant paths: files and dirs
struct ConstPaths
{
    // constant directories
    struct ConstFolders
    {

        const std::string cgroup_base_path{"/sys/fs/cgroup/"};
        const std::string proc_base_path{"/proc/"};
    };

    // constant filepaths
    struct ConstFiles
    {

        const std::string host_meminfo_file_path{"/proc/meminfo"};
        const std::string host_cpu_stats_file_path{"/proc/stat"};
        const std::string net_dev_file_rel_path{"/net/dev"};
        const std::string json_config_filename{"config.v2.json"};
    };

    ConstFolders folders;
    ConstFiles files;
};

// container dynamic stats
struct ContainerCPUStats
{
    unsigned int cpu_usage_usec;
    unsigned int cpu_user_usec;
    unsigned int cpu_system_usec;
    unsigned int nice_usec;
    unsigned int core_sched_force_idle_usec;
    unsigned int nr_periods;
    unsigned int nr_throttled;
    unsigned int throttled_usec;
    unsigned int nr_bursts;
    unsigned int burst_usec;
};

// host dynamic stats
struct HostCPUStats
{
    unsigned int cpu_user;
    unsigned int cpu_nice;
    unsigned int cpu_system;
    unsigned int cpu_idle;
    unsigned int cpu_iowait;
    unsigned int cpu_irq;
    unsigned int cpu_softirq;
    unsigned int cpu_steal;
    unsigned int cpu_guest;
    unsigned int cpu_guest_nice;

    unsigned int processes_total;
    unsigned int processes_running;
};

// host dynamic stats + static
struct MemInfoData
{
    unsigned int mem_total_kB;
    unsigned int mem_free_kB;
    unsigned int mem_avail_kB;
};

// common
struct MetricArgs
{
    std::string m_name;
    std::string m_description;
    std::string m_unit;
    std::string m_value;
};

// container dynamic stats + static
struct ContainerDockerdData
{
    size_t main_pid;
    std::string name;
    // std::string command;
    std::string health_status;
    bool is_running;
};

// container dynamic stats
struct ContainerMemoryStats
{
    size_t memory_current;
    size_t memory_swap_current;
};

// container dynamic stats
struct ContainerIOStats
{
    std::string io_rbytes;
    std::string io_wbytes;
};

// container dynamic stats
struct Cgroup2StatsData
{
    ContainerCPUStats cpu_stats;
    ContainerMemoryStats mem_stats;
    ContainerIOStats io_stats;
    std::vector<std::string> pid_list;
};

// one process dynamic stats
struct NetworkStatsData
{
    size_t rx_bytes;
    size_t tx_bytes;
};

// host static info
struct StaticHostData
{
    size_t vcpus_count;
    size_t memory_max;
    std::string hostname; // as label
};

// host dynamic stats
struct HostStatsData
{
    MemInfoData memory;
    HostCPUStats cpu;
};

struct ContainerStatsData
{
    ContainerDockerdData json_stats;
    Cgroup2StatsData resource_stats;
    NetworkStatsData net_stats;
};

#endif