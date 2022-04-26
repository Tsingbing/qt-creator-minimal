#include "mainform.h"
#include "ui_mainform.h"
#include <QMessageBox>

#include <QTextStream>
#include <QDateTime>
#include <synchapi.h>  //QSleep
#include <QDebug>

MainForm::MainForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainForm)
{
    ui->setupUi(this);

    // this->setMaximumSize(800,600);
    SendCntlab=new QLabel("发送计数:");
    RecvCntlab=new QLabel("接收计数:");
    SendCntlab->setMinimumWidth(400);
    RecvCntlab->setMinimumWidth(400);

    mtimer=new QTimer(this);
    AutoSendTimer=new QTimer(this);
    connect(mtimer,&QTimer::timeout,this,&MainForm::RealTimeUpdateMsg);
    connect(AutoSendTimer,&QTimer::timeout,this,&MainForm::on_Send_Btn_clicked);
    StartInit();
}

MainForm::~MainForm()
{
    delete ui;
}

void MainForm::StartInit()
{
    theModel=new QStandardItemModel(this);
    QStringList lstr;

    lstr<<QString::fromLocal8Bit("序号")
        <<QString::fromLocal8Bit("系统时间")
        <<QString::fromLocal8Bit("时间标识")
        <<QString::fromLocal8Bit("CAN通道")
        <<QString::fromLocal8Bit("传输方向")
        <<QString::fromLocal8Bit("ID号")
        <<QString::fromLocal8Bit("帧类型")
        <<QString::fromLocal8Bit("帧格式")
        <<QString::fromLocal8Bit("长度")
        <<QString::fromLocal8Bit("数据");
    theModel->setHorizontalHeaderLabels(lstr);
    ui->RecvText->setModel(theModel);
    ui->RecvText->setColumnWidth(0,60);
    ui->RecvText->setColumnWidth(3,70);
    ui->RecvText->setColumnWidth(4,70);
    ui->RecvText->setColumnWidth(9,460); //设置tableView的数据显示列宽


    ui->FrameId_LineE->setText("0232");
    ui->SendData_Text->setText("11 22 33 44 55 66 77 88");
    ui->SendPeriod_LineE->setText("1000");
    ui->BaudRate_ComB->setCurrentIndex(0);
    ui->CanRecv_Chek->setChecked(true);
    m_CanIndex=static_cast<DWORD>(ui->CanIndex_ComB->currentIndex());  //Can1或者Can2
    m_DevType=VCI_USBCAN2;
    m_DevIndex=0;  // 第一个设备
}

void MainForm::RealTimeUpdateMsg()
{
    VCI_CAN_OBJ Recv[2500];
    DWORD Recv_Len;
    QString RowStr;
    bool IsStartRecv=ui->CanRecv_Chek->isChecked();
    timeStamp+=0.03; //30ms更新一次
    if(IsStartRecv==false) return;
    Recv_Len=VCI_Receive(m_DevType, m_DevIndex,m_CanIndex,Recv,2500,0);
   // qDebug()<<Recv_Len;
    if(Recv_Len>0)
    {

        for(DWORD i=0;i<Recv_Len;i++){
            RecvCnt+=1;
            theModel->appendRow(AddRowRecv(&Recv[i],&RowStr));
            ui->RecvText->scrollToBottom();
            //PlotViewUpdate(Recv[i].ID, Recv[i].Data); //波形更新
            RecvCntlab->setText("接收计数:"+QString::number(RecvCnt,10));
            if(ui->RealStore_Chek->isChecked()) //如果勾选了保存实时数据
            {
                SaveData2File(file,RowStr);
            }
        }
    }
}

