#ifndef EUROPEANOPTION_H
#define EUROPEANOPTION_H
#include "MarketParam.h"
#include "MarketParameters.h"
#include "Payoff.h"

class EuropeanOption{
public:
	EuropeanOption(double _refprice, signed int _expiryd,const Payoff& ThePayoff_);
	virtual ~EuropeanOption();
	double Calc(MarketParam& para);
	double Calc2(MarketParam& para);
	double Calc(MarketParameters& para);
	double Calc2(MarketParameters& para);
	void Simulation2(MarketParameters& paras, long nMC, bool db = false);

	signed int GetExpiryd() const;
	double GetRefPrice() const;
	std::vector<double> GetResult() const;
	
protected:
	double refprice;
	signed int expiry_date;
	Payoff* ThePayoffPtr;
	std::vector<double> result;
private:
	unsigned int getIndex(double target, double * px, int i_min, int i_max) const;
	mutable int init_i = 0;
};
#endif