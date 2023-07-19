#include "mainwindow.h"
#include "ui_mainwindow.h"

bool lock_flag = false;
QDateTime time_recv;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tcpServer = new QTcpServer(this);
    tcpSocket = new QTcpSocket(this);
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(newConnection_Slot()));
    ui->portEdit->setText("1234");
    tcpServer->listen(QHostAddress::Any,ui->portEdit->text().toUInt());

    ui->red_label->show();
    ui->green_label->hide();

    timer->start(1000);

    connect(timer,SIGNAL(timeout()),this,SLOT(timeout_slot()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_connect_btn_clicked()
{
    tcpServer->listen(QHostAddress::Any,ui->portEdit->text().toUInt());
}

void MainWindow::newConnection_Slot()
{
    tcpSocket = tcpServer->nextPendingConnection(); //获得已经连接的客户端的Socket
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(readyRead_Slot()));
    ui->green_label->show();
    ui->red_label->hide();
}


void MainWindow::readyRead_Slot()
{
    QString buf;

    buf = tcpSocket->readAll();
//    if(buf.contains("aaa"))
    if(buf.at(0) == '\x01')
    {
        if(lock_flag)   lock_flag=0;
        else    lock_flag=1;
    }
    else
    {
        ui->recvEdit->append(time_recv.toString("yyyy年MM月dd日hh:mm:ss"));
        ui->recvEdit->append(buf);
    }


}

void MainWindow::on_send_btn_clicked()
{
    tcpSocket->write(ui->sendEdit->toPlainText().toLocal8Bit().data());        //转成char*
}


void MainWindow::on_close_btn_clicked()
{
    tcpServer->close();
    ui->red_label->show();
    ui->green_label->hide();
}


void MainWindow::timeout_slot()
{

    QDateTime time = QDateTime::currentDateTime();
    time_recv = time;
    ui->time_label->setText(time.toString("hh:mm:ss"));

}


