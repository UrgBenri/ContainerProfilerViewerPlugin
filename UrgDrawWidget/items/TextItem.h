#ifndef TEXTITEM_H
#define TEXTITEM_H

#include <QGLWidget>
#include <QPointF>
#include <QColor>

#include "DataItem.h"

class TextItem : public DataItem
{
public:
    TextItem(QGLWidget *widget
             , const QString &text
             , const QPointF &position = QPointF(0, 0)
             , int fontSize = 12
             , const QColor &color = Qt::red);
    virtual ~TextItem();

    void drawPoints(qreal drawSize
                    , const QVector<QColor> &colors
                    , const QVector<bool> &flags
                    , qreal ratio
                    , const QPointF &offset
                    , qreal alpha
                    , int selectedStep);

    QPointF position() const { return m_position; }
    void setPosition(const QPointF &position) { m_position = position; }

    int fontSize() const { return m_fontSize; }
    void setFontSize(int fontSize) { m_fontSize = fontSize; }

    QColor color() const { return m_color; }
    void setColor(const QColor &color) { m_color = color; }

private:
    QString m_text;
    QGLWidget *m_widget;
    QPointF m_position;
    int m_fontSize;
    QColor m_color;
};

#endif // TEXTITEM_H
