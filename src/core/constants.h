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
    const int UPDATE_INTERVAL = 1000;              // General monitoring updates
    const int FAST_UPDATE_INTERVAL = 500;          // High-frequency updates
    const int SLOW_UPDATE_INTERVAL = 5000;         // Low-frequency updates
    const int NETWORK_UPDATE_INTERVAL = 2000;      // Network speed calculation
    const int STORAGE_UPDATE_INTERVAL = 5000;      // Storage monitoring
    const int ALERT_CHECK_INTERVAL = 3000;         // Alert threshold checking
    const int ALERT_CLEANUP_INTERVAL = 300000;     // Alert cleanup (5 minutes)

    // Alert Thresholds (percentage)
    const double CPU_WARNING_THRESHOLD = 75.0;
    const double CPU_CRITICAL_THRESHOLD = 90.0;
    const double RAM_WARNING_THRESHOLD = 80.0;
    const double RAM_CRITICAL_THRESHOLD = 95.0;
    const double TEMP_WARNING_THRESHOLD = 70.0;    // Celsius
    const double TEMP_CRITICAL_THRESHOLD = 80.0;   // Celsius

    // UI Dimensions
    const int WINDOW_WIDTH = 800;
    const int WINDOW_HEIGHT = 600;
    const int MIN_WINDOW_WIDTH = 480;
    const int MIN_WINDOW_HEIGHT = 320;
    const int CIRCULAR_PROGRESS_SIZE = 120;

    // System File Paths (Linux /proc filesystem)
    const QString PROC_STAT = "/proc/stat";
    const QString PROC_MEMINFO = "/proc/meminfo";
    const QString PROC_CPUINFO = "/proc/cpuinfo";
    const QString PROC_NET_DEV = "/proc/net/dev";
    const QString PROC_MOUNTS = "/proc/mounts";
    const QString PROC_VERSION = "/proc/version";
    const QString PROC_UPTIME = "/proc/uptime";

    // Colors (Hex values)
    // Color Schemes (Hex values)
    const QString CPU_COLOR = "#E74C3C";           // Red
    const QString RAM_COLOR = "#3498DB";           // Blue
    const QString NETWORK_UP_COLOR = "#27AE60";    // Green
    const QString NETWORK_DOWN_COLOR = "#F39C12";  // Orange
    const QString STORAGE_COLOR = "#9B59B6";       // Purple
    const QString WARNING_COLOR = "#F39C12";       // Orange
    const QString CRITICAL_COLOR = "#E74C3C";      // Red
    const QString SUCCESS_COLOR = "#27AE60";       // Green
    const QString INFO_COLOR = "#3498DB";          // Blue

    // Settings Keys (for QSettings persistence)
    const QString SETTINGS_GROUP_THRESHOLDS = "Thresholds";
    const QString SETTINGS_GROUP_UI = "UserInterface";
    const QString SETTINGS_GROUP_GENERAL = "General";
    const QString SETTINGS_CPU_WARNING = "CPUWarning";
    const QString SETTINGS_CPU_CRITICAL = "CPUCritical";
    const QString SETTINGS_RAM_WARNING = "RAMWarning";
    const QString SETTINGS_RAM_CRITICAL = "RAMCritical";
    const QString SETTINGS_THEME = "Theme";
    const QString SETTINGS_UPDATE_INTERVAL = "UpdateInterval";
    const QString SETTINGS_WINDOW_GEOMETRY = "window_geometry";
    const QString SETTINGS_WINDOW_STATE = "window_state";
    const QString SETTINGS_AUTOSTART = "autostart";
    const QString SETTINGS_ALERTS_ENABLED = "alerts_enabled";
    const QString SETTINGS_MONITORING_ENABLED = "monitoring_enabled";

    // Controller States (for MVC pattern)
    const QString STATE_STOPPED = "Stopped";
    const QString STATE_STARTING = "Starting";
    const QString STATE_RUNNING = "Running";
    const QString STATE_STOPPING = "Stopping";
    const QString STATE_ERROR = "Error";

    // Network Interface Names (common Linux interfaces)
    const QStringList NETWORK_INTERFACES = {
        "eth0", "wlan0", "enp0s3", "wlp2s0", "ens33", "ens32"
    };

    // Default Values
    const int DEFAULT_HISTORY_SIZE = 60;           // Keep 60 data points
    const int MAX_ALERTS_HISTORY = 100;            // Maximum alerts to store
    const double EPSILON = 0.001;                  // For floating point comp

    // File size constants
    const qint64 BYTES_PER_KB = 1024;
    const qint64 BYTES_PER_MB = 1024 * 1024;
    const qint64 BYTES_PER_GB = 1024 * 1024 * 1024;
}

#endif // CONSTANTS_H
