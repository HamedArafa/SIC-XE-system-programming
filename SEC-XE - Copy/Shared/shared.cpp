#include "Numbering.cpp"

struct Instruction
{
	/* OPERANDS VALUE TYPES */
	static const int FOLLOW_OPERAND_STYLE = 1, EXPRESSION_OPERAND_VALUE = 2, CHAR_DATA_OPERAND_VALUE = 3, HEX_DATA_OPERAND_VALUE = 4, NUMBER_OPERAND_VALUE = 5, ASTERISK_OPERAND_VALUE = 6, EMPTY_OPERAND_VALUE = 7;
	
	/* MNEMONIC TYPES */
	static const int D_OPERAND_STYLE = 0, M_OPERAND_STYLE = 1, R1R2_OPERAND_STYLE = 2, R1_OPERAND_STYLE = 3,  R1N_OPERAND_STYLE = 4, N_OPERAND_STYLE = 5, EMPTY_OPERAND_STYLE = 6;
	
	/* INSTRUCTION ORIGINAL DATA FORMAT */
	vector<string> tokens;
	
	/* INSTRUCTION INITIAL DATA */
	string originalLabel, originalCommand, originalOperandsString;
	string label, command, operandsString, opCode; // Example for corresponding values respectively: ALPHA, LDA, 105A0, 58
	int operandsStyle, format, operandsValueType; // Example for corresponding values respectively: the value of R1R2_OPERAND_STYLE which corresponds to the operandsStyle (r1,r2), 2 (i.e. format of the instruction is 2), one of the specified constants at the top which corresponds to the type of value in the operandsString
	bool isIndirect, isImmediate, isIndexed, isExtended, isLiteral; // isLiteral is triggered when there is '=' sign in the operands
	
	/* INSTRUCTION AUXILIARY DATA */
	string destinationRegister, sourceRegister; // The registers specified in the operands (if any).
	int N; // The n which might be accompanied with a register, for example in SHIFTR or come alone like in SVC
	string expressionString; // Expression (as a string) to be executed later to get the target address TA.
	vector<pair<char, string> > expressionTerms;
	
	/* INSTRUCTION TO-BE-PROSESSED DATA (to be processed in pass 1 and 2) */
	string location, objectCode;
	bool baseRelative, PCRelative;
	string expressionEquivalentValue; // The equivalent value of the expression in the operands, for example: LENGTH-INF-(ZERO-ALPHA). TO BE DONE
	
	/* INSTRUCTION CONSTRUCTORS */
	// Empty Constructor
	Instruction(){}
	// Initial Constructor (used by the parser)
	Instruction(vector<string> tokens, string label, string command, string operandsString, int operandsStyle, int format, string opCode, int operandsValueType, bool isIndirect, bool isImmediate, bool isIndexed, bool isExtended, bool isLiteral, string destinationRegister, string sourceRegister, int N, string expressionString, vector<pair<char, string> > expressionTerms)
	{ 
		this -> tokens = tokens;
		this -> originalLabel = tokens[0];
		this -> originalCommand = tokens[1];
		this -> originalOperandsString = tokens[2];
		
		this -> label = label;
		this -> command = command;
		this -> operandsString = operandsString;
		this -> operandsStyle = operandsStyle;
		this -> format = format;
		this -> opCode = opCode;
		this -> operandsValueType = operandsValueType;
		this -> isIndirect = isIndirect;
		this -> isImmediate = isImmediate;
		this -> isIndexed = isIndexed;
		this -> isExtended = isExtended;
		this -> isLiteral = isLiteral;
		this -> destinationRegister = destinationRegister;
		this -> sourceRegister = sourceRegister;
		this -> N = N;
		this -> expressionTerms = expressionTerms;
		this -> expressionString = expressionString;
	}
	// Specific constructor for literals definition after LTORG
	Instruction(vector<string> tokens, string label, string command, string operandsString, int operandsValueType)
	{ 
		this -> tokens = tokens;
		this -> originalLabel = tokens[0];
		this -> originalCommand = tokens[1];
		this -> originalOperandsString = tokens[2];
		
		this -> label = label;
		this -> command = command;
		this -> operandsString = operandsString;
		this -> operandsValueType = operandsValueType;
		
		operandsStyle = D_OPERAND_STYLE;
		opCode = "";
		format = -1;
		isIndirect = isImmediate = isIndexed = isExtended = false;
		isLiteral = true;
		N = 0;
	}
	void print()
	{
		printf("location:%-5s ", HEX :: leadZeros(location, 4).c_str());
		printf("label:%-7s command:%-7s opCode:%-3s operandsString:%-7s operandsValueType:%d operandsStyle:%d format:%d n:%d i:%d x:%d e:%d l:%d sR:%1s dR:%1s N:%d expressionString:%s\n", label.c_str(), command.c_str(), opCode.c_str(), operandsString.c_str(), operandsValueType, operandsStyle, format, isIndirect, isImmediate, isIndexed, isExtended, isLiteral, sourceRegister.c_str(), destinationRegister.c_str(), N, expressionString.c_str());
	}
};

