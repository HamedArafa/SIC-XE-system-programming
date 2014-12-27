#include<bits/stdc++.h>
using namespace std;

#include "shared.cpp"

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
	
};

int main()
{
	Assembler assembler(getInputCode());
	
}
