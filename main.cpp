#include "src/view/mainwindow.h"
#include "src/core/constants.h"
#include "src/model/cpumonitor.h"
#include "src/model/memorymonitor.h"
#include "src/view/dashboardwidget.h"
#include "src/controller/appcontroller.h"
#include <QApplication>
#include <QTimer>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qDebug() << "=== PHASE 6: SIMPLE APPCONTROLLER INTEGRATION TEST ===";

    // Create AppController (Phase 6)
    AppController *appController = new AppController(&app);

    // Create existing monitors (Phase 4)
    CPUMonitor *cpuMonitor = new CPUMonitor(&app);
    MemoryMonitor *memoryMonitor = new MemoryMonitor(&app);

    QObject::connect(appController, &AppController::applicationReady, [=]() {
        qDebug() << "Application ready - connecting existing system";

        // Get dashboard from mainwindow
        DashboardWidget *dashboard = appController->mainWindow()->dashboardWidget();

        // Connect monitors to dashboard (Phase 4 + 5 integration)
        dashboard->connectCPUMonitor(cpuMonitor);
        dashboard->connectMemoryMonitor(memoryMonitor);

        // Start monitoring
        cpuMonitor->start();
        memoryMonitor->start();

        qDebug() << "Complete integration successful";
        qDebug() << "MainWindow displayed with real-time dashboard";
    });

    qDebug() << "Initializing AppController...";
    if (!appController->initialize()) {
        qDebug() << "Failed to initialize:" << appController->lastError();
        return -1;
    }

    qDebug() << "Expected: MainWindow với dashboard và real-time monitoring";

    return app.exec();
}
