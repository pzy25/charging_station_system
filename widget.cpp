#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    client = new QModbusTcpClient(this);
//    m_client = new QMqttClient(this);
    my_login = new Dialog1(this);
    timer = new QTimer(this);
    timer->setInterval(1000);

    InitTime();
    InitiatChart();
    InitiatModClient();
    InitiatMQTT();
    InitLogin();
    InitDataBase();
    InitiatData();
    connect(timer,&QTimer::timeout,this,&Widget::ReadRequest);
    connect(timer,&QTimer::timeout,this,&Widget::InitTime);

}
Widget::~Widget()
{
    delete ui;
}

// =========== ModbusTCP代码 ===========

void Widget::InitiatModClient()
{
    client->setConnectionParameter(QModbusDevice::NetworkPortParameter,500);
    client->setConnectionParameter(QModbusDevice::NetworkAddressParameter,"192.168.1.100");
    client->setTimeout(1000);
    client->setNumberOfRetries(1);
    if(client->state()!=QModbusDevice::ConnectedState)
    {
        client->connectDevice();
    }
    else{
        client->disconnectDevice();
    }

}

void Widget::Receive_Data()
{
    displayData();
    generaChart();
    uploadali();
    alarm();

    static int timenumber = 2;
    while (!(timenumber--)) {
       InsertValue();
       InsertValue2();
       timenumber = 2;
    }
}

void Widget::ReadRequest()
{
    QModbusDataUnit readunit(QModbusDataUnit::HoldingRegisters,0,24); //24个寄存器
   if(auto *reply =client->sendReadRequest(readunit,1))
    {
        if (!reply->isFinished())
                {
                   QObject::connect(reply, &QModbusReply::finished,this,&Widget::Receive_Data);
                    qDebug()<<"zhengzaiduqu"<<endl;
                }
                else
                {
                    delete reply;
                }
   }
}

void Widget::displayData()
{
    auto *reply = qobject_cast<QModbusReply *>(sender());
    if (!reply){
        return ;
    }
    if (reply->error() == QModbusDevice::NoError)
    {

        const QModbusDataUnit readData = reply->result();
        vector.append(readData.value(0));

        temperature =readData.value(0);
        humidty = readData.value(1);
        voltage = readData.value(2);
        current = readData.value(3)/100.0; //电流改变
        flagswich = readData.value(4);

        temperature2 =readData.value(8);
        humidty2 = readData.value(9);
        voltage2 = readData.value(10);
        current2 = readData.value(11)/100.0;
        flagswich2 =readData.value(12);

        temperature3 =readData.value(16);
        humidty3 = readData.value(17);
        voltage3 = readData.value(18);
        current3 = readData.value(19)/100.0;
        flagswich3 =readData.value(20);


        QString str1 =QString("%1").arg(temperature);
        QString str2 =QString("%1").arg(humidty);
        QString str3 =QString("%1").arg(voltage,0,'f',1);
        QString str4 =QString("%1").arg(current,0,'f',2);//显示的是小数点后的位数

        QString str5 =QString("%1").arg(temperature2);
        QString str6 =QString("%1").arg(humidty2);
        QString str7 =QString("%1").arg(voltage2,0,'f',1);
        QString str8 =QString("%1").arg(current2,0,'f',2);//显示的是小数点后的位数

        QString str9 =QString("%1").arg(temperature3);
        QString str10 =QString("%1").arg(humidty3);
        QString str11 =QString("%1").arg(voltage3,0,'f',1);
        QString str12 =QString("%1").arg(current3,0,'f',2);//显示的是小数点后的位数


        ui->temp1->setText(str1+"℃");
        ui->hum1->setText(str2+"%");
        ui->vol1->setText(str3);
        ui->current1->setText(str4);

        ui->temp2->setText(str5+"℃");
        ui->hum2->setText(str6+"%");
        ui->vol2->setText(str7);
        ui->current2->setText(str8);

        ui->temp3->setText(str9+"℃");
        ui->hum3->setText(str10+"%");
        ui->vol3->setText(str11);
        ui->current3->setText(str12);


        if(flagswich)
        {
        ui->swich1->setText("ON");
        ui->swich1Button->setText("断电");
        chargeTime();
         }
        else
        {
            ui->swich1->setText("OFF");
           ui->swich1Button->setText("充电");
           //断电后不显示电压电流
           ui->vol1->setText("0.0");
           ui->current1->setText("0.00");
        }

        if(flagswich2)
        {
        ui->swich2->setText("ON");
        ui->swich2Button->setText("断电");
        chargeTime2();
         }
        else
        {
            ui->swich2->setText("OFF");
           ui->swich2Button->setText("充电");
           //断电后不显示电压电流
           ui->vol2->setText("0.0");
           ui->current2->setText("0.00");
        }

        if(flagswich3)
        {
        ui->swich3->setText("ON");
        ui->swich3Button->setText("断电");
        chargeTime2();
         }
        else
        {
            ui->swich3->setText("OFF");
           ui->swich3Button->setText("充电");
           //断电后不显示电压电流
           ui->vol3->setText("0.0");
           ui->current3->setText("0.00");
        }


        qDebug()<<temperature3<< humidty3<<voltage3<<current3<<flagswich3<<endl;
        qDebug()<<"读取成功"<<endl;
        vector.clear();
    }
    else
    {
        qDebug()<<"读取失败"<<endl;

    }
    reply->deleteLater(); // delete the reply
}

