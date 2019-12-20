#pragma once
//#include "PayoffAutocall.h"
#include <vector>
class PayoffAutocallStd  {
public:
	PayoffAutocallStd(int nb_autocall_, signed int* date_, double* strike_, double* coupon, double ki_barrier_, double put_strike_, double dummy_coupon, double refprice_);
	virtual void updator(signed int td, double* vold, double* uold, double *px, int mini, int maxi) const;
	virtual void final_updator(double* vold, double* uold, double *px, int mini, int maxi) const;
	virtual void copy_v_to_u(double* v, double*  u, int mini, int maxi) const;
	virtual ~PayoffAutocallStd() {}
	virtual PayoffAutocallStd* clone() const;
	virtual void ResetFDGrid(double* px, double* dpx, int minnode, int maxnode) const;
	int GetNbAutocall() const;
	std::vector<signed int> GetAutocall_date() const;	
	double GetKiBarrier() const;
	std::vector<double> GetAutocall_strike() const;
	std::vector<double> GetAutocall_coupon() const;
	double GetPutStrike() const;
	double GetDummyCoupon() const;
	double GetRefPrice() const;
	


protected:
	std::vector<signed int> autocall_date;
	std::vector<double> strike;
	
	double ki_barier;
	double put_strike;
	double refprice;
	std::vector<double> autocall_coupon;
	int nb_autocall;
	double dummy_coupon;

};