QList<QStandardItem *> MainForm::AddRowRecv(PVCI_CAN_OBJ Recv, QString *rowstr)
{
    QList<QStandardItem *> Rowtext;
    QString index=QString::number(RecvCnt);

    QString systime=QDateTime::currentDateTime().toString("hh:mm:ss.zzz"); //系统时间
    QString timeStamp=QString::number(QDateTime::currentMSecsSinceEpoch());  //当前毫秒级时间戳
    QString channel=ui->CanIndex_ComB->currentText(); //当前can通道（can1或者can2）
    QString direct="Recv"; //接收
    QString id=QString::number(Recv->ID,16).toUpper();  //CanID
    QString type=ui->FrameType_ComB->currentText();  //帧类型
    QString format=ui->FrameFormt_ComB->currentText(); //帧格式
    QString datalen=QString::number(Recv->DataLen,10); //数据长度
    QString data=hex2Str(Recv->Data,Recv->DataLen); //数据
    *rowstr=index+":    "+systime+"   "+timeStamp+"   "+channel+"   "+direct+"   "+id+"   "+type+"   "+format+"   "+datalen+"   "+data+"\n";
    Rowtext<<new QStandardItem(index)<<new QStandardItem(systime)<<new QStandardItem(timeStamp)<<new QStandardItem(channel)
           <<new QStandardItem(direct)<<new QStandardItem(id)<<new QStandardItem(type)<<new QStandardItem(format)
           <<new QStandardItem(datalen)<<new QStandardItem(data);
    return Rowtext;
}

QList<QStandardItem *> MainForm::AddRowSend(QString sendstr, uchar len, QString *rowstr)
{
    QList<QStandardItem *> Rowtext;
    QString index=QString::number(SendCnt);
    QString systime=QDateTime::currentDateTime().toString("hh:mm:ss.zzz"); //系统时间
    QString timeStamp=QString::number(QDateTime::currentMSecsSinceEpoch());  //当前毫秒级时间戳
    QString channel=ui->CanIndex_ComB->currentText(); //当前can通道（can1或者can2）
    QString direct="Send"; //接收
    QString id=ui->FrameId_LineE->text().toUpper();  //CanID
    QString type=ui->FrameType_ComB->currentText();  //帧类型
    QString format=ui->FrameFormt_ComB->currentText(); //帧格式
    QString datalen=QString::number(len,10); //数据长度
    QString data=sendstr; //数据
    *rowstr=index+":   "+systime+"   "+timeStamp+"   "+channel+"   "+direct+"   "+id+"   "+type+"   "+format+"   "+datalen+"   "+data+"\n";
    Rowtext<<new QStandardItem(index)<<new QStandardItem(systime)<<new QStandardItem(timeStamp)<<new QStandardItem(channel)
           <<new QStandardItem(direct)<<new QStandardItem(id)<<new QStandardItem(type)<<new QStandardItem(format)
           <<new QStandardItem(datalen)<<new QStandardItem(data);
    return Rowtext;
}

void MainForm::SaveData2File(QFile *file, QString msg)
{
    if(!file->exists())
    {
        QMessageBox::warning(this,"警告","文件不存在");
        return;
    }
    else {
        QTextStream stream(file);
        stream<<msg;
    }
}

QString MainForm::hex2Str(unsigned char *in, int len)
{
    QString s;
    for(int i=0;i<len;i++)
    {
        s+=QString("%1").arg(in[i]&0xFF,2,16,QLatin1Char('0')).toUpper();
        s.append(" ");
    }
    return s;
}

float MainForm::hex2Float(unsigned char *m)
{
    union{
        unsigned char data[4];
        float num;
   }v_type;
  // memcpy(v_type.data,m,4);
   v_type.data[0]=m[3];
   v_type.data[1]=m[2];
   v_type.data[2]=m[1];
   v_type.data[3]=m[0];
   return v_type.num;
}

QString MainForm::hex2StrFloat(unsigned char *m)
{
    union{
        unsigned char data[4];
        float num;
   }v_type;
  // memcpy(v_type.data,m,4);
   v_type.data[0]=m[3];
   v_type.data[1]=m[2];
   v_type.data[2]=m[1];
   v_type.data[3]=m[0];
   return QString("%1").arg(static_cast<double>(v_type.num),0,'f');
}

