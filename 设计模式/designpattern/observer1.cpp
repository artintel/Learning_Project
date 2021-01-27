/*
 * @Author: Mayc
 * @Date: 2021-01-25 16:08:45
 * @LastEditTime: 2021-01-27 18:14:24
 * @FilePath: \设计模式\designpattern\observer1.cpp
 * @这是一个漂亮的注释头
 */

class DisplayA {
public:
    void Show(float temperature);
};

class DisplayB {
public:
    void Show(float temperature);
};

class WeatherData {
};

class DataCenter {
public:
    float CalcTemperature() {
        WeatherData * data = GetWeatherData();
        // ...
        float temper/* = */;
        return temper;
    }
private:
    WeatherData * GetWeatherData() {}; // 不同的方式
};

int main() {
    DataCenter *center = new DataCenter;
    DisplayA *da = new DisplayA;
    DisplayB *db = new DisplayB;
    float temper = center->CalcTemperature();
    da->Show(temper);
    db->Show(temper);
    return 0;
}
// 终端变化（增加和删除）   数据中心 不应该受终端变化的影响
