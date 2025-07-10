#ifndef SYSTEMUTILS_H
#define SYSTEMUTILS_H

#include <QString>
#include <QStringList>

class SystemUtils
{
public:
    /*File operations - Core system interface */
    static QString readFile(const QString &filePath);
    static QStringList readFileLines(const QString &filePath);
    static double parseDouble(const QString &str, bool *ok = nullptr);
    static qint64 parseInt64(const QString &str, bool *ok = nullptr);
    static QString getHostname();
    static QString getCurrentTime();
    static QString getUptime();
    static double getCPUUsageSimple();
    static double getMemoryUsageSimple();
    static double getNetworkSpeedUp();
    static double getNetworkSpeedDown();
    static bool isValidThreshold(double value);
    static bool fileExits(const QString &path);
    static bool executeCommand(const QString &command);
};

#endif /* SYSTEMUTILS_H */