void Widget::on_swich1Button_clicked()
{
    if(!flagswich)
    {
        QModbusDataUnit writeunit(QModbusDataUnit::HoldingRegisters,6,1);//充电
        writeunit.setValue(0,1);
         QModbusReply *reply = client->sendWriteRequest(writeunit,1);
         if(reply){
             reply->deleteLater();
         }
        flagswich = 1;
        ui->swich1->setText("ON");
        ui->swich1Button->setText("断电");
    }
    else
    {
        QModbusDataUnit writeunit(QModbusDataUnit::HoldingRegisters,6,1);//断电
        writeunit.setValue(0,0);
         QModbusReply *reply = client->sendWriteRequest(writeunit,1);
         if(reply){
             reply->deleteLater();
         }
         flagswich = 0;
         ui->swich1->setText("OFF");
        ui->swich1Button->setText("充电");
        //断电后不显示电压电流
        ui->vol1->setText("0.00");
        ui->current1->setText("0.00");
    }
}

void Widget::on_swich2Button_clicked()
{
    if(!flagswich2)
    {
        QModbusDataUnit writeunit(QModbusDataUnit::HoldingRegisters,14,1);//充电
        writeunit.setValue(0,1);
         QModbusReply *reply = client->sendWriteRequest(writeunit,1);
         if(reply){
             reply->deleteLater();
         }
        flagswich2 = 1;
        ui->swich2->setText("ON");
        ui->swich2Button->setText("断电");
    }
    else
    {
        QModbusDataUnit writeunit(QModbusDataUnit::HoldingRegisters,14,1);//断电
        writeunit.setValue(0,0);
         QModbusReply *reply = client->sendWriteRequest(writeunit,1);
         if(reply){
             reply->deleteLater();
         }
         flagswich2 = 0;
         ui->swich2->setText("OFF");
        ui->swich2Button->setText("充电");
        //断电后不显示电压电流
        ui->vol2->setText("0.00");
        ui->current2->setText("0.00");
    }
}

void Widget::on_swich3Button_clicked()
{
    if(!flagswich3)
    {
        QModbusDataUnit writeunit(QModbusDataUnit::HoldingRegisters,22,1);//充电
        writeunit.setValue(0,1);
         QModbusReply *reply = client->sendWriteRequest(writeunit,1);
         if(reply){
             reply->deleteLater();
         }
        flagswich3 = 1;
        ui->swich3->setText("ON");
        ui->swich3Button->setText("断电");
    }
    else
    {
        QModbusDataUnit writeunit(QModbusDataUnit::HoldingRegisters,22,1);//断电
        writeunit.setValue(0,0);
         QModbusReply *reply = client->sendWriteRequest(writeunit,1);
         if(reply){
             reply->deleteLater();
         }
         flagswich3 = 0;
         ui->swich3->setText("OFF");
        ui->swich3Button->setText("充电");
        //断电后不显示电压电流
        ui->vol3->setText("0.00");
        ui->current3->setText("0.00");
    }
}

void Widget::chargeTime()
{
   int min1;
   static int ChargeTime1 = 0;
   ChargeTime1++;
   min1=ChargeTime1/60;
   QString str =QString("%1分钟").arg(min1);
   ui->chargetime->setText(str);

}

void Widget::chargeTime2()
{
    int min1;
    static int ChargeTime1 = 0;
    ChargeTime1++;
    min1=ChargeTime1/60;
    QString str =QString("%1分钟").arg(min1);
    ui->chargetime2->setText(str);
}


// =========== MQTT代码 ===========

void Widget::InitiatMQTT()
{

    m_strProductKey="ie44FqhJNgx";  //需要跟阿里云Iot平台一致;
    m_strDeviceName="D001";   //需要跟阿里云Iot平台一致;
    m_strDeviceSecret="867e361641c03ffdf283fd4bccc593c8";   //需要跟阿里云平台一致
    m_strRegionId="cn-shanghai";
    m_strTargetServer = m_strProductKey + ".iot-as-mqtt." + m_strRegionId + ".aliyuncs.com";//域名

    QString clientId="abcdefg";       //表示客户端ID，建议使用设备的MAC地址或SN码，64字符内。
    QString signmethod = "hmacsha1";    //加密方式
    QString message ="clientId"+clientId+"deviceName"+m_strDeviceName+"productKey"+m_strProductKey;

    QHostAddress m_address(m_strTargetServer);
    m_client = new QMQTT::Client(m_address,1883);
    m_client->setClientId(clientId + "|securemode=3,signmethod=" + signmethod + /*",timestamp="+timestamp+ */"|");
    m_client->setUsername(m_strDeviceName + "&" + m_strProductKey);
    m_client->setPassword(QMessageAuthenticationCode::hash(message.toLocal8Bit(),m_strDeviceSecret.toLocal8Bit(),
                                                           QCryptographicHash::Sha1).toHex());
    m_client->setKeepAlive(120);
    m_client->setHostName(m_strTargetServer);
    m_client->setPort(1883);
    m_client->connectToHost();

    QString subscription="/ie44FqhJNgx/D001/user/get";
    //QString topic="/ie44FqhJNgx/D001/user/update";
    m_client->subscribe(subscription);
    connect(m_client,SIGNAL(received(QMQTT::Message)),this,SLOT(receiveMessageSlot(QMQTT::Message)));
    qDebug()<<"连接阿里云"<<endl;
}

