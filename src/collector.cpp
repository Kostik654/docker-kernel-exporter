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

Cgroup2StatsData Collector::collect_host_data()
{
    Cgroup2StatsData host_data;

    // host_data.cgroup2data.

    return host_data;
};

void Collector::printConfig()
{
    printf("\n== Collector configuration BEGIN ==\n");
    printf("Dockerd path: %s\n", this->cfg_data.default_dockerd_base_path.c_str());
    printf("cgroupv2 path: %s\n", this->cgroup_base_path.c_str());
    printf("Metrics file path: %s\n", this->cfg_data.default_metrics_file.c_str());
    printf("Network stats path: %s<container-internal-proc-id>%s\n", this->proc_base_path.c_str(), this->net_dev_file_rel_path.c_str());
    printf("Scrape period in seconds: %d\n", this->cfg_data.scrape_period);
    printf("== Collector configuration END ==\n");
};

bool Collector::check_paths()
{

    std::cout << "\nChecking paths...\n"
              << std::endl;

    // Directories revision
    if (!(check_object_path(this->cfg_data.default_dockerd_base_path, "Dockerd", true) &&
          check_object_path(this->cgroup_base_path, "cgroup2 base path", true) &&
          check_object_path(this->proc_base_path, "proc base path", true)))
        return false;

    // Files revision
    if (!(check_object_path(this->host_meminfo_file_path, "MemInfo file path", false) &&
          check_object_path(this->host_cpu_stats_file_path, "CPU stat file path", false)))
        return false;

    // Check base path of future files
    fs::path metricsfile_base = fs::path(this->cfg_data.default_metrics_file).parent_path();
    if (!(check_object_path(metricsfile_base, "Metrics file base", true)))
        return false;

    std::cout << "== All the paths are OK ==" << std::endl;

    return true;
};

bool Collector::set_static_host_info(StaticHostData *host_stats)
{
    try
    {
        host_stats->vcpus_count = std::thread::hardware_concurrency();
        host_stats->memory_max = get_meminfo_data(this->host_meminfo_file_path).mem_total_kB;

        printf("\n== Static host data is collected ==\n");
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
    return this->cgroup_base_path + "system.slice/docker-" + cfid_ + ".scope/";
};
std::string Collector::get_pid_netdev_full_path(std::string cfid_)
{
    return this->proc_base_path + cfid_ + this->net_dev_file_rel_path;
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

    static_data = get_host_static_metric_field(this->static_host_data);

    try
    {

        while (!Collector::exit_flag)
        {

            std::string total_data{static_data};

            // total_data += get_host_cpu_stats_metric_field();
            // total_data += get_host_memory_stats_metric_field();

            if (update_containers_list(this->cfg_data.default_dockerd_base_path, &this->actual_containers_list))
            {

                // std::cout << "GOT IT: " << this->actual_containers_list.size() << std::endl;

                for (const std::string &container_id : this->actual_containers_list)
                {
                    std::string c_cgroup2path = get_container_cgroup2_full_path(container_id);
                    std::string c_dockerdpath = get_container_dockerd_full_path(container_id);
                }
            }
            else
            {
                std::cerr << "Unable to get the actual containers ids list in [" << this->cfg_data.default_dockerd_base_path << "] directory" << std::endl;
                Collector::exit_code = 101;
                break;
            }

            if (!output_metrics(total_data, this->cfg_data.default_metrics_file))
            {
                printf("Collecting was failed\n");
                Collector::exit_code = 102;
                break;
            }

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
