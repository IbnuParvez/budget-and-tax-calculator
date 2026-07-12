#include <iostream>
#include <string>

using namespace std;

class TaxPayer{
  private:
    string name;
    int salary;
  public:
    int setSalary(int s){
      return  salary = s;
    }
    int getSalary(){
      return salary;
    }
};

int main(){
  TaxPayer me;
  me.setSalary(100000);
  cout << me.getSalary();
  return 0;
}