void Widget::uploadali()
{
    int aliswich1,aliswich2,aliswich3;
    if(ui->swich1->text()=="OFF")
    {
        aliswich1 = 0;
    }
    else
    {
        aliswich1 = 1;
    }
    if(ui->swich2->text()=="OFF")
    {
        aliswich2 = 0;
    }
    else
    {
        aliswich2 = 1;
    }
    if(ui->swich3->text()=="OFF")
    {
        aliswich3 = 0;
    }
    else
    {
        aliswich3 = 1;
    }

    QString topic="/ie44FqhJNgx/D001/user/update";
    QString msg="{\"temperature1\":";
            msg+=ui->temp1->text().mid(0,2);
            msg+=",\"Humidity1\":";
             msg+=ui->hum1->text().mid(0,2);
              msg+=",\"RMSCurrent1\":";
              msg+=ui->current1->text();
              msg+=",\"RMSVoltage1\":";
              msg+=ui->vol1->text();
              msg+=",\"NO1\":";
              msg+=QString::number(aliswich1);
              msg+=",\"GeoLocation1\":";
              msg+=QString::number(aliswich1);
              msg+="}";
    QByteArray ba;
    ba.append(msg);
    QMQTT::Message message(1,topic,ba);
    m_client->publish(message);

    QString msg2="{\"temperature2\":";
    msg2+=ui->temp2->text().mid(0,2);
    msg2+=",\"Humidity2\":";
    msg2+=ui->hum2->text().mid(0,2);
    msg2+=",\"RMSCurrent2\":";
    msg2+=ui->current2->text();
    msg2+=",\"RMSVoltage2\":";
    msg2+=ui->vol2->text();
    msg2+=",\"NO2\":";
    msg2+=QString::number(aliswich2);
    msg2+=",\"GeoLocation2\":";
    msg2+=QString::number(aliswich2);
    msg2+="}";
    QByteArray ba2;
    ba2.append(msg2);
    QMQTT::Message message2(2,topic,ba2);
    m_client->publish(message2);

    QString msg3="{\"temperature3\":";
    msg3+=ui->temp3->text().mid(0,2);
    msg3+=",\"Humidity3\":";
    msg3+=ui->hum3->text().mid(0,2);
    msg3+=",\"RMSCurrent3\":";
    msg3+=ui->current3->text();
    msg3+=",\"RMSVoltage3\":";
    msg3+=ui->vol3->text();
    msg3+=",\"NO3\":";
    msg3+=QString::number(aliswich3);
    msg3+=",\"GeoLocation3\":";
    msg3+=QString::number(aliswich3);
    msg3+="}";
    QByteArray ba3;
    ba3.append(msg3);
    QMQTT::Message message3(3,topic,ba3);
    m_client->publish(message3);

    qDebug()<<"上传1,2,3成功"<<endl;
   // qDebug()<<topic<<endl;
}

void Widget::receiveMessageSlot(QMQTT::Message message)
{
    QByteArray pay_load = message.payload();
    QJsonParseError jsonError;
    QJsonDocument doc = QJsonDocument::fromJson(pay_load, &jsonError);
    qDebug()<<"收到消息";
    if (doc.isObject()) {
            QJsonObject object = doc.object();  // 转化为对象

            QJsonValue value = object.value("data");
            QString str1 = value.toString();
            int data= value.toInt();
            QString num=QString::number(data);

            QJsonValue name = object.value("cmd");  // 获取指定 key 对应的 value
            QString cmd = name.toString();// 将 value 转化为字符串

            if(cmd=="NO1"){
              if( (data&&(!flagswich)) | ((!data)&&flagswich) )
               on_swich1Button_clicked();
            }
           else if(cmd=="NO2"){
              if( (data&&(!flagswich2)) | ((!data)&&flagswich2) )
                   on_swich2Button_clicked();
               }
           else if(cmd=="NO3"){
              if( (data&&(!flagswich3)) | ((!data)&&flagswich3) )
                   on_swich3Button_clicked();
               }
           else if(cmd=="TimerSwitch1"){
                QModbusDataUnit writeunit(QModbusDataUnit::HoldingRegisters,5,1);
                writeunit.setValue(0,data);
                 QModbusReply *reply = client->sendWriteRequest(writeunit,1);
                 if(reply){
                     reply->deleteLater();
                 }
                 if(data)
                 {
                    ui->appoint1->setText("已预约");
                 }
                 else {
                    ui->appoint1->setText("未预约");
                 }

            }
          else  if(cmd=="TimerSwitch2"){
                QModbusDataUnit writeunit(QModbusDataUnit::HoldingRegisters,13,1); //寄存器+1
                writeunit.setValue(0,data);
                 QModbusReply *reply = client->sendWriteRequest(writeunit,1);
                 if(reply){
                     reply->deleteLater();
                 }
                 if(data)
                 {
                    ui->appoint2->setText("已预约");
                 }
                 else {
                    ui->appoint2->setText("未预约");
                 }


            }

        else   if(cmd=="TimerSwitch3"){
                QModbusDataUnit writeunit(QModbusDataUnit::HoldingRegisters,21,1); //寄存器+1
                writeunit.setValue(0,data);
                 QModbusReply *reply = client->sendWriteRequest(writeunit,1);
                 if(reply){
                     reply->deleteLater();
                 }
                 if(data)
                 {
                    ui->appoint3->setText("已预约");
                 }
                 else {
                    ui->appoint3->setText("未预约");
                 }

            }
        else   if(cmd=="ParkedNumber"){
               ui->packedNumber->setText(str1);
               qDebug()<<"parkednumber"<<str1;

            }

     }
}

