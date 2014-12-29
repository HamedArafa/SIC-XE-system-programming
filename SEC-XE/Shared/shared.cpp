struct InstructionSymbol{		// contains the format definition for any instruction from the SIC/XE instruction set
	string symbol;
	string opCode;
	int format;
	int numberOfOperands ;
	InstructionSymbol(string symbol,string opCode, int format , int numberOfOperands)
	{
		this->symbol=symbol;
		this->opCode=opCode;
		this->format=format;
		this->numberOfOperands=numberOfOperands;
	}
};
class InstructionSet{		// loads the instruction set and provide functions for fetching
									// opcode, format, number of operands using the symbol of the instruction
									// exmaple: instance.getFormat("ADD")  returns 3
									//				instance.getFormat("+ADD")	returns 4
									//	note : you have to call intialize to load the SIC/XE instruction set
	vector<InstructionSymbol>instructionSymbols;
	public:
	string getOpCode(string symbol)
	{
		for (int i=0;i<instructionSymbols.size();i++){
			if (symbol==instructionSymbols[i].symbol){
				return instructionSymbols[i].opCode;
			}
		}
		return "-1";
	}
	int getFormat(string symbol)
	{
		for (int i=0;i<instructionSymbols.size();i++){
			if (symbol==instructionSymbols[i].symbol){
				return instructionSymbols[i].format;
			}
		}
		return -1;
	}
	int getNumberOfOperands(string symbol)
	{
		for (int i=0;i<instructionSymbols.size();i++){
			if (symbol==instructionSymbols[i].symbol){
				return instructionSymbols[i].numberOfOperands;
			}
		}
		return -1;
	}
	void initialize()
	{
		char temp[20];
		string symbol,operands,format,opCode;
		int realOperands,realFormat;
		FILE * inp=fopen("../instructionSet.txt","r");
		int cnt =0;
		while( fscanf(inp,"%s",temp)!=EOF )			// read the instrction set from instructionSet.txt and load it
		{
			if (cnt%4==0){
				symbol=temp;
			}
			else if (cnt%4==1){
				operands=temp;
			}
			else if (cnt%4==2){
				format=temp;
			}
			else{
				opCode=temp;
			}
			if (cnt%4==3){
				if (format=="2"){
					realFormat=2;
				}
				else if (format=="1"){
					realFormat=1;
				}
				else {
					realFormat=3;
				}
				realOperands=1;
				if (operands=="0"){
					realOperands=0;
				}
				for (int i=0;i<operands.size();i++){
					if (operands[i]==','){
						realOperands++;
					}
				}
				instructionSymbols.push_back( InstructionSymbol(symbol,opCode,realFormat,realOperands)  );
				if (realFormat==3){
					instructionSymbols.push_back( InstructionSymbol("+"+symbol,opCode,4,realOperands)  );
				}
			}
			cnt++;
		}
	}
};
struct Instruction
{
	vector<string> tokens;
	string label, type, operands, location, objectCode;
	int format;
	bool indexed, extended, indirect, immediate;

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
