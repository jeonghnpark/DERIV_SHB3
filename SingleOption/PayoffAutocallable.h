//2019.3.21 작업중
//payoff class 일반화, 초기화 모듈화 진행

#pragma once
//#include "PayoffAutocall.h"
#include <vector>
#include "MarketParam.h"
#include "volatility.h"
class PayoffAutocallable {
public:
	PayoffAutocallable(int nb_autocall_, signed int* date_, double* strike_, double* coupon, double ki_barrier_, double put_strike_, double dummy_coupon, double refprice_, int maxi=300);
	virtual void updator(signed int td) const;
	virtual void final_updator() const;
	virtual void copy_v_to_u(double* v, double*  u, int mini, int maxi) const;
	virtual void copy_for_theta() const;

	virtual ~PayoffAutocallable();
	virtual PayoffAutocallable* clone() const;
	virtual void ResetFDGrid() const;
	int GetNbAutocall() const;
	std::vector<signed int> GetAutocall_date() const;
	double GetKiBarrier() const;
	std::vector<double> GetAutocall_strike() const;
	std::vector<double> GetAutocall_coupon() const;
	double GetPutStrike() const;
	double GetDummyCoupon() const;
	void init(double refprice, signed int vd, Vol vol);
	

	void backward(MarketParam& para, int tau);

protected:
	std::vector<signed int> autocall_date;
	std::vector<double> strike;

	double ki_barier;
	int kiindex;
	double put_strike;
	double refprice;
	std::vector<double> autocall_coupon;
	int nb_autocall;
	double dummy_coupon;
	int hitflag;

	int maxassetnodeindex;

	double *px;
	double *dpx;

	double *vold;
	double *vold_up;
	double *vold_down;

	double *uold;
	double *uold_up;
	double *uold_down;

	double *vold_next;  //reserve for theta
	double *uold_next;  //reserve for theta

	double *vnew;
	double *vnew_up;
	double *vnew_down;


	double *unew;
	double *unew_up;
	double *unew_down;

	int* idxS;
	int* idxT;

};