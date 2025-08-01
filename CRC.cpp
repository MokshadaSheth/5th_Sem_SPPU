#include<bits/stdc++.h>
using namespace std;
class Binary
{
    vector<int> num;
    int origi=0;
    int n;

    public:
        Binary()
        {
            // num = -1;
            n = -1;
        }
        void takeIp()
        {
            cout<<"\nEnter total bits: ";
            cin>>n;
            cout<<"Enter bits one by one(space or enter): ";
            for(int i=0; i<n; i++)
            {
                int t;
                cin>>t;
                num.push_back(t);
                origi = origi*10;  //Extra zero if after
                origi += t;
                
            }
            cout<<"\nThe binary number is : "<<origi;
        }
        friend class CRC;
};
class CRC
{
    Binary message;
    Binary generator;
    public: 
    void takeIP()
    {
        cout<<"\nEnter the data word: ";
        message.takeIp();
        cout<<"\nEnter generator: ";
        generator.takeIp();
    }

    void codeWord()
    {
        int append = generator.num.size()-1;
        for(int i=0; i<append; i++)
        {
            message.num.push_back(0);
        }
        cout<<"\nSending this:\n ";
        for(int n : message.num)
        {
            cout<<n;
        }

        message.num[6] = 1;

        cout<<"\nReceived this: \n";
        for(int n : message.num)
        {
            cout<<n;
        }
        
        //Doing calculation
        
    }
};
int main()
{
    CRC obj;
    obj.takeIP();
    obj.codeWord();
}