#ifndef CPUMONITOR_H
#define CPUMONITOR_H

#include "base/basemonitor.h"
#include <QVector>

struct CPUData {
    double usage = 0.0;           // CPU usage percentage (0-100)
    double temperature = 0.0;     // CPU temperature in Celsius
    double frequency = 0.0;       // CPU frequency in MHz
    int coreCount = 1;            // Number of CPU cores
    QString model;                // CPU model name

    // Time-based data for calculation
    qint64 totalTime = 0;        // Total CPU time
    qint64 idleTime = 0;         // Idle CPU time
    qint64 lastTotalTime = 0;    // Previous total time
    qint64 lastIdleTime = 0;     // Previous idle time

    bool isValid() const {
        return totalTime > 0;
    }
};

class CPUMonitor : public BaseMonitor
{
    Q_OBJECT

public:
    explicit CPUMonitor(QObject *parent = nullptr);

    // Returns const reference - read-only access
    const CPUData &data() const {
        return m_cpuData;
    }
    double usage() const {
        return m_cpuData.usage;
    }
    double temperature() const {
        return m_cpuData.temperature;
    }
    double frequency() const {
        return m_cpuData.frequency;
    }
    QString model() const {
        return m_cpuData.model;
    }

    // History tracking
    QVector<double> usageHistory() const { return m_usageHistory; }

    void clearHistory();

signals:
    void cpuDataUpdated(const CPUData &data);

protected:
    // Parse /proc/stat và calculate CPU usage
    void collectData() override;

private:
    // Extract CPU time values
    bool parseProcStat();
    // Get CPU model, core count
    bool parseProcCpuinfo();
    // Get CPU temperature
    bool parseTemperature();
    // Get current CPU frequency
    bool parseFrequency();
    // Using current and previous time values
    double calculateUsage();
    // Maintain fixed-size history buffer
    void addToHistory(double usage);

private:
    CPUData m_cpuData;                    // Current CPU data
    QVector<double> m_usageHistory;       // Usage history for charts
    const int MAX_HISTORY_SIZE = 60;      // Keep 60 data points (1 minute)

    // File paths for different data sources
    static const QString THERMAL_ZONE_PATH;
    static const QString CPUFREQ_PATH;
};

#endif // CPUMONITOR_H