struct ProgramSection
{
	string programName, startLocation;
	vector<Instruction> instructions;
	map<string, string> symbolTable;
	
	ProgramSection(string programName, string startLocation)
	{
		this -> programName = programName;
		this -> startLocation = startLocation;
		instructions.clear();
		symbolTable.clear();
	}
	ProgramSection(string programName, string startLocation, vector<Instruction> instructions, map<string, string> symbolTable)
	{
		this -> programName = programName;
		this -> startLocation = startLocation;
		this -> instructions = instructions;
		this -> symbolTable = symbolTable;
	}
};

class Literal
{
	public:
	string location, programSectionName, data;
	int programSectionIndex;
};

class InstructionSetElement
{
	public:
	string command, opCode, operandsStyle;
	int format;
	
	InstructionSetElement(){}
	InstructionSetElement(string command, string operandsStyle, int format, string opCode)
	{
		this -> command = command;
		this -> operandsStyle = operandsStyle;
		this -> format = format;
		this -> opCode = opCode;
	}
};

class InstructionSet
{	public:
	
	static vector<InstructionSetElement> instructionSet;
	
	static int getMnemonicId(string operandsStyle)
	{
		if(operandsStyle == "d") return Instruction :: D_OPERAND_STYLE;
		if(operandsStyle == "m") return Instruction :: M_OPERAND_STYLE;
		if(operandsStyle == "r1,r2") return Instruction :: R1R2_OPERAND_STYLE;
		if(operandsStyle == "r1") return Instruction :: R1_OPERAND_STYLE;
		if(operandsStyle == "r1,n") return Instruction :: R1N_OPERAND_STYLE;
		if(operandsStyle == "n") return Instruction :: N_OPERAND_STYLE;
		return Instruction :: EMPTY_OPERAND_STYLE;
	}
	static int getMnemonic(string command)
	{
		for (int i=0;i<instructionSet.size();i++){
			if (command == instructionSet[i].command){
				return getMnemonicId(instructionSet[i].operandsStyle);
			}
		}
		return Instruction :: EMPTY_OPERAND_STYLE;
	}
	static int getFormat(string command)
	{
		for (int i=0;i<instructionSet.size();i++){
			if (command == instructionSet[i].command){
				return instructionSet[i].format;
			}
		}
		return -1;
	}
	static string getOpCode(string command)
	{
		for (int i=0;i<instructionSet.size();i++){
			if (command == instructionSet[i].command){
				return instructionSet[i].opCode;
			}
		}
		return "-1";
	}
};
vector<InstructionSetElement> InstructionSet::instructionSet;

struct READER
{
	static vector<string> scanFileLines(string filePath)
	{
		freopen(filePath.c_str(), "r", stdin);
		
		vector<string> lines;
		char line[100001];
		
		while(gets(line))
			if(strlen(line))
				lines.push_back(line);
		
		return lines;
	}
};

