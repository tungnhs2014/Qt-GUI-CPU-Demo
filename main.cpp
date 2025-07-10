#include "mainwindow.h"
#include "src/core/constants.h"
#include "src/core/systemUtils.h"
#include "src/model/cpumonitor.h"
#include "src/model/memorymonitor.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application properties
    app.setApplicationName(Constants::APP_NAME);
    app.setApplicationVersion(Constants::APP_VERSION);
    app.setOrganizationName(Constants::APP_ORGANIZATION);

    qDebug() << "==TESTING MODEL LAYER===";

    // Create monitor instances
    CPUMonitor *cpuMonitor = new CPUMonitor(&app);
    MemoryMonitor *memoryMonitor = new MemoryMonitor(&app);

    // Connect signals for testing
    QObject::connect(cpuMonitor, &CPUMonitor::cpuDataUpdated,
                     [](const CPUData &data) {
                         qDebug() << "CPU Update - Usage:" << data.usage << "% Temp:"
                                  << data.temperature << "°C Model:" << data.model;
                     });

    QObject::connect(memoryMonitor, &MemoryMonitor::memoryDataUpdated,
                     [](const MemoryData &data) {
                         qDebug() << "Memory Update - Usage:" << data.usagePercentage
                                  << "% Total:" << data.totalMemoryFormatted()
                                  << "Available:" << data.availableMemoryFormatted();
                     });

    QObject::connect(cpuMonitor, &BaseMonitor::stateChanged,
                     [](BaseMonitor::MonitorState state) {
                         qDebug() << "CPU Monitor state changed to:" << state;
                     });

    QObject::connect(memoryMonitor, &BaseMonitor::errorOccurred,
                     [](const QString &error) {
                         qDebug() << "Memory Monitor error:" << error;
                     });

    // Test immediate updates
    qDebug() << "\n--- Testing Immediate Updates ---";
    cpuMonitor->updateNow();
    memoryMonitor->updateNow();

    // Start continuous monitoring
    qDebug() << "\n--- Starting Continuous Monitoring ---";
    cpuMonitor->start();
    memoryMonitor->start();

    // Stop monitoring after 10 seconds for testing
    QTimer::singleShot(10000, [=]() {
        qDebug() << "\n--- Stopping Monitors ---";
        cpuMonitor->stop();
        memoryMonitor->stop();

        // Show history
        auto cpuHistory = cpuMonitor->usageHistory();
        qDebug() << "CPU History size:" << cpuHistory.size();
        if (!cpuHistory.isEmpty()) {
            qDebug() << "CPU History:" << cpuHistory;
        }

        auto memHistory = memoryMonitor->usageHistory();
        qDebug() << "Memory History size:" << memHistory.size();
    });

    MainWindow window;
    window.resize(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT);
    window.setWindowTitle(Constants::APP_NAME + " v" + Constants::APP_VERSION);
    window.show();

    return app.exec();
}
