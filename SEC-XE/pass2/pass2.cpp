#include<bits/stdc++.h>
using namespace std;
#include "../Shared/shared.cpp"
#include "memoryunits.cpp"
#include "modificationbox.cpp"
string getFormat1ObjectCode(Instruction&);
string getFormat2ObjectCode(Instruction&);
int main ()
{

}
string getFormat1ObjectCode(Instruction & instruction)
{
   string opCode = instruction.opCode;
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
string getObjectCode(Instruction & instruction)
{

}
string getFormat2ObjectCode(Instruction& instruction)
{
   string objectCode;
   objectCode+=instruction.opCode;
   objectCode += ( getRegisterNumber( instruction.sourceRegister) +'0');
   objectCode+= (  getRegisterNumber( instruction.destinationRegister ) +'0' );
   return objectCode;
}
string getFormat3ObjectCode (Instruction& instruction)
{



}
int evaluateOperand(Instruction & instruction, ModificationBox & modBox, map <string,string > & symbolTable)
{
   int equiValue =0;
   vector< pair<char,string> >terms = instruction.expressionTerms;
   for (int i=0;i<terms.size();i++){
         if ( symbolTable.find( terms[i].second  )!=symbolTable.end() ){   // the current symbol is not external
            if (terms[i].first=='+'){
               equiValue+= HEX::getInt(symbolTable[ terms[i].second] );
            }
            else{
               equiValue-= HEX::getInt(symbolTable[ terms[i].second ]);
            }
         }
         else{
            if (instruction.format==4){
               modBox.addRecord( ModificationRecord( HEX::add(instruction.location,"1"),"5",(""+terms[i].first)+terms[i].second));
            }
            else{
               modBox.addRecord( ModificationRecord(instruction.location,"6",(""+terms[i].first)+terms[i].second));
            }
         }
   }
}
