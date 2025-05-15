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

std::string get_container_stats_fields(ContainerStatsData c_data, std::string c_id)
{
    std::string total_fields{""};

    printf("\n\nContainer %s\nNAME: %s\n", c_id.c_str(), c_data.json_stats.name.c_str());
    printf("Health data: %s\n", c_data.json_stats.health_status.c_str());
    
    printf("RX bytes: %d\n", c_data.net_stats.rx_bytes);
    printf("TX bytes: %d\n", c_data.net_stats.tx_bytes);
    printf("Memory current: %s\n", c_data.resource_stats.mem_stats.memory_current);
    printf("CPU system usec: %s\n", c_data.resource_stats.cpu_stats.cpu_system_usec);
    printf("IO wb: %s\n", c_data.resource_stats.io_stats.io_wbytes);
    printf("First PID from list: %s\n", c_data.resource_stats.pid_list[0]);

    if (c_data.json_stats.is_running)
    {
        printf("Container is running\n\n");
    }

    return total_fields;
}
