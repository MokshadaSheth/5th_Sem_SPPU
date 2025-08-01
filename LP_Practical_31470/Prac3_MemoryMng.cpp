#include <bits/stdc++.h>
using namespace std;

class Block
{
    int id;
    int totalmem;
    int remaining; // totalmem - required
    bool isFree;

public:
    Block()
    {
        id = -1;
        totalmem = -1;
        remaining = -1;
        isFree = true;
    }
    Block(int i, int m)
    {
        id = i;
        totalmem = m;
        remaining = totalmem;
        isFree = true;
    }
    friend class Memory;
    friend class ProcessAllocation;
};

class Memory
{
    Block *allBlocks;
    int totalBlocks;

public:
    Memory()
    {
        totalBlocks = -1;
        allBlocks = nullptr;
    }
    void takeIP()
    {
        ifstream file("Memory.txt");

        if (!file)
        {
            cout << "\nFile access failed";
            return;
        }

        file >> totalBlocks;
        allBlocks = new Block[totalBlocks];
        int i = 0;
        while (!file.eof())
        {
            allBlocks[i].id = i;
            file >> allBlocks[i].totalmem;
            allBlocks[i].remaining = allBlocks[i].totalmem;
            i++;
        }
        file.close();
        displayMem();
    }
    void displayMem()
    {
        cout << "\n------------------------Memory-------------------------------\n";
        cout << "\n+-----------------------+\n";
        cout << "|" << setw(5) << "ID";
        cout << "|" << setw(5) << "Total";
        cout << "|" << setw(5) << "Rem";
        cout << "|" << setw(5) << "Free?";
        cout << "|\n";
        cout << "+-----------------------+\n";

        for (int i = 0; i < totalBlocks; i++)
        {
            cout << "|" << setw(5) << allBlocks[i].id;
            cout << "|" << setw(5) << allBlocks[i].totalmem;
            cout << "|" << setw(5) << allBlocks[i].remaining;
            cout << "|" << setw(5) << allBlocks[i].isFree;
            cout << "|\n";
        }
        cout << "+-----------------------+\n";
    }
    friend class ProcessAllocation;
};
struct Process
{
public:
    int no;
    int size;
    bool isAllocated;

    Process()
    {
        no = -1;
        size = -1;
        isAllocated = false;
    }
};

class ProcessAllocation
{
    int totalProcess;
    Process *allProcess;

public:
    void takeP()
    {
        ifstream file("MemProcess.txt");

        if (!file)
        {
            cout << "\nFile access failed";
            return;
        }

        file >> totalProcess;
        allProcess = new Process[totalProcess];
        for (int i = 0; i < totalProcess; i++)
        {
            allProcess[i].no = i;
            file >> allProcess[i].size;
            allProcess[i].isAllocated = false;
        }
        file.close();
        displayProcess();
    }
    void displayProcess()
    {

        cout << "\n------------------------Process-------------------------------\n";
        cout << "\n+-------------------+\n";
        cout << "|" << setw(5) << "ID";
        cout << "|" << setw(5) << "Size";
        cout << "|" << setw(5) << "Slot?";
        cout << "|\n";
        cout << "+-------------------+\n";

        for (int i = 0; i < totalProcess; i++)
        {
            cout << "|" << setw(5) << allProcess[i].no;
            cout << "|" << setw(5) << allProcess[i].size;
            cout << "|" << setw(5) << allProcess[i].isAllocated;
            cout << "|\n";
        }
        cout << "+-------------------+\n";
    }
    /*            Allocation Startegies             */
    void firstFit(Memory obj)
    {
        for (int i = 0; i < totalProcess; i++)
        {
            int pSize = allProcess[i].size;
            bool flag = false;
            for (int j = 0; j < obj.totalBlocks; j++)
            {

                if (obj.allBlocks[j].remaining >= pSize && obj.allBlocks[j].isFree == true)
                {
                    obj.allBlocks[j].remaining = obj.allBlocks[j].totalmem - pSize;
                    obj.allBlocks[j].isFree = false;
                    flag = true;
                    break;
                }
            }
            if (flag)
            {
                allProcess[i].isAllocated = true;
                cout << "\nAllocated Memory for " << allProcess[i].no << endl;
            }
        }

        obj.displayMem();
        displayProcess();
    }

