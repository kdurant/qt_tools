#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    configIni = new QSettings("./config.ini", QSettings::IniFormat);

    initParameter();
    serialPortDetect();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// configIni->value("System/oceanPort")
void MainWindow::initParameter()
{
}

//configIni->setValue("Laser/freq", 1111);
void MainWindow::saveParameter()
{
    configIni->setValue("System/RadarType", "land");
    configIni->setValue("Laser/freq", 1111);
}

void MainWindow::serialPortDetect()
{
    QList<QSerialPortInfo> port = QSerialPortInfo::availablePorts();
    ui->serialPortComb->clear();
    for(int i = 0; i < port.length(); i++)
        ui->serialPortComb->addItem(port.at(i).portName());
    return;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveParameter();
}