// =========== Chart代码 ===========

void Widget::InitiatChart()
{
    ui->chargetime->setText("0分钟");
    ui->chargetime2->setText("0分钟");
    ui->chargetime3->setText("0分钟");
    ui->stopStream->setEnabled(false);

    m_axisX1 = new QValueAxis();
    m_axisY1 = new QValueAxis();
   // m_axisX1->setTitleText("X-时间");
    m_axisY1->setTitleText("Y-温度");
    m_axisX1->setMin(0);
    m_axisY1->setMin(0);
    m_axisX1->setMax(AXIS_MAX_X1);
    m_axisY1->setMax(AXIS_MAX_Y1);

    m_axisX2 = new QValueAxis();
    m_axisY2 = new QValueAxis();
   // m_axisX2->setTitleText("X-时间");
    m_axisY2->setTitleText("Y-湿度");
    m_axisX2->setMin(0);
    m_axisY2->setMin(0);
    m_axisX2->setMax(AXIS_MAX_X2);
    m_axisY2->setMax(AXIS_MAX_Y2);

    m_axisX3 = new QValueAxis();
    m_axisY3 = new QValueAxis();
   // m_axisX3->setTitleText("X-时间");
    m_axisY3->setTitleText("Y-电压");
    m_axisX3->setMin(0);
    m_axisY3->setMin(0);
    m_axisX3->setMax(AXIS_MAX_X3);
    m_axisY3->setMax(AXIS_MAX_Y3);

    m_axisX4 = new QValueAxis();
    m_axisY4 = new QValueAxis();
   // m_axisX4->setTitleText("X-时间");
    m_axisY4->setTitleText("Y-电流");
    m_axisX4->setMin(0);
    m_axisY4->setMin(0);
    m_axisX4->setMax(AXIS_MAX_X4);
    m_axisY4->setMax(AXIS_MAX_Y4);

    m_lineSeries1 = new QLineSeries();                             // 创建曲线绘制对象
    m_lineSeries1->setPointsVisible(true);                         // 设置数据点可见
    m_lineSeries1->setName("充电桩1");                            // 图例名称

    m_lineSeries2 = new QLineSeries();
    m_lineSeries2->setPointsVisible(true);
    m_lineSeries2->setName("充电桩1");

    m_lineSeries3 = new QLineSeries();
    m_lineSeries3->setPointsVisible(true);
    m_lineSeries3->setName("充电桩1");

    m_lineSeries4 = new QLineSeries();
    m_lineSeries4->setPointsVisible(true);
    m_lineSeries4->setName("充电桩1");

    m_lineSeries5 = new QLineSeries();
    m_lineSeries5->setPointsVisible(true);
    m_lineSeries5->setName("充电桩2");

    m_lineSeries6 = new QLineSeries();
    m_lineSeries6->setPointsVisible(true);
    m_lineSeries6->setName("充电桩2");

    m_lineSeries7 = new QLineSeries();
    m_lineSeries7->setPointsVisible(true);
    m_lineSeries7->setName("充电桩2");

    m_lineSeries8 = new QLineSeries();
    m_lineSeries8->setPointsVisible(true);
    m_lineSeries8->setName("充电桩2");

    m_lineSeries9 = new QLineSeries();                             // 创建曲线绘制对象
    m_lineSeries9->setPointsVisible(true);                         // 设置数据点可见
    m_lineSeries9->setName("充电桩3");                            // 图例名称

    m_lineSeries10 = new QLineSeries();
    m_lineSeries10->setPointsVisible(true);
    m_lineSeries10->setName("充电桩3");

    m_lineSeries11 = new QLineSeries();
    m_lineSeries11->setPointsVisible(true);
    m_lineSeries11->setName("充电桩3");

    m_lineSeries12 = new QLineSeries();
    m_lineSeries12->setPointsVisible(true);
    m_lineSeries12->setName("充电桩3");


    m_chart1 = new QChart();                                        // 创建图表对象
    m_chart1->addAxis(m_axisX1, Qt::AlignBottom);                      // 将X轴添加到图表上
    m_chart1->addAxis(m_axisY1, Qt::AlignLeft);                    // 将Y轴添加到图表上
    m_chart1->addSeries(m_lineSeries1);                              // 将曲线对象添加到图表上
    m_chart1->addSeries(m_lineSeries5);
    m_chart1->addSeries(m_lineSeries9);

    m_chart2 = new QChart();
    m_chart2->addAxis(m_axisX2, Qt::AlignBottom);
    m_chart2->addAxis(m_axisY2, Qt::AlignLeft);
    m_chart2->addSeries(m_lineSeries2);
    m_chart2->addSeries(m_lineSeries6);
    m_chart2->addSeries(m_lineSeries10);

    m_chart3 = new QChart();
    m_chart3->addAxis(m_axisX3, Qt::AlignBottom);
    m_chart3->addAxis(m_axisY3, Qt::AlignLeft);
    m_chart3->addSeries(m_lineSeries3);
    m_chart3->addSeries(m_lineSeries7);
    m_chart3->addSeries(m_lineSeries11);

    m_chart4 = new QChart();
    m_chart4->addAxis(m_axisX4, Qt::AlignBottom);
    m_chart4->addAxis(m_axisY4, Qt::AlignLeft);
    m_chart4->addSeries(m_lineSeries4);
    m_chart4->addSeries(m_lineSeries8);
    m_chart4->addSeries(m_lineSeries12);

    m_chart1->setAnimationOptions(QChart::SeriesAnimations);        // 动画：能使曲线绘制显示的更平滑，过渡效果更好看
    m_chart2->setAnimationOptions(QChart::SeriesAnimations);
    m_chart3->setAnimationOptions(QChart::SeriesAnimations);
    m_chart4->setAnimationOptions(QChart::SeriesAnimations);
    m_chart1->setTheme(QChart::ChartThemeBlueCerulean);
    m_chart2->setTheme(QChart::ChartThemeBlueCerulean);
    m_chart3->setTheme(QChart::ChartThemeBlueCerulean);
    m_chart4->setTheme(QChart::ChartThemeBlueCerulean);

    m_lineSeries1->attachAxis(m_axisX1);                             // 曲线对象关联上X轴，此步骤必须在m_chart->addSeries之后
    m_lineSeries1->attachAxis(m_axisY1);

    m_lineSeries2->attachAxis(m_axisX2);
    m_lineSeries2->attachAxis(m_axisY2);

    m_lineSeries3->attachAxis(m_axisX3);
    m_lineSeries3->attachAxis(m_axisY3);

    m_lineSeries4->attachAxis(m_axisX4);
    m_lineSeries4->attachAxis(m_axisY4);

    m_lineSeries5->attachAxis(m_axisX1);                             // 曲线对象关联上X轴，此步骤必须在m_chart->addSeries之后
    m_lineSeries5->attachAxis(m_axisY1);

    m_lineSeries6->attachAxis(m_axisX2);
    m_lineSeries6->attachAxis(m_axisY2);

    m_lineSeries7->attachAxis(m_axisX3);
    m_lineSeries7->attachAxis(m_axisY3);

    m_lineSeries8->attachAxis(m_axisX4);
    m_lineSeries8->attachAxis(m_axisY4);

    m_lineSeries9->attachAxis(m_axisX1);                             // 曲线对象关联上X轴，此步骤必须在m_chart->addSeries之后
    m_lineSeries9->attachAxis(m_axisY1);

    m_lineSeries10->attachAxis(m_axisX2);
    m_lineSeries10->attachAxis(m_axisY2);

    m_lineSeries11->attachAxis(m_axisX3);
    m_lineSeries11->attachAxis(m_axisY3);

    m_lineSeries12->attachAxis(m_axisX4);
    m_lineSeries12->attachAxis(m_axisY4);


    ui->chartView1->setChart(m_chart1);
    ui->chartView1->setRenderHint(QPainter::Antialiasing);
 //   ui->chartView1->chart()->setTheme(QChart::ChartThemeBlueCerulean);

    ui->chartView2->setChart(m_chart2);
    ui->chartView2->setRenderHint(QPainter::Antialiasing);
//    ui->chartView2->chart()->setTheme(QChart::ChartThemeBlueCerulean);

    ui->chartView3->setChart(m_chart3);
    ui->chartView3->setRenderHint(QPainter::Antialiasing);
 //   ui->chartView3->chart()->setTheme(QChart::ChartThemeBlueCerulean);

    ui->chartView4->setChart(m_chart4);
    ui->chartView4->setRenderHint(QPainter::Antialiasing);
 //   ui->chartView4->chart()->setTheme(QChart::ChartThemeBlueCerulean);


}

