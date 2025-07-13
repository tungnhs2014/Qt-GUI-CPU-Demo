#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QTimer>
#include <QDebug>
#include "src/view/widgets/circularprogress.h"
#include "src/core/constants.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName(Constants::APP_NAME);

    qDebug() << "=== TESTING CIRCULARPROGRESS ONLY ===";

    // Create test window
    QWidget *window = new QWidget;
    window->setWindowTitle("CircularProgress Test");
    window->resize(600, 400);
    window->setStyleSheet("background-color: #2C3E50;");

    QVBoxLayout *mainLayout = new QVBoxLayout(window);

    // Create multiple CircularProgress widgets for testing
    QHBoxLayout *progressLayout = new QHBoxLayout;

    // CPU Progress
    CircularProgress *cpuProgress = new CircularProgress(window);
    cpuProgress->setTitle("CPU");
    cpuProgress->setColor(QColor(Constants::CPU_COLOR));
    cpuProgress->setValue(45.5);
    progressLayout->addWidget(cpuProgress);

    // RAM Progress
    CircularProgress *ramProgress = new CircularProgress(window);
    ramProgress->setTitle("RAM");
    ramProgress->setColor(QColor(Constants::RAM_COLOR));
    ramProgress->setValue(78.3);
    progressLayout->addWidget(ramProgress);

    // Storage Progress
    CircularProgress *storageProgress = new CircularProgress(window);
    storageProgress->setTitle("Storage");
    storageProgress->setColor(QColor("#9B59B6"));
    storageProgress->setValue(67.2);
    progressLayout->addWidget(storageProgress);

    mainLayout->addLayout(progressLayout);

    // Test controls
    QHBoxLayout *controlLayout = new QHBoxLayout;

    // Value slider
    QLabel *sliderLabel = new QLabel("Test Value:", window);
    sliderLabel->setStyleSheet("color: white;");
    QSlider *valueSlider = new QSlider(Qt::Horizontal, window);
    valueSlider->setRange(0, 100);
    valueSlider->setValue(50);
    QLabel *valueLabel = new QLabel("50%", window);
    valueLabel->setStyleSheet("color: white;");

    controlLayout->addWidget(sliderLabel);
    controlLayout->addWidget(valueSlider);
    controlLayout->addWidget(valueLabel);

    mainLayout->addLayout(controlLayout);

    // Connect slider to all progress widgets
    QObject::connect(valueSlider, &QSlider::valueChanged, [=](int value) {
        cpuProgress->setValue(value);
        ramProgress->setValue(value);
        storageProgress->setValue(value);
        valueLabel->setText(QString::number(value) + "%");
        qDebug() << "Slider value changed to:" << value;
    });

    // Auto-animation test
    QTimer *animTimer = new QTimer(window);
    int animValue = 0;
    bool increasing = true;

    QObject::connect(animTimer, &QTimer::timeout, [&]() {
        if (increasing) {
            animValue += 2;
            if (animValue >= 100) increasing = false;
        } else {
            animValue -= 2;
            if (animValue <= 0) increasing = true;
        }

        // Update one widget for auto-animation demo
        storageProgress->setValue(animValue);
        qDebug() << "Auto-animation value:" << animValue;
    });

    // Control buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout;

    QPushButton *startAnimBtn = new QPushButton("Start Auto Animation", window);
    QPushButton *stopAnimBtn = new QPushButton("Stop Auto Animation", window);
    QPushButton *testColorsBtn = new QPushButton("Test Colors", window);

    QObject::connect(startAnimBtn, &QPushButton::clicked, [=]() {
        animTimer->start(100);  // 100ms intervals
        qDebug() << "Auto-animation started";
    });

    QObject::connect(stopAnimBtn, &QPushButton::clicked, [=]() {
        animTimer->stop();
        qDebug() << "Auto-animation stopped";
    });

    QObject::connect(testColorsBtn, &QPushButton::clicked, [=]() {
        static bool colorTest = false;
        if (!colorTest) {
            cpuProgress->setColor(QColor("#E74C3C"));  // Red
            ramProgress->setColor(QColor("#F39C12"));  // Orange
            storageProgress->setColor(QColor("#27AE60")); // Green
        } else {
            cpuProgress->setColor(QColor(Constants::CPU_COLOR));
            ramProgress->setColor(QColor(Constants::RAM_COLOR));
            storageProgress->setColor(QColor("#9B59B6"));
        }
        colorTest = !colorTest;
        qDebug() << "Colors changed, test mode:" << colorTest;
    });

    buttonLayout->addWidget(startAnimBtn);
    buttonLayout->addWidget(stopAnimBtn);
    buttonLayout->addWidget(testColorsBtn);

    mainLayout->addLayout(buttonLayout);

    // Info label
    QLabel *infoLabel = new QLabel(
        "CircularProgress Test:\n"
        "• Use slider to test manual value changes\n"
        "• Auto animation tests smooth transitions\n"
        "• Color button tests theme changes\n"
        "• All widgets should show smooth animations", window);
    infoLabel->setStyleSheet("color: #ECF0F1; font-size: 10px;");
    infoLabel->setWordWrap(true);
    mainLayout->addWidget(infoLabel);

    window->show();

    qDebug() << "CircularProgress test window displayed";
    qDebug() << "Expected: 3 circular progress widgets with different colors";
    qDebug() << "Expected: Smooth animations when values change";
    qDebug() << "Expected: Interactive controls working";

    return app.exec();
}
