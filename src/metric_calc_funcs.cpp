#include "metric_calc_funcs.h"

float count_host_cpu_load(HostCPUStats cpu_stats) {

    size_t total = cpu_stats.cpu_user + 
    cpu_stats.cpu_system + 
    cpu_stats.cpu_nice + 
    cpu_stats.cpu_softirq + 
    cpu_stats.cpu_steal + cpu_stats.cpu_irq + 
    cpu_stats.cpu_idle + 
    cpu_stats.cpu_iowait;

    return (cpu_stats.cpu_user + cpu_stats.cpu_system + cpu_stats.cpu_nice) / total * 100.00f;

}
