#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

namespace Constants
{
    // Application Information
    const QString APP_NAME = "System Monitor";
    const QString APP_VERSION = "1.0.0";
    const QString APP_ORGANIZATION  = "EmbeddedSystems";

    // Update Intervals (milliseconds)
    const int UPDATE_INTERVAL = 1000;
    const int FAST_UPDATE_INTERVAL = 500;

    // Alert Thresholds (percentage)
    const double CPU_WARNING_THRESHOLD = 75.0;
    const double CPU_CRITICAL_THRESHOLD = 90.0;
    const double RAM_WARNING_THRESHOLD = 80.0;
    const double RAM_CRITICAL_THRESHOLD = 95.0;

    // UI Dimensions
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;
    const int MIN_WINDOW_WIDTH = 480;
    const int MIN_WINDOW_HEIGHT = 320;

    // System File Paths (Linux /proc filesystem)
    const QString PROC_STAT = "/proc/stat";
    const QString PROC_MEMINFO = "/proc/meminfo";
    const QString PROC_NET_DEV = "/proc/net/dev";
    const QString PROC_UPTIME = "/proc/uptime";

    // Colors (Hex values)
    const QString CPU_COLOR = "#E74C3C";           // Red
    const QString RAM_COLOR = "#3498DB";           // Blue
    const QString WARNING_COLOR = "#F39C12";       // Orange
    const QString CRITICAL_COLOR = "#E74C3C";      // Red
    const QString SUCCESS_COLOR = "#27AE60";       // Green
}

#endif // CONSTANTS_H
