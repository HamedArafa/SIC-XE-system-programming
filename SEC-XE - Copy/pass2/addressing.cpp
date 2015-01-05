class Addressing
{
   private :
   string  value, base;
   string mode;
   Instruction inst;
   public:
   Addressing (){}
   void setQuery (Instruction inst , string value, string base)
   {
      this->inst=inst;
      this->value=value;
      this->base=base;
      this->mode="";
   }
   string fetchResult ()
   {
      int pc= HEX::getInt( HEX::add (inst.location , "" +( inst.format +'0')  ));
      int target= HEX::getInt(value);
      int disp= target-pc;
      if (disp >=-2048 &&disp<=2047){
         return HEX::getString(disp);           // 2's complement
         mode="pc";
      }
      disp = target - HEX::getInt(base);
      if (disp>=0&&disp<=4095){
         return HEX::getString(disp) ;        // 2's complement
         mode="base";
      }
      mode="error";
      return "error";
   }
   string getAddressingMode(){
      if (mode==""){
         return "unset";
      }
      return mode;
   }
};
