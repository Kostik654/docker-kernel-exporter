#include "collector.h"

namespace fs = std::filesystem;

bool Collector::exit_flag = false;
unsigned int Collector::exit_code = 0;

Collector::Collector(config_data cfg)
{

    if (fs::exists(cfg.default_dockerd_base_path))
    {
        this->cfg_data.default_dockerd_base_path = cfg.default_dockerd_base_path;
    }
    else
    {
        if (cfg_data.default_dockerd_base_path.back() != '/')
            cfg_data.default_dockerd_base_path += '/';

        if (!fs::exists(this->cfg_data.default_dockerd_base_path))
        {
            printf("WARNING: Could not find the dockerd path [%s], the default one path [%s] also does not exist\nThe exit code: 1\n", cfg.default_dockerd_base_path.c_str(), this->cfg_data.default_dockerd_base_path.c_str());
            // exit(1);
        }
        else
        {
            printf("WARNING: Could not find the dockerd path [%s]. Using default dockerd path: %s\n", cfg.default_dockerd_base_path.c_str(), this->cfg_data.default_dockerd_base_path.c_str());
        }
    }

    this->cfg_data.scrape_period = cfg.scrape_period;
    this->cfg_data.default_metrics_file = cfg.default_metrics_file;

    printf("\n== Collector was added ==\n");
}

HostStatsData Collector::collect_host_data()
{
    HostStatsData host_data;

    host_data.memory = get_host_meminfo_data(this->const_paths.files.host_meminfo_file_path);

    HostCPUStats cpu_a, cpu_b;
    cpu_a = get_host_cpu_data(this->const_paths.files.host_cpu_stats_file_path);
    std::this_thread::sleep_for(std::chrono::milliseconds(this->cfg_data.h_cpu_int));
    cpu_b = get_host_cpu_data(this->const_paths.files.host_cpu_stats_file_path);
    host_data.cpu_delta = return_host_cpu_delta(cpu_a, cpu_b);

    return host_data;
}

ContainerStatsData Collector::collect_container_data(std::string c_id)
{
    ContainerStatsData c_total_data;

    c_total_data.json_stats = get_container_json_data(get_container_dockerd_full_path(c_id) + this->const_paths.files.json_config_filename);

    if (c_total_data.json_stats.is_running)
    {
        c_total_data.resource_stats = get_container_cgroup_data(get_container_cgroup2_full_path(c_id), this->cfg_data.c_cpu_int);
        c_total_data.net_stats = get_processes_sum_network_data(c_total_data.resource_stats.pid_list);
    }

    return c_total_data;
}

void Collector::printConfig()
{
    printf("\n== Collector configuration BEGIN ==\n");
    printf("Dockerd path: %s\n", this->cfg_data.default_dockerd_base_path.c_str());
    printf("cgroupv2 path: %s\n", this->const_paths.folders.cgroup_base_path.c_str());
    printf("Metrics file path: %s\n", this->cfg_data.default_metrics_file.c_str());
    printf("Network stats path: %s<container-internal-proc-id>%s\n", this->const_paths.folders.proc_base_path.c_str(), this->const_paths.files.net_dev_file_rel_path.c_str());
    printf("Scrape period in seconds: %d\n", this->cfg_data.scrape_period);
    printf("== Collector configuration END ==\n");
};

bool Collector::check_paths()
{

    std::cout << "\nChecking paths...\n"
              << std::endl;

    // Directories revision
    if (this->cfg_data.default_dockerd_base_path.back() != '/')
        this->cfg_data.default_dockerd_base_path += '/';

    if (!(check_object_path(this->cfg_data.default_dockerd_base_path, "Dockerd", true) &&
          check_object_path(this->const_paths.folders.cgroup_base_path, "cgroup2 base path", true) &&
          check_object_path(this->const_paths.folders.proc_base_path, "proc base path", true)))
        return false;

    // Files revision
    if (!(check_object_path(this->const_paths.files.host_meminfo_file_path, "MemInfo file path", false) &&
          check_object_path(this->const_paths.files.host_cpu_stats_file_path, "CPU stat file path", false)))
        return false;

    // Check base path of future files
    std::string metricsfile_base = fs::path(this->cfg_data.default_metrics_file).parent_path().string();
    if (!(check_object_path(metricsfile_base, "Metrics file base", true)))
        return false;

    std::cout << "== All the paths are OK ==" << std::endl;

    return true;
};

bool Collector::set_static_host_info(StaticHostData *host_stats)
{
    try
    {
        // get vcpus count
        host_stats->vcpus_count = std::thread::hardware_concurrency();

        // get JUST total memory
        host_stats->memory_max = get_host_meminfo_data(this->const_paths.files.host_meminfo_file_path, true).mem_total_kB;

        // get hostname
        char hostname[HOST_NAME_MAX + 1];
        gethostname(hostname, HOST_NAME_MAX + 1);
        host_stats->hostname = hostname;

        // print for log revision
        printf("\n== Static host [%s] data is collected ==\n", host_stats->hostname.c_str());
        printf(">> VCPUs: %ld\n", host_stats->vcpus_count);
        printf(">> Total Memory: %ld kB\n", host_stats->memory_max);

        return true;
    }
    catch (const std::exception &err)
    {
        std::cerr << "Error: unable to get the all static host data: " << err.what() << std::endl;
        return false;
    }
};

std::string Collector::get_container_dockerd_full_path(std::string cfid_)
{
    return this->cfg_data.default_dockerd_base_path + cfid_ + "/";
};
std::string Collector::get_container_cgroup2_full_path(std::string cfid_)
{
    return this->const_paths.folders.cgroup_base_path + "system.slice/docker-" + cfid_ + ".scope/";
};

void Collector::startCollecting()
{
    std::string static_data{};

    printf("\n== Starting collector ==\n\n");

    if (!set_static_host_info(&this->static_host_data))
    {
        Collector::exit_code = 100;
        return;
    }

    static_data = get_host_static_metric_fields(this->static_host_data);

    try
    {

        while (!Collector::exit_flag)
        {
            std::ostringstream oss;

            oss << static_data; // start var with static host data

            oss << get_host_stats_fields(collect_host_data());

            if (update_containers_list(this->cfg_data.default_dockerd_base_path, &this->actual_containers_list))
            {
                for (const std::string &container_id : this->actual_containers_list)
                {
                    oss << get_container_stats_fields(collect_container_data(container_id), container_id);
                }
            }
            else
            {
                std::cerr << "Unable to get the actual containers ids list in [" << this->cfg_data.default_dockerd_base_path << "] directory" << std::endl;
                Collector::exit_code = 101;
                break;
            };

            
            this->lock_data = true;
            this->collected_data = oss.str();
            this->lock_data = false;

            std::this_thread::sleep_for(std::chrono::seconds(this->cfg_data.scrape_period));
        };
    }
    catch (const std::exception &err)
    {
        std::cerr << "Collecting data error occurred: " << err.what() << std::endl;
        Collector::exit_code = 103;
        return;
    }
};

Collector::~Collector() {};
