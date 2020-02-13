#include "EuropeanOption_lib.h"
#include "../SingleOption/PayoffCall.h"
#include "../SingleOption/PayoffPut.h"
#include "../SingleOption/EuropeanOption.h"  //including marketparam.h, marketparameter.h
//#include "../SingleOption/EuropeanOptionMC.h"
#include "../SingleOption/Rate.h"
#include "../SingleOption/volatility.h"
#include "../SingleOption/Dividend.h"


//void __stdcall plus3(double *arr, int row, int col)
//{
//

//	for (int i = 0; i < row; i++)
//		for (int j = 0; j < col; j++)
//			*(arr + i + j*row) += i + j;  //edit arr(i,j)+i+j  
//
//
//}

void __stdcall EuropeanOption_lib(int CP, double refprice, signed int expiryDate, 
	signed int vd, double spot,
	double strike, 
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

	//MarketParam para(vd, spot, volat, r, q);
	MarketParameters paras(vd, spot, volat, r, q);

	if (CP == -1) {
		//Plain Put when cp=-1
		EuropeanOption Eur(refprice, expiryDate, PayoffPut(strike));
		//Eur.Calc2(para);
		Eur.Calc(paras);
		std::vector<double> rs = Eur.GetResult();
		for (int i = 0; i < 30; i++)
			result[i] = rs[i];

	}
	else if (CP == 1) {
		//code here for plain call
		EuropeanOption Eur(refprice, expiryDate, PayoffCall(strike));
		Eur.Calc(paras);
		std::vector<double> rs = Eur.GetResult();
		for (int i = 0; i < 30; i++)
			result[i] = rs[i];
	}

	
}

void __stdcall EuropeanOption_MC_lib(int CP, double refprice, signed int expiryDate,
	signed int vd, double spot,
	double strike,
	double* Ivol, double* vol_term, double* vol_strike, int nb_vol_term, int nb_vol_strike,
	double* rfrate, double* rfrate_term, int nb_rfrate,
	double* divrate, double* divrate_term, int nb_divrate,
	double* result, signed int nM)
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

	//MarketParam para(vd, spot, volat, r, q);
	MarketParameters paras(vd, spot, volat, r, q);

	if (CP == -1) {
		//Plain Put when cp=-1
		EuropeanOption Eur(refprice, expiryDate, PayoffPut(strike));
		//Eur.Calc2(para);
		Eur.CalcMC(paras,nM);
		std::vector<double> rs = Eur.GetResult();
		for (int i = 0; i < 30; i++)
			result[i] = rs[i];

	}
	else if (CP == 1) {
		//code here for plain call
		EuropeanOption Eur(refprice, expiryDate, PayoffCall(strike));
		Eur.CalcMC(paras,nM);
		std::vector<double> rs = Eur.GetResult();
		for (int i = 0; i < 30; i++)
			result[i] = rs[i];
	}


}

