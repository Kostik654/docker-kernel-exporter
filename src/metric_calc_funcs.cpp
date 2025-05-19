#include "metric_calc_funcs.h"

HostCPUStats return_host_cpu_delta(HostCPUStats cpu_stats_a, HostCPUStats cpu_stats_b)
{
    HostCPUStats delta;
    // last data
    delta.processes_running = cpu_stats_b.processes_running;
    delta.processes_total = cpu_stats_b.processes_total;
    // delta
    delta.cpu_user = cpu_stats_b.cpu_user - cpu_stats_a.cpu_user;
    delta.cpu_system = cpu_stats_b.cpu_system - cpu_stats_a.cpu_system;
    delta.cpu_nice = cpu_stats_b.cpu_nice - cpu_stats_a.cpu_nice;
    delta.cpu_guest_nice = cpu_stats_b.cpu_guest_nice - cpu_stats_a.cpu_guest_nice;
    delta.cpu_idle = cpu_stats_b.cpu_idle - cpu_stats_a.cpu_idle;
    delta.cpu_iowait = cpu_stats_b.cpu_iowait - cpu_stats_a.cpu_iowait;
    delta.cpu_steal = cpu_stats_b.cpu_steal - cpu_stats_a.cpu_steal;
    delta.cpu_guest = cpu_stats_b.cpu_guest - cpu_stats_a.cpu_guest;
    delta.cpu_irq = cpu_stats_b.cpu_irq - cpu_stats_a.cpu_irq;
    delta.cpu_softirq = cpu_stats_b.cpu_softirq - cpu_stats_a.cpu_softirq;

    return delta;
}

ContainerCPUStats return_container_cpu_delta(ContainerCPUStats c_cpu_stats_a, ContainerCPUStats c_cpu_stats_b)
{
    ContainerCPUStats delta;

    delta.cpu_user_usec = c_cpu_stats_b.cpu_user_usec - c_cpu_stats_a.cpu_user_usec;
    delta.cpu_system_usec = c_cpu_stats_b.cpu_system_usec - c_cpu_stats_a.cpu_system_usec;
    delta.nice_usec = c_cpu_stats_b.nice_usec - c_cpu_stats_a.nice_usec;
    delta.burst_usec = c_cpu_stats_b.burst_usec - c_cpu_stats_a.burst_usec;
    delta.nr_bursts = c_cpu_stats_b.nr_bursts - c_cpu_stats_a.nr_bursts;
    delta.nr_periods = c_cpu_stats_b.nr_periods - c_cpu_stats_a.nr_periods;
    delta.nr_throttled = c_cpu_stats_b.nr_throttled - c_cpu_stats_a.nr_throttled;
    delta.throttled_usec = c_cpu_stats_b.throttled_usec - c_cpu_stats_a.throttled_usec;
    delta.core_sched_force_idle_usec = c_cpu_stats_b.core_sched_force_idle_usec - c_cpu_stats_a.core_sched_force_idle_usec;
    delta.cpu_usage_usec = c_cpu_stats_b.cpu_usage_usec - c_cpu_stats_a.cpu_usage_usec;

    return delta;
}

float count_host_cpu_load(HostCPUStats cpu_stats_delta)
{

    size_t total = cpu_stats_delta.cpu_user +
                   cpu_stats_delta.cpu_system +
                   cpu_stats_delta.cpu_nice +
                   cpu_stats_delta.cpu_softirq +
                   cpu_stats_delta.cpu_steal +
                   cpu_stats_delta.cpu_irq +
                   cpu_stats_delta.cpu_idle +
                   cpu_stats_delta.cpu_iowait +
                   cpu_stats_delta.cpu_guest +
                   cpu_stats_delta.cpu_guest_nice;

    return ((float)(cpu_stats_delta.cpu_user + cpu_stats_delta.cpu_system + cpu_stats_delta.cpu_nice)) /
           ((float)total) *
           100.0f;
}

// interval in microseconds
float get_container_cpu_usage(ContainerCPUStats delta, uint64_t interval_usec_ms)
{
    if (interval_usec_ms == 0) {
        return 0.0f;
    }

    interval_usec_ms *= 1000; // microseconds

    // optional
    // return ((float)delta.cpu_usage_usec / (float)interval_usec) * 100.0f;

    uint64_t active_usec = delta.cpu_user_usec + delta.cpu_system_usec + delta.nice_usec;

    return ((float)active_usec / (float)interval_usec_ms) * 100.0f;
}