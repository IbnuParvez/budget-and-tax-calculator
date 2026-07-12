#include <iostream>
#include <string>

using namespace std;

class TaxPayer{
  private:
    string name;
    int taxableIncome;
   public:
    int setIncome(int s){
      return  taxableIncome = s;
    }
    int getIncome(){
      return taxableIncome;
    }
};

class NonIndividual: public TaxPayer{

};

class Individual: public TaxPayer{
 
};

int main(){
  TaxPayer me;
  me.setIncome(100000);
  cout << me.getIncome();
  return 0;
}