void Widget::generaChart()
{
    if(pointCount > AXIS_MAX_X1)
    {
        m_lineSeries1->remove(0);
        m_lineSeries2->remove(0);
        m_lineSeries3->remove(0);
        m_lineSeries4->remove(0);
        m_lineSeries5->remove(0);
        m_lineSeries6->remove(0);
        m_lineSeries7->remove(0);
        m_lineSeries8->remove(0);
        m_lineSeries9->remove(0);
        m_lineSeries10->remove(0);
        m_lineSeries11->remove(0);
        m_lineSeries12->remove(0);
        m_chart1->axes(Qt::Horizontal).back()->setMin(pointCount - AXIS_MAX_X1);
        m_chart1->axes(Qt::Horizontal).back()->setMax(pointCount);                    // 更新X轴范围
        m_chart2->axes(Qt::Horizontal).back()->setMin(pointCount - AXIS_MAX_X2);
        m_chart2->axes(Qt::Horizontal).back()->setMax(pointCount);                    // 更新X轴范围
        m_chart3->axes(Qt::Horizontal).back()->setMin(pointCount - AXIS_MAX_X3);
        m_chart3->axes(Qt::Horizontal).back()->setMax(pointCount);                    // 更新X轴范围
        m_chart4->axes(Qt::Horizontal).back()->setMin(pointCount - AXIS_MAX_X4);
        m_chart4->axes(Qt::Horizontal).back()->setMax(pointCount);                    // 更新X轴范围
    }
    m_lineSeries1->append(QPointF(pointCount, ui->temp1->text().mid(0,2).toInt()));
    m_lineSeries2->append(QPointF(pointCount, ui->hum1->text().mid(0,2).toInt()));
    m_lineSeries3->append(QPointF(pointCount, ui->vol1->text().toDouble()));//220
    m_lineSeries4->append(QPointF(pointCount, ui->current1->text().toDouble()));//0.13
    m_lineSeries5->append(QPointF(pointCount, ui->temp2->text().mid(0,2).toInt()));
    m_lineSeries6->append(QPointF(pointCount, ui->hum2->text().mid(0,2).toInt()));
    m_lineSeries7->append(QPointF(pointCount, ui->vol2->text().toDouble()));//220
    m_lineSeries8->append(QPointF(pointCount, ui->current2->text().toDouble()));//0.13
    m_lineSeries9->append(QPointF(pointCount, ui->temp3->text().mid(0,2).toInt()));
    m_lineSeries10->append(QPointF(pointCount, ui->hum3->text().mid(0,2).toInt()));
    m_lineSeries11->append(QPointF(pointCount, ui->vol3->text().toDouble()));//220
    m_lineSeries12->append(QPointF(pointCount, ui->current3->text().toDouble()));//0.13

    pointCount++;
}

