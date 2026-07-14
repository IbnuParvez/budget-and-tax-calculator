#include <iostream>
#include <string>
#include <iomanip> //this is for limiting the number of decimal numbers
#include <algorithm> // thid is for the min() function which selects the smaller of two numbers

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
    double remainingIncome = taxableIncome;

    double limits[] = {24000.0, 8333.33, 467666.67, 300000.0};

    double rates[]  = {0.10, 0.25, 0.30, 0.325};

    for (int i = 0; i < 4 && remainingIncome>0; i++) {
      double amountToTax = min(remainingIncome,limits[i]);
      tax+= amountToTax*rates[i];
      remainingIncome-=amountToTax;
    }
    if (remainingIncome >0) {
      tax+=remainingIncome*0.35;
    }
    tax-=2400.0;
    if (tax<0) {
      tax =0.;
    }
 }
};

int main(){
  Individual me(100000);
  cout << me.getTaxableIncome()<<endl;
  cout <<fixed<<setprecision(3)<< me.getTax();
  return 0;
}
