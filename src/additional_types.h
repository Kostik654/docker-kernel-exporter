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

struct config_data
{
    int scrape_period = 3;
    std::string default_dockerd_base_path{"/var/lib/docker/containers/"};
    std::string default_metrics_file{"./metrics.data"};
};

struct MemInfoData
{
    unsigned int mem_total_kB;
    unsigned int mem_free_kB;
    unsigned int mem_avail_kB;
};

struct MetricField
{
    std::string m_name;
    std::string m_description;
    std::string m_unit;
};

struct ContainerData
{
    size_t main_pid;
    std::string name;
    std::string run_state;
    std::string command;
    std::string health_status;
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
struct StaticHostData
{
    size_t vcpus_count;
    size_t memory_max;
};

#endif