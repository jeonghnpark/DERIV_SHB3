#pragma once
#include "PayoffAutocallStd.h"
class PayoffAutocallSwap :public PayoffAutocallStd {
public:
	PayoffAutocallSwap(int nb_autocall_, signed int* date_, double* strike_, double* coupon, double ki_barrier_, double put_strike_, double dummy_coupon, double refprice_)
		:PayoffAutocallStd(nb_autocall_, date_, strike_, coupon, ki_barrier_, put_strike_, dummy_coupon, refprice_)
	{}
	PayoffAutocallSwap(int nb_autocall_, vector<signed int> autocall_date_, vector<double> strike_, vector<double> coupon_, double ki_barrier_, double put_strike_, double dummy_coupon_, double refprice_)
		:PayoffAutocallStd(nb_autocall_, autocall_date_, strike_, coupon_, ki_barrier_, put_strike_, dummy_coupon_, refprice_)
	{}

	virtual ~PayoffAutocallSwap() {}
	virtual PayoffAutocallStd* clone() const;
	virtual void final_updator(double* vold, double* uold, double *px, int mini, int maxi) const override;
	virtual void updator(signed int td, double* vold, double* uold, double *px, int mini, int maxi) const;

};