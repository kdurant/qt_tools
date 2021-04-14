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
    textFormat = ui->rbt_recvHexFormat->isChecked() ? AppConfig::HEX : AppConfig::ASCII;
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
    label_recvCnt = new QLabel(QString("%1").arg("接收计数：0", -20));
    label_sendCnt = new QLabel(QString("%1").arg("发送计数：0", -20));
    btn_resetCnt  = new QPushButton("重置计数");
    //    btn_resetCnt->setFixedSize(40, 20);
    btn_resetCnt->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Policy::Preferred);
    ui->statusbar->addPermanentWidget(label_recvCnt);
    ui->statusbar->addPermanentWidget(label_sendCnt);
    ui->statusbar->addPermanentWidget(btn_resetCnt);
    ui->statusbar->setStyleSheet("QStatusBar {min-height: 35;}");
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

    /*
     * 发送数据部分设置
     */
    connect(ui->checkBox_sendCyclic, &QCheckBox::stateChanged, this, [this](int state) {
        if(state == Qt::Checked)
        {
            qint32 interval = ui->lineEdit_sendCyclicTime->text().toUInt();
            timer1S         = startTimer(interval);
        }
        else
            killTimer(timer1S);
    });
    connect(ui->btn_sendData, &QAbstractButton::clicked, this, [this]() {
        sendFlow();
    });
    connect(ui->btn_recvBufferClear, &QPushButton::clicked, this, [this]() {
        ui->pte_recvData->clear();
    });

    /*
     * 接收数据部分
     */
    connect(&serialPort, &QIODevice::readyRead, this, [this]() {
        curRecvBuffer = serialPort.readAll();

        if(!curRecvBuffer.isEmpty())
        {
            if(textFormat == AppConfig::HEX)
            {
            }
            else
            {
                QString data = curRecvBuffer;
                ui->pte_recvData->appendPlainText(data);
            }
        }

        recvCnt += curRecvBuffer.size();
        label_recvCnt->setText(QString("%1").arg("接收计数：" + QString::number(recvCnt), -20));
    });

    connect(btn_resetCnt, &QPushButton::clicked, this, [this]() {
        sendCnt = 0;
        label_sendCnt->setText(QString("%1").arg("发送计数：" + QString::number(sendCnt), -20));

        recvCnt = 0;
        label_recvCnt->setText(QString("%1").arg("接收计数：" + QString::number(recvCnt), -20));
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

void MainWindow::timerEvent(QTimerEvent *event)
{
    if(timer1S == event->timerId())
    {
        sendFlow();
    }
}

void MainWindow::setRecvTextFormat()
{
    textFormat = ui->rbt_recvHexFormat->isChecked() ? AppConfig::HEX : AppConfig::ASCII;

    QString recvData = ui->pte_recvData->toPlainText();
    if(textFormat == AppConfig::HEX)
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

void MainWindow::sendFlow()
{
    if(ui->checkBox_sendShowContent->isChecked())
    {
        // yyyy.MM.dd hh:mm:ss.zzz ddd
        QString prefix = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
        prefix += "--> ";
        ui->pte_recvData->appendPlainText(prefix + ui->pte_sendData->toPlainText());
    }

    QString data = ui->pte_sendData->toPlainText();
    ui->listWidget_sendHistory->addItem(data);

    if(ui->checkBox_sendAddNewLine->isChecked())
        data.append("\r\n");
    sendUserData(data);
}

void MainWindow::sendUserData(QString &data)
{
    if(data.isEmpty())
        return;

    textFormat = ui->rbtn_sendHexFormat->isChecked() ? AppConfig::HEX : AppConfig::ASCII;

    QByteArray frame;
    if(textFormat == AppConfig::HEX)
        frame = QByteArray::fromHex(data.toLatin1().data());
    else
        frame = data.toUtf8();
    serialPort.write(frame);

    sendCnt += frame.size();
    label_sendCnt->setText(QString("%1").arg("发送计数：" + QString::number(sendCnt), -20));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    saveParameter();
}
