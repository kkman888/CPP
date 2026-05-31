#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>

class Process {
public:
    int id;                 
    int arrivalTime;        
    int burstTime;          
    int remainingTime;      // 為了 Round Robin 新增：剩餘需要執行的時間
    int completionTime;     
    int turnaroundTime;     
    int waitingTime;        

    Process(int pid, int arrival, int burst) {
        id = pid;
        arrivalTime = arrival;
        burstTime = burst;
        remainingTime = burst; // 初始化時，剩餘時間等於總執行時間
        completionTime = 0;
        turnaroundTime = 0;
        waitingTime = 0;
    }

    void calculateMetrics() {
        turnaroundTime = completionTime - arrivalTime;
        waitingTime = turnaroundTime - burstTime;
    }
};
void simulateRoundRobin(std::vector<Process>& processes, int quantum) {
    if (processes.empty()) return;

    // 依據到達時間排序，確保時間流逝時行程能按順序「進場」
    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });
    // 建立標準的先進先出佇列 (FIFO Ready Queue),queue提供push及pop功能，上面的sort是整個process，queue是動態排序
    std::queue<Process> readyQueue;
    std::vector<Process> resultList;

    int systemTime = 0;
    size_t procIndex = 0;
    int completedCount = 0;
    int n = processes.size();

    std::cout << "====================================\n";
    std::cout << "  Round Robin 排程模擬開始 (時間片: " << quantum << ")\n";
    std::cout << "====================================\n";

    // 先把第 0 秒就已經在現場的行程抓進隊伍
    while (procIndex < processes.size() && processes[procIndex].arrivalTime <= systemTime) {
        readyQueue.push(processes[procIndex]);
        procIndex++;
    }

    while (completedCount < n) {
        if (!readyQueue.empty()) {
            Process currentProc = readyQueue.front(); // 拿出隊伍最前端的人
            readyQueue.pop();

            std::cout << "[時間 " << systemTime << "] CPU 選中 P" << currentProc.id 
                      << " (剩餘時間: " << currentProc.remainingTime << "s)\n";

            // 計算這一次它實際能跑多久
            // 如果剩餘時間大於時間片，它只能跑 quantum 秒；否則就把剩下的時間跑完
            int runTime = std::min(currentProc.remainingTime, quantum);
            
            // 模擬執行：時間往前推進
            systemTime += runTime;
            currentProc.remainingTime -= runTime;

            // 在目前這段執行時間內，檢查有沒有「新行程」到達，有就立刻加入隊伍 1
            while (procIndex < processes.size() && processes[procIndex].arrivalTime <= systemTime) {
                readyQueue.push(processes[procIndex]);
                std::cout << "[時間 " << processes[procIndex].arrivalTime << "] 新行程 P" << processes[procIndex].id << " 到達，進入隊伍。\n";
                procIndex++;
            }

            // 檢查剛剛跑的行程有沒有完工？
            if (currentProc.remainingTime > 0) {
                // 沒完工，強制被踢回隊伍最後面排隊，在OS中先加入新行程在將踢出的往後擺
                readyQueue.push(currentProc);
                std::cout << "[時間 " << systemTime << "] P" << currentProc.id << " 時間片用盡，退回隊伍尾端。\n";
            } else {
                // 完工了！
                currentProc.completionTime = systemTime;
                currentProc.calculateMetrics();
                resultList.push_back(currentProc);
                completedCount++;
                std::cout << "[時間 " << systemTime << "] P" << currentProc.id << " 順利完工！\n";
            }
        } else {
            // 如果隊伍是空的，代表沒有行程到達，CPU 閒置
            std::cout << "[時間 " << systemTime << "] CPU 處於閒置狀態 (Idle)...\n";
            systemTime++;
            
            // 閒置的一秒內也要檢查有沒有新行程到 2
            while (procIndex < processes.size() && processes[procIndex].arrivalTime <= systemTime) {
                readyQueue.push(processes[procIndex]);
                procIndex++;
            }
        }
    }

    // 印出統計表格
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
    
    // 測試測資
    plist.push_back(Process(1, 0, 5)); // P1: 0秒到，要跑5秒
    plist.push_back(Process(2, 1, 4)); // P2: 1秒到，要跑4秒
    plist.push_back(Process(3, 2, 2)); // P3: 2秒到，要跑2秒

    int timeQuantum = 1; // 設定時間片為 2 秒
    simulateRoundRobin(plist, timeQuantum);

    return 0;
}