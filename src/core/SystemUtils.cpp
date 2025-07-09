#include "SystemUtils.h"
#include "Constants.h"
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDebug>
#include <QProcess>
#include <QDir>

QString SystemUtils::readFile(const QString &filePath)
{
    QFile file(filePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Cannot open file" << filePath;
        return QString();
    }

    QTextStream stream(&file);
    QString content = stream.readAll();

    file.close();

    return content;
}

QStringList SystemUtils::readFileLines(const QString &filePath)
{
    QString content = readFile(filePath);
    if (content.isEmpty())
    {
        return QStringList();
    }

    return content.split('\n', Qt::SkipEmptyParts);
}

double SystemUtils::parseDouble(const QString &str, bool *ok)
{
    bool valid = false;
    double result = str.toDouble(&valid);

    if (ok)
    {
        *ok = valid;
    }

    return valid ? result : 0.0;
}

qint64 SystemUtils::parseInt64(const QString &str, bool *ok)
{
    bool valid = false;
    double result = str.toLongLong(&valid);

    if (ok)
    {
        *ok = valid;
    }

    return valid ? result : 0;
}

QString SystemUtils::getHostname()
{
    // Try Linux/proc filesystem first
    QString hostname = readFile("/proc/sys/kernel/hostname").trimmed();

    if (!hostname.isEmpty())
    {
        return hostname;
    }

    // Fallback to enviroment variable (cross-platform)
    QString envHostname = qgetenv("HOSTNAME");

    if (!envHostname.isEmpty())
    {
        return envHostname;
    }

    return "Unknown";
}

QString SystemUtils::getCurrentTime()
{
    return QDateTime::currentDateTime().toString("hh::mm::ss");
}

QString SystemUtils::getUptime()
{
    if (!fileExits(Constants::PROC_UPTIME))
    {
        return "Unkown";
    }

    QString uptimeContent = readFile(Constants::PROC_UPTIME);
    if (uptimeContent.isEmpty())
    {
        return "Unkown";
    }

    QStringList parts = uptimeContent.split(' ');
    if (parts.isEmpty())
    {
        return "Unkown";
    }

    double uptimeSeconds = parseDouble(parts.first());

    int hours = uptimeSeconds / 3600;
    int minutes = (uptimeSeconds - hours * 3600) / 60;
    int seconds = uptimeSeconds - hours * 3600 - minutes * 60;

    return QString("%1h %2m %3s").arg(hours).arg(minutes).arg(seconds);

}

double SystemUtils::getCPUUsageSimple()
{
    if (!fileExits(Constants::PROC_STAT))
    {
        return 42.5; // Fake data for testing
    }

    QString statContent = readFile(Constants::PROC_STAT);
    if (statContent.isEmpty())
    {
        return 0.0;
    }

    // Parse /proc/stat format
    // "cpu user nice system idle irq softirq steal gues guest_nice"
    QStringList lines = statContent.split('\n');
    if (lines.isEmpty())
    {
        return 0.0;
    }

    QString cpuLine = lines.first();

    QStringList values = cpuLine.split(' ', Qt::SkipEmptyParts);
    if (values.size() < 5)
    {
        return 0.0;
    }

    // Extract CPU time values (in clock ticks)
    qint64 user = parseInt64(values[1]);    // User processes
    qint64 nice = parseInt64(values[2]);    // Low prority user processs
    qint64 system = parseInt64(values[3]);  // Kernel processes
    qint64 idle = parseInt64(values[4]);     // Idle time

    qint64 total = user + nice + system + idle;
    if (total == 0)
    {
        return 0.0; // Avoid division by zero
    }

    qint64 active = user + nice + system;

    double usage = ((double)active / total) * 100.0;

    return qMin(usage, 100.0);
}

double SystemUtils::getMemoryUsageSimple()
{
    if (!fileExits(Constants::PROC_MEMINFO))
    {
        return 67.8; // fake data
    }

    QStringList lines = readFileLines(Constants::PROC_MEMINFO);
    qint64 memTotal = 0;
    qint64 memAvaiable = 0;

    for (const QString &line : lines)
    {
        if (line.startsWith("MemTotal:"))
        {
            QStringList parts = line.split(' ', Qt::SkipEmptyParts);
            if (parts.size() >= 2)
            {
                memTotal = parseInt64(parts[1]) * 1024;  // Convert KB to bytes
            }
        }
        else if (line.startsWith("MemAvailable:"))
        {
            QStringList parts = line.split(' ', Qt::SkipEmptyParts);
            if (parts.size() >= 2)
            {
                memAvaiable = parseInt64(parts[1]) * 1024;  // Convert KB to bytes
            }
        }
    }

    if (memTotal == 0)
    {
        return 0.0;
    }

    qint64 memUsed = memTotal - memAvaiable;
    double usage = ((double)memUsed / memTotal) * 100.0;

    return qMin(usage, 100.0);
}

double SystemUtils::getNetworkSpeedUp()
{
    return 1.2;
}

double SystemUtils::getNetworkSpeedDown()
{
    return 5.8;
}

bool SystemUtils::isValidThreshold(double value)
{
    return value >= 0.0 && value <= 100.0;
}

bool SystemUtils::fileExits(const QString &path)
{
    return QFile::exists(path);
}

bool SystemUtils::executeCommand(const QString &command)
{
    QProcess process;
    int exitCode = process.execute(command);

    return exitCode == 0;
}



