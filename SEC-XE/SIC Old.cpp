#include<bits/stdc++.h>
using namespace std;

struct Instruction
{
	vector<string> tokens;
	string label, type, operands, location, objectCode;
	Instruction(){}
	Instruction(vector<string> tokens)
	{
		this -> tokens = tokens;
		this -> label = tokens[0];
		this -> type = tokens[1];
		this -> operands = tokens[2];
	}
};

struct Symbol
{
	string name, location;
	Symbol(){}
	Symbol(string name, string location)
	{
		this->name = name;
		this->location = location;
	}
};

class HEX
{
	public:
	static string add(string A, string B)
	{
		int a = getInt(A), b = getInt(B);
		return getString(a+b);
	}
	static string subtract(string A, string B)
	{
		int a = getInt(A), b = getInt(B);
		return getString(a-b);
	}
	static int getInt(string S)
	{
		int len = S.size(), ret = 0, factor = 1;
		for(int i=len-1; i>=0; i--)
		{
			ret += factor * getDigitValue(S[i]);
			factor *= 16;
		}
		return ret;
	}
	static int getDigitValue(char c)
	{
		if(isdigit(c))return c-'0';
		return 10 + toupper(c) - 'A';
	}
	static string getString(int x)
	{
		if(!x)return "0";
		string ret = "";
		while(x)
		{
			ret += getDigitChar(x%16);
			x/=16;
		}
		reverse(ret.begin(), ret.end());
		return ret;
	}
	static int getDigitChar(int x)
	{
		if(x<10)return x + '0';
		return 'A' + (x-10);
	}
};

class DEC
{
	public:
	static string add(string A, string B)
	{
		int a = getInt(A), b = getInt(B);
		return getString(a+b);
	}
	static int getInt(string S)
	{
		int len = S.size(), ret = 0, factor = 1;
		for(int i=len-1; i>=0; i--)
		{
			ret += factor * getDigitValue(S[i]);
			factor *= 10;
		}
		return ret;
	}
	static int getDigitValue(char c)
	{
		return c-'0';
	}
	static string getString(int x)
	{
		if(!x)return "0";
		string ret = "";
		while(x)
		{
			ret += getDigitChar(x%10);
			x/=10;
		}
		reverse(ret.begin(), ret.end());
		return ret;
	}
	static int getDigitChar(int x)
	{
		return x + '0';
	}
};

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
		else token+=line[i];
	}
	return tokens;
}

vector<Instruction> getInputCode()
{
	freopen("code.in","r",stdin);
	vector<Instruction> instructions;
	char line[100001];
	while(gets(line))
	{
		int len = strlen(line);
		for(int i=0; i<len; i++)
			if(isalpha(line[i]))
				line[i] = toupper(line[i]);
		vector<string> tokens = tokenize(line);
		instructions.push_back(Instruction(tokens));
	}
	return instructions;
}

struct Assembler
{
	string programName, startLocation;
	vector<Instruction> instructions;
	int instructionCount;

	Assembler(vector<Instruction> instructions)
	{
		this -> instructions = instructions;
		this -> instructionCount = instructions.size();
	}

