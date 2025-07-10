#include "cpumonitor.h"
#include "../core/constants.h"
#include "../core/systemUtils.h"
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

void CPUMonitor::clearHistory()
{
    m_usageHistory.clear();
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

bool CPUMonitor::parseProcStat()
{
    if (!SystemUtils::fileExits(Constants::PROC_STAT)) {
        // Development on Windows - use fake data
        m_cpuData.totalTime = 1000000;
        m_cpuData.idleTime = 600000;    // 40% usage
        return true;
    }

    QString content = SystemUtils::readFile(Constants::PROC_STAT);
    if (content.isEmpty()) {
        return false;
    }

    // Parse first line: "cpu user nice system idle iowait irq softirq steal
    QStringList lines = content.split('\n');
    if (lines.isEmpty()) {
        return false;
    }

    QString cpuLine = lines.first();
    if (!cpuLine.startsWith("cpu ")) {
        return false;
    }

    QStringList values = cpuLine.split(' ', Qt::SkipEmptyParts);
    if (values.size() < 5) {
        return false;
    }

    // Extract CPU time values (all in clock ticks)
    qint64 user = SystemUtils::parseInt64(values[1]);
    qint64 nice = SystemUtils::parseInt64(values[2]);
    qint64 system = SystemUtils::parseInt64(values[3]);
    qint64 idle = SystemUtils::parseInt64(values[4]);
    qint64 iowait = values.size() > 5 ? SystemUtils::parseInt64(values[5]) : 0;
    qint64 irq = values.size() > 6 ? SystemUtils::parseInt64(values[6]) : 0;
    qint64 softirq = values.size() > 7 ? SystemUtils::parseInt64(values[7]) : 0;

    // Calculate totals
    qint64 idleTotal = idle + iowait;   // Total idle time
    qint64 nonIdleTotal = user + nice + system + irq + softirq; // Active time
    qint64 total = idleTotal + nonIdleTotal;   // Total time

    // Store for usage calculation
    m_cpuData.lastTotalTime = m_cpuData.totalTime;
    m_cpuData.lastIdleTime = m_cpuData.idleTime;
    m_cpuData.totalTime = total;
    m_cpuData.idleTime = idleTotal;

    return true;
}

bool CPUMonitor::parseProcCpuinfo()
{
    if(SystemUtils::fileExits(Constants::PROC_CPUINFO)) {
        // Devlopment fallback
        m_cpuData.model = "Unknown CPU";
        m_cpuData.coreCount = 1;
        return true;
    }

    QStringList lines = SystemUtils::readFileLines(Constants::PROC_CPUINFO);

    int coreCount = 0;
    QString model;

    for (const QString &line : lines) {
        if (line.startsWith("processor")) {
            coreCount++;
        }
        else if (line.startsWith("model name") && model.isEmpty()) {
            // Extract CPU model from first occurrence
            QStringList parts = line.split(':');
            if (parts.size() > 2) {
                model = parts[1].trimmed();            }
        }
    }

    m_cpuData.coreCount = qMax(coreCount, 1); // At least 1 core
    if (!model.isEmpty()) {
        m_cpuData.model = model;
    }
    else {
        m_cpuData.model = "Unknown CPU";
    }

    return true;
}

bool CPUMonitor::parseTemperature()
{
    if (!SystemUtils::fileExits(THERMAL_ZONE_PATH)) {
        m_cpuData.temperature = 0.0; // Unknown
        return false;
    }

    QString tempStr = SystemUtils::readFile(THERMAL_ZONE_PATH).trimmed();
    if (tempStr.isEmpty()) {
        return false;
    }

    bool ok;
    qint64 tempMilliC = SystemUtils::parseInt64(tempStr, &ok);
    if (!ok) {
        return false;
    }

    // Convert millidegrees to degrees Celsius
    m_cpuData.temperature = tempMilliC / 1000.0;

    return true;
}

bool CPUMonitor::parseFrequency()
{
    if (!SystemUtils::fileExits(CPUFREQ_PATH)) {
        m_cpuData.frequency = 0.0;  // Unknown
        return false;
    }

    QString freqStr = SystemUtils::readFile(CPUFREQ_PATH).trimmed();
    if (freqStr.isEmpty()) {
        return false;
    }

    bool ok;
    qint64 freqKHz = SystemUtils::parseInt64(freqStr, &ok);
    if (!ok) {
        return false;
    }

    // Convert KHz to MHz
    m_cpuData.frequency = freqKHz / 1000.0;

    return true;
}

double CPUMonitor::calculateUsage()
{
    if (m_cpuData.lastTotalTime == 0) {
        return 0.0;
    }

    // Calucate time differences
    qint64 totalDiff = m_cpuData.totalTime - m_cpuData.lastTotalTime;
    qint64 idleDiff = m_cpuData.idleTime - m_cpuData.lastIdleTime;

    if (totalDiff <= 0) {
        return 0.0;  // Avoid division by zero
    }

    // Calculate usage percentage
    double usage = ((double)(totalDiff - idleDiff) / totalDiff) * 100.0;

    // Clamp to valid range
    return qMax(0.0, qMin(100.0, usage));
}

void CPUMonitor::addToHistory(double usage)
{
    m_usageHistory.append(usage);

    // Maintain fixed-size history
    while (m_usageHistory.size() > MAX_HISTORY_SIZE) {
        m_usageHistory.removeFirst();
    }
}

