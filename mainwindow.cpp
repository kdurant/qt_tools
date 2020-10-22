#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    configIni = new QSettings("./config.ini", QSettings::IniFormat);

    initParameter();
    serialPortDetect();
    connectSignalSolt();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// configIni->value("System/oceanPort")
void MainWindow::initParameter()
{
    uartRecv.textFormat = ui->rbt_recvHexFormat->isChecked() ? UartRecv::HEX : UartRecv::ASCII;
}

//configIni->setValue("Laser/freq", 1111);
void MainWindow::saveParameter()
{
    configIni->setValue("System/RadarType", "land");
    configIni->setValue("Laser/freq", 1111);
}

void MainWindow::connectSignalSolt()
{
    connect(ui->rbt_recvHexFormat, SIGNAL(clicked(bool)), this, SLOT(setRecvTextFormat()));
    connect(ui->rbt_recvAsciiFormat, SIGNAL(clicked(bool)), this, SLOT(setRecvTextFormat()));
}

void MainWindow::serialPortDetect()
{
    QList<QSerialPortInfo> port = QSerialPortInfo::availablePorts();
    ui->serialPortComb->clear();
    for(int i = 0; i < port.length(); i++)
        ui->serialPortComb->addItem(port.at(i).portName());
    return;
}

void MainWindow::setRecvTextFormat()
{
    uartRecv.textFormat = ui->rbt_recvHexFormat->isChecked() ? UartRecv::HEX : UartRecv::ASCII;

    QString recvData = ui->pte_recvData->toPlainText();
    if(uartRecv.textFormat == UartRecv::HEX)
    {
        ui->pte_recvData->setPlainText(recvData.toUtf8().toHex());
    }
    else
    {
        ui->pte_recvData->setPlainText(QByteArray::fromHex(recvData.toUtf8()));
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveParameter();
}
