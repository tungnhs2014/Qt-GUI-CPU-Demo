#include "memorymonitor.h"
#include "../core/constants.h"
#include "../core/systemUtils.h"
#include <QDebug>

MemoryMonitor::MemoryMonitor(QObject *parent)
    : BaseMonitor{parent}
{
    qDebug() << "Memory Monitor initialized";
}

void MemoryMonitor::clearHistory()
{
    m_usageHistory.clear();
}

void MemoryMonitor::collectData()
{
    bool success = parseProcMeminfo();

    if (!success) {
        throw std::runtime_error("Failed to parse memory data");
    }

    calculatePercentages();
    addToHistory(m_memoryData.usagePercentage);

    emit memoryDataUpdated(m_memoryData);

    qDebug() << "Memory Usage:" << m_memoryData.usagePercentage
             << "% Total:" << formatBytes(m_memoryData.totalMemory);
}


bool MemoryMonitor::parseProcMeminfo()
{
    if (!SystemUtils::fileExits(Constants::PROC_MEMINFO)) {
        // Development fallback - fakedata
        m_memoryData.totalMemory = 4LL * 1024 * 1024 * 1024;    // 4GB
        m_memoryData.availableMemory = 1LL * 1024 * 1024 * 1024; // 1GB available
        m_memoryData.usedMemory = m_memoryData.totalMemory - m_memoryData.availableMemory;
        return true;
    }

    QStringList lines = SystemUtils::readFileLines(Constants::PROC_MEMINFO);

    // Parse memory information from /proc/meminfo
    // Format: "MemTotal:        1024000 kB"
    for (const QString &line : lines) {
        QStringList parts = line.split(':', Qt::SkipEmptyParts);
        if (parts.size() < 2) {
            continue;
        }

        QString key = parts[0].trimmed();
        QString valueStr = parts[1].trimmed();

        // Extract number value (remove " kB" suffix)
        QStringList valueParts = valueStr.split(' ', Qt::SkipEmptyParts);
        if (valueParts.isEmpty()) {
            continue;
        }

        bool ok;
        qint64 valueKB = SystemUtils::parseInt64(valueParts[0], &ok);
        if (!ok) {
            continue;
        }

        qint64 valueBytes = valueKB * 1024; // Convert KB to bytes

        // Parse different memory types
        if (key == "MemTotal") {
            m_memoryData.totalMemory = valueBytes;
        }
        else if (key == "MemAvailable") {
            m_memoryData.availableMemory = valueBytes;
        }
        else if (key == "MemFree") {
            m_memoryData.freeMemory = valueBytes;
        }
        else if (key == "Buffers") {
            m_memoryData.buffers = valueBytes;
        }
        else if (key == "Cached") {
            m_memoryData.cached = valueBytes;
        }
        else if (key == "SwapTotal") {
            m_memoryData.totalSwap = valueBytes;
        }
        else if (key == "SwapFree") {
            m_memoryData.freeSwap = valueBytes;
        }
    }

    // Calculate derived values
    if (m_memoryData.totalSwap > 0) {
        m_memoryData.usedSwap = m_memoryData.totalSwap - m_memoryData.freeSwap;
    }

    // If MemAvailable not available, estimate it
    if (m_memoryData.availableMemory == 0) {
        m_memoryData.availableMemory = m_memoryData.freeMemory +
                                       m_memoryData.buffers +
                                       m_memoryData.cached;
    }

    m_memoryData.usedMemory = m_memoryData.totalMemory - m_memoryData.availableMemory;

    return m_memoryData.totalMemory > 0;
}

void MemoryMonitor::calculatePercentages()
{
    if (m_memoryData.totalMemory > 0) {
        m_memoryData.usagePercentage = ((double)m_memoryData.usedMemory / m_memoryData.totalMemory) * 100.0;
        m_memoryData.usagePercentage = qMax(0.0, qMin(100.0, m_memoryData.usagePercentage));
    }

    if (m_memoryData.totalSwap > 0) {
        m_memoryData.swapPercentage = ((double)m_memoryData.usedSwap / m_memoryData.totalSwap) * 100.0;
        m_memoryData.swapPercentage = qMax(0.0, qMin(100.0, m_memoryData.swapPercentage));
    }
}

void MemoryMonitor::addToHistory(double usage)
{
    m_usageHistory.append(usage);

    while (m_usageHistory.size() > MAX_HISTORY_SIZE) {
        m_usageHistory.removeFirst();
    }
}

QString MemoryMonitor::formatBytes(qint64 bytes)
{
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unitIndex = 0;
    double size = bytes;

    while (size >= 1024.0 && unitIndex < 4) {
        size /= 1024.0;
        unitIndex++;
    }

    return QString::number(size, 'f', 1) + " " + units[unitIndex];
}

QString MemoryData::totalMemoryFormatted() const
{
    return MemoryMonitor::formatBytes(totalMemory);
}

QString MemoryData::usedMemoryFormatted() const
{
    return MemoryMonitor::formatBytes(usedMemory);
}

QString MemoryData::availableMemoryFormatted() const
{
    return MemoryMonitor::formatBytes(availableMemory);
}
