#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

class TaxPayer{
  private:
    string name;
    
   public:
double taxableIncome;
    double tax;
    double setTaxableIncome(double s){
      return  taxableIncome = s;
    }
    double getTaxableIncome(){
      return taxableIncome;
    }
    double getTax(){
      return tax;
    }
};

class NonIndividual: public TaxPayer{
  NonIndividual(){
     }
};

class Individual: public TaxPayer{
  public:
 Individual(double startingIncome){
   taxableIncome = startingIncome;
       if (taxableIncome >800000.0) {
       tax = taxableIncome * 0.35;
    }else if (taxableIncome>=500001.0) {
      tax = taxableIncome * 0.325;
    }else if (taxableIncome >= 32334.0) {
      tax = taxableIncome * 0.3;
    }else if (taxableIncome>=24001.0) {
      tax = taxableIncome * 0.25;
    }else {
      tax = taxableIncome * 0.1;
    }

 }
};

int main(){
  Individual me(100000);
  cout << me.getTaxableIncome()<<endl;
  cout <<fixed<<setprecision(3)<< me.getTax();
  return 0;
}
