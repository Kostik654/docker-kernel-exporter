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
                list->insert(list->end(), entry.path().filename().string());
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
                //throw std::runtime_error("<Health> was not found");
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

Cgroup2StatsData get_container_cgroup_data(std::string filepath)
{
    Cgroup2StatsData c_cg2_stats;

    return c_cg2_stats;
}

NetworkStatsData get_process_network_data(std::string filepath, size_t pid_, std::string if_name)
{
    NetworkStatsData c_net_stats;

    return c_net_stats;
}
