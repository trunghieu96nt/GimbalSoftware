#include "draggablewidget.h"

DraggableWidget::DraggableWidget(QWidget *parent) : QWidget(parent)
{

}

void DraggableWidget::paintEvent(QPaintEvent *)
 {
     QStyleOption opt;
     opt.init(this);
     QPainter p(this);
     style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
 }

void DraggableWidget::mousePressEvent(QMouseEvent *event)
{
    emit mousePressing(event);
}
void DraggableWidget::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMoving(event);
}
