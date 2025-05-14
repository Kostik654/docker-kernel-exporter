#include "additional_funcs.h"

namespace fs = std::filesystem;

bool check_object_path(std::string obj_path, std::string obj_name, bool is_dir)
{
    if (is_dir)
    {
        if (!(fs::exists(obj_path) && fs::is_directory(obj_path)))
        {
            std::cerr << ">> Directory [" << obj_path << "] of object [" << obj_name << "] does not exist" << std::endl;
            return false;
        }
    }
    else
    {
        if (!(fs::exists(obj_path) && fs::is_regular_file(obj_path)))
        {
            std::cerr << ">> File [" << obj_path << "] of object [" << obj_name << "] does not exist" << std::endl;
            return false;
        }
    }

    return true;
};

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

MemInfoData get_meminfo_data(std::string filepath, bool just_total)
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

                    unsigned int HostCPUStats::*fields[] = {
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

                    for (unsigned int HostCPUStats::* field : fields)
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

bool output_metrics(std::string content, std::string filepath)
{

    try
    {
        std::ofstream out(filepath);
        out << content;
        out.close();
        return true;
    }
    catch (const std::exception &err)
    {
        std::cerr << "Writing metrics to file [" << filepath << "] error occurred: " << err.what() << std::endl;
        return false;
    }
}

config_data upload_config_data(std::string filepath)
{
    config_data cfg;

    std::ifstream conf_file(filepath);
    std::string line;
    std::vector<std::string> lines;

    bool scrapeperiod_initialized = false;
    bool metricsfile_initialized = false;
    bool dockerdpath_initialized = false;

    if (!fs::exists(filepath))
    {
        printf("Configuration file [%s] does not exists. Using default values.\n", filepath.c_str());
        return cfg;
    }
    else
        try
        {
            printf("\nUploading configuration from file: %s\n", filepath.c_str());

            if (conf_file.is_open())
            {
                while (std::getline(conf_file, line))
                {

                    std::string clean_line{""};

                    // delete all spaces
                    for (char c : line)
                    {
                        if (!std::isspace(static_cast<unsigned char>(c)))
                            clean_line = clean_line + c;
                    }

                    if (clean_line.empty() || clean_line[0] == '#')
                        continue;

                    // returns npos if char # was not found
                    size_t comment_pos = clean_line.find('#');
                    if (comment_pos != std::string::npos)
                        clean_line = clean_line.substr(0, comment_pos);

                    size_t eq_pos = clean_line.find('=');
                    if (eq_pos == std::string::npos)
                    {
                        std::cerr << "Syntax error, '=' was not found in line: " << line << std::endl;
                        exit(2);
                    }
                    else
                    {

                        if (clean_line.length() < 2 || clean_line[0] == '=')
                        {
                            std::cerr << "Error: line [" << line << "] is empty for parsing" << std::endl;
                            exit(2);
                        }

                        std::string param_name = clean_line.substr(0, eq_pos);
                        std::string value_ = clean_line.substr(eq_pos + 1);

                        if (param_name == "scrape_period")
                        {
                            cfg.scrape_period = std::stoi(value_);
                            scrapeperiod_initialized = true;
                        }
                        else if (param_name == "metrics_file")
                        {
                            cfg.default_metrics_file = value_;
                            metricsfile_initialized = true;
                        }
                        else if (param_name == "dockerd_path")
                        {
                            cfg.default_dockerd_base_path = value_;
                            dockerdpath_initialized = true;
                        }
                        else
                        {
                            std::cerr << "Error: param [" << param_name << "] does not exist" << std::endl;
                            exit(2);
                        }

                        printf(">> uploader: %s = %s\n", param_name.c_str(), value_.c_str());

                        if (metricsfile_initialized && dockerdpath_initialized && scrapeperiod_initialized)
                        {
                            conf_file.close();
                            break;
                        }
                    }
                }
            }
            else
            {
                std::cerr << ">> Error: unable to open the configuration file [" << filepath << "].\nThe uninitialized vars will be set to defaults.\n";
                return config_data();
            }

            if (metricsfile_initialized && dockerdpath_initialized && scrapeperiod_initialized)
                printf("\n== All the configuration variables were updated successfully ==\n");
            else
            {
                printf("\nWARNING: Not all the configuration variables were updated successfully.\n The uninitialized vars will be set to defaults.\n");
                conf_file.close();
            }
            return cfg;
        }
        catch (const std::exception &err)
        {
            std::cerr << "Reading configuration file [" << filepath.c_str() << "] error occurred: " << err.what() << std::endl;
            printf("\n== Not all the configuration variables were updated successfully. Uninitialized vars will be set to defaults. ==\n");
            conf_file.close();
            // returns default values
            return config_data();
        }
}
