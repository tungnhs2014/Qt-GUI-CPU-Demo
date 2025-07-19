#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QAction>

class DashboardWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // Access to dashboard for external connections
    DashboardWidget *dashboardWidget() const {
        return m_dashboardWidget;
    }

private slots:
    void showAbout();
    void onTabChanged(int index);

private:
    void setupUI();
    void setupMenuBar();
    void setupStatusBar();
    void setupTabs();
    void applyMainWindowStyling();

    // Main UI
    QTabWidget *m_tabWidget;

    // Tabs
    DashboardWidget *m_dashboardWidget;
    QWidget *m_alertsWidget;
    QWidget *m_settingsWidget;

    // Status bar
    QLabel *m_statusLabel;
    QLabel *m_connectionLabel;

    // Menu actions
    QAction *m_aboutAction;
    QAction *m_exitAction;
};

#endif // MAINWINDOW_H