void MainForm::Str2CharSplitBlank(QString s, uchar *c, uchar *len)
{
    bool ok;
    QStringList lstr=s.split(" ");
    *len =static_cast<uchar>(lstr.length());
    if(*len!=0){
        for(int i=0;i<*len;i++)
        {
             c[i]=static_cast<unsigned char>(lstr[i].toInt(&ok,16));
        }
    }
}

void MainForm::on_OpenDev_Btn_clicked()
{
    DWORD Reserved=0;
    uchar BaudIndex=static_cast<uchar>(ui->BaudRate_ComB->currentIndex());  //获取波特率选取序号
    //打开设备
    if(VCI_OpenDevice(m_DevType,m_DevIndex,Reserved)!=1)
    {
        QMessageBox::warning(this,tr(""),"open failed");
        return;
    }
    VCI_INIT_CONFIG InitInfo[1];
    InitInfo->Timing0=Bsp[BaudIndex][0];
    InitInfo->Timing1=Bsp[BaudIndex][1];
    InitInfo->Filter=static_cast<UCHAR>(ui->filterMod->currentIndex()+1);
    InitInfo->AccCode=0x80000008;
    //InitInfo->AccCode=0x80000000;
    InitInfo->AccMask=0xFFFFFFFF;
    InitInfo->Mode=static_cast<UCHAR>(ui->WorkMode_ComB->currentIndex());  //正常模式
    //初始化通道can1
    m_CanIndex=static_cast<DWORD>(ui->CanIndex_ComB->currentIndex());  //Can1或者Can2
    if(VCI_InitCAN(m_DevType,m_DevIndex,m_CanIndex,InitInfo)!=1)	//
    {
        VCI_CloseDevice(m_DevType, m_DevIndex);
        QMessageBox::warning(this,tr(""),"Init-CAN failed!");
        return;
    }
    Sleep(100);
    if(VCI_StartCAN(m_DevType,m_DevIndex,m_CanIndex)!=1)	//
    {
        QMessageBox::warning(this,tr(""),"Start-CAN1 failed!");
        return;
    }
//    //初始化通道can2
//    if(VCI_InitCAN(m_DevType,m_DevIndex,1,InitInfo)!=1)	//can1
//    {
//        VCI_CloseDevice(m_DevType, m_DevIndex);
//        QMessageBox::warning(this,tr(""),"Init-CAN1 failed!");
//        return;
//    }
//    Sleep(100);
//    if(VCI_StartCAN(m_DevType,m_DevIndex,1)!=1)	//can2
//    {
//        QMessageBox::warning(this,tr(""),"Start-CAN2 failed!");
//        return;
//    }
//    Sleep(100);
    QMessageBox::warning(this,tr(""),"Open successfule!\n Start CAN OK!");
    mtimer->start(30); //30ms
}

void MainForm::on_CloseDev_Btn_clicked()
{
    mtimer->stop();
    AutoSendTimer->stop();
    VCI_ClearBuffer(m_DevType, m_DevIndex, m_CanIndex);//清理接收和发送缓冲器数据
    if(VCI_CloseDevice(m_DevType,m_DevIndex)!=1)
    {
        QMessageBox::warning(this,tr(""),"Close failed！");
        return;
    }
    QMessageBox::warning(this,tr(""),"Close successful!");

}

