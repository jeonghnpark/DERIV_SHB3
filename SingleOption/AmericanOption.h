#pragma once
#ifndef AMERICANOPTION_H
#define AMERICANOPTION_H
#include "MarketParam.h"
#include "Payoff.h"

class AmericanOption {
public:
	AmericanOption(double _refprice, signed int _expiryd, const Payoff& ThePayoff_);
	virtual ~AmericanOption();
	double Calc(MarketParam& para);
	signed int GetExpiryd() const;
	double GetRefPrice() const;
	std::vector<double> GetResult() const;

protected:
	double refprice;
	signed int expiry_date;
	Payoff* ThePayoffPtr;
	std::vector<double> result;

};
#endif