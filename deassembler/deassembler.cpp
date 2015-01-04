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
	bool pointToData;
	
	InstructionSetElement() {}
	InstructionSetElement(string mnemonic, string operandFormat, int format, int opCode) : 
	    mnemonic(mnemonic), operandFormat(operandFormat), format(format), opCode(opCode), pointToData(false) {}
};

char buffer[BUFFER];
map<int, string> code;
set<int> dataStart;
set<int> labels;
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
//        printf("%.2X",byte);
        code[start] += byte;
    }
//    printf("\n");
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
        int format, opCode = -1, dataSize;
        sscanf(tokens[2].c_str(), "%d", &format);
        sscanf(tokens[3].c_str(), "%X", &opCode);
        InstructionSetElement element(mnemonic, operandStyle, format, opCode);
        if (~opCode) {
            sscanf(tokens[4].c_str(), "%d", &dataSize);
            if (dataSize)
                element.pointToData = true;
            opCodeToInstruction[(char)opCode] = element;    
        }
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


int format1(const InstructionSetElement& instruction, const char* segment, int address) {
    printf("%.6X:\t%s\n",address, instruction.mnemonic.c_str());
    return 1;
}

int format2(const InstructionSetElement& instruction, const char* segment, int address) {
    int r1 = (segment[1] & 0xF0) >> 4;
    int r2 = segment[1] & 0x0F;
    printf("%.6X:\t%s\t%d",address, instruction.mnemonic.c_str(), r1);
    if (instruction.operandFormat == "r1,r2")
        printf(",%d",r2);
    printf("\n");
    return 2;
}

char accessSign(bool indirect, bool immediate) {
    if (indirect && immediate) return 0;
    if (indirect) return '@';
    return '#';
}

int format3(const InstructionSetElement& instruction, const char* segment, int address, bool indirect, bool immediate, 
            bool base, bool pc, bool x) {
    int disp = (((segment[1] & 0x0F) << 8) | segment[2]);
//    printf("%d %d\n",(segment[1] & 0x0F), segment[2]);
//    printf("%d\n",disp);
    int A = disp;
    if (pc) A += address + 3;
    if (base) return 3;
    printf("%.6X:\t%s\t%c",address, instruction.mnemonic.c_str(), accessSign(indirect, immediate));
    if (immediate && !indirect) printf("%d", A);
    else printf("%.6X",A);
    printf("\n");
    if (instruction.pointToData) 
        dataStart.insert(A);
    labels.insert(A);
    return 3;
}

int format4(const InstructionSetElement& instruction, const char* segment, int address, bool indirect, bool immediate) {
    int A = ((((segment[1] & 0x0F) << 8) | segment[2]) << 8) | segment[3];
    printf("%.6X:\t+%s\t%c",address, instruction.mnemonic.c_str(), accessSign(indirect, immediate));
    if (immediate && !indirect) printf("%d", A);
    else printf("%.6X",A);
    printf("\n");
    if (instruction.pointToData)
        dataStart.insert(A);
    labels.insert(A);
    return 4;
}

int format3Or4(const InstructionSetElement& instruction, const char* segment, int address) {
    bool indirect, immediate, base, pc, x, e;
    indirect = segment[0] & (1 << 1);
    immediate = segment[0] & (1 << 0);
    x = segment[1] & (1 << 7);
    base = segment[1] & (1 << 6);
    pc = segment[1] & (1 << 5);
    e = segment[1] & (1 << 4);
//    printf("%.2X%.2X%.2X",segment[0],segment[1],segment[2]);
//    if (e) printf("%.2X",segment[3]);
//    printf("\n");
//    printf("%d%d%d%d%d%d\n",indirect, immediate, x, base, pc, e);
    if (e) {
        return format4(instruction, segment, address, indirect, immediate);
    } else {
        return format3(instruction, segment, address, indirect, immediate, base, pc, x);
    }
}

int deassembleInstruction(const char* segment, int address) {
    int opCode = segment[0] & ~3;
//    printf("%.2X\n",opCode);
    InstructionSetElement instruction = opCodeToInstruction[opCode];
    if (instruction.mnemonic == "RSUB") {
        printf("%.6X: RSUB\n",address);
        return 3;
    }
    if (instruction.format == 1)
        return format1(instruction, segment, address);
    else if (instruction.format == 2)
        return format2(instruction, segment, address);
    else return format3Or4(instruction, segment, address);
}

int deassembleData(const char* segment, int address) {
    int dataSize = strlen(segment);
    set<int> :: iterator it = labels.upper_bound(address);
    if (it != labels.end()) 
        dataSize = min(dataSize, *it - address);
    printf("%.6X: ", address);
    for (int c=0;c<dataSize;c++)
        printf("%.2X",segment[c]);
    printf("\n");
    return dataSize;
}

// returns the instruction size
int deassembleLine(const char* segment, int address) {
    if (dataStart.count(address)) 
        return deassembleData(segment, address);
    else return deassembleInstruction(segment, address);
}

void deassemble() {
    foreach(it, code) {
        const string& segment = it -> second;        
        for (ptr = 0; ptr < segment.size(); ptr += deassembleLine(segment.c_str() + ptr, it->first + ptr));
    }
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
//    foreach(it, code) {
//        printf("%.6X: ", it->first);
//        const string& s = it->second;
//        for (c=0;c<s.size();c++)
//            printf("(%.2X)",s[c]);
//        printf("\n");
//    }
//    printf("-----------\n\n");
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
    printf("-----------\n\n");
    printf("Code\n\n");
    deassemble();
}
