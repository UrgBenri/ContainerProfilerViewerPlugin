#include "TextItem.h"

TextItem::TextItem(QGLWidget *widget, const QString &text, const QPointF &position, int fontSize, const QColor &color)
    : DataItem(-1)
    , m_widget(widget)
    , m_text(text)
    , m_position(position)
    , m_fontSize(fontSize)
    , m_color(color)
{

}

TextItem::~TextItem()
{

}

void TextItem::drawPoints(qreal drawSize, const QVector<QColor> &colors, const QVector<bool> &flags, qreal ratio, const QPointF &offset, qreal alpha, int selectedStep)
{
    if(m_widget){
        QFont font = m_widget->font();
        font.setPointSize(12);
        glColor4d(m_color.redF()
                  , m_color.greenF()
                  , m_color.blueF()
                  , m_color.alphaF());
        m_widget->renderText(m_position.x(), m_position.y(), m_text, font);
    }

    glLineWidth(1);
    glColor4d(0, 0, 0, 1);
}
