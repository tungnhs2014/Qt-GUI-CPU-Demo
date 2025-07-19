#include "appcontroller.h"
#include "../view/mainwindow.h"
#include "../core/constants.h"
#include <QDebug>
#include <QTimer>
#include <QDir>
#include <QStandardPaths>

AppController::AppController(QApplication *app, QObject *parent)
    : QObject(parent)
    , m_application(app)
    , m_initTimer(new QTimer(this))
    , m_shutdownTimer(new QTimer(this))
{
    // Setup application properties
    setupApplicationProperties();

    // Connect application signals
    connect(m_application, &QApplication::aboutToQuit, this, &AppController::onApplicationAboutToQuit);

    // Setup initialization timer
    m_initTimer->setSingleShot(true);
    connect(m_initTimer, &QTimer::timeout, this, &AppController::onInitializationTimer);

    // Setup shutdown timer
    m_shutdownTimer->setSingleShot(true);
    m_shutdownTimer->setInterval(5000);     // 5 seconds timeout for shutdown

    qDebug() << "AppController created";
}

AppController::~AppController()
{
    cleanupComponents();
    qDebug() << "Application destroyed";
}

bool AppController::initialize()
{
    qDebug() << "AppController::initialize() - Starting initialization";

    if (m_state != Stopped) {
        setError("Cannot initialize - application not in Stopped state");
        return false;
    }

    setState(Starting);

    // Create settings first
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QDir().mkpath(configPath);
    QString settingsPath = configPath + "/" + Constants::APP_NAME.toLower().replace(" ", "_") + ".ini";

    m_settings = new QSettings(settingsPath, QSettings::IniFormat, this);
    qDebug() << "Settings file:" << settingsPath;

    m_settings = new QSettings(settingsPath, QSettings::IniFormat, this);
    qDebug() << "Settings file: " << settingsPath;

    // Load initial settings
    if (!loadInitialSettings()) {
        setError("Failed to load initial settings");
        return false;
    }

    // Create components
    if (!createComponents()) {
        setError("Failed to create components");
        return false;
    }

    // Connect components (delayed to allow proper initialization)
    m_initTimer->start(100); // 100ms delay

    return true;
}

void AppController::startup()
{
    qDebug() << "AppController::startup() - Starting application";

    if (m_state != Starting) {
        qDebug() << "Cannot startup - not in Starting state, current state:" << stateString();
        return;
    }

    // Show main window
    if (m_mainWindow) {
        m_mainWindow->show();
        qDebug() << "Main window displayed";
    }

    // Start data monitoring
    if (m_dataController) {
        // Will implement when DataController is ready
        qDebug() << "DataController ready for startup";
    }

    setState(Running);
    emit applicationReady();

    qDebug() << "Application startup complete";
}

void AppController::shutdown()
{
    qDebug() << "AppController::shutdown() - Starting shutdown";

    if (m_state == Stopped || m_state == Stopping) {
        qDebug() << "Already stopping or stopped";
        return;
    }

    setState(Stopping);
    m_shutdownRequested = true;

    // Save settings before shutdown
    saveSettings();

    // Stop data monitoring
    if (m_dataController) {
        // Will implement when DataController is ready
        qDebug() << "DataController stopping";
    }

    // Hide main window
    if (m_mainWindow) {
        m_mainWindow->hide();
        qDebug() << "Main windwo hidden";
    }

    // Complete shutdown
    setState(Stopped);
    emit applicationShutdown();

    qDebug() << "Application shutdown complete";
}

QString AppController::stateString() const
{
    switch (m_state) {
    case Stopped:
        return Constants::STATE_STOPPED;
        break;
    case Starting:
        return Constants::STATE_STARTING;
        break;
    case Running:
        return Constants::STATE_RUNNING;
        break;
    case Stopping:
        return Constants::STATE_STOPPING;
        break;
    case Error:
        return Constants::STATE_ERROR;
        break;
    }

    return "Unknown";
}

void AppController::saveSettings()
{
    if (!m_settings) {
        return;
    }

    qDebug() << "Saving application settings";

    // Save window geometry
    if (m_mainWindow) {
        m_settings->setValue(Constants::SETTINGS_WINDOW_GEOMETRY, m_mainWindow->saveGeometry());
        m_settings->setValue(Constants::SETTINGS_WINDOW_STATE, m_mainWindow->saveState());
    }

    // Save other settings
    m_settings->setValue(Constants::SETTINGS_UPDATE_INTERVAL, Constants::UPDATE_INTERVAL);
    m_settings->setValue(Constants::SETTINGS_ALERTS_ENABLED, true);
    m_settings->setValue(Constants::SETTINGS_MONITORING_ENABLED, true);

    m_settings->sync();
    emit settingsChanged();
}

