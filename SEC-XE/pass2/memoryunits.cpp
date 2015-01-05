string bin [] ={"0000","0001","0010","0011","0100","0101","0110",
"0111","1000","1001","1010","1011","1100","1101","1110","1111" };
string getHex(string binary)
{
   string ret;
   for (int i=0;i<16;i++){
      if (binary==bin[i]){
         if (i<10){
            ret += (i+'0');
            return ret;
         }
         else{
            ret+=('A'+i-10);
            return ret;
         }
      }
   }
}
class memoryUnitFormat3{
   private :
   string status;
   public:
   memoryUnitFormat3()
   {
      status="000000000000000000000000"  ;
   }
   void setOpCode(string opCode)
   {
      int first,second ;
      if (opCode[0]>='A'){
         first=10 +opCode[0]-'A';
      }
      else{
         first=opCode[0]-'0';
      }
      if (opCode[1] >='A'){
         second =10 +opCode[1]-'A';
      }
      else{
         second =opCode[1]-'0';
      }
      for (int i=0;i<4;i++){
         status[i] =bin[first][i];
         status[i+4]=bin[second][i];
      }
   }
   void setImmediate()
   {
      status[7]='1';
   }
   void setIndirect()
   {
      status[6]='1';
   }
   void setIndexed()
   {
      status[8]='1';
   }
   void setPcRelative()
   {
      status[10]='1';
   }
   void setBaseRelative()
   {
      status[9]='1';
   }
   void setDisp(int disp)
   {
      string dispString =HEX::getString(disp);
      while (dispString.size()<3){
         dispString="0"+dispString;
      }
      for (int i=0;i<3;i++){
         int v;
         if (dispString[i]>='A'){
            v= dispString[i]-'A' +10;
         }
         else{
            v=dispString[i]-'0';
         }
         for (int j=0;j<4;j++){
            status[12+i*4+j]= bin[v][j];
         }
      }
   }
   string fetch()
   {
      string conc ;
      string hexVal;
      for (int i=0;i<=24;i++){
         if (i%4==0 && i!=0){
            hexVal+= getHex(conc);
            cout <<conc <<"b ";
            conc.clear();
         }
         conc+=status[i];
      }
      return hexVal;
   }
};
class memoryUnitFormat4
{
   private :
   string status;
   public:
   memoryUnitFormat4()
   {
      status="00000000000000000000000000000000";
   }
   void setOpCode(string opCode)
   {
      int first,second ;
      if (opCode[0]>='A'){
         first=10 +opCode[0]-'A';
      }
      else{
         first=opCode[0]-'0';
      }
      if (opCode[1] >='A'){
         second =10 +opCode[1]-'A';
      }
      else{
         second =opCode[1]-'0';
      }
      for (int i=0;i<4;i++){
         status[i] =bin[first][i];
         status[i+4]=bin[second][i];
      }
   }
   void setImmediate()
   {
      status[7]='1';
   }
   void setIndirect()
   {
      status[6]='1';
   }
   void setIndexed()
   {
      status[8]='1';
   }
   void setPcRelative()
   {
      status[10]='1';
   }
   void setBaseRelative()
   {
      status[9]='1';
   }

   void setAddress(int address)
   {
      string addressString = HEX::getString(address);
      while(addressString.size()<5){
         addressString="0" + addressString;
      }
      for (int i=0;i<5;i++){
         int v;
         if (addressString[i]>='A'){
            v= addressString[i]-'A' +10;
         }
         else{
            v=addressString[i]-'0';
         }
         for (int j=0;j<4;j++){
            status[12 +i*4+j ]  =bin[v][j];
         }
      }
   }
   string fetch()
   {
      string conc ;
      string hexVal;
      for (int i=0;i<=32;i++){
         if (i%4==0 && i!=0){
            hexVal+= getHex(conc);
            cout <<conc <<"b ";
            conc.clear();
         }
         conc+=status[i];
      }
      return hexVal;
   }
};
