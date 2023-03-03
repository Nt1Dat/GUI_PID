#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <sstream>
#include <iostream>
#include <QTimer>
#include <cstring>
#include <QVector>
#include <QThread>
int k , counter = 800;

QVector<double> a(801), b(801);

QVector<double> c , d;


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

    plotConfig();




}
void MainWindow::configAllButton(bool status)
{
   ui-> ClearRButton -> setEnabled(status);
   ui-> ClearTButton -> setEnabled(status);
   ui-> stopButton -> setEnabled(status);
   ui->vButton->setEnabled(status);
   ui->pButton->setEnabled(status);
   ui->sendButton->setEnabled(status);
}
void MainWindow:: plotSetting(QCustomPlot  *plot, const char* xLabel, const char * yLabel)
{
       QFont legendFont = font();
       legendFont.setPointSize(8);
       plot  -> yAxis->setLabel(yLabel);
       plot  -> xAxis-> setLabel(xLabel);
       plot  -> setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
       plot  -> legend -> setVisible(true);
       plot  -> legend->setFont(legendFont);
       plot  -> legend->setSelectedFont(legendFont);
       plot  -> legend->setSelectableParts(QCPLegend::spItems);
}

void MainWindow::plotConfig()
{
           QPen pen;
           pen.setStyle(Qt::SolidLine);
           pen.setWidth(3);
           pen.setColor("#E2483E");
           QPen pen2;
           pen2.setStyle(Qt::SolidLine);
           pen2.setWidth(3);
           pen2.setColor("#5C97E3");
       // plot init//


       ui -> plot -> addGraph();
       ui -> plot -> graph(0) -> setLineStyle(QCPGraph::lsLine);
       ui->plot->graph(0)->setPen(pen);
       ui->plot->graph(0)->setName("Ref");

       ui -> plot -> addGraph();
       ui -> plot -> graph(1) -> setLineStyle(QCPGraph::lsLine);
       ui->plot->graph(1)->setPen(pen2);
       ui->plot->graph(1)->setName("Tunning");

       b[0]=0;


       plotSetting(ui-> plot, "Time", "Velocity");
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

    QByteArray b ("03");
    b=QByteArray::fromHex(b);
    bytes.append(a+bKp+bKi+bKd+bsetPoint+b);

    qDebug() << bytes;

    mSerial->write(bytes);

    if(mSerial -> isWritable())
            {
        QString text = "Send succeed \n";
        ui->textTransmit->append(text);
            }
    //Plot print SetPoint
    c={0,800};
    d={fsetPoint,fsetPoint};
    ui -> plot -> graph(0) -> setData(c,d);
    ui->plot->rescaleAxes();
    ui->plot->replot();
    ui-> plot -> update();
}






void MainWindow::on_stopButton_clicked()
{
    QByteArray a ("02 53 54 4F 50 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 03");
    a = QByteArray::fromHex(a);
    mSerial->write(a);

    if(mSerial -> isWritable())
            {
        QString text = "Send succeed \n";
        ui->textTransmit->append(text);
            }
}


void MainWindow::Recievedata()
{
    code=".";

    QByteArray data=mSerial->readAll();
    qDebug()<<data;
    QString buffer = QString(data);
    tex= tex.append(buffer);
    int index = tex.indexOf(code);

    if(index != -1)
    {
       tex.replace(" ","");
       QString text = tex.mid(0,index);


    ui->textRecieve->append(text);
    if(k<counter)
    {
        a[k]=(text.toFloat());
        if(k>1){
            b[k]=(b[k-1]+1);
        }

        ui -> plot -> graph(1) -> setData(b,a);
        ui->plot->rescaleAxes();
        ui->plot->replot();
        ui-> plot -> update();

        k++;


    }
    else if(k==counter)
    {

        QByteArray a ("02 53 54 4F 50 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 03");
        a = QByteArray::fromHex(a);
        mSerial->write(a);

        if(mSerial -> isWritable())
                {
            QString text = "STOP \n";
            ui->textTransmit->append(text);
                }

   }
    tex.remove(0,index+code.size());
}
}


void MainWindow::on_pushButton_clicked()
{
    for(k=0;k<counter;k++)
    {
        a[k]=0;
        b[k]=0;
    }
    QVector<double> emptx = {0}, empty ={0};
    ui->plot->graph(1)->setData(emptx,empty);
    ui->plot->graph(0)->setData(emptx,empty);
    ui->plot->rescaleAxes();
    ui->plot->replot();
    ui-> plot -> update();
    k=0;

}




void MainWindow::on_vButton_clicked()
{
    QByteArray a ("02 56 54 55 4E 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 03");
    a = QByteArray::fromHex(a);
    mSerial->write(a);

    if(mSerial -> isWritable())
            {
        QString text = "Send succeed \n";
        ui->textTransmit->append(text);
            }

}


void MainWindow::on_pButton_clicked()
{
    QByteArray a ("02 50 54 55 4E 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 03");
    a = QByteArray::fromHex(a);
    mSerial->write(a);

    if(mSerial -> isWritable())
            {
        QString text = "Send succeed \n";
        ui->textTransmit->append(text);
            }

}