void AppController::loadSettings()
{
    if (!m_settings) {
        return;
    }

    qDebug() << "Loading application settings";

    // Restore window geometry
    if (m_mainWindow) {
        QByteArray geometry = m_settings->value(Constants::SETTINGS_WINDOW_GEOMETRY).toByteArray();
        QByteArray state = m_settings->value(Constants::SETTINGS_WINDOW_STATE).toByteArray();

        if (!geometry.isEmpty()) {
            m_mainWindow->restoreGeometry(geometry);
        }
        if (!state.isEmpty()) {
            m_mainWindow->restoreState(state);
        }
    }

    emit settingsChanged();
}

void AppController::onApplicationAboutToQuit()
{
    qDebug() << "Application about to quit - triggering shutdown";
    if (!m_shutdownRequested) {
        shutdown();
    }
}

void AppController::onControllerError(const QString &error)
{
    setError("Controller error: " + error);
}

void AppController::onShutdonwRequested()
{
    qDebug() << "Shutdown requested by user";
    m_application->quit();
}

void AppController::onInitializationTimer()
{
    qDebug() << "AppController::onInitializationTimer() - Connecting components";

    if (!connectComponents()) {
        setError("Failed to connect components");
        return;
    }

    m_componentsConnected = true;

    // load settings after components are connected
    loadSettings();

    // Ready to startup
    QTimer::singleShot(50, this, &AppController::startup);
}

void AppController::onDataControllerStateChanged()
{
    // Will implement when DataController is ready
    qDebug() << "DataController state changed";
}

void AppController::onUIControllerStateChanged()
{
    // Will implement when UIController is ready
    qDebug() << "UIController state changed";
}

bool AppController::createComponents()
{
    qDebug() << "Creating application components";

    try {
        // Create main window
        m_mainWindow = new MainWindow();
        if (!m_mainWindow) {
            throw std::runtime_error("Failed to create MainWindow");
        }

        // Connect main window signals
        // We'll add more connections when other controllers are ready

        m_componentsCreated = true;
        qDebug() << "All components created successfully";
        return true;

    } catch (const std::exception &e){
        qDebug() << "Exception creating components: " << e.what();
        cleanupComponents();
        return false;
    }
}

bool AppController::connectComponents()
{
    qDebug() << "Connecting application components";

    if (!m_componentsCreated) {
        qDebug() << "Cannot connect - components not created";
        return false;
    }

    // Basic connection established
    // More connections will be added when other controllers are implemented

    qDebug() << "All components connected successfully";
    return true;
}

bool AppController::loadInitialSettings()
{
    qDebug() << "Loading initial settings";

    // Set default values if no settings exist
    if (!m_settings->contains(Constants::SETTINGS_UPDATE_INTERVAL)) {
        m_settings->setValue(Constants::SETTINGS_UPDATE_INTERVAL, Constants::UPDATE_INTERVAL);
    }

    if (!m_settings->contains(Constants::SETTINGS_ALERTS_ENABLED)) {
        m_settings->setValue(Constants::SETTINGS_ALERTS_ENABLED, true);
    }

    if (!m_settings->contains(Constants::SETTINGS_AUTOSTART)) {
        m_settings->setValue(Constants::SETTINGS_AUTOSTART, true);
    }

    if (!m_settings->contains(Constants::SETTINGS_MONITORING_ENABLED)) {
        m_settings->setValue(Constants::SETTINGS_MONITORING_ENABLED, true);
    }

    return true;
}

void AppController::setupApplicationProperties()
{
    if (m_application) {
        m_application->setApplicationName(Constants::APP_NAME);
        m_application->setApplicationVersion(Constants::APP_VERSION);
        m_application->setOrganizationName(Constants::APP_ORGANIZATION);
    }
}

void AppController::setState(AppState newState)
{
    if (m_state == newState) {
        return;
    }

    AppState oldState = m_state;
    m_state = newState;

    qDebug() << "AppController state changed: " << stateString();

    // Clear error when leaving Error state
    if (oldState == Error && newState != Error) {
        clearError();
    }

    handStateTransition(oldState, newState);
    emit stateChanged(newState, oldState);
}

void AppController::handStateTransition(AppState oldState, AppState newState)
{
    Q_UNUSED(oldState)

    // Handle specific state transitions
    switch (newState) {
    case Starting:
        qDebug() << "Application starting...";
        break;
    case Running:
        qDebug() << "Application running normally";
        break;
    case Stopping:
        qDebug() << "Application stopping...";
        break;
    case Stopped:
        qDebug() << "Application stopped";
        break;
    case Error:
        qDebug() << "Application in error state:" << m_lastError;
        break;
    }
}

void AppController::setError(const QString &error)
{
    m_lastError = error;
    setState(Error);
    emit errorOccurred(error);
    qDebug() << "AppController error:" << error;
}

void AppController::clearError()
{
    m_lastError.clear();
}

void AppController::cleanupComponents()
{
    qDebug() << "Cleaning up components";

    // Cleanup in reverse order of creation
    if (m_mainWindow) {
        delete m_mainWindow;
        m_mainWindow = nullptr;
    }

    // Other controllers will be cleaned up when implemented
    m_componentsCreated = false;
    m_componentsConnected = false;
}

