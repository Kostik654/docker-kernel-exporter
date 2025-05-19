#include "metric_form_funcs.h"

std::string get_host_static_metric_fields(StaticHostData host_data)
{
    std::ostringstream total_fields;

    MetricArgs h_memory, h_vcpuc; // gauge

    std::string obj_type = {"{object=\"host\", stat_type=\"static\"}"};
    h_memory.label_substr = obj_type;
    h_vcpuc.label_substr = obj_type;

    h_memory.m_name = {"host_total_memory"};
    h_vcpuc.m_name = {"host_vcpu_count"};

    h_memory.m_description = {"Total Host RAM in kB"};
    h_vcpuc.m_description = {"Total Host VCPUs number"};

    h_memory.m_value = {std::to_string(host_data.memory_max)};
    h_vcpuc.m_value = {std::to_string(host_data.vcpus_count)};

    total_fields << get_stat_metric_field(h_memory);
    total_fields << get_stat_metric_field(h_vcpuc);

    return total_fields.str();
};

std::string get_stat_metric_field(MetricArgs base_args)
{
    std::ostringstream oss;
    oss << "# HELP " << base_args.get_m_name() << " " << base_args.m_description << std::endl;
    oss << "# TYPE " << base_args.get_m_name() << " " << base_args.m_unit << std::endl;
    oss << base_args.get_m_name() << base_args.label_substr << " " << base_args.m_value << std::endl;
    return oss.str();
};

std::string get_host_stats_fields(HostStatsData host_data)
{
    std::ostringstream total_fields;

    MetricArgs h_cpu_usage, h_procs_total, h_procs_run; // gauge
    MetricArgs h_memory_avail, h_memory_free; // gauge

    std::string obj_type = {"{object=\"host\", stat_type=\"dynamic\"}"};
    h_cpu_usage.label_substr = obj_type;
    h_procs_total.label_substr = obj_type;
    h_procs_run.label_substr = obj_type;
    h_memory_avail.label_substr = obj_type;
    h_memory_free.label_substr = obj_type;

    h_cpu_usage.m_name = {"host_cpu_usage"};
    h_procs_total.m_name = {"host_total_procs"};
    h_procs_run.m_name = {"host_running_procs"};
    h_memory_avail.m_name = {"host_memory_avail"};
    h_memory_free.m_name = {"host_memory_free"};

    h_cpu_usage.m_description = {"Host CPU usage in percents"};
    h_procs_total.m_description = {"Host total processes number"};
    h_procs_run.m_description = {"Host running processes number"};
    h_memory_avail.m_description = {"Host available memory in kB"};
    h_memory_free.m_description = {"Host free memory in kB"};

    h_cpu_usage.m_value = std::to_string(count_host_cpu_load(host_data.cpu_delta));
    h_procs_total.m_value = std::to_string(host_data.cpu_delta.processes_total);
    h_procs_run.m_value = std::to_string(host_data.cpu_delta.processes_running);
    h_memory_avail.m_value = std::to_string(host_data.memory.mem_avail_kB);
    h_memory_free.m_value = std::to_string(host_data.memory.mem_free_kB);

    total_fields << get_stat_metric_field(h_cpu_usage);
    total_fields << get_stat_metric_field(h_procs_run);
    total_fields << get_stat_metric_field(h_procs_total);
    total_fields << get_stat_metric_field(h_memory_avail);
    total_fields << get_stat_metric_field(h_memory_free);

    return total_fields.str();
};

std::string get_container_stats_fields(ContainerStatsData c_data, std::string c_id)
{
    std::ostringstream total_fields;

    // printf("\n\nContainer %s\nNAME: %s\n", c_id.c_str(), c_data.json_stats.name.c_str());
    // printf("Health data: %s\n", c_data.json_stats.health_status.c_str());

    if (c_data.json_stats.is_running)
    {
        // printf("Container is running\n\n");
        // printf("RX bytes: %s\n", std::to_string(c_data.net_stats.rx_bytes).c_str());
        // printf("TX bytes: %s\n", std::to_string(c_data.net_stats.tx_bytes).c_str());
        // printf("Memory current: %s\n", std::to_string(c_data.resource_stats.mem_stats.memory_current).c_str());
        // printf("CPU system usec: %s\n", std::to_string(c_data.resource_stats.cpu_stats.cpu_system_usec).c_str());
        // printf("IO wb: %s\n", c_data.resource_stats.io_stats.io_wbytes.c_str());
        // printf("First PID from list: %s\n", c_data.resource_stats.pid_list[0].c_str());
    }

    return total_fields.str();
};
