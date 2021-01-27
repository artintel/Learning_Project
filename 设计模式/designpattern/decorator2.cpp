/*
 * @Author: Mayc
 * @Date: 2021-01-25 16:08:45
 * @LastEditTime: 2021-01-27 18:00:31
 * @FilePath: \设计模式\designpattern\decorator2.cpp
 * @这是一个漂亮的注释头
 */
// 普通员工有销售奖金，累计奖金，部门经理除此之外还有团队奖金；后面可能会添加环比增长奖金，同时可能产生不同的奖金组合；
// 销售奖金 = 当月销售额 * 4%
// 累计奖金 = 总的回款额 * 0.2%
// 部门奖金 = 团队销售额 * 1%
// 环比奖金 = (当月销售额-上月销售额) * 1%
// 销售后面的参数可能会调整
class Context {
public:
    bool isMgr;
    // User user;
    // double groupsale;
};

// 试着从职责出发，将职责抽象出来
class CalcBonus {    
public:
    // CalcBonus(CalcBonus * c = nullptr) {}
    virtual double Calc(Context &ctx) {
        return 0.0; // 基本工资
    }
    virtual ~CalcBonus() {}
protected:
    CalcBonus* cc;
};

class CalcMonthBonus : public CalcBonus {
public:
    CalcMonthBonus(CalcBonus * c) : cc(c) {}
    ~CalcMonthBonus(){}
    virtual double Calc(Context &ctx) {
        double mbonus /*= 计算流程忽略*/; 
        return mbonus + cc->Calc(ctx);
    }
protected:
    CalcBonus* cc;
};

class CalcSumBonus : public CalcBonus {
public:
    CalcSumBonus(CalcBonus * c) : cc(c) {}
    ~CalcSumBonus(){}
    virtual double Calc(Context &ctx) {
        double sbonus /*= 计算流程忽略*/; 
        return sbonus + cc->Calc(ctx);
    }
protected:
    CalcBonus* cc;
};

class CalcGroupBonus : public CalcBonus {
public:
    CalcGroupBonus(CalcBonus * c) : cc(c) {}
    virtual double Calc(Context &ctx) {
        double gbnonus /*= 计算流程忽略*/; 
        return gbnonus + cc->Calc(ctx);
    }
protected:
    CalcBonus* cc;
};

class CalcCycleBonus : public CalcBonus {
public:
    CalcCycleBonus(CalcBonus * c) : cc(c) {}
    virtual double Calc(Context &ctx) {
        double gbnonus /*= 计算流程忽略*/; 
        return gbnonus + cc->Calc(ctx);
    }
protected:
    CalcBonus* cc;
};

int main() {
    // 1. 普通员工
    Context ctx1;
    CalcBonus *base = new CalcBonus();
    CalcBonus *cb1 = new CalcMonthBonus(base);
    CalcBonus *cb2 = new CalcSumBonus(cb1);
    cb2->Calc(ctx1);
    // 2. 部门经理
    Context ctx2;
    CalcBonus *cb3 = new CalcGroupBonus(cb2);
    cb3->Calc(ctx2);
}

