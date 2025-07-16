#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QTimer>
#include <QRandomGenerator>
#include <QDebug>
#include "src/view/widgets/metriccard.h"
#include "src/core/constants.h"

double randomBetween(double min, double max) {
    return min + QRandomGenerator::global()->generateDouble() * (max - min);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName(Constants::APP_NAME);

    qDebug() << "=== TESTING METRICCARD + CIRCULARPROGRESS ===";

    // Create test window
    QWidget *window = new QWidget;
    window->setWindowTitle("MetricCard Test");
    window->resize(800, 600);
    window->setStyleSheet("background-color: #2C3E50;");

    QVBoxLayout *mainLayout = new QVBoxLayout(window);

    // Create different types of MetricCards
    QHBoxLayout *cardsLayout = new QHBoxLayout;

    // CPU Card (CircularType)
    MetricCard *cpuCard = new MetricCard(MetricCard::CircularType, window);
    cpuCard->setTitle("CPU");
    cpuCard->setColor(QColor(Constants::CPU_COLOR));
    cpuCard->setValue(75.5);
    cpuCard->setSubtitle("Temp: 65°C");
    cardsLayout->addWidget(cpuCard);

    // RAM Card (CircularType)
    MetricCard *ramCard = new MetricCard(MetricCard::CircularType, window);
    ramCard->setTitle("RAM");
    ramCard->setColor(QColor(Constants::RAM_COLOR));
    ramCard->setValue(68.3);
    ramCard->setSubtitle("6.8GB / 16GB");
    cardsLayout->addWidget(ramCard);

    // Network Card (NetworkType)
    MetricCard *networkCard = new MetricCard(MetricCard::NetworkType, window);
    networkCard->setTitle("Network");
    networkCard->setColor(QColor("#27AE60"));
    networkCard->setNetworkSpeeds(1.2, 5.8);
    networkCard->setSubtitle("Wi-Fi Connected");
    cardsLayout->addWidget(networkCard);

    // System Info Card (TextType)
    MetricCard *infoCard = new MetricCard(MetricCard::TextType, window);
    infoCard->setTitle("System");
    infoCard->setColor(QColor("#9B59B6"));
    infoCard->setText("Online");
    infoCard->setSubtitle("3d 4h 15m");
    cardsLayout->addWidget(infoCard);

    mainLayout->addLayout(cardsLayout);

    // Test controls
    QHBoxLayout *controlLayout = new QHBoxLayout;

    QPushButton *updateValuesBtn = new QPushButton("Update Random Values", window);
    QPushButton *testColorsBtn = new QPushButton("Test Colors", window);
    QPushButton *startAnimBtn = new QPushButton("Start Auto Update", window);
    QPushButton *stopAnimBtn = new QPushButton("Stop Auto Update", window);

    controlLayout->addWidget(updateValuesBtn);
    controlLayout->addWidget(testColorsBtn);
    controlLayout->addWidget(startAnimBtn);
    controlLayout->addWidget(stopAnimBtn);

    mainLayout->addLayout(controlLayout);

    // Auto-update timer
    QTimer *updateTimer = new QTimer(window);

    // Connect buttons
    QObject::connect(updateValuesBtn, &QPushButton::clicked, [=]() {
        // Generate random values
        double cpuVal = QRandomGenerator::global()->bounded(100.0);
        double ramVal = QRandomGenerator::global()->bounded(100.0);
        double upSpeed = QRandomGenerator::global()->bounded(10.0);
        double downSpeed = QRandomGenerator::global()->bounded(50.0);

        cpuCard->setValue(cpuVal);
        ramCard->setValue(ramVal);
        networkCard->setNetworkSpeeds(upSpeed, downSpeed);
        infoCard->setText(QString::number(cpuVal, 'f', 1) + "%");

        qDebug() << "Random values - CPU:" << cpuVal << "RAM:" << ramVal
                 << "Network:" << upSpeed << "/" << downSpeed;
    });

    QObject::connect(testColorsBtn, &QPushButton::clicked, [=]() {
        static bool testMode = false;
        if (!testMode) {
            cpuCard->setColor(QColor("#E74C3C"));    // Critical red
            ramCard->setColor(QColor("#F39C12"));    // Warning orange
            networkCard->setColor(QColor("#27AE60")); // Success green
            infoCard->setColor(QColor("#9B59B6"));    // Info purple
        } else {
            cpuCard->setColor(QColor(Constants::CPU_COLOR));
            ramCard->setColor(QColor(Constants::RAM_COLOR));
            networkCard->setColor(QColor("#27AE60"));
            infoCard->setColor(QColor("#9B59B6"));
        }
        testMode = !testMode;
        qDebug() << "Color test mode:" << testMode;
    });

    QObject::connect(startAnimBtn, &QPushButton::clicked, [=]() {
        updateTimer->start(2000);  // Update every 2 seconds
        qDebug() << "Auto-update started";
    });

    QObject::connect(stopAnimBtn, &QPushButton::clicked, [=]() {
        updateTimer->stop();
        qDebug() << "Auto-update stopped";
    });

    QObject::connect(updateTimer, &QTimer::timeout, [=]() {
        // Simulate realistic system changes
        static double cpuBase = 45.0;
        static double ramBase = 65.0;

        cpuBase += randomBetween(-5.0, 5.0);
        ramBase += randomBetween(-2.0, 2.0);

        cpuBase = qMax(0.0, qMin(100.0, cpuBase));
        ramBase = qMax(0.0, qMin(100.0, ramBase));

        cpuCard->setValue(cpuBase);
        ramCard->setValue(ramBase);

        double upSpeed = QRandomGenerator::global()->bounded(5.0);
        double downSpeed = QRandomGenerator::global()->bounded(20.0);
        networkCard->setNetworkSpeeds(upSpeed, downSpeed);

        qDebug() << "Auto-update - CPU:" << cpuBase << "RAM:" << ramBase;
    });

    // Info label
    QLabel *infoLabel = new QLabel(
        "MetricCard Test:\n"
        "• 4 different card types displayed\n"
        "• CPU & RAM: CircularProgress integration\n"
        "• Network: Up/down speed display\n"
        "• System: Simple text display\n"
        "• All cards should show smooth animations\n"
        "• Colors and styling should be consistent", window);
    infoLabel->setStyleSheet("color: #ECF0F1; font-size: 10px;");
    infoLabel->setWordWrap(true);
    mainLayout->addWidget(infoLabel);

    window->show();

    qDebug() << "MetricCard test window displayed";
    qDebug() << "Expected: 4 styled cards with different content types";
    qDebug() << "Expected: Smooth value updates with animations";
    qDebug() << "Expected: Professional card styling with headers";

    return app.exec();
}
