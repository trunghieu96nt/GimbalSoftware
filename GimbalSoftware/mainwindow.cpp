#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* Connect Signals and Slots */
    connect(ui->draggableTitleBar, SIGNAL(mousePressing(QMouseEvent*)),
            this, SLOT(on_draggableTitleBar_custom_mousePressing(QMouseEvent*)));
    connect(ui->draggableTitleBar, SIGNAL(mouseMoving(QMouseEvent*)),
            this, SLOT(on_draggableTitleBar_custom_mouseMoving(QMouseEvent*)));

    connect(serialCOMPort, SIGNAL(readyRead()), this, SLOT(on_serialCOMPort_custom_readyRead()));

    connect(ui->btnClose, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->btnMinimize, SIGNAL(clicked()), this, SLOT(on_btnMinimize_custom_clicked()));
    connect(ui->btnConnect, SIGNAL(clicked()), this, SLOT(on_btnConnect_custom_clicked()));
    connect(ui->btnHome, SIGNAL(clicked()), this, SLOT(on_btnHome_custom_clicked()));
    connect(ui->btnStop, SIGNAL(clicked()), this, SLOT(on_btnStop_custom_clicked()));
    connect(ui->btnManual, SIGNAL(clicked()), this, SLOT(on_btnManual_custom_clicked()));
    connect(ui->btnTracking, SIGNAL(clicked()), this, SLOT(on_btnTracking_custom_clicked()));
    connect(ui->btnPointing, SIGNAL(clicked()), this, SLOT(on_btnPointing_custom_clicked()));
    connect(ui->btnAZSetPos, SIGNAL(clicked()), this, SLOT(on_btnAZSetPos_custom_clicked()));
    connect(ui->btnAZSetVel, SIGNAL(clicked()), this, SLOT(on_btnAZSetVel_custom_clicked()));
    connect(ui->btnAZSetPosVel, SIGNAL(clicked()), this, SLOT(on_btnAZSetPosVel_custom_clicked()));
    connect(ui->btnAZGetPos, SIGNAL(clicked()), this, SLOT(on_btnAZGetPos_custom_clicked()));
    connect(ui->btnELSetPos, SIGNAL(clicked()), this, SLOT(on_btnELSetPos_custom_clicked()));
    connect(ui->btnELSetVel, SIGNAL(clicked()), this, SLOT(on_btnELSetVel_custom_clicked()));
    connect(ui->btnELSetPosVel, SIGNAL(clicked()), this, SLOT(on_btnELSetPosVel_custom_clicked()));
    connect(ui->btnELGetPos, SIGNAL(clicked()), this, SLOT(on_btnELGetPos_custom_clicked()));
    connect(ui->btnSetKp, SIGNAL(clicked()), this, SLOT(on_btnSetKp_custom_clicked()));
    connect(ui->btnSetKi, SIGNAL(clicked()), this, SLOT(on_btnSetKi_custom_clicked()));
    connect(ui->btnSetKd, SIGNAL(clicked()), this, SLOT(on_btnSetKd_custom_clicked()));
    connect(ui->btnSetKff1, SIGNAL(clicked()), this, SLOT(on_btnSetKff1_custom_clicked()));
    connect(ui->btnSetKff2, SIGNAL(clicked()), this, SLOT(on_btnSetKff2_custom_clicked()));
    connect(ui->btnGetParams, SIGNAL(clicked()), this, SLOT(on_btnGetParams_custom_clicked()));

    /* QComboBox Initialization */
    ui->cboModeAxis->setCurrentText("Both");

    /* QLineEdit Validator */
    QDoubleValidator *paramsValidator = new QDoubleValidator(this);
    QDoubleValidator *posValidator = new QDoubleValidator(-180.0, 180, 2, this);
    QDoubleValidator *velValidator = new QDoubleValidator(this);

    paramsValidator->setDecimals(6);
    paramsValidator->setBottom(0.0);
    ui->ledKp->setValidator(paramsValidator);
    ui->ledKi->setValidator(paramsValidator);
    ui->ledKd->setValidator(paramsValidator);
    ui->ledKff1->setValidator(paramsValidator);
    ui->ledKff2->setValidator(paramsValidator);
    posValidator->setNotation(QDoubleValidator::StandardNotation);
    ui->ledAZPos->setValidator(posValidator);
    ui->ledELPos->setValidator(posValidator);
    velValidator->setDecimals(2);
    velValidator->setBottom(0.0);
    ui->ledAZVel->setValidator(velValidator);
    ui->ledELVel->setValidator(velValidator);

    /* Serial Port */
    serialCOMPort->setBaudRate(QSerialPort::Baud115200);
    serialCOMPort->setDataBits(QSerialPort::Data8);
    serialCOMPort->setParity(QSerialPort::NoParity);
    serialCOMPort->setStopBits(QSerialPort::OneStop);
    serialCOMPort->setFlowControl(QSerialPort::NoFlowControl);

    /* The first seacrh COM Port */
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
//        qDebug() << "Name : " << info.portName();
//        qDebug() << "Description : " << info.description();
//        qDebug() << "Manufacturer: " << info.manufacturer();
        ui->cboCom->addItem(info.portName());
    }

    /* Timer 1s to search exist COM Port */
    timerCOMPort = new QTimer(this);
    connect(timerCOMPort, SIGNAL(timeout()), this, SLOT(on_timerCOMPort_custom_timeout()));
    timerCOMPort->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_draggableTitleBar_custom_mousePressing(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::on_draggableTitleBar_custom_mouseMoving(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

void MainWindow::on_serialCOMPort_custom_readyRead()
{
    int idxGB, msgLength;
    QByteArray receivedMsg;

    dataSerialCOMPort.append(serialCOMPort->readAll());
    idxGB = dataSerialCOMPort.indexOf("GB");
    if (idxGB == -1) return;
    dataSerialCOMPort.remove(0, idxGB);
    msgLength = dataSerialCOMPort[5] + 6;
    if (dataSerialCOMPort.size() >= msgLength)
    {
        receivedMsg = dataSerialCOMPort.left(msgLength);
        dataSerialCOMPort.remove(0, msgLength);
        parseMsg(receivedMsg);
    }
}

void MainWindow::on_btnMinimize_custom_clicked()
{
    this->setWindowState(Qt::WindowMinimized);
}

void MainWindow::on_btnConnect_custom_clicked()
{
    if (ui->btnConnect->text() == "Connect")
    {
        serialCOMPort->setPortName(ui->cboCom->currentText());

        if (serialCOMPort->open(QSerialPort::ReadWrite) == true)
        {
            ui->btnConnect->setText("Disconnect");
            ui->btnConnect->setStyleSheet("QPushButton {\
                                           font: bold 18px; color: white;\
                                           border: 2px solid #ff0000;\
                                           background-color: #ff0000;\
                                           }\
                                           QPushButton:pressed { background-color: #e60000; }\
                                           QPushButton:hover { border: 2px solid #8f8f8f; }");
            ui->pteStatus->appendPlainText("- " + serialCOMPort->portName() + " is connected");
            timerCOMPort->stop();
        }
    }
    else
    {
        ui->btnConnect->setText("Connect");
        ui->btnConnect->setStyleSheet("QPushButton {\
                                       font: bold 18px; color: white;\
                                       border: 2px solid #33cc33;\
                                       background-color: #33cc33;\
                                       }\
                                       QPushButton:pressed { background-color: #2eb82e; }\
                                       QPushButton:hover { border: 2px solid #8f8f8f; }");
        serialCOMPort->close();
        ui->pteStatus->appendPlainText("- " + serialCOMPort->portName() + " is disconnected");
        timerCOMPort->start(1000);
    }
}

void MainWindow::on_btnHome_custom_clicked()
{
    QByteArray dataArray;
    dataArray.append((char)(1 + ui->cboModeAxis->currentIndex()));
    if (sendCommand(0x01, dataArray) == true)
        ui->pteStatus->appendPlainText("- Send: Set Home " + ui->cboModeAxis->currentText());
    else
        ui->pteStatus->appendPlainText("- No COM Port is connected");
}

void MainWindow::on_btnStop_custom_clicked()
{
    QByteArray dataArray;
    dataArray.append((char)(1 + ui->cboModeAxis->currentIndex()));
    if (sendCommand(0x02, dataArray) == true)
        ui->pteStatus->appendPlainText("- Send: Set Stop " + ui->cboModeAxis->currentText());
    else
        ui->pteStatus->appendPlainText("- No COM Port is connected");
}

void MainWindow::on_btnManual_custom_clicked()
{
    QByteArray dataArray;
    dataArray.append((char)(1 + ui->cboModeAxis->currentIndex()));
    dataArray.append((char)0x00);
    if (sendCommand(0x04, dataArray) == true)
        ui->pteStatus->appendPlainText("- Send: Set Manual " + ui->cboModeAxis->currentText());
    else
        ui->pteStatus->appendPlainText("- No COM Port is connected");
}

void MainWindow::on_btnTracking_custom_clicked()
{
    QByteArray dataArray;
    dataArray.append((char)(1 + ui->cboModeAxis->currentIndex()));
    dataArray.append((char)0x01);
    if (sendCommand(0x04, dataArray) == true)
        ui->pteStatus->appendPlainText("- Send: Set Tracking " + ui->cboModeAxis->currentText());
    else
        ui->pteStatus->appendPlainText("- No COM Port is connected");
}

void MainWindow::on_btnPointing_custom_clicked()
{
    QByteArray dataArray;

    dataArray.append((char)(1 + ui->cboModeAxis->currentIndex()));
    dataArray.append((char)0x02);
    if (sendCommand(0x04, dataArray) == true)
        ui->pteStatus->appendPlainText("- Send: Set Pointing " + ui->cboModeAxis->currentText());
    else
        ui->pteStatus->appendPlainText("- No COM Port is connected");
}

void MainWindow::on_btnAZSetPos_custom_clicked()
{
    QByteArray dataArray;
    qint32 scaledValue = 0;

    if (ui->ledAZPos->text() == NULL)
    {
        QToolTip::showText(ui->ledAZPos->mapToGlobal(QPoint()), "Enter value");
    }
    else
    {
        dataArray.append((char)0x01);

        scaledValue = qint32 (ui->ledAZPos->text().toDouble() * 100);
        if ((scaledValue > 18000) || (scaledValue < -18000))
        {
            QToolTip::showText(ui->ledAZPos->mapToGlobal(QPoint()), "Range: (-180, 180)");
        }
        else
        {
            dataArray.append((char)((scaledValue >> 24) & 0x0ff));
            dataArray.append((char)((scaledValue >> 16) & 0x0ff));
            dataArray.append((char)((scaledValue >> 8) & 0x0ff));
            dataArray.append((char)((scaledValue) & 0x0ff));

            if (sendCommand(0x05, dataArray) == true)
            {
                ui->pteStatus->appendPlainText("- Send: Set AZ Pos");
            }
            else
                ui->pteStatus->appendPlainText("- No COM Port is connected");
        }
    }
}

void MainWindow::on_btnAZSetVel_custom_clicked()
{
    QByteArray dataArray;
    qint32 scaledValue = 0;

    if (ui->ledAZVel->text() == NULL)
    {
        QToolTip::showText(ui->ledAZVel->mapToGlobal(QPoint()), "Enter value");
    }
    else
    {
        dataArray.append((char)0x01);

        scaledValue = qint32 (ui->ledAZVel->text().toDouble() * 100);
//        if ((scaledValue > 18000) || (scaledValue < -18000))
//        {
//            QToolTip::showText(ui->ledAZPos->mapToGlobal(QPoint()), "Range: (-180, 180)");
//        }
//        else
        {
            dataArray.append((char)((scaledValue >> 24) & 0x0ff));
            dataArray.append((char)((scaledValue >> 16) & 0x0ff));
            dataArray.append((char)((scaledValue >> 8) & 0x0ff));
            dataArray.append((char)((scaledValue) & 0x0ff));

            if (sendCommand(0x06, dataArray) == true)
            {
                ui->pteStatus->appendPlainText("- Send: Set AZ Vel");
            }
            else
                ui->pteStatus->appendPlainText("- No COM Port is connected");
        }
    }
}

void MainWindow::on_btnAZSetPosVel_custom_clicked()
{
    QByteArray dataArray;
    qint32 scaledValue = 0;

    if (ui->ledAZPos->text() == NULL)
    {
        QToolTip::showText(ui->ledAZPos->mapToGlobal(QPoint()), "Enter value");
    }
    else if (ui->ledAZVel->text() == NULL)
    {
        QToolTip::showText(ui->ledAZVel->mapToGlobal(QPoint()), "Enter value");
    }
    else
    {
        dataArray.append((char)0x01);

        scaledValue = qint32 (ui->ledAZPos->text().toDouble() * 100);
        if ((scaledValue > 18000) || (scaledValue < -18000))
        {
            QToolTip::showText(ui->ledAZPos->mapToGlobal(QPoint()), "Range: (-180, 180)");
            return;
        }
        dataArray.append((char)((scaledValue >> 24) & 0x0ff));
        dataArray.append((char)((scaledValue >> 16) & 0x0ff));
        dataArray.append((char)((scaledValue >> 8) & 0x0ff));
        dataArray.append((char)((scaledValue) & 0x0ff));

        scaledValue = qint32 (ui->ledAZVel->text().toDouble() * 100);
//        if ((scaledValue > 18000) || (scaledValue < -18000))
//        {
//            QToolTip::showText(ui->ledAZPos->mapToGlobal(QPoint()), "Range: (-180, 180)");
//            return;
//        }
        dataArray.append((char)((scaledValue >> 24) & 0x0ff));
        dataArray.append((char)((scaledValue >> 16) & 0x0ff));
        dataArray.append((char)((scaledValue >> 8) & 0x0ff));
        dataArray.append((char)((scaledValue) & 0x0ff));

        if (sendCommand(0x07, dataArray) == true)
        {
            ui->pteStatus->appendPlainText("- Send: Set AZ Pos Vel");
        }
        else
            ui->pteStatus->appendPlainText("- No COM Port is connected");
    }
}

void MainWindow::on_btnAZGetPos_custom_clicked()
{
    QByteArray dataArray;

    dataArray.append((char)0x01);
    if (sendCommand(0x08, dataArray) == true)
    {
        ui->pteStatus->appendPlainText("- Send: Get AZ Pos");
    }
    else
        ui->pteStatus->appendPlainText("- No COM Port is connected");
}

void MainWindow::on_btnELSetPos_custom_clicked()
{
    QByteArray dataArray;
    qint32 scaledValue = 0;

    if (ui->ledELPos->text() == NULL)
    {
        QToolTip::showText(ui->ledELPos->mapToGlobal(QPoint()), "Enter value");
    }
    else
    {
        dataArray.append((char)0x02);

        scaledValue = qint32 (ui->ledELPos->text().toDouble() * 100);
        if ((scaledValue > 3000) || (scaledValue < -3000))
        {
            QToolTip::showText(ui->ledELPos->mapToGlobal(QPoint()), "Range: (-30, 30)");
        }
        else
        {
            dataArray.append((char)((scaledValue >> 24) & 0x0ff));
            dataArray.append((char)((scaledValue >> 16) & 0x0ff));
            dataArray.append((char)((scaledValue >> 8) & 0x0ff));
            dataArray.append((char)((scaledValue) & 0x0ff));

            if (sendCommand(0x05, dataArray) == true)
            {
                ui->pteStatus->appendPlainText("- Send: Set EL Pos");
            }
            else
                ui->pteStatus->appendPlainText("- No COM Port is connected");
        }
    }
}

void MainWindow::on_btnELSetVel_custom_clicked()
{
    QByteArray dataArray;
    qint32 scaledValue = 0;

    if (ui->ledELVel->text() == NULL)
    {
        QToolTip::showText(ui->ledELVel->mapToGlobal(QPoint()), "Enter value");
    }
    else
    {
        dataArray.append((char)0x02);

        scaledValue = qint32 (ui->ledELVel->text().toDouble() * 100);
//        if ((scaledValue > 18000) || (scaledValue < -18000))
//        {
//            QToolTip::showText(ui->ledAZPos->mapToGlobal(QPoint()), "Range: (-180, 180)");
//        }
//        else
        {
            dataArray.append((char)((scaledValue >> 24) & 0x0ff));
            dataArray.append((char)((scaledValue >> 16) & 0x0ff));
            dataArray.append((char)((scaledValue >> 8) & 0x0ff));
            dataArray.append((char)((scaledValue) & 0x0ff));

            if (sendCommand(0x06, dataArray) == true)
            {
                ui->pteStatus->appendPlainText("- Send: Set EL Vel");
            }
            else
                ui->pteStatus->appendPlainText("- No COM Port is connected");
        }
    }
}

void MainWindow::on_btnELSetPosVel_custom_clicked()
{
    QByteArray dataArray;
    qint32 scaledValue = 0;

    if (ui->ledELPos->text() == NULL)
    {
        QToolTip::showText(ui->ledELPos->mapToGlobal(QPoint()), "Enter value");
    }
    else if (ui->ledELVel->text() == NULL)
    {
        QToolTip::showText(ui->ledELVel->mapToGlobal(QPoint()), "Enter value");
    }
    else
    {
        dataArray.append((char)0x02);

        scaledValue = qint32 (ui->ledELPos->text().toDouble() * 100);
        if ((scaledValue > 18000) || (scaledValue < -18000))
        {
            QToolTip::showText(ui->ledELPos->mapToGlobal(QPoint()), "Range: (-180, 180)");
            return;
        }
        dataArray.append((char)((scaledValue >> 24) & 0x0ff));
        dataArray.append((char)((scaledValue >> 16) & 0x0ff));
        dataArray.append((char)((scaledValue >> 8) & 0x0ff));
        dataArray.append((char)((scaledValue) & 0x0ff));

        scaledValue = qint32 (ui->ledELVel->text().toDouble() * 100);
//        if ((scaledValue > 18000) || (scaledValue < -18000))
//        {
//            QToolTip::showText(ui->ledELPos->mapToGlobal(QPoint()), "Range: (-180, 180)");
//            return;
//        }
        dataArray.append((char)((scaledValue >> 24) & 0x0ff));
        dataArray.append((char)((scaledValue >> 16) & 0x0ff));
        dataArray.append((char)((scaledValue >> 8) & 0x0ff));
        dataArray.append((char)((scaledValue) & 0x0ff));

        if (sendCommand(0x07, dataArray) == true)
        {
            ui->pteStatus->appendPlainText("- Send: Set EL Pos Vel");
        }
        else
            ui->pteStatus->appendPlainText("- No COM Port is connected");
    }
}

void MainWindow::on_btnELGetPos_custom_clicked()
{
    QByteArray dataArray;

    dataArray.append((char)0x02);
    if (sendCommand(0x08, dataArray) == true)
    {
        ui->pteStatus->appendPlainText("- Send: Get EL Pos");
    }
    else
        ui->pteStatus->appendPlainText("- No COM Port is connected");
}

void MainWindow::on_btnSetKp_custom_clicked()
{
    QByteArray dataArray;
    quint32 scaledValue = 0;

    if (ui->ledKp->text() == NULL)
    {
        QToolTip::showText(ui->ledKp->mapToGlobal(QPoint()), "Enter value");
    }
    else
    {
        dataArray.append((char)(1 + ui->cboPIDAxis->currentIndex()));
        dataArray.append((char)(1 + ui->cboPIDName->currentIndex()));

        scaledValue = quint32 (ui->ledKp->text().toDouble() * 1000000);
        dataArray.append((char)((scaledValue >> 24) & 0x0ff));
        dataArray.append((char)((scaledValue >> 16) & 0x0ff));
        dataArray.append((char)((scaledValue >> 8) & 0x0ff));
        dataArray.append((char)((scaledValue) & 0x0ff));

        if (sendCommand(0x09, dataArray) == true)
        {
            ui->pteStatus->appendPlainText("- Send: Set Kp " + ui->cboPIDAxis->currentText() +
                                           " " + ui->cboPIDName->currentText());
        }
        else
            ui->pteStatus->appendPlainText("- No COM Port is connected");
    }
}

void MainWindow::on_btnSetKi_custom_clicked()
{
    QByteArray dataArray;
    quint32 scaledValue = 0;

    if (ui->ledKi->text() == NULL)
    {
        QToolTip::showText(ui->ledKi->mapToGlobal(QPoint()), "Enter value");
    }
    else
    {
        dataArray.append((char)(1 + ui->cboPIDAxis->currentIndex()));
        dataArray.append((char)(1 + ui->cboPIDName->currentIndex()));

        scaledValue = quint32 (ui->ledKi->text().toDouble() * 1000000);
        dataArray.append((char)((scaledValue >> 24) & 0x0ff));
        dataArray.append((char)((scaledValue >> 16) & 0x0ff));
        dataArray.append((char)((scaledValue >> 8) & 0x0ff));
        dataArray.append((char)((scaledValue) & 0x0ff));

        if (sendCommand(0x0a, dataArray) == true)
        {
            ui->pteStatus->appendPlainText("- Send: Set Ki " + ui->cboPIDAxis->currentText() +
                                           " " + ui->cboPIDName->currentText());
        }
        else
            ui->pteStatus->appendPlainText("- No COM Port is connected");
    }
}

void MainWindow::on_btnSetKd_custom_clicked()
{
    QByteArray dataArray;
    quint32 scaledValue = 0;

    if (ui->ledKd->text() == NULL)
    {
        QToolTip::showText(ui->ledKd->mapToGlobal(QPoint()), "Enter value");
    }
    else
    {
        dataArray.append((char)(1 + ui->cboPIDAxis->currentIndex()));
        dataArray.append((char)(1 + ui->cboPIDName->currentIndex()));

        scaledValue = quint32 (ui->ledKd->text().toDouble() * 1000000);
        dataArray.append((char)((scaledValue >> 24) & 0x0ff));
        dataArray.append((char)((scaledValue >> 16) & 0x0ff));
        dataArray.append((char)((scaledValue >> 8) & 0x0ff));
        dataArray.append((char)((scaledValue) & 0x0ff));

        if (sendCommand(0x0b, dataArray) == true)
        {
            ui->pteStatus->appendPlainText("- Send: Set Kd " + ui->cboPIDAxis->currentText() +
                                           " " + ui->cboPIDName->currentText());
        }
        else
            ui->pteStatus->appendPlainText("- No COM Port is connected");
    }
}

void MainWindow::on_btnSetKff1_custom_clicked()
{
    QByteArray dataArray;
    quint32 scaledValue = 0;

    if (ui->ledKff1->text() == NULL)
    {
        QToolTip::showText(ui->ledKff1->mapToGlobal(QPoint()), "Enter value");
    }
    else
    {
        dataArray.append((char)(1 + ui->cboPIDAxis->currentIndex()));
        dataArray.append((char)(1 + ui->cboPIDName->currentIndex()));

        scaledValue = quint32 (ui->ledKff1->text().toDouble() * 1000000);
        dataArray.append((char)((scaledValue >> 24) & 0x0ff));
        dataArray.append((char)((scaledValue >> 16) & 0x0ff));
        dataArray.append((char)((scaledValue >> 8) & 0x0ff));
        dataArray.append((char)((scaledValue) & 0x0ff));

        if (sendCommand(0x0c, dataArray) == true)
        {
            ui->pteStatus->appendPlainText("- Send: Set Kff1 " + ui->cboPIDAxis->currentText() +
                                           " " + ui->cboPIDName->currentText());
        }
        else
            ui->pteStatus->appendPlainText("- No COM Port is connected");
    }
}

void MainWindow::on_btnSetKff2_custom_clicked()
{
    QByteArray dataArray;
    quint32 scaledValue = 0;

    if (ui->ledKff2->text() == NULL)
    {
        QToolTip::showText(ui->ledKff2->mapToGlobal(QPoint()), "Enter value");
    }
    else
    {
        dataArray.append((char)(1 + ui->cboPIDAxis->currentIndex()));
        dataArray.append((char)(1 + ui->cboPIDName->currentIndex()));

        scaledValue = quint32 (ui->ledKff2->text().toDouble() * 1000000);
        dataArray.append((char)((scaledValue >> 24) & 0x0ff));
        dataArray.append((char)((scaledValue >> 16) & 0x0ff));
        dataArray.append((char)((scaledValue >> 8) & 0x0ff));
        dataArray.append((char)((scaledValue) & 0x0ff));

        if (sendCommand(0x0d, dataArray) == true)
        {
            ui->pteStatus->appendPlainText("- Send: Set Kff2 " + ui->cboPIDAxis->currentText() +
                                           " " + ui->cboPIDName->currentText());
        }
        else
            ui->pteStatus->appendPlainText("- No COM Port is connected");
    }
}

void MainWindow::on_btnGetParams_custom_clicked()
{
    QByteArray dataArray;

    dataArray.append((char)(1 + ui->cboPIDAxis->currentIndex()));
    dataArray.append((char)(1 + ui->cboPIDName->currentIndex()));

    if (sendCommand(0x0e, dataArray) == true)
    {
        ui->pteStatus->appendPlainText("- Send: Get Params " + ui->cboPIDAxis->currentText() +
                                       " " + ui->cboPIDName->currentText());
    }
    else
        ui->pteStatus->appendPlainText("- No COM Port is connected");
}

void MainWindow::on_timerCOMPort_custom_timeout()
{
    listCOMPort.clear();

    /* Add new item */
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        if (ui->cboCom->findText(info.portName()) == -1)
        {
            ui->cboCom->addItem(info.portName());
            ui->cboCom->setCurrentText(info.portName());
        }
        listCOMPort.append(info.portName());
    }

    /* Remove missing item */
    for (int i = 0; i < ui->cboCom->count(); i++)
    {
        if (listCOMPort.contains(ui->cboCom->itemText(i)) == false)
        {
            ui->cboCom->removeItem(i);
        }
    }
}

/* Private functions */
bool MainWindow::serialCOMPort_write(const QByteArray &data)
{
    if (serialCOMPort->isOpen() == true)
    {
        serialCOMPort->write(data);
        return true;
    }
    return false;
}

bool MainWindow::sendCommand(char msgID, const QByteArray &payload)
{
    QByteArray dataArray;
    quint16 checkSum = 0;

    /* Header */
    dataArray.append(QByteArray::fromRawData("\x47\x42\x02\x01\x00",5));

    /* Length */
    dataArray.append((char)(1 + payload.count() + 2));

    /* MsgID */
    dataArray.append(msgID);

    /* Payload */
    dataArray.append(payload);

    /* Check Sum */
    for (int i = 0; i < dataArray.count(); i++)
    {
        checkSum += static_cast<unsigned char>(dataArray.at(i));
    }
    checkSum = ~checkSum;
    dataArray.append((char)((checkSum >> 8) & 0x0ff));
    dataArray.append((char)(checkSum & 0x0ff));

    return serialCOMPort_write(dataArray);
}

bool MainWindow::parseMsg(const QByteArray &msg)
{
    QString showResMsg;
    quint16 checkSum = 0, checkSumMask = 0;
    int curValue = 0;

    /* Check DstID */
    if (msg.size() < 2) return false;
    if (msg.at(2) != 0x01) return false;

    /* Checksum */
    checkSumMask = (msg.at(msg.size() - 2) << 8) & 0x0ff00;
    checkSumMask += msg.at(msg.size() - 1) & 0x0ff;
    for (int i = 0; i < (msg.size() - 2); i++)
    {
        checkSum += static_cast<unsigned char>(msg.at(i));
    }
    checkSum = ~checkSum;
    if (checkSumMask != checkSum) return false;

    /* Handle Respond Set Message */
    showResMsg.clear();
    if (msg.size() < 6) return false;
    switch (msg.at(6))
    {
    case 0x01:
        showResMsg.append("- Receive: Set Home ");
        break;
    case 0x02:
        showResMsg.append("- Receive: Set Stop ");
        break;
    case 0x03:
        showResMsg.append("- Receive: Set Emergency Stop ");
        break;
    case 0x04:
        showResMsg.append("- Receive: Set Mode ");
        break;
    case 0x05:
        showResMsg.append("- Receive: Set Pos ");
        break;
    case 0x06:
        showResMsg.append("- Receive: Set Vel ");
        break;
    case 0x07:
        showResMsg.append("- Receive: Set Pos Vel ");
        break;
    case 0x09:
        showResMsg.append("- Receive: Set Kp ");
        break;
    case 0x0a:
        showResMsg.append("- Receive: Set Ki ");
        break;
    case 0x0b:
        showResMsg.append("- Receive: Set Kd ");
        break;
    case 0x0c:
        showResMsg.append("- Receive: Set Kff1 ");
        break;
    case 0x0d:
        showResMsg.append("- Receive: Set Kff2 ");
        break;
    }

    if (showResMsg.isEmpty() == false)
    {
        if (msg.size() < 8) return false;
        if (msg.at(7) == 0x01) showResMsg.append("AZ ");
        else if (msg.at(7) == 0x02) showResMsg.append("EL ");
        else if (msg.at(7) == 0x03) showResMsg.append("Both ");

        if (msg.at(8) == 0x00) showResMsg.append("Ok");
        else if (msg.at(8) == 0x01) showResMsg.append("Error");

        ui->pteStatus->appendPlainText(showResMsg);
        return true;
    }

    /* Handle Respond Get Message */
    switch (msg.at(6))
    {
    case 0x08:
        if (msg.size() < 11) return false;
        curValue = (msg.at(8) << 24) & 0x0ff000000;
        curValue += (msg.at(9) << 16) & 0x0ff0000;
        curValue += (msg.at(10) << 8) & 0x0ff00;
        curValue += msg.at(11) & 0x0ff;

        showResMsg.append("- Receive: Get Pos ");

        if (msg.at(7) == 0x01)
        {
            showResMsg.append("AZ ");
            ui->ledAZPos->setText(QString::number((double)curValue / 100.0, 'g', 2));
        }
        else if (msg.at(7) == 0x02)
        {
            showResMsg.append("EL ");
            ui->ledELPos->setText(QString::number((double)curValue / 100.0, 'g', 2));
        }

        showResMsg.append("Done");
        break;
    case 0x0e:
        if (msg.size() < 27) return false;
        curValue = (msg.at(8) << 24) & 0x0ff000000;
        curValue += (msg.at(9) << 16) & 0x0ff0000;
        curValue += (msg.at(10) << 8) & 0x0ff00;
        curValue += msg.at(11) & 0x0ff;
        ui->ledKp->setText(QString::number((double)curValue / 1000000.0, 'g', 6));

        curValue = (msg.at(12) << 24) & 0x0ff000000;
        curValue += (msg.at(13) << 16) & 0x0ff0000;
        curValue += (msg.at(14) << 8) & 0x0ff00;
        curValue += msg.at(15) & 0x0ff;
        ui->ledKi->setText(QString::number((double)curValue / 1000000.0, 'g', 6));

        curValue = (msg.at(16) << 24) & 0x0ff000000;
        curValue += (msg.at(17) << 16) & 0x0ff0000;
        curValue += (msg.at(18) << 8) & 0x0ff00;
        curValue += msg.at(19) & 0x0ff;
        ui->ledKd->setText(QString::number((double)curValue / 1000000.0, 'g', 6));

        curValue = (msg.at(20) << 24) & 0x0ff000000;
        curValue += (msg.at(21) << 16) & 0x0ff0000;
        curValue += (msg.at(22) << 8) & 0x0ff00;
        curValue += msg.at(23) & 0x0ff;
        ui->ledKff1->setText(QString::number((double)curValue / 1000000.0, 'g', 6));

        curValue = (msg.at(24) << 24) & 0x0ff000000;
        curValue += (msg.at(25) << 16) & 0x0ff0000;
        curValue += (msg.at(26) << 8) & 0x0ff00;
        curValue += msg.at(27) & 0x0ff;
        ui->ledKff2->setText(QString::number((double)curValue / 1000000.0, 'g', 6));

        showResMsg.append("- Receive: Get Params Done ");
        break;
    }
    ui->pteStatus->appendPlainText(showResMsg);
    return true;
}


