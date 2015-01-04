#include<bits/stdc++.h>
using namespace std;

#include "../Shared/shared.cpp"
#include "../Shared/Common.cpp"

/* +++++++++ NOTES +++++++++ */
// Empty command in an instruction means a literal definition


struct Assembler
{
	// MAIN DATA
	vector<ProgramSection> programSections;
	map<pair<char, string>, pair<string, string> > literalsTable; // Takes a pair for example ('C', 'EOF') and returns a pair (programSectionName, location)
	string entryPoint; // The entry point of the program which is specified beside the END
	
	// AUXILIARY DATA
	map<string, int> programSectionIndex; // Give it the program section name, returns its ID or index in the programSections vector.
	map<pair<char, string>, bool> definedLiterals; // Is a map of the defined Literals as pairs of char (which is X or C) and string which is the data between single quotes.
	
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
			if(instruction.command == "BYTE" || instruction.command == "")
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
				if((i == instructions.size() && p == 0) || instructions[i].command == "LTORG")
				{
					for(int j=0; j<undefinedLiterals.size(); j++)
					{
						vector<string> tokens;
						tokens.push_back("*"), tokens.push_back(""), tokens.push_back(undefinedLiterals[j].first + "'" + undefinedLiterals[j].second + "'");
						if(!definedLiterals.count(undefinedLiterals[j]))
						{
							definedLiterals[undefinedLiterals[j]] = 1;
							newInstructions.push_back(Instruction(tokens, "*", "", undefinedLiterals[j].second, undefinedLiterals[j].first == 'X' ? (Instruction :: HEX_DATA_OPERAND_VALUE):(Instruction :: CHAR_DATA_OPERAND_VALUE)));
						}
					}
					undefinedLiterals.clear();
				}
				else if(i < instructions.size() && instructions[i].isLiteral)
				{
					char type = 'X';
					if(instructions[i].operandsValueType == Instruction :: CHAR_DATA_OPERAND_VALUE)
						type = 'C';
					undefinedLiterals.push_back(make_pair(type, instructions[i].operandsString));
				}
				else if(i < instructions.size()) newInstructions.push_back(instructions[i]);
			}
			instructions = newInstructions;
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
	void getSymbolTables()
	{
		for(int p=0; p<programSections.size(); p++)
		{
			vector<Instruction> &instructions = programSections[p].instructions;
			for(int i=0; i<instructions.size(); i++)
			{
				if(instructions[i].label != "" && instructions[i].label != "*")
				{
					if(instructions[i].command == "EQU" && instructions[i].operandsString != "*") continue;
					programSections[p].symbolTable[instructions[i].label] = instructions[i].location;
				}
			}
		}
	}
	void getLiteralsTable()
	{
		for(int p=0; p<programSections.size(); p++)
		{
			vector<Instruction> &instructions = programSections[p].instructions;
			for(int i=0; i<instructions.size(); i++)
			{
				if(instructions[i].command == "") // This means this is a literal.
					literalsTable[make_pair(instructions[i].operandsValueType == Instruction::CHAR_DATA_OPERAND_VALUE ? 'C' : 'X', instructions[i].operandsString)] = make_pair(programSections[p].programName, instructions[i].location);
			}
		}
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
	void printSymbolTables()
	{
		for(int i=0; i<programSections.size(); i++)
		{
			printf("\nSymbol Table of Program #%d\n", i);
			for(map<string, string>::iterator it = programSections[i].symbolTable.begin(); it != programSections[i].symbolTable.end(); it++)
			{
				pair<string, string> symbol = *it;
				printf("%s -> %s\n",symbol.first.c_str(), symbol.second.c_str());
			}
		}
	}
	void printLiteralsTable()
	{
		printf("\nLiterals Table:\n");
		for(map<pair<char, string>, pair<string, string> >::iterator it = literalsTable.begin(); it != literalsTable.end(); it++)
		{
			pair<pair<char, string>, pair<string, string> > literal = *it;
			printf("%c'%s' -> %s:%s\n",literal.first.first, literal.first.second.c_str(), literal.second.first.c_str(), literal.second.second.c_str());
		}
	}
	void runPass1()
	{
		addLiteralsDefinitions();
		getLocations();
		getSymbolTables();
		getLiteralsTable();
	}
};

int main()
{
	Assembler assembler;
	assembler.runPass1();
	assembler.printPass1();
	assembler.printSymbolTables();
	assembler.printLiteralsTable();
}
