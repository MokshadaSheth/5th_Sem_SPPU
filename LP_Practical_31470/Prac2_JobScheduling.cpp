#include<bits/stdc++.h>
using namespace std;

class Process
{
    private:
        int id;
        int AT;     //Arrival Time
        int BT;      //Burst Time
        int CT;      //Completion Time
        int TAT;     //Turn Around Time
        int WT;      //Wait Time
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
            WT  = 0;
        }

        void calTAT() //Turn Around Time
        {
            TAT =  CT - AT; 
        }
        void calWT()
        {
            WT = TAT - BT;
        }
};

class Scheduling
{
    private:
       Process *allProcess;  //Will assign size after knowing total process
       int totalP; 
       int currTime;
    public:
        Scheduling()
        {
            allProcess = nullptr;
            totalP = 0;
            currTime = 0;
        }

        void takeIp(const string filename)
        {
            ifstream file(filename);
            if(!file)
            {
                cerr<<"\nFailed to open";
                exit(1);
            }

            file>>totalP;
            allProcess = new Process[totalP];
            for(int m=0; m<totalP;m++)
            {
                int id, AT, BT;

                file>>id>>AT>>BT;
                allProcess[m] = Process(id,AT,BT);
            }
            //Sort allProcess
            for(int i=0; i<totalP;i++)
            {
                for(int j=0;j<totalP-1;j++)
                {
                    if(allProcess[j].AT > allProcess[j+1].AT)
                    {
                        swap(allProcess[j],allProcess[j+1]);
                    }
                }
            }
            file.close();
        }

        void displayProcess()
        {
            cout<<"\n__________________________________\n";
                cout<<"|"<<left<<setw(10)<<"ProcessId";
                cout<<"|"<<left<<setw(10)<<"AT";
                cout<<"|"<<left<<setw(10)<<"BT";
                cout<<"|"<<endl;
            for(int i=0; i<totalP;i++)
            {
            cout<<"___________________________________\n";
                cout<<"|"<<left<<setw(10)<<allProcess[i].id;
                cout<<"|"<<left<<setw(10)<<allProcess[i].AT;
                cout<<"|"<<left<<setw(10)<<allProcess[i].BT;
                cout<<"|"<<endl;
            }
            cout<<"______________________________________\n";
        }

        void FCFS()
        {
            queue<Process> fcfsq;
            
            for(int i=0; i<totalP;i++)
            {
                fcfsq.push(allProcess[i]);
            }

            cout<<"\nCurrent Queue:\n";
            cout<<"________________________________________________";
            for(int t=*fcfsq.front;t<)
        }
};
int main()
{
    Scheduling obj;
    obj.takeIp("process.txt");
    obj.displayProcess();
    return 0;
}