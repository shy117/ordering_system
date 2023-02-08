#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //分配空间，指定父对象
    tcpSocket = new QTcpSocket(this);

    setWindowTitle("客户端");

    QString ip = "127.0.0.1";
    quint16 port  =8000;

    qDebug()<<ip<<"::"<<port;

    //主动和服务器建立连接
    tcpSocket->connectToHost(QHostAddress(ip),port);

    connect(tcpSocket,&QTcpSocket::connected,[this](){
        QMessageBox::information(this,"连接","成功和服务器建立连接");
        qDebug()<<"连接成功";
    });

    connect(tcpSocket, &QTcpSocket::readyRead,this,&MainWindow::getSign);


    //获取菜单信息
    get_dish();

}

MainWindow::~MainWindow()
{
    delete ui;
}

//发送数据
void MainWindow::seedDate(int flag, int tab)
{
    Data data;
    QByteArray sendTcpData;

    QString str;

    data.flag = flag;//标记
    data.tab_num = tab;//桌号
    for (int i=0;i<3;i++) {
        for (int j=0;j<6;j++) {
            data.food[i][j] = food[i][j];
        }
    }

    int Len = 20*sizeof(int);
    sendTcpData.resize(Len);

    memcpy(sendTcpData.data(),&data,size_t(Len));

    tcpSocket->write(sendTcpData);

    sendTcpData.clear();
    sendTcpData.squeeze();
}


//获取服务端发送的信号
void MainWindow::getSign()
{
    Sign sign, *get_Sign;
    QByteArray buffer;
    //读取缓冲区数据
    buffer = tcpSocket->readAll();

    if(!buffer.isEmpty())
    {
        memset(&sign,0,sizeof (sign));

        get_Sign = (Sign*)buffer.data();        //强转为结构体，需要用结构体指针接收
        //读取数据
        sign.flag_1 = get_Sign->flag_1;
        sign.flag_2 = get_Sign->flag_2;

    }
    if(sign.flag_1==0)//入座
    {
        if(sign.flag_2==0)//入座失败
        {
            QMessageBox::information(this,"入座","入座失败，该桌已被占用");
        }
        else if(sign.flag_2==1)//入座成功
        {
            tab_num = ui->table_number->text().toInt();
            tab[tab_num-1]=1;
            QMessageBox::information(this,"入座","入座成功，请开始点菜");
            ui->textEdit->append("入座成功");
        }
    }
    else if(sign.flag_1==1)//下桌
    {
        tab[sign.flag_2-1]=0;
        if(sign.flag_2==tab_num)
        {
            tab_num=0;
            submit_flag=0;
            total_price=0;
            ui->table_number->setText("");
            ui->total_amount->setText(QString::number(total_price));
            ui->textEdit->setText("");
            QMessageBox::information(this,"下桌","你已被后台强制下桌");
        }
    }
    else if(sign.flag_1==2)//提交订单
    {
        if(sign.flag_2==0)//订单失败
        {
            QMessageBox::information(this,"订单","订单失败，选择的菜品剩余不足，\n请重新选择或呼叫服务员");
        }
        else if(sign.flag_2==1)//订单成功
        {
            submit_flag=1;//提交成功标记
            Total_price();//统计价格
            ui->table_number->setText(QString::number(tab_num));
            QMessageBox::information(this,"订单","订单提交成功");
            ui->textEdit->append("订单提交成功");
            R_order();
        }
    }
}

//入座
void MainWindow::on_table_Button_clicked()
{
    int n;
    n = ui->table_number->text().toInt();
    if(n>=1 && n<=8)
    {
        seedDate(0, n);
    }
    else {
        QMessageBox::information(this,"桌号","入座失败,请在1~8桌入座");
    }
}

//提交订单
void MainWindow::on_submit_Button_clicked()
{
    if(tab_num==0)
    {
        QMessageBox::information(this,"提交订单","提交订单失败，请先选择桌号");
    }
    else {
        seedDate(1, tab_num);
    }
}

