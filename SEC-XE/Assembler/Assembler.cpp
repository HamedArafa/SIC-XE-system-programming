#include<bits/stdc++.h>
using namespace std;

#include "../Shared/shared.cpp"
#include "../Shared/Common.cpp"

struct Assembler
{
	string programName, startLocation;
	vector<Instruction> instructions;
	int instructionCount;
	
	Assembler()
	{
		PARSER parser;
		vector<string> lines = READER :: scanFileLines(ProgramCodeFilePath);
		this -> instructions = parser.parseProgramCode(lines);;
		this -> instructionCount = instructions.size();
		this -> startLocation = instructions[0].expressionEquivilantValue;
		this -> programName = instructions[0].label;
	}
	void getLocations()
	{
		string currentPC = startLocation;
		for(int i=1; i<instructionCount; i++)
		{
			instructions[i].location = currentPC;
			currentPC = HEX :: add(currentPC, HEX :: getString(instructions[i].format));
		}
	}
	void runPass1()
	{
		getLocations();
	}
};

int main()
{
	PARSER p;
	vector<string> lines = READER :: scanFileLines(InstructionSetFilePath);
	//InstructionSet :: instructionSet = p.parseInstructionSet(lines);
	
	Assembler assembler;
	assembler.runPass1();
}
