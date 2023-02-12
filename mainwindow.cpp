#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <sstream>
#include <iostream>
#include <QTimer>
#include <cstring>
#include <QVector>

MainWindow::~MainWindow()
{
    delete ui;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("PID Tunning GUI");

    updateSerialPort();

    mSerial = new QSerialPort(this);

    configAllButton(false);

    mSerialScanTimer = new QTimer(this);
    mSerialScanTimer->setInterval(5000);
    mSerialScanTimer->start();






}
void MainWindow::configAllButton(bool status)
{
   ui-> ClearRButton -> setEnabled(status);
   ui-> ClearTButton -> setEnabled(status);
   ui-> stopButton -> setEnabled(status);
   ui->turningButton->setEnabled(status);
   ui->sendButton->setEnabled(status);
   ui->sendButton->setEnabled(status);
}

void MainWindow::updateSerialPort()
{
       mSerialPorts = QSerialPortInfo::availablePorts();
       ui->COMComboBox->clear();
       for(const QSerialPortInfo &port: mSerialPorts)
       {
           ui -> COMComboBox -> addItem(port.portName(),  port.systemLocation());
       }
}

void MainWindow::on_openButton_clicked()
{
    ui->openButton->setEnabled(false);
    QString serialLoc  =  ui->COMComboBox->currentData().toString();
    mSerial->setPortName(serialLoc);
    mSerial->setBaudRate(static_cast<QSerialPort::BaudRate>(115200));
    mSerial->setDataBits(static_cast<QSerialPort::DataBits>(8));
    mSerial->setParity(QSerialPort::NoParity);
    mSerial->setStopBits(QSerialPort::OneStop);
    mSerial->setFlowControl(QSerialPort::NoFlowControl);

    if(mSerial->open(QIODevice::ReadWrite)) {

    QString text = "SERIAL: OK!\n";
    ui->textTransmit->append(text);
} else {
    QString text = "SERIAL: ERROR!\n";
    ui->textTransmit->append(text);
}

configAllButton(true);

ui->openButton->setEnabled(true);

QObject::connect(mSerial,SIGNAL(readyRead()),this,SLOT(Recievedata()));


}


void MainWindow::on_closeButton_clicked()
{
    if (mSerial->isOpen())
        {
            mSerial->close();
            QString text = "Serial Port: Close\n";
            ui->textTransmit->append(text);
            configAllButton(false);
        }
}


void MainWindow::on_ClearRButton_clicked()
{
    ui->textRecieve->clear();
}


void MainWindow::on_ClearTButton_clicked()
{
    ui->textTransmit->clear();
}


void MainWindow::on_sendButton_clicked()
{

    float fkp, fki, fkd, fsetPoint;
    //reading params from Line Edit
    QString kp = ui ->kpEdit->text();
    QString ki = ui ->kiEdit->text();
    QString kd = ui ->kdEdit->text();
    QString setPoint= ui->setPointEdit->text();

    //kp
    fkp=kp.toFloat();
    QByteArray bKp(reinterpret_cast<const char*>(&fkp), sizeof(fkp));

    //ki
    fki=ki.toFloat();
    QByteArray bKi(reinterpret_cast<const char*>(&fki), sizeof(fki));

    //kd
    fkd=kd.toFloat();
    QByteArray bKd(reinterpret_cast<const char*>(&fkd), sizeof(fkd));

    //setPoint
    fsetPoint=setPoint.toFloat();
    QByteArray bsetPoint(reinterpret_cast<const char*>(&fsetPoint), sizeof(fsetPoint));


    QByteArray bytes;
    QByteArray a ("02 53 50 49 44");
    a = QByteArray::fromHex(a);
    bytes.append(a+bKp+bKi+bKd+bsetPoint);

    qDebug() << bytes << "\n";
    qDebug() << a << "\n";
    mSerial->write(bytes);

    if(mSerial -> isWritable())
            {
        QString text = "Send succeed \n";
        ui->textTransmit->append(text);
            }
}



void MainWindow::on_turningButton_clicked()
{

}


void MainWindow::on_stopButton_clicked()
{

}

void MainWindow::Recievedata()
{
 QByteArray data=mSerial->readAll();
QString text = QString(data);
ui->textRecieve->append(text);
}




