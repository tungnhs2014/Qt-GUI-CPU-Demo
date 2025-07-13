#ifndef CIRCULARPROGRESS_H
#define CIRCULARPROGRESS_H

#include <QWidget>
#include <QPainter>
#include <QPropertyAnimation>

class CircularProgress : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double value READ value WRITE setValue NOTIFY valueChanged)

public:
    explicit CircularProgress(QWidget *parent = nullptr);

    // Property getters/setters
    double value() const {
        return m_value;
    }
    void setValue(double value);

    // Customization
    void setColor(const QColor &color);
    void setTitle(const QString &title);
    void setUnit(const QString &unit);

    // Qt Widget interface
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

signals:
    void valueChanged(double value);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onAnimationValueChanged(const QVariant &value);

private:
    // Drawing methods
    void drawBackground(QPainter &painter);
    void drawProgressArc(QPainter &painter);
    void drawText(QPainter &painter);
    void calculateGeometry();

    // Helper methods
    QFont getTitleFont() const;
    QFont getValueFont() const;
    QFont getUnitFont() const;

private:
    // Value and display
    double m_value = 0.0;
    double m_displayValue = 0.0;  // For animation
    QString m_title = "Metric";
    QString m_unit = "%";

    // Apperance
    QColor m_color = QColor("#3498DB");
    QColor m_backgroundColor = QColor("#34495E");
    QColor m_textColor = QColor("ECF0F1");

    // Animation
    QPropertyAnimation *m_animation;

    // Geometry
    QRectF m_arcRect;
    QPointF m_center;
    int m_radius = 50;
    int m_lineWidth = 8;

    // Constants
    static const int START_ANGLE = 90 * 16;
    static const int SPAN_ANGLE = -270 * 16;
    static const int MIN_SIZE = 80;

};

#endif // CIRCULARPROGRESS_H
