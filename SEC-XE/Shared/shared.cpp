#include "Numbering.cpp"

struct ProgramSection
{
	vector<InstructionSetElement> instructionSet;
	string programName, startLocation;
	vector<Instruction> instructions;
	int instructionCount;
	map<string, string> symbolTable;
	
	this -> instructionCount = instructions.size();
		
	if(instructionCount)
		this -> startLocation = instructions[0].operandsString,
		this -> programName = instructions[0].label;
};

struct Instruction
{
	/* OPERANDS VALUE TYPES */
	static const int FOLLOW_MNEMONIC = 1, EXPRESSION_OPERAND = 2, CHAR_DATA_OPERAND = 3, HEX_DATA_OPERAND = 4, NUMBER_OPERAND = 5, ASTERISK_OPERAND = 6, EMPTY_OPERAND = 7;
	
	/* MNEMONIC TYPES */
	static const int D_MNEMONIC = 0, M_MNEMONIC = 1, R1R2_MNEMONIC = 2, R1_MNEMONIC = 3,  R1N_MNEMONIC = 4, N_MNEMONIC = 5, EMPTY_MNEMONIC = 6;
	
	/* INSTRUCTION ORIGINAL DATA FORMAT */
	vector<string> tokens;
	
	/* INSTRUCTION INITIAL DATA */
	string originalLabel, originalCommand, originalOperandsString;
	string label, command, operandsString, opCode; // Example for corresponding values respectively: ALPHA, LDA, 105A0, 58
	int mnemonic, format, operandsType; // Example for corresponding values respectively: the value of R1R2_MNEMONIC which corresponds to the mnemonic (r1,r2), 2 (i.e. format of the instruction is 2), one of the specified constants at the top which corresponds to the type of value in the operandsString
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
	Instruction(vector<string> tokens, string label, string command, string operandsString, int mnemonic, int format, string opCode, int operandsType, bool isIndirect, bool isImmediate, bool isIndexed, bool isExtended, bool isLiteral, string destinationRegister, string sourceRegister, int N, string expressionString, vector<pair<char, string> > expressionTerms)
	{ 
		this -> tokens = tokens;
		this -> originalLabel = tokens[0];
		this -> originalCommand = tokens[1];
		this -> originalOperandsString = tokens[2];
		
		this -> label = label;
		this -> command = command;
		this -> operandsString = operandsString;
		this -> mnemonic = mnemonic;
		this -> format = format;
		this -> opCode = opCode;
		this -> operandsType = operandsType;
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
};

class InstructionSetElement
{
	public:
	string command, opCode, mnemonic;
	int format;
	
	InstructionSetElement(){}
	InstructionSetElement(string command, string mnemonic, int format, string opCode)
	{
		this -> command = command;
		this -> mnemonic = mnemonic;
		this -> format = format;
		this -> opCode = opCode;
	}
};

class InstructionSet
{	public:
	
	static vector<InstructionSetElement> instructionSet;
	
	
	static int getMnemonicId(string mnemonic)
	{
		if(mnemonic == "d") return Instruction :: D_MNEMONIC;
		if(mnemonic == "m") return Instruction :: M_MNEMONIC;
		if(mnemonic == "r1,r2") return Instruction :: R1R2_MNEMONIC;
		if(mnemonic == "r1") return Instruction :: R1_MNEMONIC;
		if(mnemonic == "r1,n") return Instruction :: R1N_MNEMONIC;
		if(mnemonic == "n") return Instruction :: N_MNEMONIC;
		return Instruction :: EMPTY_MNEMONIC;
	}
	static int getMnemonic(string command)
	{
		for (int i=0;i<instructionSet.size();i++){
			if (command == instructionSet[i].command){
				return getMnemonicId(instructionSet[i].mnemonic);
			}
		}
		return Instruction :: EMPTY_MNEMONIC;
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
			if(!isdigit(s[i]) && (s[i]<'A' || s[i]>'F')) return 0;
		return 1;
	}
	
	int mnemonic, operandsType, N;
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
		