void __stdcall EuropeanOption_MC_discrete_dividend_lib(int CP, double refprice, signed int expiryDate,
	signed int vd, double spot,
	double strike,
	double* Ivol, double* vol_term, double* vol_strike, int nb_vol_term, int nb_vol_strike,
	double* rfrate, double* rfrate_term, int nb_rfrate,
	double* divrate, double* divrate_term, int nb_divrate,
	double* div_amount, signed int* div_exdate, int nb_div_amount,
	double* result, signed int nM)
{
	std::vector<double> v_r;
	std::vector<double> v_rts;
	std::vector<double> v_q;
	std::vector<double> v_qts;
	std::vector<double> v_divamount;
	std::vector<signed int> v_divexdate;

	for (int i = 0; i < nb_rfrate; i++) {
		v_r.push_back(rfrate[i]);
		v_rts.push_back(rfrate_term[i]);
	}
	for (int i = 0; i < nb_divrate; i++) {
		v_q.push_back(divrate[i]);
		v_qts.push_back(divrate_term[i]);
	}
	for (int i = 0; i < nb_div_amount; i++) {
		v_divamount.push_back(div_amount[i]);
		v_divexdate.push_back(div_exdate[i]);
	}

	Rate r(v_r, v_rts);
	Rate q(v_q, v_qts);
	Dividend div(v_divamount, v_divexdate);

	Vol volat(nb_vol_term, nb_vol_strike);
	volat.set_vol_strike(vol_strike, nb_vol_strike);
	volat.set_vol_term(vol_term, nb_vol_term);

	volat.set_vol_by_point_vba2(Ivol, nb_vol_term, nb_vol_strike);

	//MarketParam para(vd, spot, volat, r, q);
	MarketParameters paras(vd, spot, volat, r, q,div);

	if (CP == -1) {
		//Plain Put when cp=-1
		EuropeanOption Eur(refprice, expiryDate, PayoffPut(strike));
		//Eur.Calc2(para);
		Eur.CalcMC_discrete(paras, nM);
		std::vector<double> rs = Eur.GetResult();
		for (int i = 0; i < 30; i++)
			result[i] = rs[i];

	}
	else if (CP == 1) {
		//code here for plain call
		EuropeanOption Eur(refprice, expiryDate, PayoffCall(strike));
		Eur.CalcMC_discrete(paras, nM);
		std::vector<double> rs = Eur.GetResult();
		for (int i = 0; i < 30; i++)
			result[i] = rs[i];
	}


}

void __stdcall EuropeanOption_discrete_dividend_lib(int CP, double refprice, signed int expiryDate,
	signed int vd, double spot,
	double strike,
	double* Ivol, double* vol_term, double* vol_strike, int nb_vol_term, int nb_vol_strike,
	double* rfrate, double* rfrate_term, int nb_rfrate,
	double* divrate, double* divrate_term, int nb_divrate,
	double* div_amount, signed int* div_exdate, int nb_div_amount,
	double* result)
{
	std::vector<double> v_r;
	std::vector<double> v_rts;
	std::vector<double> v_q;
	std::vector<double> v_qts;
	std::vector<signed int> v_divexdate;
	std::vector<double> v_divamount;

	for (int i = 0; i < nb_rfrate; i++) {
		v_r.push_back(rfrate[i]);
		v_rts.push_back(rfrate_term[i]);
	}

	for (int i = 0; i < nb_divrate; i++) {
		v_q.push_back(divrate[i]);
		v_qts.push_back(divrate_term[i]);
	}

	for (int i = 0; i < nb_div_amount; i++) {
		v_divexdate.push_back(div_exdate[i]);
		v_divamount.push_back(div_amount[i]);
	}

	Dividend div(v_divamount,v_divexdate);

	Rate r(v_r, v_rts);
	Rate q(v_q, v_qts);

	Vol volat(nb_vol_term, nb_vol_strike);
	volat.set_vol_strike(vol_strike, nb_vol_strike);
	volat.set_vol_term(vol_term, nb_vol_term);

	volat.set_vol_by_point_vba2(Ivol, nb_vol_term, nb_vol_strike);

	//MarketParam para(vd, spot, volat, r, q);
	MarketParameters paras(vd, spot, volat, r,q, div);

	if (CP == -1) {
		//Plain Put when cp=-1
		EuropeanOption Eur(refprice, expiryDate, PayoffPut(strike));
		Eur.Calc_discrete(paras);
		std::vector<double> rs = Eur.GetResult();
		for (int i = 0; i < 30; i++)
			result[i] = rs[i];

	}
	else if (CP == 1) {
		//code here for plain call
		EuropeanOption Eur(refprice, expiryDate, PayoffCall(strike));
		Eur.Calc_discrete(paras);
		std::vector<double> rs = Eur.GetResult();
		for (int i = 0; i < 30; i++)
			result[i] = rs[i];
	}


}