struct PARSER
{
	/* PARSING HELPING FUNCTIONS */
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
	vector<string> capitalize(vector<string> lines)
	{
		for(int i=0; i<lines.size(); i++)
			for(int j=0; j<lines[i].size(); j++)
				if(isalpha(lines[i][j]))
					lines[i][j] = toupper(lines[i][j]);
		return lines;
	}
	bool isNumber(string s)
	{
		for(int i=0; i<s.size(); i++)
			if(!isdigit(s[i])) return 0;
		return 1;
	}
	bool isHexNumber(string s)
	{
		for(int i=0; i<s.size(); i++)
			if(!isdigit(s[i]) && (s[i]<'A' || s[i]>'Z')) return 0;
		return 1;
	}
	
	int operandsStyle, operandsValueType, N;
	bool isIndirect, isImmediate, isIndexed, isExtended, isLiteral;
	string sourceRegister, destinationRegister, expressionString;
	vector<pair<char, string> > expressionTerms;
	
	/* GETTERS */
	string getSourceRegister(string operands)
	{
		string ret = "";
		for(int i=0; i<operands.size(); i++)
		{
			if(operands[i] == ',')
				break;
			ret += operands[i];
		}
		return ret;
	}
	string getDestinationRegister(string operands)
	{
		int i;
		for(i=0; i<operands.size(); i++)
			if(operands[i] == ',')
				break;
		return operands.substr(i+1);
	}
	int getN(string operands)
	{
		string ret = "";
		for(int i=(int)operands.size()-1; i>=0; i--)
		{
			if(operands[i] == ',')
				break;
			ret += operands[i];
		}
		reverse(ret.begin(), ret.end());
		return DEC :: getInt(ret);
	}
	string getCommand(string original)
	{
		string reformed = original;
		if(original.size() && original[0] == '=') reformed = ""; // LITERAL: Case of "	*	=C'EOF'"
		if(original.size() && original[0] == '+') reformed = original.substr(1), isExtended = true;
		return reformed;
	}
	
	string getOperandsString(string original)
	{
		string reformed = original;
		
		if(original.size() && operandsStyle != Instruction :: M_OPERAND_STYLE && operandsStyle != Instruction :: D_OPERAND_STYLE)
		{
			if(operandsStyle == Instruction :: R1R2_OPERAND_STYLE) destinationRegister = getDestinationRegister(original), sourceRegister = getSourceRegister(original);
			if(operandsStyle == Instruction :: R1_OPERAND_STYLE) sourceRegister = getSourceRegister(original);
			if(operandsStyle == Instruction :: R1N_OPERAND_STYLE) destinationRegister = getDestinationRegister(original), N = getN(original);
			if(operandsStyle == Instruction :: N_OPERAND_STYLE) N = getN(original);
			
			operandsValueType = Instruction :: FOLLOW_OPERAND_STYLE;
			
			return reformed;
		}
		if(original.size() && original[0] == '=') original = reformed = original.substr(1), isLiteral = true;	
		if(original.size()>1 && original[(int)original.size()-1] == 'X' && original[(int)original.size()-2] == ',') original = reformed = original.substr(0, original.size()-2), isIndexed = true;	
		if(original.size() && original[0] == '#') original = reformed = original.substr(1), isImmediate = true;
		if(original.size() && original[0] == '@') original = reformed = original.substr(1), isIndirect = true;
		
		if(original.size() && original[0] == '*') operandsValueType = Instruction :: ASTERISK_OPERAND_VALUE;
		else if(!original.size()) operandsValueType = Instruction :: EMPTY_OPERAND_VALUE;
		else if(original.size()>1 && original[0] == 'C' && original[1] == '\'') original = reformed = original.substr(2, (int)original.size()-3), operandsValueType = Instruction :: CHAR_DATA_OPERAND_VALUE;
		else if(original.size()>1 && original[0] == 'X' && original[1] == '\'') original = reformed = original.substr(2, (int)original.size()-3), operandsValueType = Instruction :: HEX_DATA_OPERAND_VALUE;
		else if(isNumber(reformed)) operandsValueType = Instruction :: NUMBER_OPERAND_VALUE;
		else operandsValueType = Instruction :: EXPRESSION_OPERAND_VALUE, expressionString = reformed;
		
		return reformed;
	}
	
