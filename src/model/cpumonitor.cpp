#include "cpumonitor.h"
#include "../core/Constants.h"
#include "../core/SystemUtils.h"
#include <QDebug>
#include <QDir>
#include <QDirIterator>

// static member definitions
const QString CPUMonitor::THERMAL_ZONE_PATH = "/sys/class/thermal/thermal_zone0/temp";
const QString CPUMonitor::CPUFREQ_PATH = "/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq";

CPUMonitor::CPUMonitor(QObject *parent) : BaseMonitor(parent)
{
    // Parse static info once (model, core count)
    parseProcCpuinfo();

    qDebug() << "CPUMonitor initialized - Model: " <<m_cpuData.model << "Cores: " << m_cpuData.coreCount;
}

void CPUMonitor::collectData()
{
    bool success = true;

    // Parse /proc/stat for usage calculation
    success &= parseProcStat();

    // Parse optional data (may fail on sme systems)
    parseTemperature();  // Don't fail if temperature unavailable
    parseFrequency();    // Don't fail if frequency unavailable

    if (!success) {
        throw std::runtime_error("Failed to parse CPU data");
    }

    // Calculate usage from parsed data
    double newUsage = calculateUsage();
    m_cpuData.usage = newUsage;

    // Add to history
    addToHistory(newUsage);

    // Emit specific signal với detailed data
    emit cpuDataUpdated(m_cpuData);

    qDebug() << "CPU Usage:" << m_cpuData.usage << "% Temp:" << m_cpuData.temperature << "°C";
}
