#include <iostream>
using namespace std;

class ZooShow {
public:
    // 固定流程封装到这里
    void Show() {
        Show0();
        Show1();
        Show2();
        Show3();
    }
protected:
    // 子流程 使用protected保护起来 不被客户调用 但允许子类扩展
    virtual void Show0(){
        cout << "show0" << endl;
    }
    virtual void Show2(){
        cout << "show2" << endl;
    }
    virtual void Show1() {

    }
    virtual void Show3() {

    }
};
class ZooShowEx : public ZooShow {
protected:
    virtual void Show1(){
        cout << "show1" << endl;
    }
    virtual void Show3(){
        cout << "show3" << endl;
    }
    virtual void Show4() {
        //
    }
};

class ZooShowEx1 : public ZooShow {
protected:
    virtual void Show0(){
        cout << "show1" << endl;
    }
    virtual void Show2(){
        cout << "show3" << endl;
    }
};

class ZooShowEx2 : public ZooShow {
protected:
    virtual void Show1(){
        cout << "show1" << endl;
    }
    virtual void Show2(){
        cout << "show3" << endl;
    }
};
/*
依赖倒置原则
单一职责原则
接口隔离原则

反向控制：应用程序 框架 应用程序（变化的）应该依赖框架（稳定的），应该是框架去调应用程序，而不是应用程序去调框架
*/
int main () {
    ZooShow *zs = new ZooShowEx;
    ZooShow *zs1 = new ZooShowEx1;
    ZooShow *zs2 = new ZooShowEx2;
    zs->Show();
    return 0;
}

