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

    
}
void Pass1_DS ::displayTable()
{
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

    cout << "\n====================================================\n";
    cout << "                      SYMTAB\n";
    cout << "====================================================\n";
    cout << "Symbol\tIndex\tLC\n";
    cout << "---------------------------------\n";
    for (auto it : SYMTAB)
    {
        cout << it.first << "\t" << it.second.first << "\t" << it.second.second << endl;
    }

    cout << "\n====================================================\n";
    cout << "                      LITTAB\n";
    cout << "====================================================\n";
    cout << "Literal\tIndex\tLC\n";
    cout << "---------------------------------\n";
    for (auto it : LITTAB)
    {
        cout << it.first << "\t" << it.second.first << "\t" << it.second.second << endl;
    }

    cout << endl << endl;
}


class Pass1_Asm
{
    Pass1_DS &obj;

public:
    Pass1_Asm(Pass1_DS &ds) : obj(ds) {}

       void readAsmCode(string fileName)
    {
        ifstream fin(fileName);
        ofstream fout("Pass1_Tables_txt\\Intermediate_Code_2.txt");

        string directive, code1;
        fin >> directive >> code1;

        // START
        obj.LC = stoi(code1);
        fout << "(" << obj.OPTAB[directive].first << "," << obj.OPTAB[directive].second << ") (C," << code1 << ")\n";

        string label, opcode, op1, op2;
        while (fin >> label)
        {
            opcode = op1 = op2 = "";

            // If the first word is an opcode
            if (obj.OPTAB.find(label) != obj.OPTAB.end())
            {
                opcode = label;
            }
            else
            {
                // otherwise first word is label, second is opcode
                fin >> opcode;

                // Insert label into SYMTAB
                if (obj.SYMTAB.find(label) == obj.SYMTAB.end())
                {
                    int index = obj.SYMTAB.size() + 1;
                    obj.SYMTAB[label] = {index, obj.LC};
                }
                else
                {
                    obj.SYMTAB[label].second = obj.LC;
                }
            }

            // ===== Special directives =====
            if (opcode == "END")
            {
                fout << "(" << obj.OPTAB[opcode].first << "," << obj.OPTAB[opcode].second << ")\n";
                // dump literals
                for (auto &lit : obj.LITTAB)
                {
                    if (lit.second.second == -1)
                    {
                        lit.second.second = obj.LC;
                        fout << "(DL,01) (C," << lit.first.substr(2, lit.first.size() - 3) << ")\n";
                        obj.LC++;
                    }
                }
                break;
            }
            else if (opcode == "LTORG")
            {
                fout << "(" << obj.OPTAB[opcode].first << "," << obj.OPTAB[opcode].second << ")\n";
                for (auto &lit : obj.LITTAB)
                {
                    if (lit.second.second == -1)
                    {
                        lit.second.second = obj.LC;
                        fout << "(DL,01) (C," << lit.first.substr(2, lit.first.size() - 3) << ")\n";
                        obj.LC++;
                    }
                }
                continue;
            }
            else if (opcode == "ORIGIN")
            {
                string expr;
                fin >> expr;
                size_t pos = expr.find('+');
                if (pos != string::npos)
                {
                    string sym = expr.substr(0, pos);
                    int offset = stoi(expr.substr(pos + 1));
                    obj.LC = obj.SYMTAB[sym].second + offset;
                }
                else
                {
                    obj.LC = obj.SYMTAB[expr].second;
                }
                fout << "(" << obj.OPTAB[opcode].first << "," << obj.OPTAB[opcode].second << ") (C," << obj.LC << ")\n";
                continue;
            }
            else if (opcode == "DS")
            {
                string size; fin >> size;
                fout << "(DL,02) (C," << size << ")\n";
                obj.SYMTAB[label].second = obj.LC;
                obj.LC += stoi(size);
                continue;
            }
            else if (opcode == "DC")
            {
                string val; fin >> val;
                fout << "(DL,01) (C," << val.substr(1, val.size() - 2) << ")\n";
                obj.SYMTAB[label].second = obj.LC;
                obj.LC++;
                continue;
            }

            // ===== Normal instructions =====
            if (!(opcode == "STOP")) fin >> op1;
            if (!(fin.peek() == '\n' || fin.eof()) && opcode != "STOP")
                fin >> op2;

            fout << "(" << obj.OPTAB[opcode].first << "," << obj.OPTAB[opcode].second << ") ";

            // Operand 1
            if (!op1.empty())
            {
                if (obj.REGTAB.find(op1) != obj.REGTAB.end())
                {
                    fout << "(" << obj.REGTAB[op1] << ")";
                }
                else if (obj.CONDITIONTAB.find(op1) != obj.CONDITIONTAB.end())
                {
                    fout << "(" << obj.CONDITIONTAB[op1] << ")";
                }
                else if (op1[0] == '=')
                {
                    int index = obj.LITTAB.size() + 1;
                    obj.LITTAB[op1] = {index, -1};
                    fout << "(L," << index << ")";
                }
                else
                {
                    if (obj.SYMTAB.find(op1) == obj.SYMTAB.end())
                    {
                        int index = obj.SYMTAB.size() + 1;
                        obj.SYMTAB[op1] = {index, -1};
                    }
                    fout << "(S," << setw(2) << setfill('0') << obj.SYMTAB[op1].first << ")";
                }
            }

            // Operand 2
            if (!op2.empty())
            {
                fout << " , ";
                if (obj.REGTAB.find(op2) != obj.REGTAB.end())
                {
                    fout << "(" << obj.REGTAB[op2] << ")";
                }
                else if (obj.CONDITIONTAB.find(op2) != obj.CONDITIONTAB.end())
                {
                    fout << "(" << obj.CONDITIONTAB[op2] << ")";
                }
                else if (op2[0] == '=')
                {
                    int index = obj.LITTAB.size() + 1;
                    obj.LITTAB[op2] = {index, -1};
                    fout << "(L," << index << ")";
                }
                else
                {
                    if (obj.SYMTAB.find(op2) == obj.SYMTAB.end())
                    {
                        int index = obj.SYMTAB.size() + 1;
                        obj.SYMTAB[op2] = {index, -1};
                    }
                    fout << "(S," << setw(2) << setfill('0') << obj.SYMTAB[op2].first << ")";
                }
            }

            fout << "\n";
            obj.LC++;
        }

        fin.close();
        fout.close();
        obj.displayTable();
    }
};

int main()
{
    Pass1_DS obj;
    obj.readTable();
    Pass1_Asm obj1(obj);
    obj1.readAsmCode("Pass1_Tables_txt\\Code_ASM_2.txt");
    return 0;
}
