#include "collecting_funcs.h"

namespace fs = std::filesystem;

bool update_containers_list(std::string base_path, std::vector<std::string> *list)
{

    if (list == nullptr)
    {
        std::cerr << "NULL ptr argument for containers ids list in func <update_containers_list>" << std::endl;
        return false;
    }

    list->clear();

    try
    {
        for (const auto &entry : fs::directory_iterator(base_path))
            if (entry.is_directory())
            {
                list->push_back(entry.path().filename().string());
            }
        return true;
    }
    catch (const std::exception &err)
    {
        std::cerr << "Getting folders list in the directory [" << base_path << "] error occurred: " << err.what() << std::endl;
        return false;
    }
}

// unsigned int get_meminfo_value(std::string line, std::string var_name)
// {
//     size_t start = line.find_first_of(num_arr);
//     size_t line_l = line.length();
//     const int postfix_length = 3; // < kB>

//     if (start != std::string::npos && line_l - postfix_length >= start)
//     {
//         return std::stol(line.substr(start, line_l - 3));
//     }
//     else
//         throw std::runtime_error("unable to find value for " + var_name);
// }

MemInfoData get_host_meminfo_data(std::string filepath, bool just_total)
{
    MemInfoData mem_data;

    bool total_initialized = false;
    bool free_initialized = false;
    bool avail_initialized = false;

    std::ifstream meminfo_file(filepath);
    std::string line;

    try
    {
        if (meminfo_file.is_open())
        {

            while (std::getline(meminfo_file, line))
            {
                std::istringstream stream(line);
                std::string val_;

                stream >> val_;

                if (val_ == "MemTotal:")
                {
                    stream >> val_;
                    mem_data.mem_total_kB = std::stoi(val_);
                    total_initialized = true;

                    if (just_total)
                        break;
                    else
                        continue;
                }
                if (val_ == "MemFree:" && !just_total)
                {
                    stream >> val_;
                    mem_data.mem_free_kB = std::stoi(val_);
                    free_initialized = true;
                    continue;
                }
                if (val_ == "MemAvailable:" && !just_total)
                {
                    stream >> val_;
                    mem_data.mem_avail_kB = std::stoi(val_);
                    avail_initialized = true;
                    continue;
                }

                if (total_initialized && free_initialized && avail_initialized)
                    break;
            }

            meminfo_file.close();

            if (!(total_initialized && free_initialized && avail_initialized) && !(just_total && total_initialized))
                throw std::runtime_error("one or more parameters was not foud!");
        }
        else
            throw std::runtime_error("unable to open file");
    }
    catch (const std::exception &err)
    {
        std::cerr << "Getting memory info [" << filepath << "] error occurred: " << err.what() << std::endl;
        if (meminfo_file.is_open())
            meminfo_file.close();
        exit(3);
    }

    return mem_data;
}

HostCPUStats get_host_cpu_data(std::string filepath)
{
    HostCPUStats host_stats;

    std::ifstream cpu_file(filepath);
    std::string line;
    bool is_first_line = true;

    bool stats_initialized = false;
    bool procs_initialized = false;
    bool rprocs_initialized = false;

    try
    {
        if (cpu_file.is_open())
        {
            // cpu  289596 603 78161 4257920 11095 0 839 0 0 0
            while (std::getline(cpu_file, line))
            {

                std::istringstream stream(line);
                std::string val_;

                stream >> val_;

                if (val_ == "cpu" && is_first_line)
                {
                    is_first_line = false;

                    // access template
                    unsigned int HostCPUStats::*cpu_stat_fields[] = {
                        &HostCPUStats::cpu_user,
                        &HostCPUStats::cpu_nice,
                        &HostCPUStats::cpu_system,
                        &HostCPUStats::cpu_idle,
                        &HostCPUStats::cpu_iowait,
                        &HostCPUStats::cpu_irq,
                        &HostCPUStats::cpu_softirq,
                        &HostCPUStats::cpu_steal,
                        &HostCPUStats::cpu_guest,
                        &HostCPUStats::cpu_guest_nice};

                    // auto == unsigned int HostCPUStats::*
                    for (auto field : cpu_stat_fields)
                    {
                        if (!(stream >> val_))
                            break;
                        else
                            host_stats.*field = std::stoi(val_);
                    };

                    stats_initialized = true;
                    continue;
                }
                if (val_ == "processes")
                {
                    stream >> val_;
                    host_stats.processes_total = std::stoi(val_);
                    procs_initialized = true;
                    continue;
                }
                if (val_ == "procs_running")
                {
                    stream >> val_;
                    host_stats.processes_running = std::stoi(val_);
                    rprocs_initialized = true;
                    continue;
                }
                if (procs_initialized && rprocs_initialized && stats_initialized)
                    break;
            }

            cpu_file.close();

            if (!(procs_initialized && rprocs_initialized && stats_initialized))
                throw std::runtime_error("not all stats were found");
            else
                return host_stats;
        }
        else
            throw std::runtime_error("unable to open file");
    }
    catch (const std::exception &err)
    {
        std::cerr << "Reading cpu stats file [" << filepath.c_str() << "] error occurred: " << err.what() << std::endl;
        cpu_file.close();
        exit(3);
    }

    return host_stats;
}

