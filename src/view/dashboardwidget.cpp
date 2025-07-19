#include "dashboardwidget.h"
#include "widgets/metriccard.h"
#include "../model/cpumonitor.h"
#include "../model/memorymonitor.h"
#include "../model/networkmonitor.h"
#include "../core/constants.h"
#include "../core/systemUtils.h"
#include <QDateTime>
#include <QDebug>

DashboardWidget::DashboardWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(new QVBoxLayout(this))
    , m_metricsLayout(new QGridLayout)
    , m_systemInfoLayout(new QHBoxLayout)
    , m_timeUpdateTimer(new QTimer(this))
{
    setupUI();
    setupMetricCards();
    setupSystemInfoSection();
    applyDashboardStyling();

    // Setup time update
    connect(m_timeUpdateTimer, &QTimer::timeout, this, &DashboardWidget::updateCurrentTime);
    m_timeUpdateTimer->start(1000);
    updateCurrentTime();

    qDebug() << "DashboardWidget initialized";
}

void DashboardWidget::setupUI()
{
    // Main layout setup
    m_mainLayout->setContentsMargins(16, 16, 16, 16);
    m_mainLayout->setSpacing(16);

    // Metrics grid (2x2)
    m_metricsLayout->setSpacing(12);
    m_metricsLayout->setColumnStretch(0, 1);
    m_metricsLayout->setColumnStretch(1, 1);
    m_metricsLayout->setRowStretch(0, 1);
    m_metricsLayout->setRowStretch(1, 1);

    // Add to main layout
    m_mainLayout->addLayout(m_metricsLayout, 3);      // 75% for metrics
    m_mainLayout->addLayout(m_systemInfoLayout, 1);   // 25% for system info
}

void DashboardWidget::setupMetricCards()
{
    // CPU Card (Top-left: 0,0)
    m_cpuCard = new MetricCard(MetricCard::CircularType, this);
    m_cpuCard->setTitle("CPU");
    m_cpuCard->setColor(QColor(Constants::CPU_COLOR));
    m_metricsLayout->addWidget(m_cpuCard, 0, 0);

    // Memory Card (Top-right: 0,1)
    m_memoryCard = new MetricCard(MetricCard::CircularType, this);
    m_memoryCard->setTitle("RAM");
    m_memoryCard->setColor(QColor(Constants::RAM_COLOR));
    m_metricsLayout->addWidget(m_memoryCard, 0, 1);

    // Network Card (Bottom-left: 1,0)
    m_networkCard = new MetricCard(MetricCard::NetworkType, this);
    m_networkCard->setTitle("Network");
    m_networkCard->setColor(QColor("#27AE60"));
    m_metricsLayout->addWidget(m_networkCard, 1, 0);

    // Storage Card (Bottom-right: 1,1)
    m_storageCard = new MetricCard(MetricCard::CircularType, this);
    m_storageCard->setTitle("Storage");
    m_storageCard->setColor(QColor("#9B59B6"));
    m_metricsLayout->addWidget(m_storageCard, 1, 1);

    qDebug() << "Metric cards positioned in 2x2 grid";
}

void DashboardWidget::setupSystemInfoSection()
{
    // Create system info labels
    m_dateLabel = new QLabel(this);
    m_timeLabel = new QLabel(this);
    m_hostnameLabel = new QLabel(this);
    m_uptimeLabel = new QLabel(this);

    // Style labels
    QFont dateTimeFont;
    dateTimeFont.setPointSize(14);
    dateTimeFont.setWeight(QFont::Bold);
    m_dateLabel->setFont(dateTimeFont);
    m_timeLabel->setFont(dateTimeFont);

    QFont infoFont;
    infoFont.setPointSize(10);
    m_hostnameLabel->setFont(infoFont);
    m_uptimeLabel->setFont(infoFont);

    // Set colors
    m_dateLabel->setStyleSheet("color: #ECF0F1;");
    m_timeLabel->setStyleSheet("color: #3498DB;");
    m_hostnameLabel->setStyleSheet("color: #95A5A6;");
    m_uptimeLabel->setStyleSheet("color: #95A5A6;");

    // Layout system info
    m_systemInfoLayout->addWidget(m_dateLabel);
    m_systemInfoLayout->addStretch();
    m_systemInfoLayout->addWidget(m_timeLabel);
    m_systemInfoLayout->addStretch();
    m_systemInfoLayout->addWidget(m_hostnameLabel);
    m_systemInfoLayout->addStretch();
    m_systemInfoLayout->addWidget(m_uptimeLabel);

    // Initialize with current data
    updateSystemInfo(SystemUtils::getHostname(), SystemUtils::getUptime());
}

void DashboardWidget::applyDashboardStyling()
{
    setStyleSheet(
        "DashboardWidget {"
        "    background-color: #2C3E50;"
        "}"
        );
}

