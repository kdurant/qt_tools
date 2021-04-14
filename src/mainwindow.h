#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QSerialPort>
#include <QSerialPortInfo>

class UartSend
{
public:
    enum TextFormat
    {
        HEX = 0,
        ASCII
    };
    TextFormat textFormat;
    bool       autoSend;
    int        autoSendTime;  // unit us
    bool       addNewLine;
};

class UartRecv
{
public:
    enum TextFormat
    {
        HEX = 0,
        ASCII
    };
    TextFormat textFormat;
    bool       addNewLine;
    bool       addTimestamp;
};

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
    void closeEvent(QCloseEvent *event);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initParameter();
    void saveParameter();
    void initUI();
    void connectSignalSolt();

    // 串口相关函数
    void serialPortDetect();

protected:
    void timerEventer(QEvent *event);

public slots:
    void setRecvTextFormat();

private:
    Ui::MainWindow *ui;
    QSettings *     configIni;

    QSerialPort serialPort;
    bool        serialStatus;
    UartRecv    uartRecv;
    UartSend    uartSend;
    QString     sendBuffer;
    QByteArray  curRecvBuffer;
    QString     totRecvBuffer;
};
#endif  // MAINWINDOW_H