void MainForm::on_Send_Btn_clicked()
{
    VCI_CAN_OBJ sendbuf[1];
    bool ok;
    QString sendMsg=ui->SendData_Text->text();
    QString RowStr;
    if(ui->WorkMode_ComB->currentText()=="只听模式")
    {
        QMessageBox::warning(this,"注意!","当前为只听模式");
        ui->AutoSend_Chek->setChecked(false);
        AutoSendTimer->stop();
        return;
    }
    sendbuf->ExternFlag=static_cast<BYTE>(ui->FrameFormt_ComB->currentIndex()); //帧格式
    sendbuf->RemoteFlag=static_cast<BYTE>(ui->FrameType_ComB->currentIndex());  //帧类型
    if((sendbuf->ExternFlag)==1)//帧格式，远程帧
    {
        sendbuf->ID =(ui->FrameId_LineE->text().toUInt(&ok,16)); //&0x1FFFFFFF;
     // qDebug()<<"sendID:"<<sendbuf->ID<<endl;
    }
    else//标准帧格式
    {
        sendbuf->ID=(ui->FrameId_LineE->text().toUInt(&ok,16))&0x7FF;
        qDebug()<<"sendID:"<<sendbuf->ID<<endl;
    }
    sendbuf->SendType=1;
    sendbuf->TimeFlag=0;
    sendbuf->TimeStamp=0;
    Str2CharSplitBlank(sendMsg,sendbuf->Data,&sendbuf->DataLen);
    ULONG flag;
    SendCnt++;
    flag=VCI_Transmit(m_DevType,m_DevIndex,m_CanIndex,sendbuf,1); //调用动态链接库发送函数
    qDebug()<<flag;
    if(flag<1)  //发送不正常
    {
        QMessageBox::warning(this,"警告","发送失败\n");
        if(ui->AutoSend_Chek->isChecked())  //关闭自动发送
        {   AutoSendTimer->stop();
            ui->AutoSend_Chek->setChecked(false);
        }
    }
    else {
        theModel->appendRow(AddRowSend(sendMsg,sendbuf->DataLen,&RowStr));
        if(ui->RealStore_Chek->isChecked()){ //如果勾选了保存实时数据
            SaveData2File(file,RowStr);
          }
        //qDebug()<<RowStr;
        SendCntlab->setText("发送计数:"+QString::number(SendCnt,10)); // 显示发送计数
    }
}

void MainForm::on_clear_Btn_clicked()
{
    QStringList lstr;
    lstr<<QString::fromLocal8Bit("序号")
        <<QString::fromLocal8Bit("系统时间")
        <<QString::fromLocal8Bit("时间标识")
        <<QString::fromLocal8Bit("CAN通道")
        <<QString::fromLocal8Bit("传输方向")
        <<QString::fromLocal8Bit("ID号")
        <<QString::fromLocal8Bit("帧类型")
        <<QString::fromLocal8Bit("帧格式")
        <<QString::fromLocal8Bit("长度")
        <<QString::fromLocal8Bit("数据");

    theModel->clear();
    theModel->setHorizontalHeaderLabels(lstr);
    ui->RecvText->setModel(theModel);
    ui->RecvText->setColumnWidth(0,60);
    ui->RecvText->setColumnWidth(3,70);
    ui->RecvText->setColumnWidth(4,70);
    ui->RecvText->setColumnWidth(9,160); //设置tableView的数据显示列宽
    RecvCnt=0;
    SendCnt=0;
}

void MainForm::on_RealStore_Chek_clicked(bool checked)
{
    if(checked==true)
    {
        filename="./log/"+QDateTime::currentDateTime().toString("yyyyMMddhhmmss")+".txt";
        file=new QFile(filename);
        if(!file->open(QIODevice::WriteOnly|QIODevice::Text))
        {
            QMessageBox::warning(this,"警告","创建文件"+filename+"失败");
            return;
        }
    }
    else {
        file->close();
        file->deleteLater();
        filename.clear();
    }
}

void MainForm::on_AutoSend_Chek_clicked(bool checked)
{
    int sendPeriod=ui->SendPeriod_LineE->text().toInt();
    if(checked)
    {
        AutoSendTimer->start(sendPeriod);
    }
    else {
        AutoSendTimer->stop();
    }
}

void MainForm::on_checkBox_Rpm_clicked(bool checked)
{

}

void MainForm::on_checkBox_Volt_clicked(bool checked)
{

}

void MainForm::on_checkBox_Current_clicked(bool checked)
{

}
