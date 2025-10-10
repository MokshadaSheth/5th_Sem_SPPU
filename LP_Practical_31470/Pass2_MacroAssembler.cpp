#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>

using namespace std;

struct MNTEntry {
    int index;
    string macroName;
    int numPosParams;
    int numKeywordParams;
    int startMDTIndex;
    int startKPDTPIndex;
};

struct KPDTABEntry {
    int index;
    string keyword;
    string defaultValue;
};

struct MDTEntry {
    int index;
    string operation;
    string param;
};

struct PNTABEntry {
    int paramIndex;
    string paramName;
};

class Pass2MacroProcessor {
private:
    vector<MNTEntry> MNT;
    vector<KPDTABEntry> KPDTAB;
    vector<MDTEntry> MDT;
    vector<PNTABEntry> PNTAB;

    // Helpers for quick lookup
    unordered_map<string, MNTEntry> macroMap;
    unordered_map<string, int> pntabMap; // paramName -> paramIndex

public:
    void loadTables() {
        loadMNT();
        loadKPDTAB();
        loadMDT();
        loadPNTAB();
        // Build fast lookup maps
        for (auto &mnt : MNT) {
            macroMap[mnt.macroName] = mnt;
        }
        for (auto &pnt : PNTAB) {
            pntabMap[pnt.paramName] = pnt.paramIndex;
        }
    }

    void expandMacros(const string& sourceFile, const string& outputFile) {
        ifstream src(sourceFile);
        ofstream out(outputFile);

        string line;
        while (getline(src, line)) {
            istringstream iss(line);
            string firstWord;
            iss >> firstWord;

            // Check if this is a macro call
            if (macroMap.find(firstWord) != macroMap.end()) {
                // Macro found
                MNTEntry mnt = macroMap[firstWord];

                // Read parameters from macro call line
                vector<string> actualParams;
                string param;
                while (iss >> param) {
                    actualParams.push_back(param);
                }

                // Map actual params to PNTAB and keyword params
                // Build argument table: paramName -> actualValue
                map<string, string> argTable;

                // Positional parameters fill first
                int posCount = mnt.numPosParams;
                for (int i = 0; i < posCount && i < (int)actualParams.size(); ++i) {
                    string pname = getPNTABParam(i + 1);
                    argTable[pname] = actualParams[i];
                }

                // Keyword parameters: from actual params after positional
                // Format for keyword param: kw=val
                for (int i = posCount; i < (int)actualParams.size(); ++i) {
                    string &kwparam = actualParams[i];
                    size_t eqPos = kwparam.find('=');
                    if (eqPos != string::npos) {
                        string kw = kwparam.substr(0, eqPos);
                        string val = kwparam.substr(eqPos + 1);
                        argTable[kw] = val;
                    }
                }

                // Fill missing keyword params with default from KPDTAB
                int kpdStart = mnt.startKPDTPIndex;
                for (int i = 0; i < mnt.numKeywordParams; ++i) {
                    KPDTABEntry kpd = KPDTAB[kpdStart - 1 + i]; // -1 because indexing started from 1
                    if (argTable.find(kpd.keyword) == argTable.end()) {
                        argTable[kpd.keyword] = kpd.defaultValue;
                    }
                }

                // Now expand macro body from MDT starting at startMDTIndex till MEND
                int mdtIdx = mnt.startMDTIndex;
                while (mdtIdx <= (int)MDT.size()) {
                    MDTEntry mdtEntry = MDT[mdtIdx - 1]; // -1 for 0-based index
                    if (mdtEntry.operation == "MEND") break;

                    string expandedLine = mdtEntry.operation + " ";

                    // Substitute parameters in mdtEntry.param with actual args
                    string paramStr = mdtEntry.param;
                    // Replace &param or keyword param with argTable values

                    size_t pos = 0;
                    while ((pos = paramStr.find('&', pos)) != string::npos) {
                        size_t end = pos + 1;
                        while (end < paramStr.size() && (isalnum(paramStr[end]) || paramStr[end]=='_')) {
                            end++;
                        }
                        string paramName = paramStr.substr(pos + 1, end - pos - 1);
                        if (argTable.find(paramName) != argTable.end()) {
                            paramStr.replace(pos, end - pos, argTable[paramName]);
                            pos += argTable[paramName].size();
                        } else {
                            pos = end;
                        }
                    }

                    expandedLine += paramStr;

                    out << "+" << expandedLine << endl;

                    mdtIdx++;
                }
            } else {
                // Not a macro call, output as-is
                out << line << endl;
            }
        }

        src.close();
        out.close();

        cout << "Pass 2 Macro Processing complete. Output written to " << outputFile << endl;
    }

private:
    void loadMNT() {
        ifstream file("Macro_Tables/MNT.txt");
        if (!file.is_open()) {
            cerr << "Failed to open MNT.txt" << endl;
            return;
        }
        MNT.clear();
        string line;
        while (getline(file, line)) {
            istringstream iss(line);
            MNTEntry entry;
            iss >> entry.index >> entry.macroName >> entry.numPosParams >> entry.numKeywordParams
                >> entry.startMDTIndex >> entry.startKPDTPIndex;
            MNT.push_back(entry);
        }
        file.close();
    }

    void loadKPDTAB() {
        ifstream file("Macro_Tables/KPDTAB.txt");
        if (!file.is_open()) {
            cerr << "Failed to open KPDTAB.txt" << endl;
            return;
        }
        KPDTAB.clear();
        string line;
        while (getline(file, line)) {
            istringstream iss(line);
            KPDTABEntry entry;
            iss >> entry.index >> entry.keyword >> entry.defaultValue;
            KPDTAB.push_back(entry);
        }
        file.close();
    }

    void loadMDT() {
        ifstream file("Macro_Tables/MDT.txt");
        if (!file.is_open()) {
            cerr << "Failed to open MDT.txt" << endl;
            return;
        }
        MDT.clear();
        string line;
        while (getline(file, line)) {
            istringstream iss(line);
            MDTEntry entry;
            iss >> entry.index >> entry.operation;
            getline(iss, entry.param);
            if (!entry.param.empty() && entry.param[0] == ' ')
                entry.param.erase(0, 1); // trim leading space
            MDT.push_back(entry);
        }
        file.close();
    }

    void loadPNTAB() {
        ifstream file("Macro_Tables/PNTAB.txt");
        if (!file.is_open()) {
            cerr << "Failed to open PNTAB.txt" << endl;
            return;
        }
        PNTAB.clear();
        string line;
        while (getline(file, line)) {
            istringstream iss(line);
            PNTABEntry entry;
            iss >> entry.paramIndex >> entry.paramName;
            PNTAB.push_back(entry);
        }
        file.close();
    }

    string getPNTABParam(int idx) {
        // PNTAB paramIndex starts from 1
        for (const auto& entry : PNTAB) {
            if (entry.paramIndex == idx)
                return entry.paramName;
        }
        return "";
    }
};

int main() {
    Pass2MacroProcessor processor;
    processor.loadTables();

    // Assumed source file containing macro calls is "Macro_Tables/Macro_Call.txt"
    // Output will be "Macro_Tables/MacroOp.txt"
    processor.expandMacros("Macro_Tables/Macro_Call.txt", "Macro_Tables/MacroOp.txt");

    return 0;
}
