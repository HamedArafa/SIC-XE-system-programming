class ModificationRecord {
   public:
   string location;
   string sz;
   string modification;
   ModificationRecord(string location, string sz, string modification)
   {
      this->location=location;
      this->sz=sz;
      this->modification=modification;
   }
};
class ModificationBox{
   private:
   vector<ModificationRecord> modifications;
   int pointer=0;
   public:
   void addRecord(ModificationRecord modificationRecord)
   {
      modifications.push_back(modificationRecord);
   }
   ModificationRecord fetchRecord()
   {
      if (pointer==modifications.size()){
         return ModificationBox("0","0","0");
      }
      return modifications[pointer++];
   }
};
