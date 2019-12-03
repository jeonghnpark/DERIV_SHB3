
//2019.3.21 코드 작성중
//그리드 업데이트 방식을 일반화

#pragma once
#include "PayoffAutocallable.h"
#include "MarketParam.h"
#include <memory>

class AutocallOption2 {
public:
	AutocallOption2(double refprice_, signed int expiryd_, const PayoffAutocallable& ThePayoff_);
	virtual ~AutocallOption2();
	//double Calc(MarketParam& para);
	double Calc2(MarketParam& para);
	//double CalcMC(MarketParam& para, long nMC = 1000);
	double CalcMC2(MarketParam& para, long nMC = 1000);
	signed int GetExpiryd() const;
	double GetRefPrice() const;
	std::vector<double> GetResult() const;

protected:
	double refprice;
	signed int expiry_date;
	PayoffAutocallable* ThePayoffPtr;
	//std::unique_ptr<PayoffAutocallStd> ThePayoffPtr;
	std::vector<double> result;
	int hitflag;
};
