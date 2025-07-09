#ifndef BASEMONITOR_H
#define BASEMONITOR_H

#include <QObject>

class BaseMonitor : public QObject
{
    Q_OBJECT
public:
    explicit BaseMonitor(QObject *parent = nullptr);

signals:
};

#endif // BASEMONITOR_H
