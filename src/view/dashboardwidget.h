#ifndef DASHBOARDWIDGET_H
#define DASHBOARDWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>

class MetricCard;
class CPUMonitor;
class MemoryMonitor;
class NetworkMonitor;

class DashboardWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DashboardWidget(QWidget *parent = nullptr);

    // Monitor integration (for real data)
    void connectCPUMonitor(CPUMonitor *monitor);
    void connectMemoryMonitor(MemoryMonitor *monitor);
    void connectNetworkMonitor(NetworkMonitor *monitor);

    // Manual update methods (for testing and fake data)
    void updateCPUMetrics(double usage, double temperature = 0.0);
    void updateMemoryMetrics(double usage, const QString &detail = "");
    void updateNetworkMetrics(double up, double down);
    void updateStorageMetrics(double usage);
    void updateSystemInfo(const QString &hostname, const QString &uptime);

private slots:
    void updateCurrentTime();

private:
    void setupUI();
    void setupMetricCards();
    void setupSystemInfoSection();
    void applyDashboardStyling();

    //  Main layout
    QVBoxLayout *m_mainLayout;
    QGridLayout *m_metricsLayout;
    QHBoxLayout *m_systemInfoLayout;

    // Metrics cards (2x2 grid)
    MetricCard *m_cpuCard;          // Top-left
    MetricCard *m_memoryCard;       // Top-right
    MetricCard *m_networkCard;      // Bottom-left
    MetricCard *m_storageCard;      // Bottom-right

    // System information bar
    QLabel *m_dateLabel;
    QLabel *m_timeLabel;
    QLabel *m_hostnameLabel;
    QLabel *m_uptimeLabel;

    // Update timer
    QTimer *m_timeUpdateTimer;

    // Connected monitors (weak references)
    CPUMonitor *m_cpuMonitor = nullptr;
    MemoryMonitor *m_memoryMonitor = nullptr;
    NetworkMonitor *m_networkMonitor = nullptr;
};

#endif // DASHBOARDWIDGET_H
