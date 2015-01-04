class ObjRecord{
   private:
   string location ;
   string data;
   public:
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
   ObjRecord fetchRecord()
   {
      if (pointer==obj.size()){
         return  ObjRecord("0","0");
      }
      pointer++;
      return obj[pointer-1];
   }
};
