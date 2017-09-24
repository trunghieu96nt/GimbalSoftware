#ifndef DRAGGABLEWIDGET_H
#define DRAGGABLEWIDGET_H

#include <QWidget>
#include <QStyleOption>
#include <QPainter>
#include <QMouseEvent>

class DraggableWidget : public QWidget
{
    Q_OBJECT
private:
    QPoint dragPosition;
    void paintEvent(QPaintEvent *);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event); 

public:
    explicit DraggableWidget(QWidget *parent = nullptr);

signals:
    void mousePressing(QMouseEvent *event);
    void mouseMoving(QMouseEvent *event);

public slots:
};

#endif // DRAGGABLEWIDGET_H
