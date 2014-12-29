#include<bits/stdc++.h>
using namespace std;

#include "../Shared/shared.cpp"

struct Assembler
{
	string programName, startLocation;
	vector<Instruction> instructions;
	int instructionCount;
	
	Assembler(vector<Instruction> instructions)
	{
		vector<string> lines = READER :: scanProgram();
		PARSER :: run(lines);
		this -> instructions = PARSER :: getInstructions();
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
	Assembler assembler(getInputCode());
	assembler.runPass1();
}
