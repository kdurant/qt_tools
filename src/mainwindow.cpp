#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTimer>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    configIni = new QSettings("./config.ini", QSettings::IniFormat);

    initParameter();
    initUI();
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

void MainWindow::initUI()
{
    setWindowTitle("调试工具");
}

void MainWindow::connectSignalSolt()
{
    connect(ui->rbt_recvHexFormat, &QAbstractButton::clicked, this, &MainWindow::setRecvTextFormat);
    connect(ui->rbt_recvAsciiFormat, &QAbstractButton::clicked, this, &MainWindow::setRecvTextFormat);

    connect(ui->rbt_serialOpen, &QAbstractButton::clicked, this, [this]() {
        serialPort.setPortName(ui->comb_serialPortSelect->currentText());
        serialPort.setBaudRate(ui->comb_serialBaudrate->currentText().toInt());
        serialPort.setDataBits(QSerialPort::Data8);
        serialPort.setParity(QSerialPort::NoParity);
        serialPort.setStopBits(QSerialPort::OneStop);
        serialPort.setFlowControl(QSerialPort::NoFlowControl);
        serialPort.open(QIODevice::ReadWrite);
        ui->comb_serialPortSelect->setEnabled(false);
    });
    connect(ui->rbt_serialClose, &QAbstractButton::clicked, this, [this]() {
        serialPort.close();
        ui->comb_serialPortSelect->setEnabled(true);
    });

    connect(ui->bt_serialSend, &QAbstractButton::clicked, this, [this]() {
        uartSend.textFormat = ui->rbt_sendHexFormat->isChecked() ? UartSend::HEX : UartSend::ASCII;

        sendBuffer = ui->pte_sendData->toPlainText();
        if(uartSend.textFormat == UartSend::HEX)
        {
            serialPort.write(QByteArray::fromHex(sendBuffer.toLatin1().data()));
        }
        else
        {
            serialPort.write(sendBuffer.toUtf8());
        }
    });
    connect(&serialPort, &QIODevice::readyRead, this, [this]() {
        curRecvBuffer = serialPort.readAll();
        totRecvBuffer += curRecvBuffer;

        if(!curRecvBuffer.isEmpty())
        {
            if(uartRecv.textFormat == UartRecv::HEX)
            {
            }
            else
            {
                QString data = curRecvBuffer;
                ui->pte_recvData->appendPlainText(data);
            }
        }
    });
}

void MainWindow::serialPortDetect()
{
    QList<QSerialPortInfo> port = QSerialPortInfo::availablePorts();
    ui->comb_serialPortSelect->clear();
    for(int i = 0; i < port.length(); i++)
        ui->comb_serialPortSelect->addItem(port.at(i).portName());
    return;
}

void MainWindow::setRecvTextFormat()
{
    uartRecv.textFormat = ui->rbt_recvHexFormat->isChecked() ? UartRecv::HEX : UartRecv::ASCII;

    QString recvData = ui->pte_recvData->toPlainText();
    if(uartRecv.textFormat == UartRecv::HEX)
    {
        QByteArray  ba = recvData.toUtf8().toHex();
        QStringList s;

        for(int i = 0; i < ba.length(); i += 2)
            s.push_back(ba.mid(i, 2));
        ui->pte_recvData->setPlainText(s.join(" "));
    }
    else
    {
        ui->pte_recvData->setPlainText(QByteArray::fromHex(recvData.toUtf8()));
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    saveParameter();
}
