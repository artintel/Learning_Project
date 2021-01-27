#include <iostream>
using namespace std;

class ZooShow {
public:
    void Show0(){
        cout << "show0" << endl;
    }
    void Show2(){
        cout << "show2" << endl;
    }
};

class ZooShowEx {
public:
    void Show1(){
        cout << "show1" << endl;
    }
    void Show3(){
        cout << "show3" << endl;
    }
};

int main () {
    ZooShow *zs = new ZooShow;
    ZooShowEx *zs1 = new ZooShowEx;
    // 流程是固定的（稳定点），应该抽象出来；另外子流程是不应该暴露给客户，违反了接口隔离原则；
    zs->Show0();
    zs1->Show1();
    zs->Show2();
    zs1->Show3();
    return 0;
}

