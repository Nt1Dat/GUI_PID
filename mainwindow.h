#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <cstring>
#include <QVector>
#include <QMessageBox>
#include <QScrollBar>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_openButton_clicked();

    void on_closeButton_clicked();

    void on_ClearRButton_clicked();

    void on_ClearTButton_clicked();

    void on_sendButton_clicked();

    void on_turningButton_clicked();

    void on_stopButton_clicked();

    void Recievedata();


private:
    Ui::MainWindow *ui;

    QSerialPort *mSerial;
    QList <QSerialPortInfo> mSerialPorts;
    QTimer *mSerialScanTimer;


    void updateSerialPort();
    void configAllButton(bool status);
};
#endif // MAINWINDOW_H
