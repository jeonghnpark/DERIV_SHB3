#ifndef EUROPEANCALL_H
#define EUROPEANCALL_H

#include "EuropeanOption.h"
#include "MarketParam.h"

#include <string>

class EuropeanCall: public EuropeanOption{
public:
	EuropeanCall(double _refprice, double _strike,signed int _expiryd)
		:EuropeanOption(_refprice,_expiryd),strike(_strike){}
	virtual double pv(MarketParam& para);

protected:
	double strike;
	double payoff_at_maturity(double spot){ return std::max(spot-strike,0.0); }
};

#endif