//合计
void MainWindow::Total_price()
{
    QString str="";
    int fp = 0;
    str = QString::number(tab_num) + "号桌";
    for (int i=0;i<3;i++) {
        for (int j=0;j<6;j++) {
            if(food[i][j] > 0)
            {
                fp = food[i][j]*price[i][j];
                str = str + dish_name[i][j] + "----------------- × " + QString::number(food[i][j]) + " -- ¥ " + QString::number(fp);
                total_price += fp;
            }
        }
    }
    ui->total_amount->setText(QString::number(total_price));
    ui->textEdit->append(str);
    ui->textEdit->append("合计：¥ " + QString::number(total_price));
}

//呼叫服务员
void MainWindow::on_call_Button_clicked()
{
    seedDate(2, tab_num);
    QMessageBox::information(this,"呼叫","呼叫成功");
}

//支付
void MainWindow::on_pay_Button_clicked()
{
    if(submit_flag==0)
    {
        QMessageBox::information(this,"支付","支付失败,没有待支付订单");
    }
    else {//支付成功后离开
        //QString str = "0" + QString::number(tab_num);//以0开同做标记
        seedDate(3, tab_num);
        QMessageBox::information(this,"支付","支付成功");
        tab[tab_num-1]=0;
        tab_num=0;
        submit_flag=0;
        total_price=0;
        ui->table_number->setText("");
        ui->total_amount->setText(QString::number(total_price));
        ui->textEdit->setText("");
    }
}


//获取菜品信息
void MainWindow::get_dish()
{
    //获取价格
    price[0][0] = ui->price1_1->text().toInt();
    price[0][1] = ui->price1_2->text().toInt();
    price[0][2] = ui->price1_3->text().toInt();
    price[0][3] = ui->price1_4->text().toInt();
    price[0][4] = ui->price1_5->text().toInt();
    price[0][5] = ui->price1_6->text().toInt();

    price[1][0] = ui->price2_1->text().toInt();
    price[1][1] = ui->price2_2->text().toInt();
    price[1][2] = ui->price2_3->text().toInt();
    price[1][3] = ui->price2_4->text().toInt();
    price[1][4] = ui->price2_5->text().toInt();
    price[1][5] = ui->price2_6->text().toInt();

    price[2][0] = ui->price3_1->text().toInt();
    price[2][1] = ui->price3_2->text().toInt();
    price[2][2] = ui->price3_3->text().toInt();
    price[2][3] = ui->price3_4->text().toInt();
    price[2][4] = ui->price3_5->text().toInt();
    price[2][5] = ui->price3_6->text().toInt();

    //获取菜名
    dish_name[0][0] = ui->dishName1_1->text();
    dish_name[0][1] = ui->dishName1_2->text();
    dish_name[0][2] = ui->dishName1_3->text();
    dish_name[0][3] = ui->dishName1_4->text();
    dish_name[0][4] = ui->dishName1_5->text();
    dish_name[0][5] = ui->dishName1_6->text();

    dish_name[1][0] = ui->dishName2_1->text();
    dish_name[1][1] = ui->dishName2_2->text();
    dish_name[1][2] = ui->dishName2_3->text();
    dish_name[1][3] = ui->dishName2_4->text();
    dish_name[1][4] = ui->dishName2_5->text();
    dish_name[1][5] = ui->dishName2_6->text();

    dish_name[2][0] = ui->dishName3_1->text();
    dish_name[2][1] = ui->dishName3_2->text();
    dish_name[2][2] = ui->dishName3_3->text();
    dish_name[2][3] = ui->dishName3_4->text();
    dish_name[2][4] = ui->dishName3_5->text();
    dish_name[2][5] = ui->dishName3_6->text();
}

