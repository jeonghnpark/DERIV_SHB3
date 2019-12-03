#pragma once	
#include "EuropeanOption.h"

class AsianOption :public EuropeanOption {
public:
	AsianOption(double _refprice, signed int _expiryd, const Payoff& ThePayoff_, const vector<signed int> _od)
		:EuropeanOption(_refprice, _expiryd, ThePayoff_), obsDays(_od)
	{
	}

	double Calc(MarketParam& para, long numMc=10000);
private:
	vector<signed int> obsDays;
};