#include "metriccard.h"
#include "circularprogress.h"
#include "../../core/constants.h"
#include <QDebug>

MetricCard::MetricCard(CardType type, QWidget *parent)
    : QFrame(parent)
    , m_type(type)
    , m_mainLayout(new QVBoxLayout(this))
    , m_headerLayout(new QHBoxLayout)
    , m_contentLayout(new QVBoxLayout)
    , m_iconLabel(new QLabel(this))
    , m_titleLabel(new QLabel(this))
    , m_subtitleLabel(new QLabel(this))
{
    setupUI();
    applyCardStyling();

    qDebug() << "MetricCard created with type:" << type;
}

void MetricCard::setupUI()
{
    // Main layout
    m_mainLayout->setContentsMargins(16, 12, 16, 12);
    m_mainLayout->setSpacing(8);

    // Header layout
    m_headerLayout->setSpacing(8);
    m_headerLayout->addWidget(m_iconLabel);
    m_headerLayout->addWidget(m_titleLabel);
    m_headerLayout->addWidget(m_subtitleLabel);
    m_headerLayout->addStretch();

    m_mainLayout->addLayout(m_headerLayout);

    // Setup content based on type
    switch (m_type) {
    case CircularType:
        setupCircularLayout();
        break;
    case TextType:
        setupTextLayout();
        break;
    case NetworkType:
        setupNetworkLayout();
        break;
    }

    m_mainLayout->addLayout(m_contentLayout);
    m_mainLayout->addStretch();
}

void MetricCard::setupCircularLayout()
{
    m_circularProgress = new CircularProgress(this);
    m_circularProgress->setTitle("");  // Title handled by card header
    m_circularProgress->setColor(m_color);

    m_contentLayout->addWidget(m_circularProgress);
    m_contentLayout->setAlignment(m_circularProgress, Qt::AlignCenter);
}

void MetricCard::setupTextLayout()
{
    m_textLabel = new QLabel("--", this);
    m_textLabel->setAlignment(Qt::AlignCenter);

    QFont textFont = m_textLabel->font();
    textFont.setPointSize(24);
    textFont.setWeight(QFont::Bold);
    m_textLabel->setFont(textFont);
    m_textLabel->setStyleSheet("color: #ECF0F1;");

    m_contentLayout->addWidget(m_textLabel);
    m_contentLayout->setAlignment(m_textLabel, Qt::AlignCenter);
}

void MetricCard::setupNetworkLayout()
{
    // Create widgets
    QLabel *upIconLabel = new QLabel("↑", this);
    m_upSpeedLabel = new QLabel("0.0 MB/s", this);
    QLabel *downIconLabel = new QLabel("↓", this);
    m_downSpeedLabel = new QLabel("0.0 MB/s", this);

    // Style icons
    QFont iconFont;
    iconFont.setPointSize(16);
    iconFont.setWeight(QFont::Bold);
    upIconLabel->setFont(iconFont);
    downIconLabel->setFont(iconFont);
    upIconLabel->setStyleSheet("color: " + Constants::NETWORK_UP_COLOR + ";");
    downIconLabel->setStyleSheet("color: " + Constants::NETWORK_DOWN_COLOR + ";");

    // Style speed labels
    QFont speedFont;
    speedFont.setPointSize(14);
    m_upSpeedLabel->setFont(speedFont);
    m_downSpeedLabel->setFont(speedFont);
    m_upSpeedLabel->setStyleSheet("color: #ECF0F1;");
    m_downSpeedLabel->setStyleSheet("color: #ECF0F1;");

    // Layout
    QHBoxLayout *upLayout = new QHBoxLayout;
    upLayout->addWidget(upIconLabel);
    upLayout->addWidget(m_upSpeedLabel);
    upLayout->addStretch();

    QHBoxLayout *downLayout = new QHBoxLayout;
    downLayout->addWidget(downIconLabel);
    downLayout->addWidget(m_downSpeedLabel);
    downLayout->addStretch();

    m_contentLayout->addLayout(upLayout);
    m_contentLayout->addLayout(downLayout);
}

void MetricCard::applyCardStyling()
{
    // Frame styling
    setFrameStyle(QFrame::Box | QFrame::Raised);
    setLineWidth(1);

    // Card appearance
    setStyleSheet(QString(
                      "MetricCard {"
                      "    background-color: #34495E;"
                      "    border: 1px solid #2C3E50;"
                      "    border-radius: 8px;"
                      "    margin: 4px;"
                      "}"
                      "MetricCard:hover {"
                      "    border-color: %1;"
                      "}"
                      ).arg(m_color.name()));

    // Header styling
    QFont titleFont = m_titleLabel->font();
    titleFont.setPointSize(12);
    titleFont.setWeight(QFont::Bold);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setStyleSheet("color: #ECF0F1;");

    QFont subtitleFont = m_subtitleLabel->font();
    subtitleFont.setPointSize(9);
    m_subtitleLabel->setFont(subtitleFont);
    m_subtitleLabel->setStyleSheet("color: #95A5A6;");

    // Icon placeholder
    m_iconLabel->setFixedSize(16, 16);
    m_iconLabel->setStyleSheet("background-color: " + m_color.name() + "; border-radius: 8px;");
}

void MetricCard::setTitle(const QString &title)
{
    m_titleLabel->setText(title);
}

void MetricCard::setIcon(const QString &iconPath)
{
    // For now, just change background color as icon placeholder
    Q_UNUSED(iconPath)
    // Future: Load actual icon image
}

void MetricCard::setColor(const QColor &color)
{
    m_color = color;

    if (m_circularProgress) {
        m_circularProgress->setColor(color);
    }

    applyCardStyling();
}

void MetricCard::setSubtitle(const QString &subtitle)
{
    m_subtitleLabel->setText(subtitle);
    m_subtitleLabel->setVisible(!subtitle.isEmpty());
}

void MetricCard::setValue(double value)
{
    if (m_type == CircularType && m_circularProgress) {
        m_circularProgress->setValue(value);
    }
}

void MetricCard::setText(const QString &text)
{
    if (m_type == TextType && m_textLabel) {
        m_textLabel->setText(text);
    }
}

void MetricCard::setNetworkSpeeds(double up, double down)
{
    if (m_type == NetworkType) {
        if (m_upSpeedLabel) {
            m_upSpeedLabel->setText(QString::number(up, 'f', 1) + " MB/s");
        }
        if (m_downSpeedLabel) {
            m_downSpeedLabel->setText(QString::number(down, 'f', 1) + " MB/s");
        }
    }
}

QSize MetricCard::sizeHint() const
{
    switch (m_type) {
    case CircularType:
        return QSize(200, 180);
    case TextType:
        return QSize(200, 120);
    case NetworkType:
        return QSize(200, 100);
    }
    return QSize(200, 150);
}
