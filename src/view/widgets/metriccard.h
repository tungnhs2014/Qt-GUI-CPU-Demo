#ifndef METRICCARD_H
#define METRICCARD_H

#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class CircularProgress;

class MetricCard : public QFrame
{
    Q_OBJECT

public:
    enum CardType {
        CircularType,    // Uses CircularProgress
        TextType,        // Simple text display
        NetworkType      // Up/down network speeds
    };

    explicit MetricCard(CardType type = CircularType, QWidget *parent = nullptr);

    // Configuration
    void setTitle(const QString &title);
    void setIcon(const QString &iconPath);
    void setColor(const QColor &color);
    void setSubtitle(const QString &subtitle);

    // Value updates
    void setValue(double value);                    // CircularType
    void setText(const QString &text);              // TextType
    void setNetworkSpeeds(double up, double down);  // NetworkType

    // Access to internal widgets
    CircularProgress *circularProgress() const { return m_circularProgress; }

    QSize sizeHint() const override;

private:
    void setupUI();
    void setupCircularLayout();
    void setupTextLayout();
    void setupNetworkLayout();
    void applyCardStyling();

private:
    CardType m_type;

    // Layouts
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_headerLayout;
    QVBoxLayout *m_contentLayout;

    // Header widgets
    QLabel *m_iconLabel;
    QLabel *m_titleLabel;
    QLabel *m_subtitleLabel;

    // Content widgets
    CircularProgress *m_circularProgress = nullptr;  // CircularType
    QLabel *m_textLabel = nullptr;                   // TextType
    QLabel *m_upSpeedLabel = nullptr;                // NetworkType
    QLabel *m_downSpeedLabel = nullptr;              // NetworkType

    // Styling
    QColor m_color = QColor("#3498DB");
};

#endif // METRICCARD_H