void Widget::on_clear_chart_clicked()
{
    m_lineSeries1->clear();
    m_lineSeries2->clear();
    m_lineSeries3->clear();
    m_lineSeries4->clear();

    m_lineSeries5->clear();
    m_lineSeries6->clear();
    m_lineSeries7->clear();
    m_lineSeries8->clear();

    m_lineSeries9->clear();
    m_lineSeries10->clear();
    m_lineSeries11->clear();
    m_lineSeries12->clear();

    m_chart1->axes(Qt::Horizontal).back()->setMin(0);
    m_chart1->axes(Qt::Horizontal).back()->setMax(AXIS_MAX_X1);
    m_chart2->axes(Qt::Horizontal).back()->setMin(0);
    m_chart2->axes(Qt::Horizontal).back()->setMax(AXIS_MAX_X2);
    m_chart3->axes(Qt::Horizontal).back()->setMin(0);
    m_chart3->axes(Qt::Horizontal).back()->setMax(AXIS_MAX_X3);
    m_chart4->axes(Qt::Horizontal).back()->setMin(0);
    m_chart4->axes(Qt::Horizontal).back()->setMax(AXIS_MAX_X3);

    pointCount = 0;
    //qDebug() << 55 ;
}

void Widget::InitiatData()
{
    QBarSet *set0 = new QBarSet("充电桩1");
    QBarSet *set1 = new QBarSet("充电桩2");
    QBarSet *set2 = new QBarSet("充电桩3");

    *set0 << 11 << 33 << 27 << 60 << 59;
    *set1 << 19 << 26 << 40 << 79 << 66;
    *set2 << 5 << 40 << 18 << 22 << 33;
    QBarSeries *series = new QBarSeries();
    series->append(set0);
    series->append(set1);
    series->append(set2);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("充电桩使用时长");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    categories << "四月" << "五月" << "六月" << "七月" << "八月";
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis();
    axisY->setRange(0,100);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    ui->mchartview->setRenderHint(QPainter::Antialiasing);
    ui->mchartview->setChart(chart);
}

void Widget::on_timer0_clicked()  // start timer or stop timer
{
    if(timer->isActive())
    {
        timer->stop();
        ui->timer0->setText("启动定时器");
    }else
    {
        pointCount = 0;
        timer->start(1000);
        ui->timer0->setText("停止定时器");
    }
}



// =========== Database代码 ===========

void Widget::InitDataBase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("pzy2.db");
    if(db.open())
    {
        qDebug()<<"sqlite连接成功了";
    }
    else{
        qDebug()<<"太酷了sqlite连接失败啊";
    }
    query = new QSqlQuery();
    QString sql = " create table charge (time varchar(20),deviceid varchar(20),temperature varchar(20) "
                  ",humidty varchar(20),voltage varchar(20),current varchar(20),swich varchar(20)) ";
    if(!query->exec(sql))
    {
        qDebug()<<"no"<<endl;
    }


    table = new QSqlTableModel(this);

    table->setTable("charge");
    table->select();
    table->setHeaderData(0,Qt::Horizontal,tr("时间"));
    table->setHeaderData(1,Qt::Horizontal,tr("充电桩"));
    table->setHeaderData(2,Qt::Horizontal,tr("温度"));
    table->setHeaderData(3,Qt::Horizontal,tr("湿度"));
    table->setHeaderData(4,Qt::Horizontal,tr("电压"));
    table->setHeaderData(5,Qt::Horizontal,tr("电流"));
    table->setHeaderData(6,Qt::Horizontal,tr("状态"));

    table->setEditStrategy(QSqlTableModel::OnManualSubmit);
    ui->tableView->setModel(table);
    ui->tableView->setColumnWidth(0,130);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    CenterAlignedDelegate *delegate = new CenterAlignedDelegate;
    ui->tableView->setItemDelegate(delegate);
    ui->comboBox->setItemDelegate(delegate);
    connect(ui->comboBox,QOverload<int>::of(&QComboBox::currentIndexChanged),this,&Widget::currentIndexChangedSlot1);
}

