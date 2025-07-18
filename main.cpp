#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QTimer>
#include <QRandomGenerator>
#include <QDebug>
#include "src/view/dashboardwidget.h"
#include "src/model/cpumonitor.h"
#include "src/model/memorymonitor.h"
#include "src/core/constants.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName(Constants::APP_NAME);
    qDebug() << "=== TESTING COMPLETE DASHBOARD ===";

    // Create test window
    QWidget *window = new QWidget;
    window->setWindowTitle("Complete Dashboard Test");
    window->resize(900, 700);
    window->setStyleSheet(
        "QWidget {"
        "   background-color: #2C3E50;"
        "   color: #ECF0F1;"
        "}"
        );

    QVBoxLayout *mainLayout = new QVBoxLayout(window);

    // Create dashboard
    DashboardWidget *dashboard = new DashboardWidget(window);
    mainLayout->addWidget(dashboard);

    // Create real monitors for integration test
    CPUMonitor *cpuMonitor = new CPUMonitor(&app);
    MemoryMonitor *memoryMonitor = new MemoryMonitor(&app);

    // Connect monitors to dashboard
    dashboard->connectCPUMonitor(cpuMonitor);
    dashboard->connectMemoryMonitor(memoryMonitor);

    // Test controls
    QHBoxLayout *controlLayout = new QHBoxLayout;

    QPushButton *startRealDataBtn = new QPushButton("Start Real Data", window);
    QPushButton *stopRealDataBtn = new QPushButton("Stop Real Data", window);
    QPushButton *fakeDataBtn = new QPushButton("Generate Fake Data", window);
    QPushButton *testThresholdBtn = new QPushButton("Test Thresholds", window);

    controlLayout->addWidget(startRealDataBtn);
    controlLayout->addWidget(stopRealDataBtn);
    controlLayout->addWidget(fakeDataBtn);
    controlLayout->addWidget(testThresholdBtn);

    mainLayout->addLayout(controlLayout);

    // Fake data timer
    QTimer *fakeDataTimer = new QTimer(window);

    // Connect controls
    QObject::connect(startRealDataBtn, &QPushButton::clicked, [=]() {
        cpuMonitor->start();
        memoryMonitor->start();
        fakeDataTimer->stop();
        qDebug() << "Real data monitoring started";
    });

    QObject::connect(stopRealDataBtn, &QPushButton::clicked, [=]() {
        cpuMonitor->stop();
        memoryMonitor->stop();
        fakeDataTimer->stop();
        qDebug() << "All monitoring stopped";
    });

    QObject::connect(fakeDataBtn, &QPushButton::clicked, [=]() {
        // Stop real data first
        cpuMonitor->stop();
        memoryMonitor->stop();

        // Start fake data updates
        fakeDataTimer->start(1500);  // Every 1.5 seconds
        qDebug() << "Fake data generation started";
    });

    QObject::connect(testThresholdBtn, &QPushButton::clicked, [=]() {
        // Test threshold colors
        static int testStep = 0;

        switch (testStep) {
        case 0:
            dashboard->updateCPUMetrics(50.0, 55.0);    // Normal - green
            dashboard->updateMemoryMetrics(60.0, "Test Normal");
            dashboard->updateStorageMetrics(45.0);
            qDebug() << "Testing normal thresholds (green)";
            break;
        case 1:
            dashboard->updateCPUMetrics(80.0, 75.0);    // Warning - yellow
            dashboard->updateMemoryMetrics(85.0, "Test Warning");
            dashboard->updateStorageMetrics(85.0);
            qDebug() << "Testing warning thresholds (yellow)";
            break;
        case 2:
            dashboard->updateCPUMetrics(95.0, 85.0);    // Critical - red
            dashboard->updateMemoryMetrics(98.0, "Test Critical");
            dashboard->updateStorageMetrics(95.0);
            qDebug() << "Testing critical thresholds (red)";
            break;
        }

        testStep = (testStep + 1) % 3;
    });

    QObject::connect(fakeDataTimer, &QTimer::timeout, [=]() {
        // Generate realstic fake data
        static double cpuBase = 45.0;
        static double ramBase = 65.0;
        static double storageBase = 72.0;

        // Gradual changes
        double cpuChange = (QRandomGenerator::global()->generateDouble() * 6.0) - 3.0;    // -3.0 to +3.0
        double ramChange = (QRandomGenerator::global()->generateDouble() * 2.0) - 1.0;    // -1.0 to +1.0
        double storageChange = (QRandomGenerator::global()->generateDouble() * 1.0) -0.5;

        cpuBase += cpuChange;
        ramBase += ramChange;
        storageBase += storageChange;

        // Keep in bounds
        cpuBase = qMax(10.0, qMin(90.0, cpuBase));
        ramBase = qMax(30.0, qMin(85.0, ramBase));
        storageBase = qMax(50.0, qMin(95.0, storageBase));

        double temVariation = QRandomGenerator::global()->generateDouble() * 20.0;
        double cpuTemp = 45.0 + temVariation;  // 45-65°C

        // Update dashboard
        dashboard->updateCPUMetrics(cpuBase, cpuTemp);
        dashboard->updateMemoryMetrics(ramBase, QString("%.1f GB / 16.0 GB").arg(ramBase * 16.0 / 100.0));
        dashboard->updateStorageMetrics(storageBase);

        double upSpeed = QRandomGenerator::global()->generateDouble() * 1.5 + 0.5;   // 0.5-2.0 MB/s
        double downSpeed = QRandomGenerator::global()->generateDouble() * 8.0 + 2.0;  // 2.0-10.0 MB/s
        dashboard->updateNetworkMetrics(upSpeed, downSpeed);

        qDebug() << "Fake data update - CPU:" << cpuBase << "RAM:" << ramBase << "Storage:" << storageBase;
    });

    // Initial data setup
    dashboard->updateCPUMetrics(45.5, 62.0);
    dashboard->updateMemoryMetrics(68.3, "6.8 GB / 16.0 GB");
    dashboard->updateNetworkMetrics(1.2, 5.8);
    dashboard->updateStorageMetrics(72.4);

    // Info label
    QLabel *infoLabel = new QLabel(
        "Complete Dashboard Test:\n"
        "• 2x2 grid layout matching demo design\n"
        "• Real-time system info bar (date, time, hostname, uptime)\n"
        "• Start Real Data: Connect to actual CPU/Memory monitors\n"
        "• Fake Data: Simulate realistic changing values\n"
        "• Test Thresholds: Cycle through normal/warning/critical colors\n"
        "• All cards should update smoothly với proper color changes", window);

    infoLabel->setStyleSheet("color: #ECF0F1l; font-size: 9px;");
    infoLabel->setWordWrap(true);
    mainLayout->addWidget(infoLabel);

    window->show();
    return app.exec();
}
