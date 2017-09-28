#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>
#include <QTimer>
#include <QStringList>
#include <QToolTip>

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
    void on_draggableTitleBar_custom_mousePressing(QMouseEvent* event);
    void on_draggableTitleBar_custom_mouseMoving(QMouseEvent* event);

    void on_btnMinimize_custom_clicked();
    void on_btnConnect_custom_clicked();
    void on_btnHome_custom_clicked();
    void on_btnStop_custom_clicked();
    void on_btnManual_custom_clicked();
    void on_btnTracking_custom_clicked();
    void on_btnPointing_custom_clicked();
    void on_btnAZSetPos_custom_clicked();
    void on_btnAZSetVel_custom_clicked();
    void on_btnAZSetPosVel_custom_clicked();
    void on_btnAZGetPos_custom_clicked();
    void on_btnELSetPos_custom_clicked();
    void on_btnELSetVel_custom_clicked();
    void on_btnELSetPosVel_custom_clicked();
    void on_btnELGetPos_custom_clicked();
    void on_btnSetKp_custom_clicked();
    void on_btnSetKi_custom_clicked();
    void on_btnSetKd_custom_clicked();
    void on_btnSetKff1_custom_clicked();
    void on_btnSetKff2_custom_clicked();
    void on_btnGetParams_custom_clicked();

    void on_timerCOMPort_custom_timeout();

private:
    Ui::MainWindow *ui;
    QPoint dragPosition;
    QSerialPort *serialCOMPort = new QSerialPort;
    QTimer *timerCOMPort;
    QStringList listCOMPort;

    bool serialCOMPort_write(const QByteArray &data);
    bool sendCommand(char msgID, const QByteArray &payload);
};

#endif // MAINWINDOW_H
