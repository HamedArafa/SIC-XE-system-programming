#include<bits/stdc++.h>
using namespace std ;

#include "../SEC-XE/Shared/Common.cpp"

#define foreach(it, a) for(__typeof((a).begin()) it = (a).begin() ; it != (a).end() ; it++)

const int BUFFER = 10000;
const string instructionSetFile = "instructionSet.txt";

class InstructionSetElement
{
public:
	string mnemonic, operandFormat;
	int format, opCode;
	
	InstructionSetElement() {}
	InstructionSetElement(string mnemonic, string operandFormat, int format, int opCode) : 
	    mnemonic(mnemonic), operandFormat(operandFormat), format(format), opCode(opCode) {}
};

char buffer[BUFFER];
map<int, string> code;
int ptr;
map<int, InstructionSetElement> opCodeToInstruction;
string programName;
int programSize, entryPoint, startingAddress;

void scanHeader() {
    cin >> programName;
    scanf("%6X%6X", &startingAddress, &programSize);
}

void scanTextRecord() {
    int start, sz;
    scanf("%6X%2X", &start, &sz);
    for (int c=0;c<sz;c++) {
        int byte;
        scanf("%2X", &byte);
        code[start] += byte;
    }
}

void scanEndRecord() {
    scanf("%6X", &entryPoint);
}

void mergeConsecutiveBlocks() {
    vector<int> badKeys;
    foreach(it, code) {
        map<int, string> :: iterator nextIt = it;
        nextIt++;
        const int start = it->first;
        if (nextIt != code.end() && start + it->second.size() == nextIt->first) {
            it->second += nextIt -> second;
            nextIt -> second = "";
            badKeys.push_back(nextIt -> first);
        }
    }
    foreach(it, badKeys)
        code.erase(*it);
}

vector<string> tokenize(string line)
{
    line += '\t';
    int len = line.size();
    string token = "";
    vector<string> tokens;
    for(int i=0; i<len; i++)
    {
        if(line[i] == '\t')
        {
            tokens.push_back(token);
            token = "";
        }
        else token += line[i];
    }
    return tokens;
}

void loadInstructionSet() {
    FILE* file = fopen(instructionSetFile.c_str(), "r");
    while (fgets(buffer, BUFFER, file)) {
        vector<string> tokens = tokenize(buffer);
        const string& mnemonic = tokens[0];
        const string& operandStyle = tokens[1];
        int format, opCode = -1;
        sscanf(tokens[2].c_str(), "%d", &format);
        sscanf(tokens[3].c_str(), "%X", &opCode);
        InstructionSetElement element(mnemonic, operandStyle, format, opCode);
        if (~opCode)
            opCodeToInstruction[opCode] = element;
        
    }
    fclose(file);
}

int getInstructionOpCode(const string& s, int ptr) {
    int opCode = s[ptr];
    opCode &= ~3;
    if (opCodeToInstruction.find(opCode) == opCodeToInstruction.end()) 
        return -1;
    return opCode;
}

void extractFormatOneInstruction() {
    
}

void skim() {
    if (~opCode) extractInstruction(opCode);
}

int main() {
    loadInstructionSet();
    freopen("objectCode.asm", "r", stdin);
    int c,c2;
    char ch;
    while (~scanf(" %c", &ch)) {
        if (ch == 'H') scanHeader();
        else if (ch == 'T') scanTextRecord();
        else scanEndRecord();
    }
    mergeConsecutiveBlocks();
    printf("EntryPoint = %.6X\n", entryPoint);
    printf("ProgramSize = %.6X\n", programSize);
    printf("StartingAddress = %.6X\n", startingAddress);
    cout << programName << endl;
    foreach(it, code) {
        printf("%.6X: ", it->first);
        const string& s = it->second;
        for (c=0;c<s.size();c++)
            printf("%.2X",s[c]);
        printf("\n");
    }
    skim();
        
}
