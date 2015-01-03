#include<bits/stdc++.h>
using namespace std;

#include "../Shared/shared.cpp"
#include "../Shared/Common.cpp"

struct Assembler
{
	vector<ProgramSection> programSections;
	vector<Literal> literalsTable;
	string entryPoint;
	map<string, int> programSectionIndex;
	
	Assembler()
	{
		PARSER parser;
		
		vector<string> lines = READER :: scanFileLines(InstructionSetFilePath);
		parser.parseInstructionSet(lines);
		
		lines = READER :: scanFileLines(ProgramCodeFilePath);
		pair<pair<vector<ProgramSection>, map<string, int> >, string> parsedProgramCode = parser.parseProgramCode(lines);
		this -> programSections = parsedProgramCode.first.first;
		this -> programSectionIndex = parsedProgramCode.first.second;
		this -> entryPoint = parsedProgramCode.second;
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
				if(instruction.operandsValueType == Instruction :: CHAR_DATA_OPERAND_VALUE)
					return HEX::getString(instruction.operandsString.size());
				
				if(instruction.operandsValueType == Instruction :: HEX_DATA_OPERAND_VALUE)
					return HEX::getString(instruction.operandsString.size()/2 + instruction.operandsString.size()%2);
			}
			if(instruction.command == "WORD") return "3";
			return "0"; //////////////////////////////////// TO HANDLE THE RESERVED WORDS like USE we keda.. (TOBECHECKED)
		}
		return HEX :: getString(instruction.format);
	}
	
	void addLiteralsDefinitions()
	{
		vector<pair<char, string> > undefinedLiterals;
		for(int p=0; p<programSections.size(); p++)
		{
			vector<Instruction> &instructions = programSections[p].instructions;
			vector<Instruction> newInstructions;
			for(int i=0; i<instructions.size(); i++)
			{
				if(instructions[i].isLiteral)
				{
					char type = 'X';
					if(instructions[i].operandsValueType == Instruction :: CHAR_DATA_OPERAND_VALUE)
						type = 'C';
					undefinedLiterals.push_back(make_pair(type, instructions[i].operandsString));
				}
				else if(instructions[i].command == "LTORG")
				{
					for(int j=0; j<undefinedLiterals.size(); j++)
						newInstructions.push_back(Instruction());
					undefinedLiterals.clear();
				}
				else newInstructions.push_back(instructions[i]);
			}
			instructions = newInstructions;
		}
	}
	void getSymbolTables()
	{
		for(int p=0; p<programSections.size(); p++)
		{
			vector<Instruction> &instructions = programSections[p].instructions;
			for(int i=0; i<instructions.size(); i++)
			{
				
			}
		}
	}
	void getLocations()
	{
		for(int p=0; p<programSections.size(); p++)
		{
			string currentPC = programSections[p].startLocation;
			vector<Instruction> &instructions = programSections[p].instructions;
			for(int i=0; i<instructions.size(); i++)
			{
				instructions[i].location = currentPC;
				currentPC = HEX :: add(currentPC, getToAdd(instructions[i]));
			}
		}
	}
	void getLiteralsTable()
	{
		
	}
	
	void printPass1()
	{
		for(int i=0; i<programSections.size(); i++)
		{
			printf("\nPROGRAM NUMBER #%d [%d instruction(s)]: programName:%s with startLocation:%s\n", i, programSections[i].instructions.size(), programSections[i].programName.c_str(), programSections[i].startLocation.c_str());
			for(int j=0; j<programSections[i].instructions.size(); j++)
			{
				printf("%2d]  ",j);
				programSections[i].instructions[j].print();
			}
		}
	}
	void runPass1()
	{
		addLiteralsDefinitions();
		getSymbolTables();
		getLocations();
		getLiteralsTable();
	}
};

int main()
{
	Assembler assembler;
	assembler.runPass1();
	assembler.printPass1();
	
}
