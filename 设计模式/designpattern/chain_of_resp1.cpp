#include <string>

class Context {
public:
    std::string name;
    int day;
};

class LeaveRequest {
public:
    // 随着判断的增加，LeaveRequest类变得不稳定
    bool HandleRequest(const Context &ctx) {
        if (ctx.day <= 3)
            HandleByMainProgram(ctx);
        else if (ctx.day <= 10)
            HandleByProjMgr(ctx);
        else
            HandleByBoss(ctx);
    }

private:
    bool HandleByMainProgram(const Context &ctx) {
        
    }
    bool HandleByProjMgr(const Context &ctx) {
        
    }
    bool HandleByBoss(const Context &ctx) {

    }
};