#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QMessageBox>
#include <QHostAddress>
#include <QTcpSocket>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void operation(int flag);//加减数量
    void R_order();//重置
    void Total_price();//合计
    void get_dish();
    void seedDate(int flag,int tab);
    void getSign();

private slots:
    //入座
    void on_table_Button_clicked();

    //呼叫服务员
    void on_submit_Button_clicked();

    //提交订单
    void on_call_Button_clicked();

    //支付
    void on_pay_Button_clicked();

    //添加菜品数量
    void on_add_Button1_1_clicked();
    void on_add_Button1_2_clicked();
    void on_add_Button1_3_clicked();
    void on_add_Button1_4_clicked();
    void on_add_Button1_5_clicked();
    void on_add_Button1_6_clicked();
    void on_add_Button2_1_clicked();
    void on_add_Button2_2_clicked();
    void on_add_Button2_3_clicked();
    void on_add_Button2_4_clicked();
    void on_add_Button2_5_clicked();
    void on_add_Button2_6_clicked();
    void on_add_Button3_1_clicked();
    void on_add_Button3_2_clicked();
    void on_add_Button3_3_clicked();
    void on_add_Button3_4_clicked();
    void on_add_Button3_5_clicked();
    void on_add_Button3_6_clicked();

    //减少菜品数量
    void on_subtract_Button1_1_clicked();
    void on_subtract_Button1_2_clicked();
    void on_subtract_Button1_3_clicked();
    void on_subtract_Button1_4_clicked();
    void on_subtract_Button1_5_clicked();
    void on_subtract_Button1_6_clicked();
    void on_subtract_Button2_1_clicked();
    void on_subtract_Button2_2_clicked();
    void on_subtract_Button2_3_clicked();
    void on_subtract_Button2_4_clicked();
    void on_subtract_Button2_5_clicked();
    void on_subtract_Button2_6_clicked();
    void on_subtract_Button3_1_clicked();
    void on_subtract_Button3_2_clicked();
    void on_subtract_Button3_3_clicked();
    void on_subtract_Button3_4_clicked();
    void on_subtract_Button3_5_clicked();
    void on_subtract_Button3_6_clicked();



private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket; //通信套接字

    int food[3][6]={{0,0,0,0,0,0},
                    {0,0,0,0,0,0},
                    {0,0,0,0,0,0}};//点餐的菜品数量
    int price[3][6]={{0,0,0,0,0,0},
                    {0,0,0,0,0,0},
                    {0,0,0,0,0,0}};//价格
    QString dish_name[3][6];//菜名
    int tab[8]={0,0,0,0,0,0,0,0};//0为空，1为占用，8个桌号
    int tab_num=0;//0为没入座，1~8为入座桌号
    int submit_flag=0;//0没订单，1有订单
    int total_price=0;


};

struct Data
{
    int flag ;//标记
    int tab_num;
    int food[3][6];//点餐的菜品数量
};
Q_DECLARE_METATYPE(Data)//注册结构体

struct Sign
{
    int flag_1 ;//标记
    int flag_2;
};
Q_DECLARE_METATYPE(Sign)//注册结构体

#endif // MAINWINDOW_H
