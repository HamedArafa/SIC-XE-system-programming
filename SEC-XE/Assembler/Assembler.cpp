#include<bits/stdc++.h>
using namespace std;

#include "../Shared/shared.cpp"
#include "../Shared/Common.cpp"

struct Assembler
{
	string programName, startLocation;
	vector<Instruction> instructions;
	int instructionCount;
	
	Assembler(vector<Instruction> instructions)
	{
		vector<string> lines = READER :: scanProgramCode(Common :: ProgramCodeFilePath);
		this -> instructions = PARSER :: parseProgramCode(lines);;
		this -> instructionCount = instructions.size();
		this -> startLocation = PARSER :: getStartLocation();
		this -> programName = PARSER :: getProgramName();
	}

	void runPass1()
	{
		startLocation = PARSER :: getStartLocation();
		PARSER :: getLocations();
		printPass1();
	}
};

int main()
{
	InstructionSet :: initialize();
	Assembler assembler(READER :: getInputCode());
	assembler.runPass1();
}
