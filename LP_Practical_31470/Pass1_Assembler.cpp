#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <iomanip>

using namespace std;
class Pass1_DS
{
private:
    map<string, pair<string, string>> OPTAB; // START = {AD, 00}
    map<string, pair<int, int>> SYMTAB;      // X = {1, 100} --> {Index, LC}
    map<string, pair<int, int>> LITTAB;      // ='5' ==> {1, 101} -->{Index, LC}
    map<string, int> REGTAB;                 // [AREG] = 01
    map<string, int> CONDITIONTAB;           // [LT] = 01
    int LC;

public:
    void readTable();
    void displayTable();
    friend class Pass1_Asm;
};
void Pass1_DS ::readTable()
{
    /*-------------------Opcode Table-----------------------*/
    ifstream file("Pass1_Tables_txt\\OPTAB.txt");
    if (!file)
    {
        cout << "\nFile not open unsuccessfully!!\n";
    }
    else
    {
        cout << "\nFile of OPTAB opened successfully\n";
    }

    while (!file.eof())
    {
        string mnemonic, statement, code;
        file >> mnemonic;
        file >> statement;
        file >> code;
        OPTAB[mnemonic] = {statement, code};
    }
    file.close();

    /*-----------Registers Table----------------------*/
    file.open("Pass1_Tables_txt\\REG_TAB.txt");
    if (!file)
    {
        cout << "\nFile not open unsuccessfully!!\n";
    }
    else
    {
        cout << "\nFile of REG_TAB opened successfully\n";
    }

    while (!file.eof())
    {
        string Reg, code;
        file >> Reg;
        file >> code;
        // cout << "code:" << code;
        REGTAB[Reg] = stoi(code);
    }
    file.close();

    /*---------------Condition Code Table-----------------*/
    file.open("Pass1_Tables_txt\\ConditionCode_Tab.txt");
    if (!file)
    {
        cout << "\nFile not open unsuccessfully!!\n";
    }
    else
    {
        cout << "\nFile of ConditionCode_Tab opened successfully\n";
    }

    while (!file.eof())
    {
        string Cond, code;
        file >> Cond;
        file >> code;
        CONDITIONTAB[Cond] = stoi(code);
    }
    file.close();

    displayTable();
}
void Pass1_DS ::displayTable()
{
    // cout<<"\n+-----------------------------------------------------------------+\n";
    // cout<<setw(5)<<"|"<<"Mnemonic"<<"|"<<setw(5)<<"Type"<<"|"<<setw(5)<<"Code"<<"|"<<endl;
    cout << "\n=================================================\n";
    cout << "                       OPTAB\n";
    cout << "=================================================\n";
    for (auto it : OPTAB)
    {
        cout << it.first << "  " << it.second.first << "  " << it.second.second << endl;
    }

    cout << "\n===================================================\n";
    cout << "                       REGTAB\n";
    cout << "===================================================\n";
    for (auto it : REGTAB)
    {
        cout << it.first << "  " << it.second << endl;
    }

    cout << "\n====================================================\n";
    cout << "                    Condition Codes\n";
    cout << "====================================================\n";
    for (auto it : CONDITIONTAB)
    {
        cout << it.first << "  " << it.second << endl;
    }
    cout << endl
         << endl;
}

class Pass1_Asm
{
    Pass1_DS &obj;

public:
    Pass1_Asm(Pass1_DS &ds) : obj(ds) {}

    void readAsmCode(string fileName)
    {
        ifstream fin(fileName);
        ofstream fout("Pass1_Tables_txt\\Intermediate_Code.txt");

        string directive, label, code1, code2;
        fin >> directive;
        fin >> code1;
        // cout<<"d: "<<directive<<"c:"<<code<<endl;
        obj.LC = stoi(code1);
        cout << "\n--------------------------------\n";
        cout << "\nLC: " << obj.LC << endl;
        cout << "\n--------------------------------\n";

        fout << " (" << obj.OPTAB[directive].first << ", " << obj.OPTAB[directive].second << ")" << " (C," << code1 << ")\n";
        // while(!fin.eof())
        // {
        //     /*Now need to check where the current code should be*/
        // }
    }
};

int main()
{
    Pass1_DS obj;
    obj.readTable();
    Pass1_Asm obj1(obj);
    obj1.readAsmCode("Pass1_Tables_txt\\Code_ASM.txt");
    return 0;
}
