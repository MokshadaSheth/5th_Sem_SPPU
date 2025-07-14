#include <bits/stdc++.h>
using namespace std;

class Process
{
private:
    int id;
    int AT;  // Arrival Time
    int BT;  // Burst Time
    int CT;  // Completion Time
    int TAT; // Turn Around Time
    int WT;  // Wait Time
public:
    friend class Scheduling;
    Process()
    {
        id = -1;
        AT = -1;
        BT = -1;
        CT = -1;
        TAT = -1;
        WT = -1;
    }
    Process(int i, int A, int B)
    {
        id = i;
        AT = A;
        BT = B;
        CT = 0;
        TAT = 0;
        WT = 0;
    }

    void calTAT() // Turn Around Time
    {
        TAT = CT - AT;
    }
    void calWT()
    {
        WT = TAT - BT;
    }
};

class Scheduling
{
private:
    Process *allProcess; // Will assign size after knowing total process
    int totalP;
    int currTime;

public:
    Scheduling()
    {
        allProcess = nullptr;
        totalP = 0;
    }
    ~Scheduling()
    {
        if (allProcess != nullptr)
        {
            delete[] allProcess;
            allProcess = nullptr;
        }
    }

    void takeIp(const string filename)
    {
        ifstream file(filename);
        if (!file)
        {
            cerr << "\nFailed to open";
            exit(1);
        }

        file >> totalP;
        allProcess = new Process[totalP];
        for (int m = 0; m < totalP; m++)
        {
            int id, AT, BT;

            file >> id >> AT >> BT;
            allProcess[m] = Process(id, AT, BT);
        }
        // Sort allProcess
        for (int i = 0; i < totalP; i++)
        {
            for (int j = 0; j < totalP - 1; j++)
            {
                if (allProcess[j].AT > allProcess[j + 1].AT)
                {
                    swap(allProcess[j], allProcess[j + 1]);
                }
            }
        }
        file.close();
    }

    void displayProcess()
    {
        cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        cout << "|" << left << setw(10) << "ProcessId";
        cout << "|" << left << setw(10) << "AT";
        cout << "|" << left << setw(10) << "BT";
        cout << "|" << endl;
        for (int i = 0; i < totalP; i++)
        {
            cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
            cout << "|" << left << setw(10) << allProcess[i].id;
            cout << "|" << left << setw(10) << allProcess[i].AT;
            cout << "|" << left << setw(10) << allProcess[i].BT;
            cout << "|" << endl;
        }
        cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    }

    void displayQueue(queue<Process> currQueue)
    {
        cout << "\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n";
        while (!currQueue.empty())
        {
            Process p = currQueue.front();
            cout << "|" << setw(3) << p.id;
            currQueue.pop();
        }
        cout << "\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n";
    }
    void finalProcessTable()
    {
        cout << "\n\nFinal Process Table:\n";
        cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
        cout << "|" << left << setw(10) << "ProcessId";
        cout << "|" << left << setw(10) << "AT";
        cout << "|" << left << setw(10) << "BT";
        cout << "|" << left << setw(10) << "CT";
        cout << "|" << left << setw(10) << "TAT";
        cout << "|" << left << setw(10) << "WT";
        cout << "|" << endl;
        for (int i = 0; i < totalP; i++)
        {
            cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
            cout << "|" << left << setw(10) << allProcess[i].id;
            cout << "|" << left << setw(10) << allProcess[i].AT;
            cout << "|" << left << setw(10) << allProcess[i].BT;
            cout << "|" << left << setw(10) << allProcess[i].CT;
            cout << "|" << left << setw(10) << allProcess[i].TAT;
            cout << "|" << left << setw(10) << allProcess[i].WT;
            cout << "|" << endl;
        }
        cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
    }

    void FCFS()
    {
        queue<Process> fcfsq;
        int currTime = 0;

        for (int i = 0; i < totalP; i++)
        {
            fcfsq.push(allProcess[i]);
        }

        while (!fcfsq.empty())
        {
            cout << "\nCurrent FCFS Queue: \n";
            displayQueue(fcfsq);
            Process p = fcfsq.front();

            cout << "\nExecuting Process: " << p.id << endl;
            // Can add processing for actual process
            cout << "\nProcess Executed";

            currTime += p.BT;
            int index = 0;
            while (allProcess[index].id != p.id)
            {
                index++;
            }
            allProcess[index].CT = currTime;
            allProcess[index].calTAT();
            allProcess[index].calWT();

            fcfsq.pop();
        }
        finalProcessTable();
    }

    void SJF_Preemptive()
    {
        cout << "\n\n----- SJF Preemptive (Shortest Remaining Time First) Scheduling -----\n";

        int completed = 0, time = 0, min_remain_time;
        int shortest = -1;
        bool found = false;

        // Make a copy of allProcess burst times (remaining times)
        vector<int> remaining_time(totalP);
        for (int i = 0; i < totalP; i++)
        {
            remaining_time[i] = allProcess[i].BT;
        }

        while (completed != totalP)
        {
            // Find process with minimum remaining time at current time
            min_remain_time = INT_MAX;
            found = false;

            for (int i = 0; i < totalP; i++)
            {
                if (allProcess[i].AT <= time && remaining_time[i] > 0)
                {
                    if (remaining_time[i] < min_remain_time)
                    {
                        min_remain_time = remaining_time[i];
                        shortest = i;
                        found = true;
                    }
                }
            }

            if (!found)
            {
                time++; // No process available at this time, so idle
                continue;
            }

            // Execute process for 1 unit
            remaining_time[shortest]--;
            time++;

            // If process finished
            if (remaining_time[shortest] == 0)
            {
                completed++;
                allProcess[shortest].CT = time;
                allProcess[shortest].calTAT();
                allProcess[shortest].calWT();
            }
        }

        // Display final process table
        finalProcessTable();
    }
};
int main()
{
    int choice;
    Scheduling obj;

    do
    {
        cout << "\n################################################################\n";
        cout << setw(10) << right << "Menu:\n\n";
        cout << setw(5) << right << "1. FCFS" << endl;
        cout << setw(5) << right << "2. SJF" << endl;
        cout << setw(5) << right << "3. Round Robin" << endl;
        cout << setw(5) << right << "4. Priority" << endl;
        cout << setw(5) << right << "5. Exit Program" << endl;
        cout << setw(3) << right << "\nEnter choice code: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            obj.takeIp("process.txt");
            obj.FCFS();
            break;
        case 2:
            obj.takeIp("process.txt");
            obj.SJF_Preemptive();
            break;
        case 3: 
        obj.takeIp("process.txt");
        // obj.roundRobin();
        break;
        case 4:
        cout<<"\nIN process\n";
        break;
        case 5:
        cout<<"\nByee\n";
        break;
        
        default:
        cout<<"\nInvalid choice\n";
            break;
        }
    } while (choice !=5);

    obj.takeIp("process.txt");
    obj.displayProcess();
    obj.FCFS();
    obj.takeIp("process.txt");
    obj.SJF_Preemptive();

    return 0;
}