	vector<pair<char, string> > getExpressionTerms(string expressionString)
	{
		vector<pair<char, string> > expressionTerms;
		
		expressionTerms.push_back(make_pair('+', expressionString));
		
		return expressionTerms;
	}
	pair<pair<vector<ProgramSection>, map<string, int> >, string> parseProgramCode(vector<string> lines)
	{
		vector<ProgramSection> programSections;
		string entryPoint;
		
		map<string, int> programSectionIndex;
		lines = capitalize(lines);
		string currentProgramSectionName;
		string defaultProgramSectionName;
		string startLocation;
		
		for(int i=0; i<lines.size(); i++)
		{
			string label, command, operandsString, opCode;
			int format = -1;
			
			operandsStyle = Instruction :: EMPTY_OPERAND_STYLE, operandsValueType = Instruction :: EMPTY_OPERAND_VALUE, N = 0;
			isIndirect = isImmediate = isIndexed = isExtended = isLiteral = false;
			sourceRegister = "", destinationRegister = "", expressionString = "";
			expressionTerms.clear();
			
			vector<string> tokens = tokenize(lines[i]);
			
			label = tokens[0];
			command = getCommand(tokens[1]); // Triggers the isExtended flag.
			
			operandsStyle = InstructionSet :: getMnemonic(command);
			format = InstructionSet :: getFormat(command) + isExtended;
			opCode = InstructionSet :: getOpCode(command);
			
			if(tokens[1][0] == '=') operandsString = getOperandsString(tokens[1]);
			else operandsString = getOperandsString(tokens[2]); // Triggers the isIndexed, isIndirect, isImmediate and isLiteral flags.
			
			if(expressionString.size() && operandsValueType == Instruction :: EXPRESSION_OPERAND_VALUE) expressionTerms = getExpressionTerms(expressionString);
			
			//////////////
			
			startLocation = "0";
			
			if(command == "END") entryPoint = operandsString;
			else if(command == "START")
			{
				defaultProgramSectionName = currentProgramSectionName = label;
				startLocation = operandsString;
			}
			else if(command == "USE")
			{
				if(operandsString.size()) currentProgramSectionName = operandsString;
				else currentProgramSectionName = defaultProgramSectionName;
			}
			else if(command == "CSECT") currentProgramSectionName = label;
			
			if(!programSectionIndex.count(currentProgramSectionName))
			{
				programSectionIndex[currentProgramSectionName] = programSections.size();
				programSections.push_back(ProgramSection(currentProgramSectionName, startLocation));
			}
			
			if(command != "END" && command != "START" && command != "USE" && command != "CSECT")
				programSections[programSectionIndex[currentProgramSectionName]].instructions.push_back(Instruction(tokens, label, command, operandsString, operandsStyle, format, opCode, operandsValueType, isIndirect, isImmediate, isIndexed, isExtended, isLiteral, destinationRegister, sourceRegister, N, expressionString, expressionTerms));
		}
		
		return make_pair(make_pair(programSections, programSectionIndex), entryPoint);
	}
	void parseInstructionSet(vector<string> lines)
	{
		for(int i=0; i<lines.size(); i++)
		{
			string command, opCode, operandsStyleString;
			int format = -1;
			
			vector<string> tokens = tokenize(lines[i]);
			
			command = tokens[0];
			operandsStyleString = tokens[1];
			if(tokens[2].size()) format = tokens[2][0] - '0';
			opCode = tokens[3];
			
			InstructionSet :: instructionSet.push_back(InstructionSetElement(command, operandsStyleString, format, opCode));
		}
		return;
	}
};
int evaluateOperand(Instruction & instruction, map<string, string> & symbolTable)
{
   int equiValue = 0;
   vector< pair<char,string> >terms = instruction.expressionTerms;
   for (int i=0;i<terms.size();i++){
         if ( symbolTable.find( terms[i].second )!=symbolTable.end() ){   // the current symbol is not external
            if (terms[i].first=='+'){
               equiValue+= HEX::getInt(symbolTable[ terms[i].second] );
            }
            else{
               equiValue-= HEX::getInt(symbolTable[ terms[i].second ]);
            }
         }
   }
   return equiValue;
}