		if(original.size() && mnemonic != Instruction :: M_MNEMONIC && mnemonic != Instruction :: D_MNEMONIC)
		{
			if(mnemonic == Instruction :: R1R2_MNEMONIC) destinationRegister = getDestinationRegister(original), sourceRegister = getSourceRegister(original);
			if(mnemonic == Instruction :: R1_MNEMONIC) destinationRegister = getDestinationRegister(original);
			if(mnemonic == Instruction :: R1N_MNEMONIC) destinationRegister = getDestinationRegister(original), N = getN(original);
			if(mnemonic == Instruction :: N_MNEMONIC) N = getN(original);
			
			operandsType = Instruction :: FOLLOW_MNEMONIC;
			
			return reformed;
		}
		if(original.size() && original[0] == '=') original = reformed = original.substr(1), isLiteral = true;	
		if(original.size()>1 && original[(int)original.size()-1] == 'X' && original[(int)original.size()-2] == ',') original = reformed = original.substr(0, original.size()-2), isIndexed = true;	
		if(original.size() && original[0] == '#') original = reformed = original.substr(1), isImmediate = true;
		if(original.size() && original[0] == '@') original = reformed = original.substr(1), isIndirect = true;
		
		if(original.size() && original[0] == '*') operandsType = Instruction :: ASTERISK_OPERAND;
		else if(!original.size()) operandsType = Instruction :: EMPTY_OPERAND;
		else if(original.size()>1 && original[0] == 'C' && original[1] == '\'') original = reformed = original.substr(2, (int)original.size()-3), operandsType = Instruction :: CHAR_DATA_OPERAND;
		else if(original.size()>1 && original[0] == 'X' && original[1] == '\'') original = reformed = original.substr(2, (int)original.size()-3), operandsType = Instruction :: HEX_DATA_OPERAND;
		else if(isNumber(reformed)) operandsType = Instruction :: NUMBER_OPERAND, expressionString = reformed;
		else operandsType = Instruction :: EXPRESSION_OPERAND, expressionString = reformed;
		
		return reformed;
	}
	
	vector<pair<char, string> > getExpressionTerms(string expressionString)
	{
		
	}
	vector<Instruction> parseProgramCode(vector<string> lines)
	{
		lines = capitalize(lines);
		vector<Instruction> instructions;
		
		for(int i=0; i<lines.size(); i++)
		{
			string label, command, operandsString, opCode;
			int format = -1;
			
			mnemonic = Instruction :: EMPTY_MNEMONIC, operandsType = Instruction :: EMPTY_OPERAND, N = 0;
			isIndirect = isImmediate = isIndexed = isExtended = isLiteral = false;
			sourceRegister = "", destinationRegister = "", expressionString = "";
			expressionTerms.clear();
			
			vector<string> tokens = tokenize(lines[i]);
			
			label = tokens[0];
			command = getCommand(tokens[1]); // Triggers the isExtended flag.
			mnemonic = InstructionSet :: getMnemonic(command);
			format = InstructionSet :: getFormat(command) + isExtended;
			opCode = InstructionSet :: getOpCode(command);
			
			if(tokens[1][0] == '=') operandsString = getOperandsString(tokens[1]);
			else operandsString = getOperandsString(tokens[2]); // Triggers the isIndexed, isIndirect, isImmediate and isLiteral flags.
			
			if(expressionString.size() && operandsType == Instruction :: EXPRESSION_OPERAND) expressionTerms = getExpressionTerms(expressionString);
			
			instructions.push_back(Instruction(tokens, label, command, operandsString, mnemonic, format, opCode, operandsType, isIndirect, isImmediate, isIndexed, isExtended, isLiteral, destinationRegister, sourceRegister, N, expressionString, expressionTerms));
		}
		return instructions;
	}
	vector<InstructionSetElement> parseInstructionSet(vector<string> lines)
	{
		for(int i=0; i<lines.size(); i++)
		{
			string command, opCode, mnemonicString;
			int format = -1;
			
			vector<string> tokens = tokenize(lines[i]);
			
			command = tokens[0];
			mnemonicString = tokens[1];
			if(tokens[2].size()) format = tokens[2][0] - '0';
			opCode = tokens[3];
			
			InstructionSet :: instructionSet.push_back(InstructionSetElement(command, mnemonicString, format, opCode));
		}
		return InstructionSet :: instructionSet;
	}
};