void Widget::InsertValue()
{
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("MM-dd hh:mm:ss");
    QString sql =QString(" insert into charge values('%1','%2','%3','%4','%5','%6','%7')").arg(str).\
           arg("充电桩1").arg(ui->temp1->text().mid(0,2)).arg(ui->hum1->text().mid(0,2)).arg(ui->vol1->text()).\
           arg(ui->current1->text()).arg(ui->swich1->text());
    if(!query->exec(sql))
    {
        qDebug()<<"无法执行sql语句1";
    }

}

void Widget::InsertValue2()
{
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("MM-dd hh:mm:ss");
    QString sql =QString(" insert into charge values('%1','%2','%3','%4','%5','%6','%7')").arg(str).\
          arg("充电桩2").arg(ui->temp2->text().mid(0,2)).arg(ui->hum2->text().mid(0,2)).arg(ui->vol2->text()).\
           arg(ui->current2->text()).arg(ui->swich2->text());
    if(!query->exec(sql))
    {
        qDebug()<<"无法执行sql语句2";
    }
}

void Widget::on_displayTable_clicked()
{
    table->setTable("charge");
    table->select();
    table->setHeaderData(0,Qt::Horizontal,tr("时间"));
    table->setHeaderData(1,Qt::Horizontal,tr("充电桩"));
    table->setHeaderData(2,Qt::Horizontal,tr("温度"));
    table->setHeaderData(3,Qt::Horizontal,tr("湿度"));
    table->setHeaderData(4,Qt::Horizontal,tr("电压"));
    table->setHeaderData(5,Qt::Horizontal,tr("电流"));
    table->setHeaderData(6,Qt::Horizontal,tr("状态"));

}

void Widget::on_clearTable_clicked()
{
    int ok = QMessageBox::warning(this,tr("清除全表!"),
                                  tr("你确定清除全表吗？"), QMessageBox::Yes, QMessageBox::No);
    if(ok == QMessageBox::No)
    { // 如果不删除，则撤销
        table->revertAll();
    } else { // 否则提交，删除整个数据库
        query->exec("delete from charge");
        table->select();
    }
}

void Widget::on_refreshTable_clicked()
{
    table->database().transaction();
    if (table->submitAll()) {
        if(table->database().commit()) // 提交
            QMessageBox::information(this, tr("tableModel"),
                                     tr("数据刷新成功！"));
    } else {
        table->database().rollback(); // 回滚
        QMessageBox::warning(this, tr("tableModel"),
                             tr("数据库错误: %1").arg(table->lastError().text()),
                             QMessageBox::Ok);
    }
}

void Widget::on_findData_clicked()
{
    QString fieldName;
    switch (m_index) {
      case 0:
       fieldName ="temperature";
        break;
      case 1:
       fieldName ="humidty";
        break;
      case 2:
       fieldName ="voltage";
        break;
      case 3:
       fieldName ="current";
        break;
    }
    switch (m_index2) {
      case 0:
     table->setFilter(QString("%1 = '%2' and deviceid ='%3'").arg(fieldName).arg(ui->variable->text()).arg("充电桩1"));
        break;
      case 1:
     table->setFilter(QString("%1 = '%2' and deviceid ='%3'").arg(fieldName).arg(ui->variable->text()).arg("充电桩2"));
        break;
    }

   // table->setFilter(QString("%1 = '%2'").arg(fieldName).arg(ui->variable->text()));
    table->select();
}

void Widget::on_comboBoxID_currentIndexChanged(int index)
{
    m_index2 = index;
    qDebug()<<m_index2<<endl;
}

void Widget::currentIndexChangedSlot1(int index)
{
    m_index = index;
    qDebug()<<m_index<<endl;
}


// =========== Video代码 ===========

void Widget::on_startStream_clicked()
{
   startvideo();
   ui->startStream->setEnabled(false);
   ui->stopStream->setEnabled(true);
}

void Widget::on_stopStream_clicked()
{
   stopvideo();
   ui->startStream->setEnabled(true);
   ui->stopStream->setEnabled(false);
}

void Widget::startvideo()
{
    if(startflag) {
        return ;
    }

    //启动视频流传输前，需要检查IP地址是否有效
//    QString ipAddress = "61.183.42.64";
      QString ipAddress = "192.168.0.104";
    //视频流的http请求命令格式，例如："http://192.168.1.8:81/stream"
    QNetworkRequest request;
  //  QString url="http://" + ipAddress + ":46221//mjpeg/1";
    QString url="http://" + ipAddress + ":80//mjpeg/1";
    request.setUrl(QUrl(url));
    request.setRawHeader("Connection", "Keep-Alive");
    request.setRawHeader("User-Agent", "1601");

    Manager = new QNetworkAccessManager();
    Client = Manager->get(request);

    connect(Client, &QNetworkReply::readyRead, this, &Widget::dataReceivedVideo);

    startflag = true;

    qDebug()<<url;
}

