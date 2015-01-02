#include<bits/stdc++.h>
using namespace std;

#include "../Shared/shared.cpp"
#include "../Shared/Common.cpp"

struct Assembler
{
	vector<InstructionSetElement> instructionSet;
	string programName, startLocation;
	vector<Instruction> instructions;
	int instructionCount;
	
	Assembler()
	{
		PARSER parser;
		
		vector<string> lines = READER :: scanFileLines(InstructionSetFilePath);
		this -> instructionSet = parser.parseInstructionSet(lines);
		lines = READER :: scanFileLines(ProgramCodeFilePath);
		this -> instructions = parser.parseProgramCode(lines);
		
		this -> instructionCount = instructions.size();
		
		if(instructionCount)
			this -> startLocation = instructions[0].expressionEquivilantValue,
			this -> programName = instructions[0].label;
	}
	string getToAdd(Instruction instruction)
	{
		if(instruction.format == -1)
		{
			if(instruction.command == "RESW")
				return HEX::getString(DEC::getInt(instruction.operandsString) * 3);
			if(instruction.command == "RESB")
				return HEX::getString(DEC::getInt(instruction.operandsString));
			if(instruction.command == "BYTE")
			{
				if(instruction.operandsType == Instruction :: CHAR_DATA_OPERAND)
					return HEX::getString(instruction.operandsString.size());
				
				if(instruction.operandsType == Instruction :: HEX_DATA_OPERAND)
					return HEX::getString(instruction.operandsString.size()/2 + instruction.operandsString.size()%2);
			}
			if(instruction.command == "WORD") return "3";
			return "0"; //////////////////////////////////// TO HANDLE THE RESERVED WORDS like USE we keda.. (TOBECHECKED)
		}
		return HEX :: getString(instruction.format);
	}
	void getLocations()
	{
		string currentPC = startLocation;
		for(int i=1; i<instructionCount; i++)
		{
			instructions[i].location = currentPC;
			currentPC = HEX :: add(currentPC, getToAdd(instructions[i]));
		}
	}
	string leadZeros(string s, int length)
	{
		string zeros;
		length = length - s.size();
		while(length-->0)
			zeros += "0";
		return zeros + s;
	}
	void printPass1()
	{
		for(int i=0; i<instructionCount; i++)
			printf("%10s %10s %10s %10s\n", leadZeros(instructions[i].location, 4).c_str(), instructions[i].label.c_str(), instructions[i].command.c_str(), instructions[i].tokens[2].c_str());
	}
	void runPass1()
	{
		getLocations();
		printPass1();
	}
};

int main()
{
	Assembler assembler;
	assembler.runPass1();
}
