class memoryUnitFormat3{
   private :
   int status;
   public:
   memoryUnitFormat3()
   {
      status=0;
   }
   void setOpCode(string opCode)
   {
      status|=HEX::getInt(opCode);
   }
   void setImmediate()
   {
      status|= (1<<7);
   }
   void setIndirect()
   {
      status|= (1<<6);
   }
   void setIndexed()
   {
      status|= (1<<8);
   }
   void setPcRelative()
   {
      status|= (1<<10);
   }
   void setBaseRelative()
   {
      status|= (1<<9);
   }
   void setDisp(int disp)
   {
      status |= disp<<12;
   }
   string fetch()
   {
      string hexValue= HEX::getString(status);
      while (hexValue.size()<6){
         hexValue+="0";
      }
      return hexValue;
   }
};
class memoryUnitFormat4
{
   private :
   int status;
   public:
   memoryUnitFormat4()
   {
      status= 1<<11;
   }
   void setOpCode(string opCode)
   {
      status|=HEX::getInt(opCode);
   }
   void setImmediate()
   {
      status|= (1<<7);
   }
   void setIndirect()
   {
      status|= (1<<6);
   }
   void setIndexed()
   {
      status|= (1<<8);
   }
   void setAddress(int address)
   {
      status|= (address<<12);
   }
   string fetch()
   {
      string hexValue= HEX::getString(status);
      while (hexValue.size()<8){
         hexValue+="0";
      }
      return hexValue;
   }
};