//重置
void MainWindow::R_order()
{
    ui->count1_1->setText(QString::number(0));
    ui->count1_2->setText(QString::number(0));
    ui->count1_3->setText(QString::number(0));
    ui->count1_4->setText(QString::number(0));
    ui->count1_5->setText(QString::number(0));
    ui->count1_6->setText(QString::number(0));

    ui->count2_1->setText(QString::number(0));
    ui->count2_2->setText(QString::number(0));
    ui->count2_3->setText(QString::number(0));
    ui->count2_4->setText(QString::number(0));
    ui->count2_5->setText(QString::number(0));
    ui->count2_6->setText(QString::number(0));

    ui->count3_1->setText(QString::number(0));
    ui->count3_2->setText(QString::number(0));
    ui->count3_3->setText(QString::number(0));
    ui->count3_4->setText(QString::number(0));
    ui->count3_5->setText(QString::number(0));
    ui->count3_6->setText(QString::number(0));

    for (int i=0;i<3;i++) {
        for (int j=0;j<6;j++) {
            food[i][j] = 0;
        }
    }
    total_price = 0;
}

//数据操作函数
void MainWindow::operation(int flag)
{
    switch (flag)
    {
        case 1:ui->count1_1->setText(QString::number(food[0][0]));break;
        case 2:ui->count1_2->setText(QString::number(food[0][1]));break;
        case 3:ui->count1_3->setText(QString::number(food[0][2]));break;
        case 4:ui->count1_4->setText(QString::number(food[0][3]));break;
        case 5:ui->count1_5->setText(QString::number(food[0][4]));break;
        case 6:ui->count1_6->setText(QString::number(food[0][5]));break;

        case 7:ui->count2_1->setText(QString::number(food[1][0]));break;
        case 8:ui->count2_2->setText(QString::number(food[1][1]));break;
        case 9:ui->count2_3->setText(QString::number(food[1][2]));break;
        case 10:ui->count2_4->setText(QString::number(food[1][3]));break;
        case 11:ui->count2_5->setText(QString::number(food[1][4]));break;
        case 12:ui->count2_6->setText(QString::number(food[1][5]));break;

        case 13:ui->count3_1->setText(QString::number(food[2][0]));break;
        case 14:ui->count3_2->setText(QString::number(food[2][1]));break;
        case 15:ui->count3_3->setText(QString::number(food[2][2]));break;
        case 16:ui->count3_4->setText(QString::number(food[2][3]));break;
        case 17:ui->count3_5->setText(QString::number(food[2][4]));break;
        case 18:ui->count3_6->setText(QString::number(food[2][5]));break;
        default: qDebug() << "出错";break;
    }
}

//添加菜品数量 只有炒菜设置上限为5
void MainWindow::on_add_Button1_1_clicked()
{
    if(food[0][0] == 5)
    {
        QMessageBox::information(this,"提示","单次点单这份菜已达到上限");
    }
    else{
        food[0][0] += 1;
        operation(1);
    }
}

void MainWindow::on_add_Button1_2_clicked()
{
    if(food[0][1] == 5)
    {
        QMessageBox::information(this,"提示","单次点单这份菜已达到上限");
    }
    else{
        food[0][1] += 1;
        operation(2);
    }
}

void MainWindow::on_add_Button1_3_clicked()
{  
    if(food[0][2] == 5)
    {
        QMessageBox::information(this,"提示","单次点单这份菜已达到上限");
    }
    else{
        food[0][2] += 1;
        operation(3);
    }
}

void MainWindow::on_add_Button1_4_clicked()
{
    if(food[0][3] == 5)
    {
        QMessageBox::information(this,"提示","单次点单这份菜已达到上限");
    }
    else{
        food[0][3] += 1;
        operation(4);
    }
}

void MainWindow::on_add_Button1_5_clicked()
{
    if(food[0][4] == 5)
    {
        QMessageBox::information(this,"提示","单次点单这份菜已达到上限");
    }
    else{
        food[0][4] += 1;
        operation(5);
    }
}

void MainWindow::on_add_Button1_6_clicked()
{
    if(food[0][5] == 5)
    {
        QMessageBox::information(this,"提示","单次点单这份菜已达到上限");
    }
    else{
        food[0][5] += 1;
        operation(6);
    }
}

void MainWindow::on_add_Button2_1_clicked()
{
    food[1][0] += 1;
    operation(7);
}

void MainWindow::on_add_Button2_2_clicked()
{
    food[1][1] += 1;
    operation(8);
}

void MainWindow::on_add_Button2_3_clicked()
{
    food[1][2] += 1;
    operation(9);
}

