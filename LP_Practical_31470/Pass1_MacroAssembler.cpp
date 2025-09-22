#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>

using namespace std;

class MNTEntry {
public:
    int index;
    string macroName;
    int numPosParams;
    int numKeywordParams;
    int startMDTIndex;
    int startKPDTPIndex;

    MNTEntry(int idx, const string& name, int pos, int kwd, int mdtStart, int kpdStart)
        : index(idx), macroName(name), numPosParams(pos), numKeywordParams(kwd), 
          startMDTIndex(mdtStart), startKPDTPIndex(kpdStart) {}
};

class KPDTABEntry {
public:
    int index;
    string keyword;
    string defaultValue;

    KPDTABEntry(int idx, const string& kw, const string& def)
        : index(idx), keyword(kw), defaultValue(def) {}
};

class MDTEntry {
public:
    int index;
    string operation;
    string param;

    MDTEntry(int idx, const string& op, const string& p)
        : index(idx), operation(op), param(p) {}
};

class PNTABEntry {
public:
    int paramIndex;
    string paramName;

    PNTABEntry(int idx, const string& name)
        : paramIndex(idx), paramName(name) {}
};

class TableWriter {
public:
    static void writeMNT(const vector<MNTEntry>& mnt, const string& filename) {
        ofstream file(filename);
        for (const auto& entry : mnt) {
            file << entry.index << " " << entry.macroName << " " 
                 << entry.numPosParams << " " << entry.numKeywordParams 
                 << " " << entry.startMDTIndex << " " << entry.startKPDTPIndex << endl;
        }
    }

    static void writeKPDTAB(const vector<KPDTABEntry>& kpdTab, const string& filename) {
        ofstream file(filename);
        for (const auto& entry : kpdTab) {
            file << entry.index << " " << entry.keyword << " " 
                 << entry.defaultValue << endl;
        }
    }

    static void writeMDT(const vector<MDTEntry>& mdt, const string& filename) {
        ofstream file(filename);
        for (const auto& entry : mdt) {
            file << entry.index << " " << entry.operation << " " << entry.param << endl;
        }
    }

    static void writePNTAB(const vector<PNTABEntry>& pntab, const string& filename) {
        ofstream file(filename);
        for (const auto& entry : pntab) {
            file << entry.paramIndex << " " << entry.paramName << endl;
        }
    }
};

class Macro {
private:
    string name;
    vector<string> posParams;
    vector<string> keywordParams;
    map<string, string> paramDefaults;
    vector<string> macroBody;
    int mdtStartIndex;
    int kpdStartIndex;

public:
    Macro(const string& macroName, const vector<string>& params, 
          const vector<string>& kwParams, const map<string, string>& defaults)
        : name(macroName), mdtStartIndex(0), kpdStartIndex(0) {
        posParams = params;
        keywordParams = kwParams;
        paramDefaults = defaults;
    }

    void addMacroBody(const string& line) {
        macroBody.push_back(line);
    }

    void setMDTStartIndex(int index) {
        mdtStartIndex = index;
    }

    void setKPDStartIndex(int index) {
        kpdStartIndex = index;
    }

    const string& getName() const { return name; }
    int getMDTStartIndex() const { return mdtStartIndex; }
    int getKPDStartIndex() const { return kpdStartIndex; }
    const vector<string>& getBody() const { return macroBody; }
    const vector<string>& getPosParams() const { return posParams; }
    const vector<string>& getKeywordParams() const { return keywordParams; }
    const map<string, string>& getParamDefaults() const { return paramDefaults; }
};

class MacroProcessor {
private:
    vector<MNTEntry> MNT;
    vector<KPDTABEntry> KPDTAB;
    vector<MDTEntry> MDT;
    vector<PNTABEntry> PNTAB;
    int mntIndex;
    int mdtIndex;
    int kpdIndex;
    int pntIndex;

public:
    MacroProcessor()
        : mntIndex(1), mdtIndex(1), kpdIndex(1), pntIndex(1) {}

    void processMacros(const string& inputFilename) {
        ifstream inputFile(inputFilename);
        string line;

        while (getline(inputFile, line)) {
            if (line.find("MACRO") != string::npos) {
                processMacro(inputFile);
            }
        }

        inputFile.close();

        // Write tables to files
        TableWriter::writeMNT(MNT, "Macro_Tables/MNT.txt");
TableWriter::writeKPDTAB(KPDTAB, "Macro_Tables/KPDTAB.txt");
TableWriter::writeMDT(MDT, "Macro_Tables/MDT.txt");
TableWriter::writePNTAB(PNTAB, "Macro_Tables/PNTAB.txt");

        cout << "Macro Processor Pass 1 completed. Tables have been written to files." << endl;
    }

private:
    void processMacro(ifstream& inputFile) {
        string line;
        getline(inputFile, line);
        istringstream ss(line);
        string macroName;
        vector<string> posParams;
        vector<string> keywordParams;
        map<string, string> paramDefaults;

        ss >> macroName;

        while (ss >> line) {
            if (line.find("=") != string::npos) {
                size_t pos = line.find("=");
                string kw = line.substr(0, pos);
                string def = line.substr(pos + 1);
                keywordParams.push_back(kw);
                paramDefaults[kw] = def;
            } else {
                posParams.push_back(line);
            }
        }

        // Create the macro object
        Macro macro(macroName, posParams, keywordParams, paramDefaults);
        int mdtStart = mdtIndex;
        int kpdStart = kpdIndex;

        // Add the macro entry to MNT
        MNT.push_back(MNTEntry(mntIndex++, macro.getName(), posParams.size(), keywordParams.size(), mdtStart, kpdStart));

        // Process the macro body
        while (getline(inputFile, line) && line.find("MEND") == string::npos) {
            macro.addMacroBody(line);
            processMDTEntry(line);
        }

        // Add keyword parameter defaults to KPDTAB
        for (const auto& kw : keywordParams) {
            KPDTAB.push_back(KPDTABEntry(kpdIndex++, kw, paramDefaults[kw]));
        }

        // Set the start indices for MDT and KPDTP for the macro
        macro.setMDTStartIndex(mdtStart);
        macro.setKPDStartIndex(kpdStart);

        // Process the body of the macro to fill MDT and PNTAB
        processPNTAB(macro);
    }

    void processMDTEntry(const string& line) {
        istringstream iss(line);
        string op, param;
        iss >> op >> param;
        MDT.push_back(MDTEntry(mdtIndex++, op, param));
    }

    void processPNTAB(const Macro& macro) {
        const vector<string>& posParams = macro.getPosParams();
        for (const string& param : posParams) {
            PNTAB.push_back(PNTABEntry(pntIndex++, param.substr(1)));  // Strip '&'
        }
    }
};

int main() {
    MacroProcessor processor;
processor.processMacros("./Macro_Tables/Macro_1.txt");
    return 0;
}
