#include "metric_calc_funcs.h"

float count_host_cpu_load(HostCPUStats cpu_stats)
{

    size_t total = cpu_stats.cpu_user +
                   cpu_stats.cpu_system +
                   cpu_stats.cpu_nice +
                   cpu_stats.cpu_softirq +
                   cpu_stats.cpu_steal +
                   cpu_stats.cpu_irq +
                   cpu_stats.cpu_idle +
                   cpu_stats.cpu_iowait +
                   cpu_stats.cpu_guest +
                   cpu_stats.cpu_guest_nice;

    return ((float)(cpu_stats.cpu_user + cpu_stats.cpu_system + cpu_stats.cpu_nice)) /
           ((float)total) *
           100.0f;
}

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
