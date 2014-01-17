#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui>

#define ZYTPORT 8010
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    updateTimer = new QTimer(this);

    tcpSocket = NULL;
    tcpServer = new QTcpServer(this);

    m_bshow = true;
//    updateTimer->start(1000);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    ui->pushButton_4->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::clientConnection()
{
    tcpSocket = tcpServer->nextPendingConnection();
    if (tcpSocket) {
        connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(clientDisconnection()));
        connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(clientReadData()));
    }
}

void MainWindow::clientDisconnection()
{
    QString sTmpshow;
    ui->textBrowser->append("===============Disconnected======================");
//    sTmpshow.sprintf("time :%s",QDateTime.time().toString());
    ui->textBrowser->append(sTmpshow);
    ui->textBrowser->append("");
}

void MainWindow::clientReadData()
{
    qint64 bytes = tcpSocket->bytesAvailable();
    QByteArray block(bytes, 0);
    int read = tcpSocket->read(block.data(), block.size());
    if (read > 0 && m_bshow) {
         qDebug() << "clientReadData()"<< read;
        QString recBytes,tmpByte;
        char *pdata = block.data();
        for(int i = 0;i <read; i++)
        {
            tmpByte.sprintf("%02x  ",(unsigned char)*pdata);
            recBytes += tmpByte;
            pdata++;
        }
        ui->textBrowser->append(recBytes);
    }
}

void MainWindow::start_work()
{
    if (tcpServer->listen(QHostAddress::Any, ZYTPORT)) {
        connect(tcpServer, SIGNAL(newConnection()), this, SLOT(clientConnection()));
         qDebug() << "Start listenning at port" << ZYTPORT;
    } else {
        qDebug() << "Failed to bind to port" << ZYTPORT;
        return ;
    }
}

void MainWindow::stop_work()
{
    tcpSocket->close();
    tcpServer->close();
    disconnect(tcpServer, SIGNAL(newConnection()), this, SLOT(clientConnection()));
}

void MainWindow::set_command()
{
    qDebug() <<"set_command";
    QString stmp = ui->lineEdit->text();
    QStringList comlist =  stmp.split(" ");
    int size = comlist.size();
    QByteArray block;
    qDebug() <<"set_command size"<<size;
    for(int i = 0; i < size; i++)
    {
        bool ok;
        data[i] = (unsigned char)comlist[i].toUShort(&ok,16);
        block.append(data[i]);
        if(!ok)
        {
            qDebug() <<"Input data error!!!";
            ui->textBrowser_2->append("Input data error, data must be in Hex mod!!!");
            return;
        }
    }
    int write = tcpSocket->write(block.data(),block.size());
    if(write<0)
    {
        qDebug() << "Failed to Set Command" ;
    }
    else
    {
        qDebug()<<"write num "<<write;
        ui->textBrowser_2->append(stmp);
    }
}

void MainWindow::on_pushButton_clicked()
{
    start_work();
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    ui->pushButton_4->setEnabled(true);
    ui->pushButton->setEnabled(false);
}

void MainWindow::on_pushButton_2_clicked()
{
    stop_work();
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    ui->pushButton_4->setEnabled(false);
    ui->pushButton->setEnabled(true);
}

//下发命令
void MainWindow::on_pushButton_3_clicked()
{
    set_command();
}

//暂停上传显示
void MainWindow::on_pushButton_4_clicked()
{
    m_bshow = !m_bshow;
    if(m_bshow)
    {
        ui->pushButton_4->setText(tr("stop show"));
        ui->pushButton_4->update();
    }
    else
    {
        ui->pushButton_4->setText(tr("conitue show"));
        ui->pushButton_4->update();
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    ui->textBrowser->clear();
}
