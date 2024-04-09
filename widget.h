#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QDateTime>
#include <QDebug>
#include <QBarSet>
#include <QBarSeries>
#include <QBarCategoryAxis>
#include <QChartView>
#include <QValueAxis>
#include <QLineSeries>
#include <QDateTimeAxis>
#include <QModbusTcpClient>
#include <QModbusReply>
#include <QMessageBox>
#include <QVector>
//#include <QtMqtt/qmqttclient.h>
#include <QMessageAuthenticationCode>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>
#include "dialog1.h"
#include "centeraligneddelegate.h"

#include "mqtt/qmqtt.h"
#include <QtNetwork>
#include <QHostAddress>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlTableModel>

using namespace QT_CHARTS_NAMESPACE;
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void InitiatChart();
    void InitiatModClient();
    void InitiatMQTT();
    void displayData();
    void InitTime();
    void uploadali();
    void ReadRequest();
    void generaChart();
    void alarm();
    void InitLogin();
    void InitDataBase();
    void InsertValue();
    void InsertValue2();
    void chargeTime();
    void chargeTime2();
    void startvideo();
    void stopvideo();
    void InitiatData();

private slots:

   // void on_pushButton_4_clicked();

    void on_timer0_clicked();

    void handtimeout();

    void ReadyRead();    

    void dataReceivedVideo();

    void dataProcessVideo();

    void receiveMessageSlot(QMQTT::Message message);

    void on_pushButton_5_clicked();

    void on_swich1Button_clicked();

    void receiveAssure1(bool flagsuccess);

    void currentIndexChangedSlot(int index);

    void on_displayTable_clicked();

    void on_clearTable_clicked();

    void on_refreshTable_clicked();

    void on_findData_clicked();

    void on_homebutton_clicked();

    void on_database_clicked();

    void on_monitor_clicked();

    void on_dataVisualize_clicked();

    void on_managermode_clicked();

    void on_videomonitor_clicked();

    void on_swich2Button_clicked();

    void on_startStream_clicked();

    void on_stopStream_clicked();

    void on_data_statistics_clicked();

    void on_comboBoxID_currentIndexChanged(int index);

private:
    Ui::Widget *ui;
    QTimer *timer;

    //chart function
    QLineSeries* m_lineSeries1,* m_lineSeries2,* m_lineSeries3,* m_lineSeries4;
    QLineSeries* m_lineSeries5,* m_lineSeries6,* m_lineSeries7,* m_lineSeries8;
    QValueAxis *m_axisX1, *m_axisY1;
    QValueAxis *m_axisX2, *m_axisY2;
    QValueAxis *m_axisX3, *m_axisY3;
    QValueAxis *m_axisX4, *m_axisY4;
    QChart* m_chart1,*m_chart2,*m_chart3,*m_chart4;

    const int AXIS_MAX_X1=10,AXIS_MAX_Y1=40;
    const int AXIS_MAX_X2=10,AXIS_MAX_Y2=100;
    const int AXIS_MAX_X3=10,AXIS_MAX_Y3=250;
    const int AXIS_MAX_X4=10;
    const double AXIS_MAX_Y4=0.5;

    int pointCount = 0;

    //modbus tcp client
    QModbusTcpClient *client;
    QVector<quint16> vector;
    quint16 temperature,temperature2;
    quint16 humidty,humidty2;
    qreal voltage,voltage2;
    qreal current,current2;
    qreal voltali;
    qreal currali;
    //flag
    int flagswich = 0,flagswich2=0;
    int flagtemp = 0;
    int flagtemp2 = 0;
    int flagButton = 0;
    //MQTT
  //     QMqttClient *m_client;
       QMQTT::Client *m_client;
       QString m_strProductKey;
       QString m_strDeviceName;
       QString m_strDeviceSecret;
       QString m_strRegionId;
       QString m_strTargetServer;
    //Login
       Dialog1 *my_login;
       int flagLogin = 0;

    //QsqltableModel
       QSqlDatabase db;
       QSqlQuery *query;
       QSqlTableModel *table;
       int m_index = 0;
       int m_index2 = 0;
    //QVideo
       QQueue<QByteArray> dataQueue;           //接收数据流
       QByteArray frameData;
       QByteArray frameBuffer;
       QNetworkReply *Client = nullptr;
       QNetworkAccessManager *Manager;
       bool startflag = false;
};
#endif // WIDGET_H