void MainWindow::on_add_Button2_4_clicked()
{
    food[1][3] += 1;
    operation(10);
}

void MainWindow::on_add_Button2_5_clicked()
{
    food[1][4] += 1;
    operation(11);
}

void MainWindow::on_add_Button2_6_clicked()
{
    food[1][5] += 1;
    operation(12);
}

void MainWindow::on_add_Button3_1_clicked()
{
    food[2][0] += 1;
    operation(13);
}

void MainWindow::on_add_Button3_2_clicked()
{
    food[2][1] += 1;
    operation(14);
}

void MainWindow::on_add_Button3_3_clicked()
{
    food[2][2] += 1;
    operation(15);
}

void MainWindow::on_add_Button3_4_clicked()
{
    food[2][3] += 1;
    operation(16);
}

void MainWindow::on_add_Button3_5_clicked()
{
    food[2][4] += 1;
    operation(17);
}

void MainWindow::on_add_Button3_6_clicked()
{
    food[2][5] += 1;
    operation(18);
}

//减少菜品数量 下限0
void MainWindow::on_subtract_Button1_1_clicked()
{
    if(food[0][0] > 0)
    {
        food[0][0] -= 1;
        operation(1);
    }
}

void MainWindow::on_subtract_Button1_2_clicked()
{
    if(food[0][1] > 0)
    {
        food[0][1] -= 1;
        operation(2);
    }
}

void MainWindow::on_subtract_Button1_3_clicked()
{
    if(food[0][2] > 0)
    {
        food[0][2] -= 1;
        operation(3);
    }
}

void MainWindow::on_subtract_Button1_4_clicked()
{
    if(food[0][3] > 0)
    {
        food[0][3] -= 1;
        operation(4);
    }
}

void MainWindow::on_subtract_Button1_5_clicked()
{
    if(food[0][4] > 0)
    {
        food[0][4] -= 1;
        operation(5);
    }
}

void MainWindow::on_subtract_Button1_6_clicked()
{
    if(food[0][5] > 0)
    {
        food[0][5] -= 1;
        operation(6);
    }
}

void MainWindow::on_subtract_Button2_1_clicked()
{
    if(food[1][0] > 0)
    {
        food[1][0] -= 1;
        operation(7);
    }
}

void MainWindow::on_subtract_Button2_2_clicked()
{
    if(food[1][1] > 0)
    {
        food[1][1] -= 1;
        operation(8);
    }
}

void MainWindow::on_subtract_Button2_3_clicked()
{
    if(food[1][2] > 0)
    {
        food[1][2] -= 1;
        operation(9);
    }
}

void MainWindow::on_subtract_Button2_4_clicked()
{
    if(food[1][3] > 0)
    {
        food[1][3] -= 1;
        operation(10);
    }
}

void MainWindow::on_subtract_Button2_5_clicked()
{
    if(food[1][4] > 0)
    {
        food[1][4] -= 1;
        operation(11);
    }
}

void MainWindow::on_subtract_Button2_6_clicked()
{
    if(food[1][5] > 0)
    {
        food[1][5] -= 1;
        operation(12);
    }
}

void MainWindow::on_subtract_Button3_1_clicked()
{
    if(food[2][0] > 0)
    {
        food[2][0] -= 1;
        operation(13);
    }
}

void MainWindow::on_subtract_Button3_2_clicked()
{
    if(food[2][1] > 0)
    {
        food[2][1] -= 1;
        operation(14);
    }
}

void MainWindow::on_subtract_Button3_3_clicked()
{
    if(food[2][2] > 0)
    {
        food[2][2] -= 1;
        operation(15);
    }
}

void MainWindow::on_subtract_Button3_4_clicked()
{
    if(food[2][3] > 0)
    {
        food[2][3] -= 1;
        operation(16);
    }
}

void MainWindow::on_subtract_Button3_5_clicked()
{
    if(food[2][4] > 0)
    {
        food[2][4] -= 1;
        operation(17);
    }
}

void MainWindow::on_subtract_Button3_6_clicked()
{
    if(food[2][5] > 0)
    {
        food[2][5] -= 1;
        operation(18);
    }
}
