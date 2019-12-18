#pragma once
#include "PayoffAutocallStd.h"
#include "MarketParam.h"
#include "MarketParameters.h"
#include <memory>
class AutocallOption {
public:
	AutocallOption(double refprice_, signed int expiryd_, const PayoffAutocallStd& ThePayoff_, int hitflag=0);
	virtual ~AutocallOption();
	double Calc_old(MarketParam& para); //interpolation not optimized
	double Calc(MarketParam& para);//interpolation optimized
	double Calc(MarketParameters& paras);//class GRID under construction...
	double CalcMC(MarketParam& para, long nMC=1000); //r,div inerpolation not optimized
	double CalcMC_calc2(MarketParam & para, long nMC = 1000); //r,div optimized
	
	signed int GetExpiryd() const;
	double GetRefPrice() const;
	std::vector<double> GetResult() const;

protected:
	double refprice;
	signed int expiry_date;
	PayoffAutocallStd* ThePayoffPtr;
	std::vector<double> result;
	int hitflag;
};
