
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
		this -> format = PARSER :: getFormat();
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
