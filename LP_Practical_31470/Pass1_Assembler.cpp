#include<iostream>
#include<fstream>
#include<string>
#include<map>

using namespace std;
class Pass1_DS
{
    private:
        map <string,pair<string,string>> OPTAB;   //START = {AD, 00}
        map <string,string,string> SYMTAB;        // X = {1, 100} --> {Index, LC}
        // map <string,
        int LC;

    public:
        void readTable();
        void displayTable();
        
};
void Pass1_DS :: readTable()
{
    ifstream file("OPTAB.txt");
    if(!file)
    {
        cout<<"\nFile not open unsuccessfully!!\n";
    }
    else{
        cout<<"\nFile of OPTAB opened successfully\n";
    }

    while(!file.eof())
    {
        string mnemonic,statement,code;
        file>>mnemonic;
        file>>statement;
        file>>code;
        OPTAB[mnemonic] = {statement,code};
    }
    file.close();
    displayTable();
}
void Pass1_DS :: displayTable()
{
    for(auto it : OPTAB)
    {
        cout<<it.first<<" "<<it.second.first<<" "<<it.second.second<<endl;
    }
}

int main()
{
    Pass1_DS obj;
    obj.readTable();
    return 0;
}
