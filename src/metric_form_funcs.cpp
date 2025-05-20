#include "metric_form_funcs.h"

std::string get_host_static_metric_fields(StaticHostData host_data)
{
    std::ostringstream total_fields;

    MetricArgs h_memory, h_vcpuc; // gauge

    std::string obj_type{"{object=\"host\", stat_type=\"static\"}"};
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
    MetricArgs h_memory_avail, h_memory_free;           // gauge

    std::string obj_type{"{object=\"host\", stat_type=\"dynamic\"}"};
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

    h_cpu_usage.m_value = std::to_string(host_data.h_cpu_usage);
    h_procs_total.m_value = std::to_string(host_data.procs_total);
    h_procs_run.m_value = std::to_string(host_data.procs_running);
    h_memory_avail.m_value = std::to_string(host_data.memory.mem_avail_kB);
    h_memory_free.m_value = std::to_string(host_data.memory.mem_free_kB);

    total_fields << get_stat_metric_field(h_cpu_usage);
    total_fields << get_stat_metric_field(h_procs_run);
    total_fields << get_stat_metric_field(h_procs_total);
    total_fields << get_stat_metric_field(h_memory_avail);
    total_fields << get_stat_metric_field(h_memory_free);

    // std::cout << "Host stats collected" << std::endl;

    return total_fields.str();
};

std::string get_container_stats_fields(ContainerStatsData c_data, std::string c_id)
{
    std::ostringstream total_fields;
    std::string c_label;

    std::ostringstream obj_label;

    obj_label << "{object=\"container\", stat_type=\"dynamic\"";
    obj_label << ", c_id=\"" << c_id << "\"";
    obj_label << ", c_name=\"" << c_data.json_stats.name << "\"";
    obj_label << ", is_c_running=\"" << std::to_string(c_data.json_stats.is_running) << "\"";
    obj_label << ", c_health=\"" << c_data.json_stats.health_status << "\"";
    obj_label << ", c_main_pid=\"" << c_data.json_stats.main_pid << "\"}";
    c_label = obj_label.str();

    MetricArgs is_up;

    is_up.label_substr = c_label;
    is_up.m_name = "container_up";
    is_up.m_description = "Is container UP?";
    is_up.m_value = std::to_string(c_data.json_stats.is_running);

    total_fields << get_stat_metric_field(is_up);

    if (c_data.json_stats.is_running)
    {
        MetricArgs c_cpu_usage, c_memory_current_usage, c_memory_swap_usage, io_read, io_write, net_rx, net_tx;

        c_cpu_usage.label_substr = c_label;
        c_memory_current_usage.label_substr = c_label;
        c_memory_swap_usage.label_substr = c_label;
        io_read.label_substr = c_label;
        io_write.label_substr = c_label;
        net_rx.label_substr = c_label;
        net_tx.label_substr = c_label;

        c_cpu_usage.m_name = {"container_cpu_usage"};
        c_memory_current_usage.m_name = {"container_memory_usage"};
        c_memory_swap_usage.m_name = {"container_swap_usage"};
        io_read.m_name = {"container_reading_bytes"};
        io_write.m_name = {"container_writting_bytes"};
        net_rx.m_name = {"container_rx_bytes"};
        net_tx.m_name = {"container_tx_bytes"};

        c_cpu_usage.m_description = {"Container CPU usage in percents"};
        c_memory_current_usage.m_description = {"Container memory usage in Bytes"};
        c_memory_swap_usage.m_description = {"Container swap usage in Bytes"};
        io_read.m_description = {"Container total read data in Bytes"};
        io_write.m_description = {"Container total written data in Bytes"};
        net_rx.m_description = {"Container total received data in Bytes"};
        net_tx.m_description = {"Container total transmitted data in Bytes"};

        c_cpu_usage.m_value = std::to_string(c_data.resource_stats.c_cpu_usage);
        c_memory_current_usage.m_value = std::to_string(c_data.resource_stats.mem_stats.memory_current);
        c_memory_swap_usage.m_value = std::to_string(c_data.resource_stats.mem_stats.memory_swap_current);
        io_read.m_value = c_data.resource_stats.io_stats.io_rbytes;
        io_write.m_value = c_data.resource_stats.io_stats.io_wbytes;
        net_rx.m_value = std::to_string(c_data.net_stats.rx_bytes);
        net_tx.m_value = std::to_string(c_data.net_stats.tx_bytes);

        total_fields << get_stat_metric_field(c_cpu_usage);
        total_fields << get_stat_metric_field(c_memory_current_usage);
        total_fields << get_stat_metric_field(c_memory_swap_usage);
        total_fields << get_stat_metric_field(io_read);
        total_fields << get_stat_metric_field(io_write);
        total_fields << get_stat_metric_field(net_rx);
        total_fields << get_stat_metric_field(net_tx);

    }

    // std::cout << "Container " << c_data.json_stats.name <<" stats collected" << std::endl;

    return total_fields.str();
};
