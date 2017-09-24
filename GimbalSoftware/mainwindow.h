#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_btnMinimize_clicked();
    void on_draggableTitleBar_mousePressing(QMouseEvent* event);
    void on_draggableTitleBar_mouseMoving(QMouseEvent* event);

private:
    Ui::MainWindow *ui;
    QPoint dragPosition;
};

#endif // MAINWINDOW_H
