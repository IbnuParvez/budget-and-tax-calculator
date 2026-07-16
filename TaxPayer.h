#pragma once

// Abstract base class. calculateTax() is pure virtual, meaning TaxPayer
// itself can never be created directly - only Individual or NonIndividual
// can. Calling getTax() through a TaxPayer* pointer still runs the correct
// derived version automatically. That's polymorphism actually doing work,
// not just existing as a keyword.
class TaxPayer {
protected:
    double taxableIncome;
    double tax;

public:
    TaxPayer();
    virtual ~TaxPayer() {}

    virtual void calculateTax() = 0;

    void setTaxableIncome(double income);
    double getTaxableIncome() const;
    double getTax() const;
};

class Individual : public TaxPayer {
public:
    explicit Individual(double startingIncome);
    void calculateTax() override;
};

class NonIndividual : public TaxPayer {
private:
    bool isLocalCompany;

public:
    NonIndividual(double totalIncome, bool local);
    void calculateTax() override;
};
