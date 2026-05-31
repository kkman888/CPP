#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <iomanip>

class Process {
public:
    int id;               //行程ID  
    int arrivalTime;      //到達ready queue時間點
    int burstTime;        //總共執行時間
    int completionTime;   //完成時間點
    int turnaroundTime;   //周轉時間
    int waitingTime;      //等待時間

    Process(int pid, int arrival, int burst) {
        id = pid;
        arrivalTime = arrival;
        burstTime = burst;
        completionTime = 0;
        turnaroundTime = 0;
        waitingTime = 0;
    }
//turnaround(完成-到達readyQ) = waiting + burst
//
    void calculateMetrics() {
        turnaroundTime = completionTime - arrivalTime;
        waitingTime = turnaroundTime - burstTime;
    }
};

// 自定義比較邏輯：給 std::priority_queue 使用
// 我們希望 burstTime 越小的行程，優先權越高（排在最前面）

struct CompareBurstTime {
    bool operator()(const Process& a, const Process& b) {
        if (a.burstTime == b.burstTime) {
            return a.arrivalTime > b.arrivalTime; // 如果執行時間一樣，誰先到誰優先
        }
        return a.burstTime > b.burstTime; // 升序排序：burstTime 小的在頂端。原始sort是大頂堆，利用<把較小的往上推
        //利用bool特性
    }
};

void simulateSJF(std::vector<Process>& processes) {
    if (processes.empty()) return;

    // 先把所有行程按照「到達時間」排序，模擬時間推進時行程依序進入系統
    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    // C++ 高階容器：優先權佇列 (這就是我們的 Ready Queue)
    // 它會自動幫我們把進來的行程按照 CompareBurstTime 排序
    std::priority_queue<Process, std::vector<Process>, CompareBurstTime> readyQueue;

    std::vector<Process> resultList; // 儲存完工的行程
    int systemTime = 0;
    size_t procIndex = 0;
    int completedCount = 0;
    int n = processes.size();

    std::cout << "====================================\n";
    std::cout << "      SJF 排程模擬開始 (動態時鐘)      \n";
    std::cout << "====================================\n";

    // 模擬真實作業系統的核心迴圈
    while (completedCount < n) {
        
        // 1. 檢查目前時間點，有哪些行程「已經到達」了，把它們通通丟進 Ready Queue
        while (procIndex < processes.size() && processes[procIndex].arrivalTime <= systemTime) {
            readyQueue.push(processes[procIndex]);
            std::cout << "[時間 " << systemTime << "] 行程 P" << processes[procIndex].id << " 到達，進入就緒佇列。\n";
            procIndex++;
        }

        // 2. 如果 Ready Queue 有人，大腦（排程器）挑選最精短的任務出來執行
        if (!readyQueue.empty()) {
            Process currentProc = readyQueue.top(); // 撈出目前 burstTime 最短的
            readyQueue.pop(); // 從佇列中移除

            std::cout << "[時間 " << systemTime << "] CPU 選中 P" << currentProc.id << " 開始執行...\n";
            
            // 模擬執行：時間往前推進
            systemTime += currentProc.burstTime;
            
            // 紀錄完成時間並計算數據
            currentProc.completionTime = systemTime;
            currentProc.calculateMetrics();
            
            std::cout << "[時間 " << systemTime << "] 行程 P" << currentProc.id << " 執行結束。\n";
            
            resultList.push_back(currentProc);
            completedCount++;
        } 
        // 3. 如果時間到了但 Ready Queue 是空的，代表此時沒有任何行程到達，CPU 閒置
        else {
            std::cout << "[時間 " << systemTime << "] CPU 處於閒置狀態 (Idle)...\n";
            systemTime++; // 時間前進一秒，繼續等待
        }
    }

    // 4. 印出統計結果
    double totalWaitingTime = 0, totalTurnaroundTime = 0;
    std::cout << "\n=========================================================\n";
    std::cout << "PID\tArrival\tBurst\tComplete\tTurnaround\tWaiting\n";
    std::cout << "---------------------------------------------------------\n";
    for (const auto& p : resultList) {
        std::cout << p.id << "\t" << p.arrivalTime << "\t" << p.burstTime << "\t" 
                  << p.completionTime << "\t\t" << p.turnaroundTime << "\t\t" << p.waitingTime << "\n";
        totalWaitingTime += p.waitingTime;
        totalTurnaroundTime += p.turnaroundTime;
    }
    std::cout << "=========================================================\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "平均等待時間 (Average Waiting Time): " << totalWaitingTime / n << "\n";
}
int main() {
    std::vector<Process> plist;

    plist.push_back(Process(1, 0 ,8));
    plist.push_back(Process(2, 3 ,2));
    plist.push_back(Process(3, 1 ,5));
    
    simulateSJF(plist);

    return 0;
}
