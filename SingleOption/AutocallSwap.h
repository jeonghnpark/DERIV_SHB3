#pragma once
#include "AutocallOption.h"
#include "PayoffAutocallSwap.h"
class AutocallSwap :public AutocallOption {
public:
	virtual ~AutocallSwap();
	AutocallSwap(char* csvfile);
	virtual double Calc(MarketParameters& paras);
private:
	double Calc_floating_leg(MarketParam& paras) const;
	PayoffAutocallSwap* thePayoffPtr;
};