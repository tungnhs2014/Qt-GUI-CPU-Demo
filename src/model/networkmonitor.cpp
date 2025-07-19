#include "networkmonitor.h"
#include "../core/constants.h"
#include "../core/systemUtils.h"
#include <QDebug>
#include <QRandomGenerator>

NetworkMonitor::NetworkMonitor(QObject *parent) : BaseMonitor(parent)
{
    // Set update interval for network monitoring
    setUpdateInterval(Constants::NETWORK_UPDATE_INTERVAL);

    // Find active network interface
    m_activeInterface = findActiveInterface();

    qDebug() << "NetworkMonitor initialized - Active interface:" << m_activeInterface;
}

void NetworkMonitor::collectData()
{
    if (readNetworkStats()) {
        calculateSpeeds();
        emit networkDataUpdated(m_currentData);

        qDebug() << QString("Network: ↑%1 ↓%2 Interface: %3")
                        .arg(m_currentData.uploadSpeedFormatted())
                        .arg(m_currentData.downloadSpeedFormatted())
                        .arg(m_activeInterface);
    }
}

bool NetworkMonitor::readNetworkStats()
{
    QString content = SystemUtils::readFile(Constants::PROC_NET_DEV);
    if (content.isEmpty()) {
        // Fallback for non-Linux systems
        m_currentData.uploadSpeed = 0.5 + (QRandomGenerator::global()->bounded(20)) / 10.0;
        m_currentData.downloadSpeed = 2.0 + (QRandomGenerator::global()->bounded(50)) / 10.0;
        m_currentData.activeInterface = "fake0";
        return true;
    }

    QStringList lines = content.split('\n');

    // Find line for active interface
    for (const QString &line : lines) {
        if (line.contains(m_activeInterface + ":")) {
            QStringList parts = line.simplified().split(' ', Qt::SkipEmptyParts);

            if (parts.size() >= 10) {
                // /proc/net/dev format:
                // interface: rx_bytes rx_packets ... tx_bytes tx_packets ...
                qint64 rxBytes = parseNetworkValue(line, 1);  // Received bytes
                qint64 txBytes = parseNetworkValue(line, 9);  // Transmitted bytes

                m_currentData.totalBytesDownloaded = rxBytes;
                m_currentData.totalBytesUploaded = txBytes;
                m_currentData.activeInterface = m_activeInterface;

                return true;
            }
        }
    }

    // Interface not found, try to find new active interface
    QString newInterface = findActiveInterface();
    if (newInterface != m_activeInterface) {
        m_activeInterface = newInterface;
        qDebug() << "Network interface changed to:" << m_activeInterface;
        // Reset previous values
        m_previousRxBytes = 0;
        m_previousTxBytes = 0;
    }

    return false;
}

void NetworkMonitor::calculateSpeeds()
{
    QDateTime currentTime = QDateTime::currentDateTime();

    if (m_previousTime.isValid() && m_previousRxBytes > 0 && m_previousTxBytes > 0) {
        // Calculate time difference in seconds
        qint64 timeDiffMs = m_previousTime.msecsTo(currentTime);
        double timeDiffSec = timeDiffMs / 1000.0;

        if (timeDiffSec > 0) {
            // Calculate byte differences
            qint64 rxDiff = m_currentData.totalBytesDownloaded - m_previousRxBytes;
            qint64 txDiff = m_currentData.totalBytesUploaded - m_previousTxBytes;

            // Convert to MB/s
            m_currentData.downloadSpeed = (rxDiff / timeDiffSec) / Constants::BYTES_PER_MB;
            m_currentData.uploadSpeed = (txDiff / timeDiffSec) / Constants::BYTES_PER_MB;

            // Clamp to reasonable values (0-1000 MB/s)
            m_currentData.downloadSpeed = qMax(0.0, qMin(1000.0, m_currentData.downloadSpeed));
            m_currentData.uploadSpeed = qMax(0.0, qMin(1000.0, m_currentData.uploadSpeed));
        }
    }
    else {
        // First measuremen, no speed calculation possible
        m_currentData.downloadSpeed = 0.0;
        m_currentData.uploadSpeed = 0.0;
    }

    // Store current values for next calculation
    m_previousRxBytes = m_currentData.totalBytesDownloaded;
    m_previousTxBytes = m_currentData.totalBytesUploaded;
    m_previousTime = currentTime;
}

QString NetworkMonitor::findActiveInterface()
{
    // Check common interface names
    for (const QString &interface : Constants::NETWORK_INTERFACES) {
        if (isValidInterface(interface)) {
            return interface;
        }
    }

    // If no common interface found, try to detect from /proc/net/dev
    QString content = SystemUtils::readFile(Constants::PROC_NET_DEV);
    if (!content.isEmpty()) {
        QStringList lines = content.split('\n');

        for (const QString &line : lines) {
            if (line.contains(':')) {
                QString interface = line.split(':').first().trimmed();

                // Skip loopback and virtual interfaces
                if (interface != "lo" && !interface.startsWith("docker") &&
                    !interface.startsWith("vir") && isValidInterface(interface)) {
                    return interface;
                }
            }
        }
    }

    // Fallback
    return "eth0";
}

bool NetworkMonitor::isValidInterface(const QString &interface)
{
    QString content = SystemUtils::readFile(Constants::PROC_NET_DEV);
    return content.contains(interface + ":");
}

qint64 NetworkMonitor::parseNetworkValue(const QString &line, int position)
{
    QStringList parts = line.simplified().split(' ', Qt::SkipEmptyParts);

    if (position < parts.size()) {
        bool ok;
        qint64 value = parts[position].toLongLong(&ok);
        return ok ? value : 0;
    }

    return 0;
}

QStringList NetworkMonitor::availableInterfaces() const
{
    QStringList interfaces;

    QString content = SystemUtils::readFile(Constants::PROC_NET_DEV);
    if (!content.isEmpty()) {
        QStringList lines = content.split('\n');

        for (const QString &line : lines) {
            if (line.contains(':')) {
                QString interface = line.split(':').first().trimmed();

                // Skip loopback
                if (interface != "lo") {
                    interfaces.append(interface);
                }
            }
        }
    }

    return interfaces;
}
