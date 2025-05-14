#include "collecting_funcs.h"
#include "additional_types.h"

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
                if (line.find("cpu ") == 0 && is_first_line)
                {
                    is_first_line = false;
                    line = line.substr(line.find_first_of(num_arr));

                    std::istringstream stream(line);
                    std::string val_;

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
                if (line.find("processes ") == 0)
                {
                    host_stats.processes_total = std::stoi(line.substr(line.find_first_of(num_arr)));
                    procs_initialized = true;
                    continue;
                }
                if (line.find("procs_running ") == 0)
                {
                    host_stats.processes_running = std::stoi(line.substr(line.find_first_of(num_arr)));
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
