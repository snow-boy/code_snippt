#ifndef _CPU_MONITOR_H_
#define _CPU_MONITOR_H_

namespace agent
{

class CPUMonitor
{
public:
    CPUMonitor();

    ~CPUMonitor();

    double StatisticUsage();

private:
    void Init();

    void Uninit();

    double StatisticUsageInternal();

    unsigned long long last_total_time_;

    unsigned long long last_idle_time_;
};

} // namespace agent

#endif // !_CPU_MONITOR_H_