#pragma once
#include "EuropeanOption.h"

class EuropeanOptionMC :public EuropeanOption {
public:
	EuropeanOptionMC(double _refprice, signed int _expiryd, const Payoff& ThePayoff_)
		:EuropeanOption(_refprice, _expiryd, ThePayoff_)
	{
	}

	double Calc(MarketParam& para, long numMc = 2);
	~EuropeanOptionMC();
private:
	//vector<signed int> obsDays;
};