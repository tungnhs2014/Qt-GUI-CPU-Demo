#ifndef NETWORKMONITOR_H
#define NETWORKMONITOR_H

#include "base/basemonitor.h"
#include <QStringList>
#include <QDateTime>

struct NetworkData
{
    double uploadSpeed = 0.0;       // MB/s
    double downloadSpeed = 0.0;     // MB/s
    QString activeInterface;        // eth0, wlan0, etc.
    qint64 totalBytesUploaded = 0;
    qint64 totalBytesDownloaded = 0;

    // Helper methods
    QString uploadSpeedFormatted() const {
        return QString::number(uploadSpeed, 'f', 1) + " MB/s";
    }

    QString downloadSpeedFormatted() const {
        return QString::number(downloadSpeed, 'f', 1) + " MB/s";
    }
};

class NetworkMonitor : public BaseMonitor
{
    Q_OBJECT
public:
    explicit NetworkMonitor(QObject *parent = nullptr);

    // Current data access
    NetworkData currentData() const {
        return m_currentData;
    }

    // Network interface management
    QString activeInterface() const {
        return m_activeInterface;
    }
    QStringList availableInterfaces() const;

signals:
    void networkDataUpdated(const NetworkData &data);

protected:
    void collectData() override;

private:
    // Data collection
    bool readNetworkStats();
    void calculateSpeeds();
    QString findActiveInterface();
    qint64 parseNetworkValue(const QString &line, int position);

    // Network interface detection
    bool isValidInterface(const QString &interface);

    NetworkData m_currentData;
    NetworkData m_previousData;
    QString m_activeInterface;

    // Previous values for speed calculation
    qint64 m_previousRxBytes = 0;
    qint64 m_previousTxBytes = 0;
    QDateTime m_previousTime;

    // Available interfaces cache
    QStringList m_availableInterfaces;
};

#endif // NETWORKMONITOR_H
