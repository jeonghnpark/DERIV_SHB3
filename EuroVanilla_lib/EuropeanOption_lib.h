#pragma once
void __stdcall EuropeanOption_lib(int CP, double refprice, signed int expiryDate,
	signed int vd, double spot,
	double strike,
	double* Ivol, double* vol_term, double* vol_strike, int nb_vol_term, int nb_vol_strike,
	double* rfrate, double* rfrate_term, int nb_rfrate,
	double* divrate, double* divrate_term, int nb_divrate,
	double* result);

void __stdcall EuropeanOption_discrete_dividend_lib(int CP, double refprice, signed int expiryDate,
	signed int vd, double spot,
	double strike,
	double* Ivol, double* vol_term, double* vol_strike, int nb_vol_term, int nb_vol_strike,
	double* rfrate, double* rfrate_term, int nb_rfrate,
	double* div_amount, signed int* div_exdate, int nb_div_amount,
	double* result);

void __stdcall EuropeanOption_MC_lib(int CP, double refprice, signed int expiryDate,
	signed int vd, double spot,
	double strike,
	double* Ivol, double* vol_term, double* vol_strike, int nb_vol_term, int nb_vol_strike,
	double* rfrate, double* rfrate_term, int nb_rfrate,
	double* divrate, double* divrate_term, int nb_divrate,
	double* result);