#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QHostAddress>
#include <QTcpSocket>
#include <QTcpServer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void ClientHandler();
    void getData();
    void seedSign(int flag_1, int flag_2);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_8_clicked();

private:
    Ui::MainWindow *ui;
    QTcpServer *tcpServer; //监听套接字
    QTcpSocket *tcpSocket; //通信套接字
    int price[3][6]={{66,68,98,38,58,30},
                     {15,18,20,10,15,1},
                     {12,8,6,3,8,15}};//价格
    QString dish_name[3][6]={{"红烧肉","龙虾","烤乳猪","白斩鸡","蛤蜊炖虾","四喜丸子"},
                             {"饭团","面包","披萨","包子","面条","饭"},
                             {"燕窝","可口可乐","果粒橙","王老吉","啤酒","老村长酒"}};//菜名
    int food_max[3][6]={{10,10,10,10,10,10},
                        {100,100,100,100,100,100},
                        {100,100,100,100,100,100}};//菜品数量最大值
    int tab[8]={0,0,0,0,0,0,0,0};//0为空，1为占用，8个桌号
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
