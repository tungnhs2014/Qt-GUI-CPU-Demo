#ifndef BASEMONITOR_H
#define BASEMONITOR_H

#include <QObject>
#include <QTimer>
#include <QString>

class BaseMonitor : public QObject
{
    Q_OBJECT
public:
    enum MonitorState {
        Stopped,
        Running,
        Error
    };

    explicit BaseMonitor(QObject *parent = nullptr);
    virtual ~BaseMonitor();

    // Set uo QTimer, changes state to Running
    virtual void start();
    // Stop QTimer, changes state to Stopped
    virtual void stop();
    // Useful for manual refresh
    virtual void updateNow();

    // State management
    MonitorState state() const {
        return m_state;
    }

    // Convenince method check if active
    bool isRunning() const {
        return m_state == Running;
    }

    // Configuration
    void setUpdateInterval(int milliseconds);
    int updateInterval() const;

signals:
    // Controller connect to this signal
    void dateUpdate();
    // UI can display error messages
    void errorOccurred(const QString &error);
    // UI can update status indicators
    void stateChanged(MonitorState newState);

protected:
    // Core data collection logic goes here
    virtual void collectData() = 0;

    // Used by subclasses to report state changes
    void setState(MonitorState newState);

    // Centralized error handling
    void emitError(const QString &message);

protected slots:
    // Calls collectData() periodcally
    void onTimerTimeout();

private:
    QTimer *m_timer;        // Update timer
    MonitorState m_state;   // Currnet state
    int m_updateInterval;   // Update frequency (ms)

    // Disable copy constructor/assignment
    BaseMonitor(const BaseMonitor &) = delete;
    BaseMonitor &operator=(const BaseMonitor &) = delete;
};

#endif // BASEMONITOR_H
