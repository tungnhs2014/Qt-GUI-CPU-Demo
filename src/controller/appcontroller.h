#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QTimer>
#include <QSettings>
#include <QApplication>

// Forward declarations
class MainWindow;
class DataController;
class UIController;
class AlertManager;

class AppController : public QObject
{
    Q_OBJECT
public:
    enum AppState {
        Stopped,
        Starting,
        Running,
        Stopping,
        Error
    };
    Q_ENUM(AppState)

    explicit AppController(QApplication *app, QObject *parent = nullptr);
    ~AppController();

    // Application lifecycle
    bool initialize();
    void startup();
    void shutdown();

    // State management
    AppState state() const {
        return m_state;
    }

    QString stateString() const;

    bool isRunning() const {
        return m_state == Running;
    }

    // Componnet access
    MainWindow *mainWindow() const {
        return m_mainWindow;
    }

    DataController *dataController() const {
        return m_dataController;
    }

    UIController *uiController() const {
        return m_uiController;
    }

    AlertManager *alertManager() const {
        return m_alertManager;
    }

    // Setting management
    QSettings *settings() const {
        return m_settings;
    }

    void saveSettings();
    void loadSettings();

    // Error handling
    QString lastError() const {
        return m_lastError;
    }

    bool hasError() const {
        return m_state == Error;
    }

signals:
    void stateChanged(AppState newState, AppState oldState);
    void applicationReady();
    void applicationShutdown();
    void errorOccurred(const QString &error);
    void settingsChanged();

public slots:
    void onApplicationAboutToQuit();
    void onControllerError(const QString &error);
    void onShutdonwRequested();

private slots:
    void onInitializationTimer();
    void onDataControllerStateChanged();
    void onUIControllerStateChanged();

private:
    // Initialization helpers
    bool createComponents();
    bool connectComponents();
    bool loadInitialSettings();
    void setupApplicationProperties();

    // State management
    void setState(AppState newState);
    void handStateTransition(AppState oldState, AppState newState);

    // Error handling
    void setError(const QString &error);
    void clearError();

    // Cleanup
    void cleanupComponents();

    // Application reference
    QApplication *m_application;

    // Current state
    AppState m_state = Stopped;
    QString m_lastError;

    // Core components
    MainWindow *m_mainWindow  = nullptr;
    DataController *m_dataController = nullptr;
    UIController *m_uiController = nullptr;
    AlertManager *m_alertManager = nullptr;

    // Settings
    QSettings *m_settings = nullptr;

    // Initializtion timer
    QTimer *m_initTimer = nullptr;
    bool m_componentsCreated = false;
    bool m_componentsConnected = false;

    // Shutdown hadling
    bool m_shutdownRequested = false;
    QTimer *m_shutdownTimer = nullptr;
};

#endif // APPCONTROLLER_H
