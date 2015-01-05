class ObjRecord{
   public:
   string location ;
   string data;
   ObjRecord(string location, string data)
   {
      this->location=location;
      this->data=data;
   }
};
class ObjBox
{
   private:
   vector<ObjRecord> obj;
   int pointer ;
   public:
   ObjBox(){pointer=0;}
   void addRecord(ObjRecord record)
   {
      obj.push_back( record );
   }
   ObjRecord fetch()
   {
      if (pointer==obj.size()){
         return  ObjRecord("0","0");
      }
      pointer++;
      return obj[pointer-1];
   }
   bool connectionState()
   {

      if (pointer==0){
         return false;
      }
      if (  HEX::add( obj[pointer-1].location, HEX::getString( obj[pointer-1].location.size()/2 ) )
       != obj[pointer].location){
         return false;
      }
      return true;
   }
   bool isEmpty()
   {
      return pointer!= obj.size();
   }
};
