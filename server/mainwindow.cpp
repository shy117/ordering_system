#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //监听套接字，指定父对象，让其自动回收空间
    tcpServer = new QTcpServer(this);
    tcpServer->listen(QHostAddress::Any,8000);
    setWindowTitle("服务端");

    connect(tcpServer, &QTcpServer::newConnection,this,&MainWindow::ClientHandler);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ClientHandler()
{
    //取出建立好连接的套接字
    tcpSocket = tcpServer->nextPendingConnection();

    //获取对方的IP和端口
    QString ip = tcpSocket->peerAddress().toString();
    quint16 port = tcpSocket->peerPort();
    QString temp = QString("[%1:%2]:成功连接").arg(ip).arg(port);

    QMessageBox::information(this,"连接",temp);
    connect(tcpSocket,&QTcpSocket::readyRead,this,&MainWindow::getData);

}

//发送信号
void MainWindow::seedSign(int flag_1, int flag_2)
{
    Sign sign;
    QByteArray sendSign;

    sign.flag_1 = flag_1;//标记
    sign.flag_2 = flag_2;//桌号

    int Len = sizeof(Sign);
    sendSign.resize(Len);

    memcpy(sendSign.data(),&sign,size_t(Len));

    tcpSocket->write(sendSign);

    sendSign.clear();
    sendSign.squeeze();
}

//获取数据
void MainWindow::getData()
{
    Data data, *get_Data;
    QByteArray buffer;
    QString str;
    //读取缓冲区数据
    buffer = tcpSocket->readAll();

    if(!buffer.isEmpty())
    {
        memset(&data,0,sizeof (data));

        get_Data = (Data*)buffer.data();        //强转为结构体，需要用结构体指针接收
        //读取数据
        data.flag = get_Data->flag;
        data.tab_num = get_Data->tab_num;

        for (int i=0;i<3;i++) {
            for (int j=0;j<6;j++) {
                data.food[i][j] = get_Data->food[i][j];
            }
        }
    }
    if(data.flag==0)//入座
    {
        if(tab[data.tab_num-1]==0)//为空
        {
            tab[data.tab_num-1]=1;
            QString str = QString::number(data.tab_num)+"号桌入座";
            switch (data.tab_num)
            {
                case 1:ui->textEdit->append(str);break;
                case 2:ui->textEdit_2->append(str);break;
                case 3:ui->textEdit_3->append(str);break;
                case 4:ui->textEdit_4->append(str);break;
                case 5:ui->textEdit_5->append(str);break;
                case 6:ui->textEdit_6->append(str);break;
                case 7:ui->textEdit_7->append(str);break;
                case 8:ui->textEdit_8->append(str);break;
                default: qDebug() << "出错";break;
            }
            seedSign(0,1);//返回入座成功信息
        }
        else{
            seedSign(0,0);//返回入座失败信息
        }
    }
    else if(data.flag==1)//提交订单
    {
        int food_now[3][6];
        int fp = 0, food_num = 0, tp = 0;
        bool flag = 0;
        QString str = QString::number(data.tab_num)+"号桌提交订单\n";
        for (int i=0;i<3;i++) {
            for (int j=0;j<6;j++) {
                food_now[i][j] = food_max[i][j];//先进行一次赋值
                food_num = data.food[i][j];
                if(food_num > 0 && food_num <= food_max[i][j] && flag==0)
                {
                    food_now[i][j] = food_max[i][j] - food_num;//记录当前菜的数量
                    fp = food_num*price[i][j];
                    str = str + dish_name[i][j] + " -- ×" + QString::number(food_num) + " -- ¥" + QString::number(fp) +"\n";
                    tp += fp;
                }
                else if(food_num > food_max[i][j])//剩余菜量不足
                {
                    flag = 1;
                    seedSign(2,0);//返回订单失败
                    QMessageBox::information(this,"菜量",dish_name[i][j]+"剩余"+QString::number(food_max[i][j])+"份,该订单被退回");
                }
            }
        }
        if(flag==0)
        {
            seedSign(2,1);//返回订单成功
            for (int i=0;i<3;i++)
                for (int j=0;j<6;j++)
                    food_max[i][j] = food_now[i][j];
            str = str + "合计：¥ " + QString::number(tp);
            switch (data.tab_num)
            {
                case 1:ui->textEdit->append(str);break;
                case 2:ui->textEdit_2->append(str);break;
                case 3:ui->textEdit_3->append(str);break;
                case 4:ui->textEdit_4->append(str);break;
                case 5:ui->textEdit_5->append(str);break;
                case 6:ui->textEdit_6->append(str);break;
                case 7:ui->textEdit_7->append(str);break;
                case 8:ui->textEdit_8->append(str);break;
                default: qDebug() << "出错";break;
            }
        }
    }
    else if(data.flag==2)//呼叫服务员
    {
        QString str = QString::number(data.tab_num)+ "号桌呼叫服务员";
        QMessageBox::information(this,"呼叫",str);
        switch (data.tab_num)
        {
            case 1:ui->textEdit->append(str);break;
            case 2:ui->textEdit_2->append(str);break;
            case 3:ui->textEdit_3->append(str);break;
            case 4:ui->textEdit_4->append(str);break;
            case 5:ui->textEdit_5->append(str);break;
            case 6:ui->textEdit_6->append(str);break;
            case 7:ui->textEdit_7->append(str);break;
            case 8:ui->textEdit_8->append(str);break;
            default: qDebug() << "出错";break;
        }
    }
    else if(data.flag==3)//支付
    {
        tab[data.tab_num-1]=0;
        QMessageBox::information(this,"支付",QString::number(data.tab_num)+ "号桌完成支付");
        switch (data.tab_num)
        {
            case 1:ui->textEdit->setText("");break;
            case 2:ui->textEdit_2->setText("");break;
            case 3:ui->textEdit_3->setText("");break;
            case 4:ui->textEdit_4->setText("");break;
            case 5:ui->textEdit_5->setText("");break;
            case 6:ui->textEdit_6->setText("");break;
            case 7:ui->textEdit_7->setText("");break;
            case 8:ui->textEdit_8->setText("");break;
            default: qDebug() << "出错";break;
        }
    }

}

