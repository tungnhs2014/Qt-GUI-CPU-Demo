// #include "mainwindow.h"
// #include "src/core/Constants.h"
// #include "src/core/SystemUtils.h"
// #include <QApplication>
// #include <QDebug>

// int main(int argc, char *argv[])
// {
//     QApplication app(argc, argv);

//     // Set application properties
//     app.setApplicationName(Constants::APP_NAME);
//     app.setApplicationVersion(Constants::APP_VERSION);
//     app.setOrganizationName(Constants::APP_ORGANIZATION);

//     qDebug() << "===TESTING SystemUltis===";

//     // Test basic system information
//     qDebug() << "\n--- System Information ---";
//     qDebug() << "Hostname: " << SystemUtils::getHostname();
//     qDebug() << "Current time: " << SystemUtils::getCurrentTime();
//     qDebug() << "Uptime: " << SystemUtils::getUptime();

//     // Test system monitoring
//     qDebug() << "\n--- System Monitoring ---";
//     qDebug() << "CPU Usage:" << SystemUtils::getCPUUsageSimple() << "%";
//     qDebug() << "Memory Usage:" << SystemUtils::getMemoryUsageSimple() << "%";
//     qDebug() << "Network Up:" << SystemUtils::getNetworkSpeedUp() << "MB/s";
//     qDebug() << "Network Down:" << SystemUtils::getNetworkSpeedDown() << "MB/s";
//     // Real data on Linux, fake data on Windows

//     // Test validation functions
//     qDebug() << "\n--- Validation Tests ---";
//     qDebug() << "Valid threshold 75.0:" << SystemUtils::isValidThreshold(75.0);
//     qDebug() << "Valid threshold 150.0:" << SystemUtils::isValidThreshold(150.0);
//     qDebug() << "Valid threshold -10.0:" << SystemUtils::isValidThreshold(-10.0);
//     // Should be: true, false, false

//     // Test string parsing
//     qDebug() << "\n--- String Parsing Tests ---";
//     bool ok;
//     double parsed = SystemUtils::parseDouble("123.45", &ok);
//     qDebug() << "Parse '123.45':" << parsed << "success:" << ok;

//     qint64 parsedInt = SystemUtils::parseInt64("9876543210", &ok);
//     qDebug() << "Parse '9876543210':" << parsedInt << "success:" << ok;

//     MainWindow window;
//     window.resize(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT);
//     window.setWindowTitle(Constants::APP_NAME + " v" + Constants::APP_VERSION);
//     window.show();

//     return app.exec();
// }
