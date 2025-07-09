#include "basemonitor.h"
#include "../../core/Constants.h"
#include <QDebug>

BaseMonitor::BaseMonitor(QObject *parent)
    : QObject{parent}
    , m_timer(new QTimer(this))
    , m_state(Stopped)
    , m_updateInterval(Constants::UPDATE_INTERVAL)
{
    // Connectt timer too our slot
    connect(m_timer, &QTimer::timeout, this, &BaseMonitor::onTimerTimeout);

    m_timer->setInterval(m_updateInterval);
}

BaseMonitor::~BaseMonitor()
{
    // QTimer will be auto-deleted by Qt parent system
    stop();
}

void BaseMonitor::start()
{
    if (m_state == Running) {
        return;  // Already running
    }

    qDebug() << "Starting monitor...";

    // Change state and notify listeners
    setState(Running);

    m_timer->start();

    updateNow();
}

void BaseMonitor::stop()
{
    if (m_state == Stopped) {
        return;     // Already stopped
    }

    qDebug() << "Stopping monitor...";

    m_timer->stop();

    setState(Stopped);
}

void BaseMonitor::updateNow()
{
    if (m_state == Error) {
        // Don't update if in error state
        return;
    }

    try {
        // Pure virtual method - specific to each monitor type
        collectData();

        if (m_state != Running) {
            setState(Running); // Recover from error state
        }

        // Notify listeners that new data available
        emit dateUpdate();

    } catch (const std::exception &e) {
        emitError(QString("Data collection failed: %1").arg(e.what()));
    } catch (...) {
        emitError("Unknown error during data collection");
    }
}

void BaseMonitor::setUpdateInterval(int milliseconds)
{
    if (milliseconds <= 0) {
        qDebug() << "Invalid update interval:" << milliseconds;
        return;
    }

    // Change takes effect immediately if timer running
    m_updateInterval = milliseconds;
    m_timer->setInterval(milliseconds);

    qDebug() << "Update interval set to:" << milliseconds << "ms";
}

int BaseMonitor::updateInterval() const
{
    return m_updateInterval;
}

void BaseMonitor::setState(MonitorState newState)
{
    if (m_state == newState) {
        return;     // No change
    }

    MonitorState oldState = m_state;
    m_state = newState;

    qDebug() << "Monitor state changed from " << oldState << " to" << newState;

    // UI can update status indicators
    emit stateChanged(newState);
}

void BaseMonitor::emitError(const QString &message)
{
    qDebug() << "Monitor error: " << message;

    // Report error and change state
    setState(Error);
    emit errorOccurred(message);
}

void BaseMonitor::onTimerTimeout()
{
    // Called every m_updateInterval milliseconds
    updateNow();
}
