#include<bits/stdc++.h>
using namespace std;
#include "../Shared/shared.cpp"
#include "memoryunits.cpp"
#include "modificationbox.cpp"
#include "objbox.cpp"
#include  "addressing.cpp"
string getFormat1ObjectCode(Instruction&);
string getFormat2ObjectCode(Instruction&);
string getFormat3ObjectCode(Instruction&,map<string,string>&, map< pair<char,string>,string >&, ModificationBox &);
string getFormat4ObjectCode(Instruction&,map<string,string>&, map< pair<char,string>,string >&, ModificationBox &);
string getObjectCode(Instruction &,map<string,string>&,map< pair<char,string>,string >&, ModificationBox&);
string BASE;
int main ()
{

}
pair<ObjBox , ModificationBox  > evalutateProgram(ProgramSection prog)
{
   ObjBox objBox ;
   ModificationBox modBox;
   for (int i=0;i<prog.instructions.size();i++){
      string objectCode = getObjectCode(prog.instructions[i],prog.symbolTable,prog.literalTable,modBox);
      if (objectCode.size()){
         objBox.addRecord( ObjRecord(prog.instructions[i].location, objectCode)  );
      }
   }
   return pair<ObjBox,ModificationBox  > (objBox,modBox);
}
vector<string> HTEGenerator (ProgramSection& prog, ObjBox& objBox, ModificationBox& modBox)
{
      vector<string> HTE;
      while (prog.startLocation.size()<6){
         prog.startLocation= "0" + prog.startLocation;
      }
      while (prog.endLocation.size()<6){
         prog.endLocation= "0" + prog.endLocation;
      }
      string progSize= HEX::getString( HEX::getInt(prog.endLocation) - HEX:: getInt( prog.startLocation ) );
      while (progSize.size()<6){
         progSize="0" + progSize;
      }
      string header= "H" + prog.programName +prog.startLocation +progSize;
      HTE.push_back(header);
      while (!objBox.isEmpty()){
         if (objBox.connectionState()==false||HTE.back().size()/2>27){
            HTE.push_back( string ());
         }
         HTE.back()+= objBox.fetch().data;
      }
      string start ="000000";
      for (int i=1;i<HTE.size();i++){
         string sz = HEX::getString ( HTE[i].size()/2  );
         while (sz.size()<6){
            sz="0"+sz;
         }
         HTE[i]= "T" + start + sz +HTE[i];
         start =HEX::add(start,sz);
         while (start.size()<6){
            start="0"+start;
         }
      }
      while (modBox.isEmpty()==false){
         ModificationRecord rec = modBox.fetch();
         while (rec.location.size()<6){
            rec.location= "0"+rec.location;
         }
         HTE.push_back(rec.location + rec.sz+ rec.modification  );
      }
      HTE.push_back("E");
      return HTE;
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
bool checkNumber (string str)
{
   for (int i=0;i<str.size();i++){
      if (!isdigit(str[i])){
         return false;
      }
   }
   return true;
}

string evaluteLiteral(Instruction &instruction , map< pair<char,string> , string  > &literalTable)
{
   char type;
   if (instruction.operandsValueType==Instruction::CHAR_DATA_OPERAND_VALUE){
      type='C';
   }
   else{
      type='H';
   }
   return literalTable[ pair<char,string>(type,instruction.operandsString )   ];
}
string myEvaluateOperand(Instruction & instruction, ModificationBox & modBox, map <string,string > & symbolTable)
{
   int equiValue =0;
   vector< pair<char,string> >terms = instruction.expressionTerms;
   for (int i=0;i<terms.size();i++){
         if (checkNumber(terms[i].second)){
            if (terms[i].first=='+'){
               equiValue+= HEX::getInt(terms[i].second);
            }
            else{
               equiValue-= HEX::getInt(terms[i].second);
            }
         }
         else if ( symbolTable.find( terms[i].second  )!=symbolTable.end() ){   // the current symbol is not external
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
   return HEX::getString(equiValue);

}
string getObjectCode(Instruction & instruction, map<string,string>& symbolTable,
                     map< pair<char,string>,string >& literalTable, ModificationBox& modBox)
{
   if (instruction.format==1){
      return getFormat1ObjectCode(instruction);
   }
   else if (instruction.format==2){
      return getFormat2ObjectCode(instruction);
   }
   else if (instruction.format==3){
      return getFormat3ObjectCode(instruction,symbolTable,literalTable,modBox);
   }
   else if (instruction.format==4){
      return getFormat4ObjectCode(instruction,symbolTable,literalTable,modBox);
   }
   if (instruction.originalCommand=="RESW"|| instruction.originalCommand=="RESB"){
      return "";
   }
   if (instruction.originalCommand=="BYTE"|| (instruction.label=="*"&& instruction.command =="")  ){   // byte of generated by LTORG
      string equi;
      if ( instruction.operandsValueType==Instruction::CHAR_DATA_OPERAND_VALUE ){
            for (int i=0;i<instruction.operandsString.size();i++){
               int v=instruction.operandsString[i];
               string h=HEX::getString(v);
               if (h.size()==1){
                  h="0"+h;
               }
               equi+=h;
            }
      }
      else if (instruction.operandsValueType==Instruction::HEX_DATA_OPERAND_VALUE){
         equi=instruction.operandsString;
      }
   }
   else if (instruction.originalCommand=="WORD"){
      string equi = HEX::getString ( DEC::getInt(instruction.operandsString));
      while(equi.size()<3){
         equi="0"+equi;
      }
      return equi;
   }
   return "";
}
string getFormat1ObjectCode(Instruction & instruction )
{
   return instruction.opCode;
}
string getFormat2ObjectCode(Instruction& instruction)
{
   string objectCode;
   objectCode+=instruction.opCode;
   objectCode += ( getRegisterNumber( instruction.sourceRegister) +'0');
   if (instruction.destinationRegister==""){
      return objectCode+"0";
   }
   objectCode+= (  getRegisterNumber( instruction.destinationRegister ) +'0' );
   return objectCode;
}

string getFormat3ObjectCode (Instruction& instruction,
                              map<string,string>& symbolTable ,map< pair<char,string>,string >& literalTable
                             ,ModificationBox & modBox)
{
   memoryUnitFormat3 unit;
   Addressing addressing ;
   unit.setOpCode(instruction.opCode);
   if (instruction.isLiteral){
      unit.setImmediate();
      unit.setIndirect();
      string value =evaluteLiteral(instruction,literalTable);
      addressing.setQuery(instruction,value,BASE  );
      string disp = addressing.fetchResult();
      if (addressing.getAddressingMode()=="pc"){
         unit.setPcRelative();
      }
      else if ( addressing.getAddressingMode()=="base" ){                //LDA   ='EOF'
         unit.setBaseRelative();
      }
      else{
         return "error";
      }
      unit.setDisp( HEX::getInt( disp ) );
      if (instruction.originalCommand=="LDB"){
         BASE=value;
      }
      return unit.fetch();
   }
   bool justNumbers =true;
   for (int i=0;i<instruction.expressionTerms.size();i++){
      for (int j=0;j<instruction.expressionTerms[i].second.size();j++){
         if (!isdigit( instruction.expressionTerms[i].second[j]  )){
            justNumbers=false;
         }
      }
   }
   if (instruction.isImmediate && justNumbers){
      unit.setOpCode(instruction.opCode);
      unit.setImmediate();
      string value=myEvaluateOperand(instruction,modBox,symbolTable);         // # constant
      unit.setDisp(HEX::getInt(value));

      if (instruction.originalCommand=="LDB"){
         BASE=value;
      }

      return unit.fetch();
   }
   if (instruction.isImmediate){
      unit.setImmediate();
   }
   else if (instruction.isIndirect){
      unit.setIndirect();
   }
   else{                                                                      // # mem   ,   @ mem    ,     mem   ,  mem(indexed)
      unit.setImmediate();
      unit.setIndirect();
   }
   if (instruction.isIndexed){
      unit.setIndexed();
   }
   string value =myEvaluateOperand(instruction,modBox,symbolTable);
   addressing.setQuery(instruction,value,BASE);
   string disp =addressing.fetchResult();
   if (addressing.getAddressingMode()=="pc"){
      unit.setPcRelative();
   }
   else if (addressing.getAddressingMode()=="base"){
      unit.setBaseRelative();
   }
   else{
      return "error";
   }
   unit.setDisp(HEX::getInt(value));

      if (instruction.originalCommand=="LDB"){
         BASE=value;
      }

   return unit.fetch();
}
string getFormat4ObjectCode(Instruction &instruction , map<string,string> & symbolTable,
                           map< pair<char,string> ,string  >& literalTable, ModificationBox& modBox)
{
   memoryUnitFormat4 unit;
   Addressing addressing ;
   unit.setOpCode(instruction.opCode);
   if (instruction.isLiteral){
      unit.setImmediate();
      unit.setIndirect();
      string value =evaluteLiteral(instruction,literalTable);        // operand is a literal
      unit.setAddress(HEX::getInt(value));
      if (instruction.originalCommand== "+LDB"){
         BASE=value;
      }
      return unit.fetch();
   }


   if (instruction.isImmediate){
      unit.setImmediate();
   }
   else if (instruction.isIndirect){
      unit.setIndirect();
   }
   else{
      unit.setImmediate();
      unit.setIndirect();
   }
   if (instruction.isIndexed){
      unit.setIndexed();
   }
   string value =myEvaluateOperand(instruction,modBox,symbolTable);
   if(instruction.originalCommand=="+LDB"){
      BASE= value;
   }
   unit.setAddress( HEX::getInt( value));
   return unit.fetch();
}
