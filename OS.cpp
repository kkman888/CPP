#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip> // 用來讓輸出對齊好看

class Process {
public:
    int id;
    int arrivalTime;
    int burstTime;
    int completionTime;
    int turnaroundTime;
    int waitingTime;

    // 建構子：初始化行程
    Process(int id, int arrival, int burst) 
        : id(id), arrivalTime(arrival), burstTime(burst), 
          completionTime(0), turnaroundTime(0), waitingTime(0) {}

    // 計算周轉時間與等待時間
    void calculateMetrics() {
        turnaroundTime = completionTime - arrivalTime;
        waitingTime = turnaroundTime - burstTime;
    }
};