void DashboardWidget::connectCPUMonitor(CPUMonitor *monitor)
{
    if (m_cpuMonitor) {
        disconnect(m_cpuMonitor, nullptr, this, nullptr);
    }

    m_cpuMonitor = monitor;

    if (monitor) {
        connect(monitor, &CPUMonitor::cpuDataUpdated,
                this, [this](const CPUData &data) {
                    updateCPUMetrics(data.usage, data.temperature);

                    QString subtitle = QString("Temp: %1°C").arg(data.temperature, 0, 'f', 1);
                    if (!data.model.isEmpty() && data.model != "Unknown CPU") {
                        subtitle += QString(" | %1").arg(data.model.left(20)); // Truncate long names
                    }
                    m_cpuCard->setSubtitle(subtitle);
                });

        qDebug() << "CPU Monitor connected to dashboard";
    }
}

void DashboardWidget::connectMemoryMonitor(MemoryMonitor *monitor)
{
    if (m_memoryMonitor) {
        disconnect(m_memoryMonitor, nullptr, this, nullptr);
    }

    m_memoryMonitor = monitor;

    if (monitor) {
        connect(monitor, &MemoryMonitor::memoryDataUpdated,
                this, [this](const MemoryData &data) {
                    updateMemoryMetrics(data.usagePercentage);

                    QString subtitle = QString("%1 / %2")
                                           .arg(data.usedMemoryFormatted())
                                           .arg(data.totalMemoryFormatted());
                    m_memoryCard->setSubtitle(subtitle);
                });

        qDebug() << "Memory Monitor connected to dashboard";
    }
}

void DashboardWidget::connectNetworkMonitor(NetworkMonitor *monitor)
{
    if (m_networkMonitor) {
        disconnect(m_networkMonitor, nullptr, this, nullptr);
    }

    m_networkMonitor = monitor;

    if (monitor) {
        connect(monitor, &NetworkMonitor::networkDataUpdated, this, [this](const NetworkData &data) {
            updateNetworkMetrics(data.uploadSpeed, data.downloadSpeed);

            QString subtitle = QString("%1").arg(data.activeInterface);
            m_networkCard->setSubtitle(subtitle);
        });

        qDebug() << "Network Monitor connected to dashboard";
    }
}

void DashboardWidget::updateCPUMetrics(double usage, double temperature)
{
    if (m_cpuCard) {
        m_cpuCard->setValue(usage);

        // Dynamic color based on thresholds
        QColor color;
        if (usage >= Constants::CPU_CRITICAL_THRESHOLD) {
            color = QColor(Constants::CRITICAL_COLOR);
        } else if (usage >= Constants::CPU_WARNING_THRESHOLD) {
            color = QColor(Constants::WARNING_COLOR);
        } else {
            color = QColor(Constants::CPU_COLOR);
        }
        m_cpuCard->setColor(color);
    }

    qDebug() << "Dashboard CPU updated:" << usage << "% temp:" << temperature << "°C";
}

void DashboardWidget::updateMemoryMetrics(double usage, const QString &details)
{
    if (m_memoryCard) {
        m_memoryCard->setValue(usage);

        // Dynamic color based on thresholds
        QColor color;
        if (usage >= Constants::RAM_CRITICAL_THRESHOLD) {
            color = QColor(Constants::CRITICAL_COLOR);
        } else if (usage >= Constants::RAM_WARNING_THRESHOLD) {
            color = QColor(Constants::WARNING_COLOR);
        } else {
            color = QColor(Constants::RAM_COLOR);
        }
        m_memoryCard->setColor(color);

        if (!details.isEmpty()) {
            m_memoryCard->setSubtitle(details);
        }
    }

    qDebug() << "Dashboard Memory updated:" << usage << "%";
}

void DashboardWidget::updateNetworkMetrics(double up, double down)
{
    if (m_networkCard) {
        m_networkCard->setNetworkSpeeds(up, down);
    }

    qDebug() << "Dashboard Network updated: up:" << up << "MB/s down:" << down << "MB/s";
}

void DashboardWidget::updateStorageMetrics(double usage)
{
    if (m_storageCard) {
        m_storageCard->setValue(usage);

        // Dynamic color for storage
        QColor color;
        if (usage >= 90.0) {
            color = QColor(Constants::CRITICAL_COLOR);
        } else if (usage >= 80.0) {
            color = QColor(Constants::WARNING_COLOR);
        } else {
            color = QColor("#9B59B6");  // Default storage color
        }
        m_storageCard->setColor(color);
    }

    qDebug() << "Dashboard Storage updated:" << usage << "%";
}

void DashboardWidget::updateSystemInfo(const QString &hostname, const QString &uptime)
{
    if (m_hostnameLabel) {
        m_hostnameLabel->setText("Host: " + hostname);
    }

    if (m_uptimeLabel) {
        m_uptimeLabel->setText("Uptime: " + uptime);
    }
}

void DashboardWidget::updateCurrentTime()
{
    QDateTime now = QDateTime::currentDateTime();

    if (m_dateLabel) {
        m_dateLabel->setText(now.toString("yyyy/MM/dd"));
    }

    if (m_timeLabel) {
        m_timeLabel->setText(now.toString("hh:mm:ss"));
    }
}
