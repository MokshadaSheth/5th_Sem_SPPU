#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <sstream>

using namespace std;

class Pass1_Imported_DS
{
    public:
    map<int , int> SYMTAB;
    map<int, int> LITTAB;


    void readTables()
    {
        ifstream fin("Pass1_Tables_txt//SYMTAB.txt");
        if (fin.eof() == true)
        {
            cout << "\n\nSYMTAB is not generated for this IC\n";
        }
        while (!fin.eof())
        {
            string sym;
            int index, loc;
            fin >> sym >> index >> loc;
            SYMTAB[index] = loc;
        }
        fin.close();

        fin.open("Pass1_Tables_txt//LITTAB.txt");
        if (fin.eof() == true)
        {
            cout << "\n\nLITTAB is not generated for this IC\n";
        }
        while (!fin.eof())
        {
            string lit;
            int index, loc;
            fin >> lit >> index >> loc;
            LITTAB[index] = loc;
        }
        fin.close();
        cout << "\nTables of Pass1 imported successfully\n";
    }
};
class Pass2_Asm
{
    public:
        void machineCodeGeneration(Pass1_Imported_DS &obj)
{
    ofstream fout("Pass2_Tables_txt//Machine_Code_2.txt");
    ifstream fin("Pass1_Tables_txt//Intermediate_Code_2.txt");

    if (!fin)
    {
        cout << "Intermediate_Code.txt file not found!\n";
        return;
    }

    string line;
    while (getline(fin, line))
    {
        // Example input line tokens: (IS,04) 01 (S,02)
        istringstream iss(line);
        string token;

        bool skipLine = false; // for AD directive, skip output

        while (iss >> token)
        {
            // Remove surrounding parentheses if any
            if (token.front() == '(' && token.back() == ')')
                token = token.substr(1, token.size() - 2);

            // Token format now TYPE,INDEX or just a code
            size_t commaPos = token.find(',');

            if (commaPos != string::npos)
            {
                string type = token.substr(0, commaPos);
                string val = token.substr(commaPos + 1);

                if (type == "AD")
                {
                    // Assembler Directive - no machine code generated
                    skipLine = true;
                    break; // stop processing this line
                }
                else if (type == "IS")
                {
                    // Imperative Statement, output opcode value
                    // val is opcode number, print with leading zero if single digit
                    int opcode = stoi(val);
                    if (opcode < 10)
                        fout << "0" << opcode << " ";
                    else
                        fout << opcode << " ";
                }
                else if (type == "DL")
                {
                    // Declarative Statement
                    // e.g. (DL,01) means constant definition
                    skipLine = true; // usually no machine code to generate here
                    break;
                }
                else if (type == "S")
                {
                    // Symbol table reference, val is symbol index
                    int symIndex = stoi(val);
                    if (obj.SYMTAB.find(symIndex) != obj.SYMTAB.end())
                        fout << obj.SYMTAB[symIndex] << " ";
                    else
                        fout << "0000 "; // or some default/filler
                }
                else if (type == "L")
                {
                    // Literal table reference
                    int litIndex = stoi(val);
                    if (obj.LITTAB.find(litIndex) != obj.LITTAB.end())
                        fout << obj.LITTAB[litIndex] << " ";
                    else
                        fout << "0000 ";
                }
                else if (type == "C")
                {
                    // Constant value
                    fout << val << " ";
                }
                else
                {
                    // Unknown type, just print as is or skip
                    fout << token << " ";
                }
            }
            else
            {
                // For tokens not in (TYPE,VAL) format, print as is or handle accordingly
                // Could be register codes, labels, etc.
                fout << token << " ";
            }
        }

        if (!skipLine)
            fout << "\n";
    }

    fin.close();
    fout.close();

    cout << "Machine code generated successfully in Pass2_Tables_txt//Machine_Code.txt\n";
}

};

int main()
{
    Pass1_Imported_DS obj;
    obj.readTables();
    Pass2_Asm obj1;
    obj1.machineCodeGeneration(obj);
    return 0;
}