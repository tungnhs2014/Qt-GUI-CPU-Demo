#include "mainwindow.h"
#include "src/view/dashboardwidget.h"
#include "src/core/constants.h"
#include <QApplication>
#include <QMenuBar>
#include <QMessageBox>
#include <QLabel>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_tabWidget(new QTabWidget(this))
    , m_dashboardWidget(new DashboardWidget(this))
    , m_statusLabel(new QLabel("Ready", this))
    , m_connectionLabel(new QLabel("Disconnected", this))
{
    setupUI();
    setupMenuBar();
    setupTabs();
    applyMainWindowStyling();

    qDebug() << "MainWindow initialized";
}

MainWindow::~MainWindow()
{
    qDebug() << "MainWindow destroyed";
}

void MainWindow::showAbout()
{
    QMessageBox::about(this, "About " + Constants::APP_NAME,
          QString("<h3>%1 v%2</h3>"
                   "<p>System monitoring application for embedded Linux systems.</p>"
                   "<p>Built with Qt %3</p>"
                   "<p>Organization: %4</p>")
               .arg(Constants::APP_NAME)
               .arg(Constants::APP_VERSION)
               .arg(QT_VERSION_STR)
               .arg(Constants::APP_ORGANIZATION)
    );
}

void MainWindow::onTabChanged(int index)
{
    QString tabName = m_tabWidget->tabText(index);
    m_statusLabel->setText("Current tab: " + tabName);

    qDebug() << "Tab changed to: " << tabName << "index: " << index;
}

void MainWindow::setupUI()
{
    setCentralWidget(m_tabWidget);

    setWindowTitle(Constants::APP_NAME + " v" + Constants::APP_VERSION);
    setMinimumSize(Constants::MIN_WINDOW_WIDTH, Constants::MIN_WINDOW_HEIGHT);
    resize(Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT);

    m_tabWidget->setTabPosition(QTabWidget::North);
    m_tabWidget->setMovable(false);
    m_tabWidget->setTabsClosable(false);

    connect(m_tabWidget, QOverload<int>::of(&QTabWidget::currentChanged), this, &MainWindow::onTabChanged);
}

void MainWindow::setupMenuBar()
{
    QMenu *fileMenu = menuBar()->addMenu("&File");

    m_exitAction = new QAction("E&xit", this);
    m_exitAction->setShortcut(QKeySequence::Quit);
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(m_exitAction);

    QMenu *helpMenu = menuBar()->addMenu("&Help");

    m_aboutAction = new QAction("&About", this);
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::showAbout);
    helpMenu->addAction(m_aboutAction);
}

void MainWindow::setupStatusBar()
{
    statusBar()->addWidget(m_statusLabel, 1);
    statusBar()->addPermanentWidget(m_connectionLabel);

    m_statusLabel->setStyleSheet("color: #ECF0F1;");
    m_connectionLabel->setStyleSheet("color: #E74C3C;");
}

void MainWindow::setupTabs()
{
    // Dashboard tab
    m_tabWidget->addTab(m_dashboardWidget, "Dashbooard");

    // Alerts tab (placeholder)
    m_alertsWidget = new QWidget(this);
    QVBoxLayout *alertsLayout = new QVBoxLayout(m_alertsWidget);
    QLabel *alertsLabel = new QLabel("Alerts - Coming Soon", m_alertsWidget);
    alertsLabel->setAlignment(Qt::AlignCenter);
    alertsLabel->setStyleSheet("color: #95A5A6; font-sizeL: 18px;");
    alertsLayout->addWidget(alertsLabel);
    m_tabWidget->addTab(m_alertsWidget, "Alerts");

    // Settings tab (placeholder)
    m_settingsWidget = new QWidget(this);
    QVBoxLayout *settingsLayout = new QVBoxLayout(m_settingsWidget);
    QLabel *settingLabel = new QLabel("Settings - Coming soon", m_settingsWidget);
    settingLabel->setAlignment(Qt::AlignCenter);
    settingLabel->setStyleSheet("color: #95A5A6; font-size: 18px;");
    settingsLayout->addWidget(settingLabel);
    m_tabWidget->addTab(m_settingsWidget, "Settings");

    m_tabWidget->setCurrentIndex(0); // Default to Dashboard
}

void MainWindow::applyMainWindowStyling()
{
    setStyleSheet(
        "QMainWindow {"
        "    background-color: #2C3E50;"
        "    color: #ECF0F1;"
        "}"
        "QTabWidget::pane {"
        "    border: 1px solid #34495E;"
        "    background-color: #2C3E50;"
        "}"
        "QTabBar::tab {"
        "    background-color: #34495E;"
        "    color: #ECF0F1;"
        "    padding: 8px 16px;"
        "    margin-right: 2px;"
        "    border-top-left-radius: 4px;"
        "    border-top-right-radius: 4px;"
        "}"
        "QTabBar::tab:selected {"
        "    background-color: #3498DB;"
        "    color: #FFFFFF;"
        "}"
        "QTabBar::tab:hover {"
        "    background-color: #4A6A8A;"
        "}"
        "QMenuBar {"
        "    background-color: #34495E;"
        "    color: #ECF0F1;"
        "    border-bottom: 1px solid #2C3E50;"
        "}"
        "QMenuBar::item:selected {"
        "    background-color: #3498DB;"
        "}"
        "QMenu {"
        "    background-color: #34495E;"
        "    color: #ECF0F1;"
        "    border: 1px solid #2C3E50;"
        "}"
        "QMenu::item:selected {"
        "    background-color: #3498DB;"
        "}"
        "QStatusBar {"
        "    background-color: #34495E;"
        "    color: #ECF0F1;"
        "    border-top: 1px solid #2C3E50;"
        "}"
        );
}