    void bestFit(Memory obj)
    {
        for (int i = 0; i < totalProcess; i++)
        {
            int pSize = allProcess[i].size;
            bool flag = false;
            int min = INT_MAX;
            int min_index = -1;
            for (int j = 0; j < obj.totalBlocks; j++)
            {

                if (obj.allBlocks[j].remaining >= pSize && obj.allBlocks[j].isFree == true)
                {
                    int currMin = obj.allBlocks[j].remaining - pSize;
                    if (currMin < min)
                    {
                        min = currMin;
                        min_index = j;
                        flag = true;
                    }
                }
            }
            if (flag)
            {
                allProcess[i].isAllocated = true;
                obj.allBlocks[min_index].remaining = obj.allBlocks[min_index].totalmem - pSize;
                obj.allBlocks[min_index].isFree = false;
                cout << "\nAllocated Memory for " << allProcess[i].no << endl;
            }
        }

        obj.displayMem();
        displayProcess();
    }

    void worstFit(Memory obj)
    {

        for (int i = 0; i < totalProcess; i++)
        {
            int pSize = allProcess[i].size;
            bool flag = false;
            int max = INT_MIN;
            int max_index = -1;
            for (int j = 0; j < obj.totalBlocks; j++)
            {

                if (obj.allBlocks[j].remaining >= pSize && obj.allBlocks[j].isFree == true)
                {
                    int currMax = obj.allBlocks[j].remaining - pSize;
                    if (currMax > max)
                    {
                        max = currMax;
                        max_index = j;
                        flag = true;
                    }
                }
            }
            if (flag)
            {
                allProcess[i].isAllocated = true;
                obj.allBlocks[max_index].remaining = obj.allBlocks[max_index].totalmem - pSize;
                obj.allBlocks[max_index].isFree = false;
                cout << "\nAllocated Memory for " << allProcess[i].no << endl;
            }
        }

        obj.displayMem();
        displayProcess();
    }
    void nextFit(Memory obj)
    {
        int prev=0;
        for (int i = 0; i < totalProcess; i++)
        {
            int pSize = allProcess[i].size;
            bool flag = false;
            int n = obj.totalBlocks;
            for (int j = (prev+1)%n; j != prev; j++)
            {

                if (obj.allBlocks[j].remaining >= pSize && obj.allBlocks[j].isFree == true)
                {
                    obj.allBlocks[j].remaining = obj.allBlocks[j].totalmem - pSize;
                    obj.allBlocks[j].isFree = false;
                    flag = true;
                    prev = j;
                    break;
                }
            }
            if (flag)
            {
                allProcess[i].isAllocated = true;
                cout << "\nAllocated Memory for " << allProcess[i].no << endl;
            }
        }

        obj.displayMem();
        displayProcess();
    }
};

int main() {
    Memory obj;
    ProcessAllocation obj1;
    int choice;

    do {
        cout << "\n====== Memory Allocation Menu ======\n";
        cout << "1. First Fit\n";
        cout << "2. Best Fit\n";
        cout << "3. Worst Fit\n";
        cout << "4. Next Fit\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch(choice) {
            case 1:
                obj.takeIP();
                obj1.takeP();
                obj1.firstFit(obj);
                break;
            case 2:
                obj.takeIP();
                obj1.takeP();
                obj1.bestFit(obj);
                break;
            case 3:
                obj.takeIP();
                obj1.takeP();
                obj1.worstFit(obj);
                break;
            case 4:
                obj.takeIP();
                obj1.takeP();
                obj1.nextFit(obj);
                break;
            case 5:
                cout << "Exiting program. Goodbye!\n";
                break;
            default:
                cout << "Invalid choice! Please enter a number between 1 and 5.\n";
        }

    } while(choice != 5);

    return 0;
}