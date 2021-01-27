/*
 * @Author: Mayc
 * @Date: 2021-01-25 16:08:45
 * @LastEditTime: 2021-01-27 17:17:00
 * @FilePath: \设计模式\designpattern\chain_of_resp2.cpp
 * @这是一个漂亮的注释头
 */
#include <string>

class Context {
public:
    std::string name;
    int day;
};


class IHandler {
public:
    virtual ~IHandler() {}
    void SetNextHandler(IHandler *next) {
        next = next;
    }
    bool Handle(const Context& ctx) {
        if (CanHandle(ctx)) {
            return HandleRequest(ctx);
        } else if (GetNextHandler()) {
            return GetNextHandler()->HandleRequest(ctx);
        } else {
            // err
        }
    }
protected:
    virtual bool HandleRequest(const Context &ctx) = 0;
    virtual bool CanHandle(const Context &ctx) = 0;
    IHandler * GetNextHandler() {
        return next;
    }
private:
    IHandler *next;
};

class HandleByMainProgram : public IHandler {
protected:
    virtual bool HandleRequest(const Context &ctx){
        //
    }
    virtual bool CanHandle(const Context &ctx) {
        //
    }
    virtual ~HandleByMainProgram() {}
};

class HandleByProjMgr : public IHandler {
protected:
    virtual bool HandleRequest(const Context &ctx){
        //
    }
    virtual bool CanHandle(const Context &ctx) {
        //
    }
};
class HandleByBoss : public IHandler {
protected:
    virtual bool HandleRequest(const Context &ctx){
        //
    }
    virtual bool CanHandle(const Context &ctx) {
        //
    }
};

int main () {
    IHandler * h1 = new HandleByMainProgram();
    IHandler * h2 = new HandleByProjMgr();
    IHandler * h3 = new HandleByBoss();
    h1->SetNextHandler(h2);
    h2->SetNextHandler(h3);

    Context ctx;
    h1->Handle(ctx);
    return 0;
}