#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QDateTime>
#include <QPainter>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket;
    QTimer *timer = new QTimer(this);

private slots:
    void on_connect_btn_clicked();

    void newConnection_Slot();

    void readyRead_Slot();

    void timeout_slot();

    void on_send_btn_clicked();

    void on_close_btn_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
