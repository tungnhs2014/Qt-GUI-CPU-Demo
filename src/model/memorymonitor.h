#ifndef MEMORYMONITOR_H
#define MEMORYMONITOR_H

#include "base/basemonitor.h"
#include <QObject>

struct MemoryData {
    // Memory amounts int bytes
    qint64 totalMemory = 0;         // Total physical memory
    qint64 availableMemory = 0;     // Available memory
    qint64 usedMemory = 0;          // Used memory (calculated)
    qint64 freeMemory = 0;          // Free memory
    qint64 buffers = 0;             // Buffer memory
    qint64 cached = 0;              // Cache memory

    // Swap information
    qint64 totalSwap = 0;           // Total swap cache
    qint64 usedSwap = 0;            // Used swap space
    qint64 freeSwap = 0;            // Free swap space

    // Calculated percentages
    double usagePercentage = 0.0;   // Memory usage percentage
    double swapPercentage = 0.0;    // Swap usage percentage

    bool isValid() const{
        return totalMemory > 0;
    }

    // Helper methods
    QString totalMemoryFormatted() const;
    QString usedMemoryFormatted() const;
    QString availableMemoryFormatted() const;
};

class MemoryMonitor : public BaseMonitor
{
    Q_OBJECT
public:
    explicit MemoryMonitor(QObject *parent = nullptr);

    // Data access
    const MemoryData &data() const {
        return m_memoryData;
    }

    double usage() const {
        return m_memoryData.usagePercentage;
    }

    double swapUsage() const {
        return m_memoryData.swapPercentage;
    }

    qint64 totalMemory() const {
        return m_memoryData.totalMemory;
    }

    qint64 availableMemory() const {
        return m_memoryData.availableMemory;
    }

    // History tracking
    QVector<double> usageHistory() const {
        return m_usageHistory;
    }

    void clearHistory();

    static QString formatBytes(qint64 bytes);

signals:
    void memoryDataUpdated(const MemoryData &data);

protected:
    void collectData() override;

private:
    bool parseProcMeminfo();

    void calculatePercentages();

    void addToHistory(double usage);

    MemoryData m_memoryData;
    QVector<double> m_usageHistory;
    const int MAX_HISTORY_SIZE = 60;
};

#endif // MEMORYMONITOR_H
