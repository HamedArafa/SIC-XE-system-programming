#include<bits/stdc++.h>
using namespace std;
#include "../Shared/shared.cpp"
#include "memoryunits.cpp"
#include "modificationbox.cpp"
string getFormat1ObjectCode(string);
string getFormat2ObjectCode(string,string);
int main ()
{
 //  string testcommand= "ADD";
 //  string testoperands="A,L";
 //  string testObjectCode= getFormat1ObjectCode(testcommand); //getFormat2ObjectCode(testcommand,testoperands);
//   cout<<testObjectCode;
}
string getFormat1ObjectCode(string command)
{
   string opCode = getOpCode(command);
   return opCode;
}
int getRegisterNumber(string reg)
{
   if (reg=="A"){
      return 0;
   }
   else if (reg=="X"){
      return 1;
   }
   else if (reg=="L"){
      return 2;
   }
   else if(reg=="PC"){
      return 8;
   }
   else if (reg=="SW"){
      return 9;
   }
   else if (reg=="B"){
      return 3;
   }
   else if (reg=="S"){
      return 4;
   }
   else if(reg=="T"){
      return 5;
   }
   else if (reg=="F"){
      return  6;
   }
}
string getObjectCode(Instruction instruction)
{

}
string getFormat2ObjectCode(Instruction instruction)
{
   string objectCode;
   objectCode+=instruction.opCode;
   objectCode += ( getRegisterNumber(instruction.sourceRegister) +'0');
   objectCode+= ( getRegisterNumber( instruction.distinationRegister ) +'0' );
   return objectCode;
}
string getFormat1ObjectCode(Instructions instruction)
{
   return instruction.opCode;
}
string getFormat3ObjectCode (Instruction instruction)
{



}
