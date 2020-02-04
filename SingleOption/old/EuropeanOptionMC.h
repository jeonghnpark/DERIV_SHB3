#pragma once
#include "EuropeanOption.h"

class EuropeanOptionMC :public EuropeanOption {
public:
	EuropeanOptionMC(double _refprice, signed int _expiryd, const Payoff& ThePayoff_)
		:EuropeanOption(_refprice, _expiryd, ThePayoff_)
	{
	}

	double Calc(MarketParam& para, long numMc = 10);
	double Calc(MarketParameters& paras, long numMc = 10);

	~EuropeanOptionMC();
private:
	//vector<signed int> obsDays;
};