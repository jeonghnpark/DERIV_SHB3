#ifndef EUROPEANPUT_H
#define EUROPEANPUT_H

#include "EuropeanOption.h"
#include "MarketParam.h"

#include <string>

class EuropeanPut: public EuropeanOption{
public:
	EuropeanPut(double _refprice, double _strike,signed int _expiryd)
		:EuropeanOption(_refprice,_expiryd),strike(_strike){}
	virtual double pv(MarketParam& para);

protected:
	double strike;
	double payoff_at_maturity(double spot){ return std::max(strike-spot,0.0); }
};

#endif