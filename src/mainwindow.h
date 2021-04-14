#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore>
#include <QMainWindow>
#include <QSettings>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDateTime>
#include <QLabel>
#include <QPushButton>

class AppConfig
{
public:
    enum TextFormat
    {
        HEX = 0,
        ASCII
    };
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
    void timerEvent(QTimerEvent *event);

public slots:
    void setRecvTextFormat();
    void sendFlow(void);
    void sendUserData(QString &data);

private:
    Ui::MainWindow *ui;
    QSettings *     configIni;
    qint32          timer1S;

    QSerialPort serialPort;

    AppConfig::TextFormat textFormat;

    QByteArray curRecvBuffer;

    quint32      recvCnt{0};
    quint32      sendCnt{0};
    QLabel *     label_recvCnt;
    QLabel *     label_sendCnt;
    QPushButton *btn_resetCnt;
};
#endif  // MAINWINDOW_H