ContainerDockerdData get_container_json_data(std::string filepath)
{
    ContainerDockerdData c_js_stats;

    std::ifstream json_file(filepath);
    std::string line;

    std::regex pid_look_temp(R"("Pid":(\d+))");
    std::regex name_look_temp(R"xxx("Name":"([^"]+)")xxx");
    std::regex health_look_temp(R"xxx("Health"\s*:\s*\{[^{]*"Status"\s*:\s*"([^"]+))xxx");
    std::smatch match;

    try
    {
        if (json_file.is_open())
        {
            // raw json line
            getline(json_file, line);

            // Name parsing
            if (std::regex_search(line, match, name_look_temp))
                c_js_stats.name = std::string(match[1]).substr(1); // Name value without < / > char
            else
                throw std::runtime_error("<Name> was not found");

            // Pid parsing
            if (std::regex_search(line, match, pid_look_temp))
                c_js_stats.main_pid = std::stoi(match[1]);
            else
                throw std::runtime_error("<Pid> was not found");

            // Running status parsing
            if (line.find("{\"Running\":true,") != std::string::npos)
                c_js_stats.is_running = true;
            else if (line.find("{\"Running\":false,") != std::string::npos)
                c_js_stats.is_running = false;
            else
                throw std::runtime_error("<Running> was not found");

            // Health status parsing if exists
            // if (line.find("\"Health\":null") != std::string::npos)
            //     c_js_stats.health_status = "null";
            // else
            if (std::regex_search(line, match, health_look_temp))
                c_js_stats.health_status = match[1];
            else
                c_js_stats.health_status = "null";
            // throw std::runtime_error("<Health> was not found");
        }
        else
            throw std::runtime_error("unable to open file");
    }
    catch (const std::exception &err)
    {
        std::cerr << "Reading container json file [" << filepath.c_str() << "] error occurred: " << err.what() << std::endl;
        json_file.close();
        exit(3);
    }
    return c_js_stats;
}

ContainerCPUStats get_container_cpu_stats(std::string filepath)
{
    ContainerCPUStats c_cpu_stats;
    std::ifstream cpu_file(filepath);
    std::string line;
    std::string val_;
    // access template
    unsigned int ContainerCPUStats::*cpu_stat_fields[] = {
        &ContainerCPUStats::cpu_usage_usec,
        &ContainerCPUStats::cpu_user_usec,
        &ContainerCPUStats::cpu_system_usec,
        &ContainerCPUStats::nice_usec,
        &ContainerCPUStats::core_sched_force_idle_usec,
        &ContainerCPUStats::nr_periods,
        &ContainerCPUStats::nr_throttled,
        &ContainerCPUStats::throttled_usec,
        &ContainerCPUStats::nr_bursts,
        &ContainerCPUStats::burst_usec};

    try
    {
        if (cpu_file.is_open())
        {
            // auto == unsigned int HostCPUStats::*
            for (auto field : cpu_stat_fields)
            {
                getline(cpu_file, line);
                std::istringstream stream(line);

                stream >> val_; // skip var name

                stream >> val_; // get var value

                c_cpu_stats.*field = std::stoi(val_); // must be one to one
            };
        }
        else
            throw std::runtime_error("unable to open file");
    }
    catch (const std::exception &err)
    {
        std::cerr << "Reading container cpu.stat file [" << filepath.c_str() << "] error occurred: " << err.what() << std::endl;
        cpu_file.close();
        exit(3);
    }

    return c_cpu_stats;
};

ContainerMemoryStats get_container_mem_stats(std::string filepath)
{
    ContainerMemoryStats c_mem_stats;

    std::string line_current = read_file_line(filepath + "memory.current");
    std::string line_swap = read_file_line(filepath + "memory.swap.current");

    c_mem_stats.memory_current = std::stoi(line_current);
    c_mem_stats.memory_swap_current = std::stoi(line_swap);

    return c_mem_stats;
};

ContainerIOStats get_container_io_stats(std::string filepath)
{
    ContainerIOStats c_io_stats;

    std::string line_io = read_file_line(filepath);
    std::istringstream stream(line_io);
    std::string val_;

    stream >> val_; // skip dev id

    stream >> val_; // rbytes=1234
    c_io_stats.io_rbytes = val_.substr(val_.find('=') + 1);

    stream >> val_; // wbytes=1234
    c_io_stats.io_wbytes = val_.substr(val_.find('=') + 1);

    return c_io_stats;
};

Cgroup2StatsData get_container_cgroup_data(std::string filepath, unsigned int cpu_int)
{
    Cgroup2StatsData c_cg2_stats;
    ContainerCPUStats cpu_a, cpu_b;

    cpu_a = get_container_cpu_stats(filepath + "cpu.stat");
    std::this_thread::sleep_for(std::chrono::milliseconds(cpu_int));
    cpu_b = get_container_cpu_stats(filepath + "cpu.stat");
    c_cg2_stats.cpu_stats_delta = return_container_cpu_delta(cpu_a, cpu_b);


    c_cg2_stats.mem_stats = get_container_mem_stats(filepath);
    c_cg2_stats.io_stats = get_container_io_stats(filepath + "io.stat");
    c_cg2_stats.pid_list = read_file_lines(filepath + "cgroup.procs");

    return c_cg2_stats;
};

NetworkStatsData get_process_network_data(std::string filepath)
{
    NetworkStatsData c_net_stats{0, 0};

    std::ifstream dev_file(filepath);
    std::string line;
    std::string val_;

    try
    {
        if (dev_file.is_open())
        {
            getline(dev_file, line); // skip headers
            getline(dev_file, line); // skip headers
            getline(dev_file, line); // skip loopback iface

            while (getline(dev_file, line)) // ifaces lines
            {
                //printf("%s dev %s", filepath.c_str(), line.substr(0, 6).c_str());

                line = line.substr(line.find(':') + 2); // skip iface name
                std::istringstream stream(line);
                stream >> val_; // bytes received

                //printf(" received %s", val_.c_str());

                c_net_stats.rx_bytes += std::stoi(val_);
                for (int i = 0; i < 7; i++)
                    stream >> val_; // skip info from 7 columns
                stream >> val_;     // bytes transmitted
                //printf(" trans %s\n", val_.c_str());
                c_net_stats.tx_bytes += std::stoi(val_);
            }
        }
        else
            throw std::runtime_error("unable to open file");
        dev_file.close();
    }
    catch (const std::exception &err)
    {
        std::cerr << "Reading net dev lines [" << filepath.c_str() << "] error occurred: " << err.what() << std::endl;
        if (dev_file.is_open())
            dev_file.close();
        exit(305);
    }

    return c_net_stats;
};

NetworkStatsData get_processes_sum_network_data(std::vector<std::string> pid_list)
{
    NetworkStatsData c_net_stats_sum{0, 0};

    for (std::string pid_ : pid_list)
    {
        NetworkStatsData pid_stats = get_process_network_data(get_pid_netdev_full_path(pid_));
        c_net_stats_sum.rx_bytes += pid_stats.rx_bytes;
        c_net_stats_sum.tx_bytes += pid_stats.tx_bytes;
        // std::cout << "Pid Total rx = " << c_net_stats_sum.rx_bytes << " tx" << c_net_stats_sum.tx_bytes << std::endl;
    }

    return c_net_stats_sum;
};

std::string get_pid_netdev_full_path(std::string pid_)
{
    ConstPaths cp;
    return cp.folders.proc_base_path + pid_ + cp.files.net_dev_file_rel_path;
};