	void runPass1()
	{
		programName = getProgramName();
		startLocation = getStartLocation();
		getLocations();
		printPass1();
	}
	void runPass2()
	{
		//printf("%d **\n",instructionCount);
		getObjectCodes();
		printPass2();
	}
	void getObjectCodes()
	{
		for(int i=1; i<instructionCount-1; i++)
		{
			string opCode = getOpCode(instructions[i].type);
			string labelLocation = getLabelLocation(instructions[i]);
			instructions[i].objectCode = opCode + labelLocation;
		}
	}
	string getLabelLocation(Instruction instruction)
	{
		if(instruction.type == "START" || instruction.type == "END" || instruction.type == "RESW" || instruction.type == "RESB")
		{
			return "";
		}
		else if(instruction.type == "RSUB")
		{
			return "4C0000";
		}
		else if(instruction.type == "WORD")
		{
			string ret = HEX::getString(DEC::getInt(instruction.operands));
			string leadingZeros = "";
			for(int j=(int)ret.size(); j<4; j++)leadingZeros += "0";
			return leadingZeros + ret;
		}
		else if(instruction.type == "BYTE")
		{
			string ret = "";
			if(instruction.operands[0] == 'X')
			{
				for(int i=2; instruction.operands[i]!='\''; i++)
					ret += instruction.operands[i];
				return ret;
			}
			if(instruction.operands[0] == 'C')
			{
				for(int i=2; instruction.operands[i]!='\''; i++)
					ret += HEX::getString((int)instruction.operands[i]);
				return ret;
			}
		}
		else
		{
			string add = "0", ret;
			if(instruction.operands.find(",X") != -1)
			{
				instruction.operands.erase((int)instruction.operands.size()-2);
				add = "8000";
			}
			for(int i=0; i<instructionCount; i++)
			{
				if(instructions[i].label == instruction.operands)
				{
					ret = HEX::add(instructions[i].location, add);
					break;
				}
			}
			string leadingZeros = "";
			for(int j=(int)ret.size(); j<4; j++)leadingZeros += "0";
			return leadingZeros + ret;
		}
	}
	string getOpCode(string type)
	{
		if(type=="WORD")return "00";
		if(type=="ADD")return "18";
		if(type=="COMP")return "28";
		if(type=="JEQ")return "30";
		if(type=="JLT")return "38";
		if(type=="JSUB")return "48";
		if(type=="LDA")return "00";
		if(type=="LDX")return "04";
		if(type=="STL")return "14";
		if(type=="STX")return "10";
		if(type=="STA")return "0C";
		if(type=="TIX")return "2C";
		if(type=="J")return "3C";
		if(type=="LDL")return "08";
		if(type=="TD")return "E0";
		if(type=="RD")return "D8";
		if(type=="STCH")return "54";
		if(type=="WD")return "DC";
		if(type=="LDCH")return "50";
		return "";
	}
	string getProgramName()
	{
		return instructions[0].tokens[0];
	}
	string getStartLocation()
	{
		return instructions[0].tokens[2];
	}
	void getLocations()
	{
		string currentLocation = startLocation, toAdd;
		string leadingZeros = "";
		for(int j=(int)currentLocation.size(); j<4; j++)
			leadingZeros += '0';
		instructions[0].location = leadingZeros + currentLocation;
		for(int i=1; i<instructionCount; i++)
		{
			leadingZeros = "";
			for(int j=(int)currentLocation.size(); j<4; j++)
				leadingZeros += '0';
			instructions[i].location = leadingZeros + currentLocation;
			toAdd = getToAdd(instructions[i]);
			currentLocation = getNewLocation(currentLocation, toAdd);
		}
	}
	string getToAdd(Instruction instruction)
	{
		if(instruction.type == "RESW")
			return HEX::getString(DEC::getInt(instruction.tokens[2]) * 3);
		if(instruction.type == "RESB")
			return HEX::getString(DEC::getInt(instruction.tokens[2]));
		if(instruction.type == "BYTE")
		{
			int ret = 0;
			if(instruction.operands[0] == 'C')
			{
				for(int i=2; instruction.operands[i]!='\''; i++)
					ret++;
				return HEX::getString(ret);
			}
			if(instruction.operands[0] == 'X')
			{
				for(int i=2; instruction.operands[i]!='\''; i++)
					ret++;
				return HEX::getString(ret/2);
			}
		}
		return "3";
	}
	string getNewLocation(string currentLocation, string toAdd)
	{
		return HEX::add(currentLocation, toAdd);
	}
	void printPass1()
	{
		cout << "----------------------------- PASS 1 ------------------------------\n" << endl;
		FILE *out = fopen("pass1.out","w");
		for(int i=0; i<instructionCount; i++)
		{
			fprintf(out,"%-10s%-10s%-10s%-10s\n", instructions[i].location.c_str(), instructions[i].label.c_str(), instructions[i].type.c_str(), instructions[i].operands.c_str());
			cout << instructions[i].location << "\t" << instructions[i].label << "\t" << instructions[i].type << "\t" << instructions[i].operands << endl;
		}
		cout << "-------------------------- END OF PASS 1 --------------------------\n" << endl;
	}
	void printPass2()
	{
		cout << endl << "----------------------------- PASS 2 ------------------------------\n" << endl;
		FILE *out = fopen("pass2.out","w");
		for(int i=0; i<instructionCount; i++)
		{
			fprintf(out,"%-10s%-10s%-10s%-10s%-10s\n", instructions[i].location.c_str(), instructions[i].label.c_str(), instructions[i].type.c_str(), instructions[i].operands.c_str(), instructions[i].objectCode.c_str());
			cout << instructions[i].location << "\t" << instructions[i].label << "\t" << instructions[i].type << "\t" << instructions[i].operands << "\t" <<instructions[i].objectCode << endl;
		}
		cout << "-------------------------- END OF PASS 2 --------------------------\n" << endl;
	}
	string getCurrentEndLocation(int currentInstruction)
	{
		int offset;
		for(offset=0; currentInstruction < instructionCount-1 && offset<10; offset++)
		{
			if(instructions[currentInstruction+offset].objectCode == "")break;
		}
		return instructions[currentInstruction + offset].location;
	}
	void getHTE()
	{
		FILE *out = fopen("hte.out","w");
		fprintf(out,"H%s00%s%06s\n",instructions[0].label.c_str(), instructions[0].location.c_str(), HEX::subtract(instructions.back().location, instructions[0].location).c_str());
		int currentInstruction = 0;
		while(currentInstruction < instructionCount)
		{
			if(instructions[currentInstruction].objectCode == "")
			{
				currentInstruction++;
				continue;
			}
			fprintf(out,"T00%s",instructions[currentInstruction].location.c_str());
			string currentEndLocation = getCurrentEndLocation(currentInstruction);
			fprintf(out,"%02s",HEX::subtract(currentEndLocation, instructions[currentInstruction].location).c_str());
			while(instructions[currentInstruction].location != currentEndLocation)
			{
				fprintf(out,"%s",instructions[currentInstruction].objectCode.c_str());
				currentInstruction++;
			}
			fprintf(out,"\n");
		}
		fprintf(out,"E%06s",instructions[0].location.c_str());
	}
};

int main()
{
	Assembler assembler(getInputCode());
	assembler.runPass1();
	assembler.runPass2();
	assembler.getHTE();
}
