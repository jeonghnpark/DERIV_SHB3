#include "SingleAutocallable_lib.h"
#include "../SingleOption/AutocallOption.h"
#include "../SingleOption/PayoffAutocallStd.h"
#include "../SingleOption/Rate.h"
#include "../SingleOption/volatility.h"



void __stdcall SingleAutocallable_MC_lib(double refprice, signed int expiryDate, int hitflag, int nb_autocall, signed int* auto_date,
	signed int vd, double spot,
	double* auto_strike, double* auto_coupon, double dummy_coupon, double kibarrier, double put_strike,
	double* Ivol, double* vol_term, double* vol_strike, int nb_vol_term, int nb_vol_strike,
	double* rfrate, double* rfrate_term, int nb_rfrate,
	double* divrate, double* divrate_term, int nb_divrate,
	long numMc, double* result)
{
	std::vector<double> v_r;
	std::vector<double> v_rts;
	std::vector<double> v_q;
	std::vector<double> v_qts;
	for (int i = 0; i < nb_rfrate; i++) {
		v_r.push_back(rfrate[i]);
		v_rts.push_back(rfrate_term[i]);
	}

	for (int i = 0; i < nb_divrate; i++) {
		v_q.push_back(divrate[i]);
		v_qts.push_back(divrate_term[i]);
	}

	Rate r(v_r, v_rts);
	Rate q(v_q, v_qts);

	Vol volat(nb_vol_term, nb_vol_strike);
	volat.set_vol_strike(vol_strike, nb_vol_strike);
	volat.set_vol_term(vol_term, nb_vol_term);

	volat.set_vol_by_point_vba2(Ivol, nb_vol_term, nb_vol_strike);

	MarketParam para(vd, spot, volat, r, q);

	PayoffAutocallStd autoPayoff(nb_autocall, auto_date, auto_strike, auto_coupon, kibarrier, put_strike, dummy_coupon, refprice);
	AutocallOption AutoKOSPI(refprice, expiryDate, autoPayoff, hitflag);
	AutoKOSPI.CalcMC2(para, numMc);

	std::vector<double> rs_auto = AutoKOSPI.GetResult();

	for (int i = 0; i < 30; i++)
		result[i] = rs_auto[i];

}


void __stdcall SingleAutocallable_lib(double refprice, signed int expiryDate, int hitflag, int nb_autocall, signed int* auto_date,
	signed int vd, double spot,
	double* auto_strike, double* auto_coupon, double dummy_coupon, double kibarrier, double put_strike,
	double* Ivol, double* vol_term, double* vol_strike, int nb_vol_term, int nb_vol_strike,
	double* rfrate, double* rfrate_term, int nb_rfrate,
	double* divrate, double* divrate_term, int nb_divrate,
	double* result)
{
	std::vector<double> v_r;
	std::vector<double> v_rts;
	std::vector<double> v_q;
	std::vector<double> v_qts;
	for (int i = 0; i < nb_rfrate; i++) {
		v_r.push_back(rfrate[i]);
		v_rts.push_back(rfrate_term[i]);
	}

	for (int i = 0; i < nb_divrate; i++) {
		v_q.push_back(divrate[i]);
		v_qts.push_back(divrate_term[i]);
	}

	Rate r(v_r, v_rts);
	Rate q(v_q, v_qts);

	Vol volat(nb_vol_term, nb_vol_strike);
	volat.set_vol_strike(vol_strike, nb_vol_strike);
	volat.set_vol_term(vol_term, nb_vol_term);

	volat.set_vol_by_point_vba2(Ivol, nb_vol_term, nb_vol_strike);

	MarketParam para(vd, spot, volat, r, q);

	PayoffAutocallStd autoPayoff(nb_autocall, auto_date, auto_strike, auto_coupon, kibarrier, put_strike, dummy_coupon, refprice);
	AutocallOption AutoKOSPI(refprice, expiryDate, autoPayoff, hitflag);
	AutoKOSPI.Calc(para);

	std::vector<double> rs_auto = AutoKOSPI.GetResult();

	for (int i = 0; i < 30; i++)
		result[i] = rs_auto[i];

}

void __stdcall SingleAutocallable_discrete_dividend_lib(double refprice, signed int expiryDate, int hitflag, int nb_autocall, signed int* auto_date,
	signed int vd, double spot,
	double* auto_strike, double* auto_coupon, double dummy_coupon, double kibarrier, double put_strike,
	double* Ivol, double* vol_term, double* vol_strike, int nb_vol_term, int nb_vol_strike,
	double* rfrate, double* rfrate_term, int nb_rfrate,
	double* div_amount, signed int* div_exdate, int nb_div_amount,
	double* result)
{
	std::vector<double> v_r;
	std::vector<double> v_rts;
	std::vector<double> v_q;
	std::vector<double> v_qts;
	for (int i = 0; i < nb_rfrate; i++) {
		v_r.push_back(rfrate[i]);
		v_rts.push_back(rfrate_term[i]);
	}

	std::vector<signed int> divexdate;
	std::vector<double> divamount;

	for (int i = 0; i < nb_div_amount; i++) {
		divexdate.push_back(div_exdate[i]);
		divamount.push_back(div_amount[i]);
	}

	Dividend div(divexdate, divamount);

	Rate r(v_r, v_rts);
	//Rate q(v_q, v_qts);

	Vol volat(nb_vol_term, nb_vol_strike);
	volat.set_vol_strike(vol_strike, nb_vol_strike);
	volat.set_vol_term(vol_term, nb_vol_term);

	volat.set_vol_by_point_vba2(Ivol, nb_vol_term, nb_vol_strike);

	MarketParameters paras(vd, spot, volat, r, div);

	PayoffAutocallStd autoPayoff(nb_autocall, auto_date, auto_strike, auto_coupon, kibarrier, put_strike, dummy_coupon, refprice);
	AutocallOption AutoKOSPI(refprice, expiryDate, autoPayoff, hitflag);
	AutoKOSPI.Calc(paras);

	std::vector<double> rs_auto = AutoKOSPI.GetResult();

	for (int i = 0; i < 30; i++)
		result[i] = rs_auto[i];

}