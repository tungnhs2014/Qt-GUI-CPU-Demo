#include <QApplication>
#include <QDebug>
#include <QTimer>

#include "src/controller/appcontroller.h"
#include "src/view/dashboardwidget.h"
#include "src/model/cpumonitor.h"
#include "src/model/memorymonitor.h"
#include "src/model/networkmonitor.h"
#include "src/core/constants.h"
#include "src/view/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qDebug() << "=== PHASE 6 STEP 2: NETWORKMONITOR INTEGRATION TEST ===";

    // Create AppController
    AppController *appController = new AppController(&app);

    // Create monitors
    CPUMonitor *cpuMonitor = new CPUMonitor(&app);
    MemoryMonitor *memoryMonitor = new MemoryMonitor(&app);
    NetworkMonitor *networkMonitor = new NetworkMonitor(&app);

    // Connect to AppController ready signal
    QObject::connect(appController, &AppController::applicationReady, [=]() {
        qDebug() << "AppController ready - connecting all monitors";

        // Get MainWindow first
        MainWindow *mainWindow = appController->mainWindow();
        if (!mainWindow) {
            qCritical() << "MainWindow not available";
            return;
        }

        // Get Dashboard
        DashboardWidget *dashboard = mainWindow->dashboardWidget();
        if (!dashboard) {
            qCritical() << "Dashboard not available";
            return;
        }

        // Connect all monitors to dashboard
        dashboard->connectCPUMonitor(cpuMonitor);
        dashboard->connectMemoryMonitor(memoryMonitor);
        dashboard->connectNetworkMonitor(networkMonitor);

        // Start all monitoring
        cpuMonitor->start();
        memoryMonitor->start();
        networkMonitor->start();

        qDebug() << "Complete integration sucessfull - all monitors active";
    });

    // Initialize and start
    qDebug() << "Initializing AppController";
    if (!appController->initialize()) {
        qDebug() << "Failed to initialize: " << appController->lastError();
        return -1;
    }

    qDebug() << "Expected: MainWindow dashboard, CPU/RAM/NETWORK real-time monitoring";

    return app.exec();
}
