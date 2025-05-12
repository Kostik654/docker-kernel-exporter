#include "collector.h"

namespace fs = std::filesystem;

bool Collector::exit_flag = false;

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
};

void Collector::printConfig()
{
    printf("\n== Collector configuration BEGIN ==\n");
    printf("Dockerd path: %s\n", this->cfg_data.default_dockerd_base_path.c_str());
    printf("cgroupv2 path: %s\n", this->cgroup_base_path.c_str());
    printf("Metrics file path: %s\n", this->cfg_data.default_metrics_file.c_str());
    printf("Network stats path: %s<container-internal-proc-id>%s\n", this->net_dev_stat_base_path.c_str(), this->net_dev_file_rel_path.c_str());
    printf("Scrape period in seconds: %d\n", this->cfg_data.scrape_period);
    printf("== Collector configuration END ==\n");
};

bool Collector::check_paths()
{

    std::cout << "\nChecking paths...\n"
              << std::endl;

    if (!fs::exists(this->cfg_data.default_dockerd_base_path) && fs::is_directory(this->cfg_data.default_dockerd_base_path))
    {
        std::cerr << ">> Dockerd path [" << this->cfg_data.default_dockerd_base_path << "] does not exist" << std::endl;
        return false;
    }
    if (!fs::exists(this->cgroup_base_path) && fs::is_directory(this->cgroup_base_path))
    {
        std::cerr << ">> cgroup2 path [" << this->cgroup_base_path << "] does not exist" << std::endl;
        return false;
    }
    if (!fs::exists(this->net_dev_stat_base_path) && fs::is_directory(this->net_dev_stat_base_path))
    {
        std::cerr << ">> /proc path [" << this->net_dev_stat_base_path << "] does not exist" << std::endl;
        return false;
    }

    // Check base path of future file
    fs::path metricsfile_base = fs::path(this->cfg_data.default_metrics_file).parent_path();
    if (!fs::exists(metricsfile_base) && fs::is_directory(metricsfile_base))
    {
        std::cerr << ">> Metrics file base path [" << metricsfile_base << "] does not exist" << std::endl;
        return false;
    }

    std::cout << "== All the paths are OK ==" << std::endl;

    return true;
};

std::string Collector::get_container_dockerd_full_path(std::string cfid_)
{
    return this->cfg_data.default_dockerd_base_path + cfid_ + "/";
};
std::string Collector::get_container_cgroup2_full_path(std::string cfid_)
{
    return this->cgroup_base_path + "docker-" + cfid_ + ".scope/";
};
std::string Collector::get_pid_netdev_full_path(std::string cfid_)
{
    return this->net_dev_stat_base_path + cfid_ + this->net_dev_file_rel_path;
};

void Collector::startCollecting()
{
    std::string total_data{""};

    printf("\n== Starting collector ==\n\n");

    while (!Collector::exit_flag)
    {

        if (update_containers_list(this->cfg_data.default_dockerd_base_path, &this->actual_containers_list))
        {

            // std::cout << "GOT IT: " << this->actual_containers_list.size() << std::endl;

        }
        else
        {
            std::cerr << "Unable to get the actual containers ids list in [" << this->cfg_data.default_dockerd_base_path << "] directory" << std::endl;
            break;
        }

        if (!output_metrics(total_data, this->cfg_data.default_metrics_file))
        {
            printf("Collecting was failed\n");
            break;
        }

        std::this_thread::sleep_for(std::chrono::seconds(this->cfg_data.scrape_period));
    };
};

Collector::~Collector() {};
