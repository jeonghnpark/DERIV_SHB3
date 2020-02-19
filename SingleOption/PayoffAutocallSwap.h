#pragma once
#include "PayoffAutocallStd.h"
class PayoffAutocallSwap :public PayoffAutocallStd {
public:
	PayoffAutocallSwap(int nb_autocall_, signed int* date_, double* strike_, double* coupon, double ki_barrier_, double put_strike_, double dummy_coupon, double refprice_)
		:PayoffAutocallStd(nb_autocall_, date_, strike_, coupon, ki_barrier_, put_strike_, dummy_coupon, refprice_)
	{}
	PayoffAutocallSwap(int nb_autocall_, vector<signed int> autocall_date_, vector<double> strike_, vector<double> coupon_, double ki_barrier_, double put_strike_, double dummy_coupon_, double refprice_, vector<signed int>& ffd_, vector<signed int>& fpd_)
		:PayoffAutocallStd(nb_autocall_, autocall_date_, strike_, coupon_, ki_barrier_, put_strike_, dummy_coupon_, refprice_), 
		floating_fixing_date(ffd_), floating_payment_date(fpd_)
	{
	}

	virtual ~PayoffAutocallSwap() {}
	virtual PayoffAutocallStd* clone() const;
	virtual void final_updator(double* vold, double* uold, double *px, int mini, int maxi) const override;
	virtual void updator(signed int td, double* vold, double* uold, double *px, int mini, int maxi) const;
	vector<signed int> GetFloatingFixingDate() const;
protected:
	vector<signed int> floating_fixing_date;
	vector<signed int> floating_payment_date;
	double floating_rate_spread;
};