//下桌
void MainWindow::on_pushButton_clicked()
{
    if(tab[0]==1)
    {
        ui->textEdit->setText("");
        tab[0]=0;
        seedSign(1,1);
        QMessageBox::information(this,"下桌","1号桌强制下桌");
    }
    else{
        QMessageBox::information(this,"下桌","1号桌未被占用");
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    if(tab[1]==1)
    {
        ui->textEdit_2->setText("");
        tab[1]=0;
        seedSign(1,2);
        QMessageBox::information(this,"下桌","2号桌强制下桌");
    }
    else{
        QMessageBox::information(this,"下桌","2号桌未被占用");
    }
}

void MainWindow::on_pushButton_3_clicked()
{
    if(tab[2]==1)
    {
        ui->textEdit_3->setText("");
        tab[2]=0;
        seedSign(1,3);
        QMessageBox::information(this,"下桌","3号桌强制下桌");
    }
    else{
        QMessageBox::information(this,"下桌","3号桌未被占用");
    }
}

void MainWindow::on_pushButton_4_clicked()
{
    if(tab[3]==1)
    {
        ui->textEdit_4->setText("");
        tab[3]=0;
        seedSign(1,4);
        QMessageBox::information(this,"下桌","4号桌强制下桌");
    }
    else{
        QMessageBox::information(this,"下桌","4号桌未被占用");
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    if(tab[4]==1)
    {
        ui->textEdit_5->setText("");
        tab[4]=0;
        seedSign(1,5);
        QMessageBox::information(this,"下桌","5号桌强制下桌");
    }
    else{
        QMessageBox::information(this,"下桌","5号桌未被占用");
    }
}

void MainWindow::on_pushButton_6_clicked()
{
    if(tab[5]==1)
    {
        ui->textEdit_6->setText("");
        tab[5]=0;
        seedSign(1,6);
        QMessageBox::information(this,"下桌","6号桌强制下桌");
    }
    else{
        QMessageBox::information(this,"下桌","6号桌未被占用");
    }
}

void MainWindow::on_pushButton_7_clicked()
{
    if(tab[6]==1)
    {
        ui->textEdit_7->setText("");
        tab[6]=0;
        seedSign(1,7);
        QMessageBox::information(this,"下桌","7号桌强制下桌");
    }
    else{
        QMessageBox::information(this,"下桌","7号桌未被占用");
    }
}

void MainWindow::on_pushButton_8_clicked()
{
    if(tab[7]==1)
    {
        ui->textEdit_8->setText("");
        tab[7]=0;
        seedSign(1,8);
        QMessageBox::information(this,"下桌","8号桌强制下桌");
    }
    else{
        QMessageBox::information(this,"下桌","8号桌未被占用");
    }
}
