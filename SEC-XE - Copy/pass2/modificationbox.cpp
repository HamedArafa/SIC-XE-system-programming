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
   public:
   void addRecord(ModificationRecord modificationRecord)
   {
      modifications.push_back(modificationRecord);
   }
};
