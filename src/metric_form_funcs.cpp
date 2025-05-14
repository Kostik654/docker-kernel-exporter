#include "metric_form_funcs.h"

std::string get_host_static_metric_fields(StaticHostData host_data)
{
    std::string total_fields{""};

    printf("Hostname: %s\n", host_data.hostname.c_str());

    return total_fields;
}

std::string get_host_stats_fields(HostStatsData host_data)
{
    std::string total_fields{""};

    printf("CPU load: %.2f%%\n", count_host_cpu_load(host_data.cpu));
    printf("CPU user: %d\n", host_data.cpu.cpu_user);
    printf("CPU system: %d\n", host_data.cpu.cpu_system);
    printf("CPU nice: %d\n", host_data.cpu.cpu_nice);
    printf("CPU total processes: %d\n", host_data.cpu.processes_total);
    printf("CPU running processes: %d\n", host_data.cpu.processes_running);

    return total_fields;
}

std::string get_container_stats_fields(ContainerStatsData c_data)
{
    std::string total_fields{""};



    if (c_data.json_stats.is_running) {

    }

    return total_fields;
}
