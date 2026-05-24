#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>

class Process {
public:
    int id;                 
    int arrivalTime;        
    int burstTime;          
    int completionTime;     
    int turnaroundTime;     
    int waitingTime;        

    Process(int pid, int arrival, int burst) {
        id = pid;
        arrivalTime = arrival;
        burstTime = burst;
        completionTime = 0;
        turnaroundTime = 0;
        waitingTime = 0;
    }

    void calculateMetrics() {
        turnaroundTime = completionTime - arrivalTime;
        waitingTime = turnaroundTime - burstTime;
    }
};

void simulateFCFS(std::vector<Process>& processes) {
    // 防呆機制：如果傳進來的清單是空的，立刻結束，避免除以零崩潰
    if (processes.empty()) {
        std::cout << "錯誤：行程清單是空的，無法模擬！\n";
        return;
    }

    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    int systemTime = 0; 
    double totalWaitingTime = 0;
    double totalTurnaroundTime = 0;

    std::cout << "====================================\n";
    std::cout << "      FCFS 排程模擬開始 (時間流逝)      \n";
    std::cout << "====================================\n";

    for (size_t i = 0; i < processes.size(); ++i) {
        if (systemTime < processes[i].arrivalTime) {
            std::cout << "[時間 " << systemTime << " -> " << processes[i].arrivalTime 
                      << "] CPU 處於閒置狀態 (Idle)...\n";
            systemTime = processes[i].arrivalTime;
        }

        std::cout << "[時間 " << systemTime << "] 行程 P" << processes[i].id << " 開始執行...\n";
        systemTime += processes[i].burstTime;
        
        processes[i].completionTime = systemTime;
        processes[i].calculateMetrics();

        std::cout << "[時間 " << systemTime << "] 行程 P" << processes[i].id << " 執行結束。\n";

        totalWaitingTime += processes[i].waitingTime;
        totalTurnaroundTime += processes[i].turnaroundTime;
    }

    std::cout << "\n=========================================================\n";
    std::cout << "PID\tArrival\tBurst\tComplete\tTurnaround\tWaiting\n";
    std::cout << "---------------------------------------------------------\n";
    for (const auto& p : processes) {
        std::cout << p.id << "\t" 
                  << p.arrivalTime << "\t" 
                  << p.burstTime << "\t" 
                  << p.completionTime << "\t\t" 
                  << p.turnaroundTime << "\t\t" 
                  << p.waitingTime << "\n";
    }
    std::cout << "=========================================================\n";
    
    std::cout << std::fixed << std::setprecision(2); 
    std::cout << "平均等待時間 (Average Waiting Time): " << totalWaitingTime / processes.size() << "\n";
    std::cout << "平均周轉時間 (Average Turnaround Time): " << totalTurnaroundTime / processes.size() << "\n";
}

int main() {
    std::vector<Process> plist;
    
    // 重新確認這三行是否有正確執行
    plist.push_back(Process(1, 0, 5));
    plist.push_back(Process(2, 2, 3));
    plist.push_back(Process(3, 7, 2));

    simulateFCFS(plist);

    return 0;
}