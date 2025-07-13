#include "circularprogress.h"
#include "../../core/constants.h"
#include "QPaintEvent"
#include "QFontMetrics"
#include <QDebug>

CircularProgress::CircularProgress(QWidget *parent)
    : QWidget{parent}
    , m_animation(new QPropertyAnimation(this, "value", this))
{
    // Widget setup
    setMaximumSize(MIN_SIZE, MIN_SIZE);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Animation setup
    m_animation->setDuration(500);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);

    connect(m_animation, &QPropertyAnimation::valueChanged, this, &CircularProgress::onAnimationValueChanged);

    calculateGeometry();

    qDebug() << "CircularProgress created";
}

void CircularProgress::setValue(double value)
{
    value = qMax(0.0, qMin(100.0, value));  // Clapmp 0-100

    if (qAbs(m_value - value) < 0.01) {
        return;
    }

    m_value = value;

    // Animate to new value
    m_animation->stop();
    m_animation->setStartValue(m_displayValue);
    m_animation->setEndValue(m_value);
    m_animation->start();

    emit valueChanged(m_value);
    qDebug() << "CircularProgress value set to" << m_value;
}

void CircularProgress::setColor(const QColor &color)
{
    if (m_color != color) {
        m_color = color;
        update();
    }
}

void CircularProgress::setTitle(const QString &title)
{
    if (m_title != title) {
        m_title = title;
        update();
    }
}

void CircularProgress::setUnit(const QString &unit)
{
    if (m_unit != unit) {
        m_unit = unit;
        update();
    }
}

QSize CircularProgress::sizeHint() const
{
    return QSize(120, 120);
}

QSize CircularProgress::minimumSizeHint() const
{
    return QSize(MIN_SIZE, MIN_SIZE);
}

void CircularProgress::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawBackground(painter);
    drawProgressArc(painter);
    drawText(painter);
}

void CircularProgress::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    calculateGeometry();
}

void CircularProgress::onAnimationValueChanged(const QVariant &value)
{
    m_displayValue = value.toDouble();
    update();
}

void CircularProgress::drawBackground(QPainter &painter)
{

}

void CircularProgress::drawProgressArc(QPainter &painter)
{
    painter.save();

    QPen backgroundPen(m_backgroundColor, m_lineWidth);
    backgroundPen.setCapStyle(Qt::RoundCap);
    painter.setPen(backgroundPen);
    painter.setBrush(Qt::NoBrush);

    painter.drawArc(m_arcRect, START_ANGLE, SPAN_ANGLE);

    painter.restore();
}

void CircularProgress::drawText(QPainter &painter)
{
    painter.save();

    double percentage = m_displayValue / 100.0;
    int progressAngle = static_cast<int>(SPAN_ANGLE * percentage);

    QPen progressPen(m_color, m_lineWidth);
    progressPen.setCapStyle(Qt::RoundCap);
    painter.setPen(progressPen);
    painter.setBrush(Qt::NoBrush);

    if (qAbs(progressAngle) > 0) {
        painter.drawArc(m_arcRect, START_ANGLE, progressAngle);
    }

    painter.restore();
}

void CircularProgress::calculateGeometry()
{
    QRectF rect = this->rect();
    m_center = rect.center();

    int minDimension = qMin(rect.width(), rect.height());
    m_radius = (minDimension - m_lineWidth * 2) / 2;
    m_radius = qMax(20, m_radius);

    m_arcRect = QRectF(m_center.x() - m_radius, m_center.y() - m_radius,
                       m_radius * 2, m_radius * 2);
}

QFont CircularProgress::getTitleFont() const
{
    QFont font = this->font();
    font.setPointSize(qMax(8, m_radius / 8));
    font.setWeight(QFont::Normal);
    return font;
}

QFont CircularProgress::getValueFont() const
{
    QFont font = this->font();
    font.setPointSize(qMax(12, m_radius / 4));
    font.setWeight(QFont::Bold);
    return font;
}

QFont CircularProgress::getUnitFont() const
{
    QFont font = this->font();
    font.setPointSize(qMax(6, m_radius / 10));
    font.setWeight(QFont::Normal);
    return font;
}