void Widget::stopvideo()
{
    qDebug() << "===== STOP =====";
    if(!startflag) { //只有开启视频流才可以停止，或者防止多次触发停止视频流
        return;
    }

    Client->close();
    Client->deleteLater();
    startflag = false;
}

void Widget::dataReceivedVideo()
{
    QByteArray buffer = Client->readAll();
    QString data(buffer);

    //对接收的到的数据放到队列中
    int pos = data.indexOf("Content-Type");
    if(pos != -1)
    {
        frameData.append(buffer.left(pos));
        dataQueue.enqueue(frameData);
        frameData.clear();
        frameData.append(buffer.mid(pos));

    } else {
        frameData.append(buffer);
    }

    //触发void dataProcess()函数，合成图片和显示
    if (!dataQueue.isEmpty()) {
        frameBuffer = dataQueue.dequeue();
        dataProcessVideo();
    }
}

void Widget::dataProcessVideo()
{
    QString data = QString::fromUtf8(frameBuffer.data(), 50); //截取前面50个字符

//    qDebug() <<frameBuffer.left(100)<<"......";
//    qDebug() <<frameBuffer.right(100);
//    qDebug() <<"------------------------";
//      qDebug()<<frameBuffer;
//      qDebug() <<"------------------------";

    const QString lengthKeyword = "Content-Length: ";

    int lengthIndex = data.indexOf(lengthKeyword);
    if (lengthIndex >= 0){
        int endIndex = data.indexOf("\r\n", lengthIndex);
        int length = data.midRef(lengthIndex + 16, endIndex - (lengthIndex + 16 - 1)).toInt(); //取出Content-Length后的数字
        QPixmap pixmap;
        auto loadStatus = pixmap.loadFromData(frameBuffer.mid(endIndex + 4, length));

        //合成失败
        if (!loadStatus) {
            qDebug() << "Video load failed";
            frameBuffer.clear();
            return;
        }
        frameBuffer.clear();
        QPixmap pps = pixmap.scaled(ui->label_2->width(), ui->label_2->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ui->label_2->setPixmap(pps);
     }

}


// =========== Login代码 ===========

void Widget::InitLogin()
{
    connect(my_login,&Dialog1::assure1,this,&Widget::receiveAssure1);
    ui->managermode->setText("游客");
    ui->swich1Button->setEnabled(false);
    ui->swich2Button->setEnabled(false);
    ui->swich3Button->setEnabled(false);
   // ui->swich4Button->setEnabled(false);
    // ui->swich2->setEnabled(false);
}

void Widget::receiveAssure1(bool flagsuccess)
{
    if(flagsuccess)
    {
       qDebug()<<"登陆成功"<<endl;
       my_login->close();
       flagLogin = 1;
       ui->swich1Button->setEnabled(flagLogin);
       ui->swich2Button->setEnabled(flagLogin);
       ui->swich3Button->setEnabled(flagLogin);
      // ui->swich4Button->setEnabled(flagLogin);
       ui->managermode->setText("管理员");
    }
    else
    {
       qDebug()<<"登陆失败"<<endl;

    }
}

void Widget::on_managermode_clicked()
{
    if(!flagLogin)
    {
        my_login->show();
    }
    else
    {
        flagLogin = 0;
        ui->swich1Button->setEnabled(false);
        ui->swich2Button->setEnabled(false);
        ui->swich3Button->setEnabled(false);
        ui->managermode->setText("游客");
    }
}


// =========== Alarm代码 ===========

void Widget::alarm()
{
    if(!flagtemp)
      if((ui->temp1->text().mid(0,2).toInt())>=30)
      {
          QMessageBox::information(this,"报警","充电桩1的温度过高");
          flagtemp = 1;
          qDebug()<<ui->temp1->text()<<endl;
      }

    if(!flagtemp2)
      if((ui->temp2->text().mid(0,2).toInt())>=30)
      {
          QMessageBox::information(this,"报警","充电桩2的温度过高");
          flagtemp2 = 1;
          qDebug()<<ui->temp2->text()<<endl;
      }

}

// =========== StackWidget代码 ===========

void Widget::on_homebutton_clicked()
{
   ui->stackedWidget->setCurrentIndex(0);
}

void Widget::on_database_clicked()
{
   ui->stackedWidget->setCurrentIndex(3);
}

void Widget::on_monitor_clicked()
{
   ui->stackedWidget->setCurrentIndex(1);
}

void Widget::on_dataVisualize_clicked()
{
   ui->stackedWidget->setCurrentIndex(2);
}

void Widget::on_videomonitor_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
}

void Widget::on_data_statistics_clicked()
{
   ui->stackedWidget->setCurrentIndex(5);

}


// =========== Time代码 ===========

void Widget::InitTime()
{

       QDateTime time = QDateTime::currentDateTime();
       ui->time_hour->setText(time.toString("hh"));
       ui->time_min->setText(time.toString("mm"));
       ui->date->setText(time.toString("M月dd日"));
       ui->week->setText(time.toString("dddd"));

}






