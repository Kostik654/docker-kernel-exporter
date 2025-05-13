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

// configuration file
struct config_data
{
    int scrape_period = 3;
    std::string default_dockerd_base_path{"/var/lib/docker/containers/"};
    std::string default_metrics_file{"./metrics.data"};
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
    std::string run_state;
    std::string command;
    std::string health_status;
};

// container dynamic stats
struct Cgroup2StatsData
{
    size_t cpu_usage_usec;
    size_t cpu_user_usec;
    size_t cpu_system_usec;

    size_t memory_current;

    size_t io_rbytes;
    size_t io_wbytes;

    std::vector<size_t> pid_list;
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
};

// host dynamic stats
struct HostStatsData
{
    MemInfoData memory;
    HostCPUStats cpu;
};

#endif