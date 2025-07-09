#include "src/model/base/basemonitor.h"
#include <QApplication>
#include <QDebug>
#include <QTimer>

class SimpleTestMonitor : public BaseMonitor
{
    Q_OBJECT
public:
    explicit SimpleTestMonitor(QObject *parent = nullptr)
        : BaseMonitor(parent), callCount(0) {}

    int callCount;

protected:
    void collectData() override {
        callCount++;
        qDebug() << "SimpleTestMonitor collectData called:" << callCount;

        // Simulate some work
        if (callCount == 5) {
            emitError("Simulated error at call 5");
        }
    }
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    qDebug() << "BaseMonitor Isolated Test";

    SimpleTestMonitor monitor;

    // Test all BaseMonitor functionality
    monitor.start();

    QTimer::singleShot(6000, [&]() {
        monitor.stop();
        qDebug() << "Final call count:" << monitor.callCount;
        app.quit();
    });

    return app.exec();
}

#include "test_basemonitor.moc"
