struct ModificationRecord {
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
   int pointer;
   public:
   ModificationBox()
   {
      pointer=0;
   }
   void addRecord(ModificationRecord modificationRecord)
   {
      modifications.push_back(modificationRecord);
   }
   ModificationRecord fetch()
   {
      return modifications[pointer];
      pointer++;
   }
   bool isEmpty ()
   {
      return pointer!=modifications.size();
   }
};
