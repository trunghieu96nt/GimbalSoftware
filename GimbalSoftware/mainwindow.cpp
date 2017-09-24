#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* Connect Signals and Slots */
    connect(ui->draggableTitleBar, SIGNAL(mousePressing(QMouseEvent*)),
            this, SLOT(on_draggableTitleBar_mousePressing(QMouseEvent*)));
    connect(ui->draggableTitleBar, SIGNAL(mouseMoving(QMouseEvent*)),
            this, SLOT(on_draggableTitleBar_mouseMoving(QMouseEvent*)));
    connect(ui->btnClose, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->btnMinimize, SIGNAL(clicked()), this, SLOT(on_btnMinimize_clicked()));

    /*  */
    ui->cboAxis->addItem("AZ");
    ui->cboAxis->addItem("EL");
    ui->cboAxis->setToolTip("Axis");

    ui->cboPIDName->addItem("Manual");
    ui->cboPIDName->addItem("Outer");
    ui->cboPIDName->addItem("Inner");
    ui->cboPIDName->setToolTip("PID");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnMinimize_clicked()
{

    this->setWindowState(Qt::WindowMinimized);
}

void MainWindow::on_draggableTitleBar_mousePressing(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::on_draggableTitleBar_mouseMoving(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}
