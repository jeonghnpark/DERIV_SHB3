#include <iostream>
#include <cstdlib>
#include <cassert>
#include <random>
#include <vector> 
#include <algorithm>
#include <chrono>
#include <numeric>

#include "volatility.h"
#include "MarketParam.h"
#include "MarketParameters.h"
#include "PayoffPut.h"
#include "PayoffCall.h"
#include "EuropeanOption.h"
#include "AmericanOption.h"
#include "k_OptionFormula.hpp"
#include "PayoffBarCUO.h"
#include "BarrierOption.h"
#include "AutocallOption.h"
#include "EuropeanOptionMC.h"
#include "AsianOption.h"
#include "Dividend.h"
#include "j_fd.h"

using namespace std;

enum FvsS{flat,surface};
enum CalcMode{calc1,calc2};


void test_american(double spot)
{

	double arr_rts[] = { 0.002739726,0.25,0.5,0.75,1,1.5,2,2.5,3,4,5,7,10,15,20 };
	double arr_r[] = { 1.54,1.64,1.72,1.77,1.8,1.85,1.88,1.90,1.92,1.92,1.92,1.92,1.92,1.92,1.92 };
	for (int i = 0; i<15; i++)
		arr_r[i] /= 100.0;

	double arr_qts[] = { 0.0191780821917808,0.038356164,0.057534247,0.084931507,0.104109589,0.123287671,0.142465753,0.167123288,0.252054795,0.334246575,0.419178082,0.504109589,0.580821918,0.665753425,0.747945205,0.832876712,0.915068493,1,1.084931507,1.167123288,1.252054795,1.334246575,1.419178082,1.504109589,1.583561644,1.668493151,1.750684932,1.835616438,1.917808219,	2.002739726	,2.087671233,2.169863014,2.254794521,2.336986301,2.421917808,2.506849315,2.583561644,2.668493151,2.750684932,2.835616438,2.917808219,3.002739726 };
	double arr_q[] = { 0.00000,0.00000,0.00000,2.57097, 2.09737,1.77112,1.53270, 1.30656 ,0.86631 ,5.16283, 4.11677,3.42318,3.34816,2.92103,2.60139,2.67302,2.43293,2.22630,2.26258, 2.10325,1.96057,3.00733,2.82806 ,2.66837, 2.53449,2.53675,2.41823 ,2.46233 ,2.35680 ,2.25685 ,2.27446 ,2.18831 ,2.10588 ,2.66917,2.57598,2.48871,2.41481,2.42319,2.35115 ,2.38170 ,2.31461 ,2.24914 };
	for (int i = 0; i<42; i++)
		arr_q[i] /= 100.0;

	//double spot=297.22;
	double vol_term[] = { 0.08493150,0.16712330,0.25205480,0.50410960,0.74794520,1.00000000,1.50410960,2.00273970,2.50684930,3.00273970 }; //10terms
	double vol_strike[] = { 178.332,193.193,208.054,222.915,237.776,252.637,267.498,282.359,297.22,312.081,326.942,341.803,356.664,371.525,386.386,401.247,416.108 };//17spot
	Vol volat(10, 17);
	double t0[] = { 0.3095,0.2907,0.2719,0.2528,0.2331,0.2136,0.1816,0.1388,0.1081,0.105,0.1296,0.1606,0.1795,0.1892,0.1978,0.2057,0.2129 };
	double t1[] = { 0.2807,0.2619,0.2431,0.2239,0.2042,0.1845,0.1586,0.1285,0.1095,0.1027,0.1076,0.1306,0.1518,0.1634,0.172,0.1798,0.1869 };
	double t2[] = { 0.2621,0.2448,0.2274,0.2098,0.1916,0.172,0.1505,0.128,0.1131,0.1076,0.1125,0.1264,0.1413,0.1517,0.16,0.1676,0.1745 };
	double t3[] = { 0.2376,0.2219,0.2061,0.1902,0.1741,0.1576,0.141,0.1261,0.1169,0.1161,0.1215,0.1292,0.1371,0.1447,0.1517,0.1583,0.1644 };
	double t4[] = { 0.2286,0.214,0.1997,0.1854,0.1712,0.1571,0.1437,0.1319,0.1238,0.1211,0.1231,0.128,0.1339,0.14,0.146,0.1517,0.1571 };
	double t5[] = { 0.221,0.2077,0.1946,0.1819,0.1694,0.1574,0.1462,0.1366,0.1295,0.1261,0.1261,0.1287,0.1327,0.1374,0.1422,0.1469,0.1516 };
	double t6[] = { 0.2063,0.1949,0.1841,0.1737,0.1639,0.1549,0.1469,0.1403,0.1355,0.1329,0.1322,0.1331,0.1352,0.1379,0.1409,0.1442,0.1475 };
	double t7[] = { 0.1993,0.1897,0.1806,0.1721,0.1642,0.157,0.1506,0.1453,0.1411,0.1383,0.1368,0.1364,0.1369,0.1381,0.1398,0.1418,0.144 };
	double t8[] = { 0.1927,0.1844,0.1768,0.1696,0.1631,0.1572,0.1521,0.1477,0.1443,0.1418,0.1402,0.1394,0.1393,0.1398,0.1407,0.1419,0.1434 };
	double t9[] = { 0.1892,0.182,0.1754,0.1692,0.1637,0.1586,0.1542,0.1505,0.1474,0.145,0.1432,0.1421,0.1415,0.1414,0.1417,0.1423,0.1431 };

	volat.set_vol_strike(vol_strike, 17);
	volat.set_vol_term(vol_term, 10);

	volat.set_volsurface_by_term(t0, 0, 17);
	volat.set_volsurface_by_term(t1, 1, 17);
	volat.set_volsurface_by_term(t2, 2, 17);
	volat.set_volsurface_by_term(t3, 3, 17);
	volat.set_volsurface_by_term(t4, 4, 17);
	volat.set_volsurface_by_term(t5, 5, 17);
	volat.set_volsurface_by_term(t6, 6, 17);
	volat.set_volsurface_by_term(t7, 7, 17);
	volat.set_volsurface_by_term(t8, 8, 17);
	volat.set_volsurface_by_term(t9, 9, 17);

	std::vector<double> v_rts(std::begin(arr_rts), std::end(arr_rts));
	std::vector<double> v_r(std::begin(arr_r), std::end(arr_r));
	std::vector<double> v_qts(std::begin(arr_qts), std::end(arr_qts));
	std::vector<double> v_q(std::begin(arr_q), std::end(arr_q));

	Rate r(v_r, v_rts);
	Rate q(v_q, v_qts);

	double refprice = 297.22;
	double rf_for_bs = r.getIntpRate(1.0);
	double div_for_bs = q.getIntpRate(1.0);
	double putstrike = 297.22*0.9;
	double callstrike = 297.22*1.1;
	double intpvol = volat.getInpVol(1.0, putstrike);

	MarketParam para(0, spot, volat, r, q);

	PayoffPut putpay(putstrike);
	//EuropeanOption EurPut(refprice, 365, putpay);
	//EurPut.Calc(para);
	AmericanOption AmPut(refprice, 365, putpay);
	AmPut.Calc(para);

	/*EuropeanVanilla Vput(putpay,refprice, ed);

	EuropeanPut eput(refprice,putstrike,365);
	EuropeanCall ecall(refprice,callstrike,365);

	*/
	std::cout.precision(8);
	std::cout << std::fixed;

	//std::cout <<"eput:  s=" <<spot<<" "<< eput.pv(para)<<" vol " << volat.getInpVol(365/365,putstrike)<< "r " << r.getIntpRate(1.0) << " q " << q.getIntpRate(1.0) <<  std::endl;
	//std::cout <<"ecall: "<< ecall.pv(para)<< "s= " <<spot<<" "<<" vol " <<volat.getInpVol(365/365,callstrike)<< " r " << r.getIntpRate(1.0) << " q " << q.getIntpRate(1.0) << std::endl;
	std::vector<double> rs = AmPut.GetResult();

	std::cout << "pv(AMERICAN) " << rs[0] << std::endl;
	std::cout << "delta " << rs[1] << std::endl;
	std::cout << "gamma " << rs[2] << std::endl;
	std::cout << "spot " << rs[5] << std::endl;
	std::cout << "BS vol=" << volat.getBSVol(365 / 365, putpay.GetStrike()) << std::endl;
	std::cout << "implied vol=" << put_iv(spot, putstrike, rf_for_bs, div_for_bs, 0.1, 1.0, rs[0]) << std::endl;
	//std::cout << "delta(closed, Flatvol)=" << k_fmla_BSPutDelta(spot, putstrike, rf_for_bs, div_for_bs, volat.getBSVol(1.0, putpay.GetStrike()), 1.0) << std::endl;
	//std::cout << "gamma(closed, Flatvol)=" << k_fmla_BSGamma(spot, putstrike, rf_for_bs, div_for_bs, volat.getBSVol(1.0, putpay.GetStrike()), 1.0) << std::endl;

	std::cout << "\n";

	//Vol volat_up(volat);
	//volat_up.Vol_up(0.01);
	//MarketParam para_volup(0, spot, volat_up, r, q);
	////EuropeanOption EurPut_vega(refprice, 365, putpay);
	//EurPut_vega.Calc(para_volup);

	//std::vector<double> rs_vega = AmPut_vega.GetResult();
	//std::cout << "pv(AMERICAN)(for vega)" << rs_vega[0] << std::endl;
	//std::cout << "delta(for vega) " << rs_vega[1] << std::endl;
	//std::cout << "gamma(for vega) " << rs_vega[2] << std::endl;
	//std::cout << "spot(for vega) " << rs_vega[5] << std::endl;
	//std::cout << "BS vol(for vega)=" << volat_up.getBSVol(365 / 365, putpay.GetStrike()) << std::endl;
	//std::cout << "implied vol(for vega)=" << put_iv(spot, putstrike, rf_for_bs, div_for_bs, 0.1, 1.0, rs_vega[0]) << std::endl;
	////std::cout << "delta(for vega)(closed, Flatvol)=" << k_fmla_BSPutDelta(spot, putstrike, rf_for_bs, div_for_bs, volat_up.getBSVol(1.0, putpay.GetStrike()), 1.0) << std::endl;
	////std::cout << "gamma(for vega)(closed, Flatvol)=" << k_fmla_BSGamma(spot, putstrike, rf_for_bs, div_for_bs, volat_up.getBSVol(1.0, putpay.GetStrike()), 1.0) << std::endl;
	////std::cout << "\n";

	////std::cout << "vega(closed, Flatvol)=" << k_fmla_BSVega(spot, putstrike, rf_for_bs, div_for_bs, volat.getBSVol(1.0, putpay.GetStrike()), 1.0) / 100.0 << std::endl;
	////std::cout << "vega(for vega) " << rs_vega[0] - rs[0] << std::endl;


}
void test_europeanMC(double spot)
{
	
}
void test_european(double spot, long numMC)
{
	
	double arr_rts[]={0.002739726,0.25,0.5,0.75,1,1.5,2,2.5,3,4,5,7,10,15,20};
	double arr_r[]={1.54,1.64,1.72,1.77,1.8,1.85,1.88,1.90,1.92,1.92,1.92,1.92,1.92,1.92,1.92};
	for(int i=0;i<15;i++)
		arr_r[i]/=100.0;

	double arr_qts[]={0.0191780821917808,0.038356164,0.057534247,0.084931507,0.104109589,0.123287671,0.142465753,0.167123288,0.252054795,0.334246575,0.419178082,0.504109589,0.580821918,0.665753425,0.747945205,0.832876712,0.915068493,1,1.084931507,1.167123288,1.252054795,1.334246575,1.419178082,1.504109589,1.583561644,1.668493151,1.750684932,1.835616438,1.917808219,	2.002739726	,2.087671233,2.169863014,2.254794521,2.336986301,2.421917808,2.506849315,2.583561644,2.668493151,2.750684932,2.835616438,2.917808219,3.002739726};
	double arr_q[]={0.00000,0.00000,0.00000,2.57097, 2.09737,1.77112,1.53270, 1.30656 ,0.86631 ,5.16283, 4.11677,3.42318,3.34816,2.92103,2.60139,2.67302,2.43293,2.22630,2.26258, 2.10325,1.96057,3.00733,2.82806 ,2.66837, 2.53449,2.53675,2.41823 ,2.46233 ,2.35680 ,2.25685 ,2.27446 ,2.18831 ,2.10588 ,2.66917,2.57598,2.48871,2.41481,2.42319,2.35115 ,2.38170 ,2.31461 ,2.24914 };
	for(int i=0;i<42;i++)
		arr_q[i]/=100.0;

	//double spot=297.22;
	double vol_term[]={0.08493150,0.16712330,0.25205480,0.50410960,0.74794520,1.00000000,1.50410960,2.00273970,2.50684930,3.00273970}; //10terms
	double vol_strike[]={178.332,193.193,208.054,222.915,237.776,252.637,267.498,282.359,297.22,312.081,326.942,341.803,356.664,371.525,386.386,401.247,416.108};//17spot
	Vol volat(10,17);
	double t0[]={0.3095,0.2907,0.2719,0.2528,0.2331,0.2136,0.1816,0.1388,0.1081,0.105,0.1296,0.1606,0.1795,0.1892,0.1978,0.2057,0.2129};
	double t1[]={0.2807,0.2619,0.2431,0.2239,0.2042,0.1845,0.1586,0.1285,0.1095,0.1027,0.1076,0.1306,0.1518,0.1634,0.172,0.1798,0.1869};
	double t2[]={0.2621,0.2448,0.2274,0.2098,0.1916,0.172,0.1505,0.128,0.1131,0.1076,0.1125,0.1264,0.1413,0.1517,0.16,0.1676,0.1745};
	double t3[]={0.2376,0.2219,0.2061,0.1902,0.1741,0.1576,0.141,0.1261,0.1169,0.1161,0.1215,0.1292,0.1371,0.1447,0.1517,0.1583,0.1644};
	double t4[]={0.2286,0.214,0.1997,0.1854,0.1712,0.1571,0.1437,0.1319,0.1238,0.1211,0.1231,0.128,0.1339,0.14,0.146,0.1517,0.1571};
	double t5[]={0.221,0.2077,0.1946,0.1819,0.1694,0.1574,0.1462,0.1366,0.1295,0.1261,0.1261,0.1287,0.1327,0.1374,0.1422,0.1469,0.1516};
	double t6[]={0.2063,0.1949,0.1841,0.1737,0.1639,0.1549,0.1469,0.1403,0.1355,0.1329,0.1322,0.1331,0.1352,0.1379,0.1409,0.1442,0.1475};
	double t7[]={0.1993,0.1897,0.1806,0.1721,0.1642,0.157,0.1506,0.1453,0.1411,0.1383,0.1368,0.1364,0.1369,0.1381,0.1398,0.1418,0.144};
	double t8[]={0.1927,0.1844,0.1768,0.1696,0.1631,0.1572,0.1521,0.1477,0.1443,0.1418,0.1402,0.1394,0.1393,0.1398,0.1407,0.1419,0.1434};
	double t9[]={0.1892,0.182,0.1754,0.1692,0.1637,0.1586,0.1542,0.1505,0.1474,0.145,0.1432,0.1421,0.1415,0.1414,0.1417,0.1423,0.1431};

	volat.set_vol_strike(vol_strike, 17);
	volat.set_vol_term(vol_term,10);
	
	volat.set_volsurface_by_term(t0,0,17);
	volat.set_volsurface_by_term(t1,1,17);
	volat.set_volsurface_by_term(t2,2,17);
	volat.set_volsurface_by_term(t3,3,17);
	volat.set_volsurface_by_term(t4,4,17);
	volat.set_volsurface_by_term(t5,5,17);
	volat.set_volsurface_by_term(t6,6,17);
	volat.set_volsurface_by_term(t7,7,17);
	volat.set_volsurface_by_term(t8,8,17);
	volat.set_volsurface_by_term(t9,9,17);

	std::vector<double> v_rts(std::begin(arr_rts), std::end(arr_rts));
	std::vector<double> v_r(std::begin(arr_r), std::end(arr_r));
	std::vector<double> v_qts(std::begin(arr_qts), std::end(arr_qts));
	std::vector<double> v_q(std::begin(arr_q), std::end(arr_q));

	Rate r(v_r,v_rts);
	Rate q(v_q,v_qts);
	
	double refprice=297.22;
	double rf_for_bs=r.getIntpRate(1.0);
	double div_for_bs=q.getIntpRate(1.0);
	double putstrike=297.22*0.9;
	double callstrike=297.22*1.1;
	double intpvol=volat.getInpVol(1.0,putstrike);
	
	MarketParam para(43340,spot,volat,r,q);

	PayoffPut putpay(putstrike);
	EuropeanOption EurPut(refprice,43705,putpay);
	EuropeanOptionMC EurPutMC(refprice, 43705, putpay);

	EurPut.Calc(para);
	EurPutMC.Calc(para,numMC);
	/*EuropeanVanilla Vput(putpay,refprice, ed);

	EuropeanPut eput(refprice,putstrike,365);
	EuropeanCall ecall(refprice,callstrike,365);
	
	*/
	std::cout.precision(8);
	std::cout <<std::fixed;

	//std::cout <<"eput:  s=" <<spot<<" "<< eput.pv(para)<<" vol " << volat.getInpVol(365/365,putstrike)<< "r " << r.getIntpRate(1.0) << " q " << q.getIntpRate(1.0) <<  std::endl;
	//std::cout <<"ecall: "<< ecall.pv(para)<< "s= " <<spot<<" "<<" vol " <<volat.getInpVol(365/365,callstrike)<< " r " << r.getIntpRate(1.0) << " q " << q.getIntpRate(1.0) << std::endl;
	std::vector<double> rs=EurPut.GetResult();
	
	std::cout<< "pv "<<rs[0]<<std::endl;
	std::cout<< "delta "<<rs[1]<<std::endl;
	std::cout<< "gamma "<<rs[2]<<std::endl;
	std::cout << "spot " <<rs[5]<<std::endl;
	std::cout<< "BS vol="<<volat.getBSVol(365/365, putpay.GetStrike())<<std::endl;
	std::cout << "implied vol="<<put_iv(spot,putstrike,rf_for_bs, div_for_bs,0.1,(43705-43340)/365,rs[0])<<std::endl;
	std::cout << "delta(closed, Flatvol)="<<k_fmla_BSPutDelta(spot,putstrike,rf_for_bs, div_for_bs,volat.getBSVol(1.0, putpay.GetStrike()),1.0)<<std::endl;
	std::cout << "gamma(closed, Flatvol)="<<k_fmla_BSGamma(spot,putstrike,rf_for_bs, div_for_bs,volat.getBSVol(1.0, putpay.GetStrike()),1.0)<<std::endl;
	std::cout<<"\n";

	std::vector<double> rsMc = EurPutMC.GetResult();

	std::cout << "pv MC " << rsMc[0] << std::endl;
	std::cout << "delta MC " << rsMc[1] << std::endl;
	std::cout << "gamma MC" << rsMc[2] << std::endl;
	std::cout << "spot " << rsMc[5] << std::endl;
	std::cout << "BS vol=" << volat.getBSVol(365 / 365, putpay.GetStrike()) << std::endl;
	std::cout << "implied vol=" << put_iv(spot, putstrike, rf_for_bs, div_for_bs, 0.1, 1.0, rs[0]) << std::endl;
	std::cout << "delta(closed, Flatvol)=" << k_fmla_BSPutDelta(spot, putstrike, rf_for_bs, div_for_bs, volat.getBSVol(1.0, putpay.GetStrike()), 1.0) << std::endl;
	std::cout << "gamma(closed, Flatvol)=" << k_fmla_BSGamma(spot, putstrike, rf_for_bs, div_for_bs, volat.getBSVol(1.0, putpay.GetStrike()), 1.0) << std::endl;
	std::cout << "\n";

	Vol volat_up(volat);
	volat_up.Vol_up(0.01);
	MarketParam para_volup(0,spot,volat_up,r,q);
	EuropeanOption EurPut_vega(refprice,365,putpay);
	EurPut_vega.Calc(para_volup);

	std::vector<double> rs_vega=EurPut_vega.GetResult();
	std::cout<< "pv(for vega)"<<rs_vega[0]<<std::endl;
	std::cout<< "delta(for vega) "<<rs_vega[1]<<std::endl;
	std::cout<< "gamma(for vega) "<<rs_vega[2]<<std::endl;
	std::cout << "spot(for vega) " <<rs_vega[5]<<std::endl;
	std::cout<< "BS vol(for vega)="<<volat_up.getBSVol(365/365, putpay.GetStrike())<<std::endl;
	std::cout << "implied vol(for vega)="<<put_iv(spot,putstrike,rf_for_bs, div_for_bs,0.1,1.0,rs_vega[0])<<std::endl;
	std::cout << "delta(for vega)(closed, Flatvol)="<<k_fmla_BSPutDelta(spot,putstrike,rf_for_bs, div_for_bs,volat_up.getBSVol(1.0, putpay.GetStrike()),1.0)<<std::endl;
	std::cout << "gamma(for vega)(closed, Flatvol)="<<k_fmla_BSGamma(spot,putstrike,rf_for_bs, div_for_bs,volat_up.getBSVol(1.0, putpay.GetStrike()),1.0)<<std::endl;
	std::cout<<"\n";

	std::cout << "vega(closed, Flatvol)="<<k_fmla_BSVega(spot,putstrike,rf_for_bs, div_for_bs,volat.getBSVol(1.0, putpay.GetStrike()),1.0)/100.0<<std::endl;
	std::cout<< "vega(for vega) "<<rs_vega[0]-rs[0]<<std::endl;
	
	
}



void test_asian(double spot, long numMC)
{

	double arr_rts[] = { 0.002739726,0.25,0.5,0.75,1,1.5,2,2.5,3,4,5,7,10,15,20 };
	double arr_r[] = { 1.54,1.64,1.72,1.77,1.8,1.85,1.88,1.90,1.92,1.92,1.92,1.92,1.92,1.92,1.92 };
	for (int i = 0; i<15; i++)
		arr_r[i] /= 100.0;

	double arr_qts[] = { 0.0191780821917808,0.038356164,0.057534247,0.084931507,0.104109589,0.123287671,0.142465753,0.167123288,0.252054795,0.334246575,0.419178082,0.504109589,0.580821918,0.665753425,0.747945205,0.832876712,0.915068493,1,1.084931507,1.167123288,1.252054795,1.334246575,1.419178082,1.504109589,1.583561644,1.668493151,1.750684932,1.835616438,1.917808219,	2.002739726	,2.087671233,2.169863014,2.254794521,2.336986301,2.421917808,2.506849315,2.583561644,2.668493151,2.750684932,2.835616438,2.917808219,3.002739726 };
	double arr_q[] = { 0.00000,0.00000,0.00000,2.57097, 2.09737,1.77112,1.53270, 1.30656 ,0.86631 ,5.16283, 4.11677,3.42318,3.34816,2.92103,2.60139,2.67302,2.43293,2.22630,2.26258, 2.10325,1.96057,3.00733,2.82806 ,2.66837, 2.53449,2.53675,2.41823 ,2.46233 ,2.35680 ,2.25685 ,2.27446 ,2.18831 ,2.10588 ,2.66917,2.57598,2.48871,2.41481,2.42319,2.35115 ,2.38170 ,2.31461 ,2.24914 };
	for (int i = 0; i<42; i++)
		arr_q[i] /= 100.0;

	//double spot=297.22;
	double vol_term[] = { 0.08493150,0.16712330,0.25205480,0.50410960,0.74794520,1.00000000,1.50410960,2.00273970,2.50684930,3.00273970 }; //10terms
	double vol_strike[] = { 178.332,193.193,208.054,222.915,237.776,252.637,267.498,282.359,297.22,312.081,326.942,341.803,356.664,371.525,386.386,401.247,416.108 };//17spot
	Vol volat(10, 17);
	double t0[] = { 0.3095,0.2907,0.2719,0.2528,0.2331,0.2136,0.1816,0.1388,0.1081,0.105,0.1296,0.1606,0.1795,0.1892,0.1978,0.2057,0.2129 };
	double t1[] = { 0.2807,0.2619,0.2431,0.2239,0.2042,0.1845,0.1586,0.1285,0.1095,0.1027,0.1076,0.1306,0.1518,0.1634,0.172,0.1798,0.1869 };
	double t2[] = { 0.2621,0.2448,0.2274,0.2098,0.1916,0.172,0.1505,0.128,0.1131,0.1076,0.1125,0.1264,0.1413,0.1517,0.16,0.1676,0.1745 };
	double t3[] = { 0.2376,0.2219,0.2061,0.1902,0.1741,0.1576,0.141,0.1261,0.1169,0.1161,0.1215,0.1292,0.1371,0.1447,0.1517,0.1583,0.1644 };
	double t4[] = { 0.2286,0.214,0.1997,0.1854,0.1712,0.1571,0.1437,0.1319,0.1238,0.1211,0.1231,0.128,0.1339,0.14,0.146,0.1517,0.1571 };
	double t5[] = { 0.221,0.2077,0.1946,0.1819,0.1694,0.1574,0.1462,0.1366,0.1295,0.1261,0.1261,0.1287,0.1327,0.1374,0.1422,0.1469,0.1516 };
	double t6[] = { 0.2063,0.1949,0.1841,0.1737,0.1639,0.1549,0.1469,0.1403,0.1355,0.1329,0.1322,0.1331,0.1352,0.1379,0.1409,0.1442,0.1475 };
	double t7[] = { 0.1993,0.1897,0.1806,0.1721,0.1642,0.157,0.1506,0.1453,0.1411,0.1383,0.1368,0.1364,0.1369,0.1381,0.1398,0.1418,0.144 };
	double t8[] = { 0.1927,0.1844,0.1768,0.1696,0.1631,0.1572,0.1521,0.1477,0.1443,0.1418,0.1402,0.1394,0.1393,0.1398,0.1407,0.1419,0.1434 };
	double t9[] = { 0.1892,0.182,0.1754,0.1692,0.1637,0.1586,0.1542,0.1505,0.1474,0.145,0.1432,0.1421,0.1415,0.1414,0.1417,0.1423,0.1431 };

	volat.set_vol_strike(vol_strike, 17);
	volat.set_vol_term(vol_term, 10);

	volat.set_volsurface_by_term(t0, 0, 17);
	volat.set_volsurface_by_term(t1, 1, 17);
	volat.set_volsurface_by_term(t2, 2, 17);
	volat.set_volsurface_by_term(t3, 3, 17);
	volat.set_volsurface_by_term(t4, 4, 17);
	volat.set_volsurface_by_term(t5, 5, 17);
	volat.set_volsurface_by_term(t6, 6, 17);
	volat.set_volsurface_by_term(t7, 7, 17);
	volat.set_volsurface_by_term(t8, 8, 17);
	volat.set_volsurface_by_term(t9, 9, 17);

	std::vector<double> v_rts(std::begin(arr_rts), std::end(arr_rts));
	std::vector<double> v_r(std::begin(arr_r), std::end(arr_r));
	std::vector<double> v_qts(std::begin(arr_qts), std::end(arr_qts));
	std::vector<double> v_q(std::begin(arr_q), std::end(arr_q));

	Rate r(v_r, v_rts);
	Rate q(v_q, v_qts);

	double refprice = 297.22;
	double rf_for_bs = r.getIntpRate(1.0);
	double div_for_bs = q.getIntpRate(1.0);
	double putstrike = 297.22*0.9;
	double callstrike = 297.22*1.1;
	double intpvol = volat.getInpVol(1.0, putstrike);

	MarketParam para(43340, spot, volat, r, q);

	PayoffPut putpay(putstrike);
	

	EuropeanOption EurPut(refprice, 43705, putpay);
	//vector<signed int> od{43432,43524,43613,43705};
	vector<signed int> od{ 43524,43705 };


	AsianOption Asian(refprice, 43705 ,putpay,od);
	Asian.Calc(para, numMC);
	EurPut.Calc(para);

	/*EuropeanVanilla Vput(putpay,refprice, ed);

	EuropeanPut eput(refprice,putstrike,365);
	EuropeanCall ecall(refprice,callstrike,365);

	*/
	std::cout.precision(8);
	std::cout << std::fixed;

	//std::cout <<"eput:  s=" <<spot<<" "<< eput.pv(para)<<" vol " << volat.getInpVol(365/365,putstrike)<< "r " << r.getIntpRate(1.0) << " q " << q.getIntpRate(1.0) <<  std::endl;
	//std::cout <<"ecall: "<< ecall.pv(para)<< "s= " <<spot<<" "<<" vol " <<volat.getInpVol(365/365,callstrike)<< " r " << r.getIntpRate(1.0) << " q " << q.getIntpRate(1.0) << std::endl;
	std::vector<double> rs = Asian.GetResult();
	std::vector<double> euro_rs = EurPut.GetResult();

	std::cout << "   " <<"Asian " << "        "  << "european" << std::endl;
	std::cout << "pv " << rs[0] << "   " << euro_rs[0] <<std::endl;
	std::cout << "delta " << rs[1] << "  " << euro_rs[1]  << std::endl;
	std::cout << "gamma " << rs[2] <<  "  "  << euro_rs[2] << std::endl;
	std::cout << "spot " << rs[5] << std::endl;
	std::cout << "BS vol=" << volat.getBSVol(365 / 365, putpay.GetStrike()) << std::endl;
	std::cout << "implied vol=" << put_iv(spot, putstrike, rf_for_bs, div_for_bs, 0.1, 1.0, rs[0]) << std::endl;
	std::cout << "delta(closed, Flatvol)=" << k_fmla_BSPutDelta(spot, putstrike, rf_for_bs, div_for_bs, volat.getBSVol(1.0, putpay.GetStrike()), 1.0) << std::endl;
	std::cout << "gamma(closed, Flatvol)=" << k_fmla_BSGamma(spot, putstrike, rf_for_bs, div_for_bs, volat.getBSVol(1.0, putpay.GetStrike()), 1.0) << std::endl;

	std::cout << "\n";


}

void test_barrier_flat(double spot)
{
	double arr_rts[] = { 0.002739726,0.25,0.5,0.75,1,1.5,2,2.5,3,4,5,7,10,15,20 };
	double arr_r[] = { 1.54,1.64,1.72,1.77,1.8,1.85,1.88,1.90,1.92,1.92,1.92,1.92,1.92,1.92,1.92 };
	for (int i = 0; i<15; i++)
		arr_r[i] /= 100.0;

	double arr_qts[] = { 0.0191780821917808,0.038356164,0.057534247,0.084931507,0.104109589,0.123287671,0.142465753,0.167123288,0.252054795,0.334246575,0.419178082,0.504109589,0.580821918,0.665753425,0.747945205,0.832876712,0.915068493,1,1.084931507,1.167123288,1.252054795,1.334246575,1.419178082,1.504109589,1.583561644,1.668493151,1.750684932,1.835616438,1.917808219,	2.002739726	,2.087671233,2.169863014,2.254794521,2.336986301,2.421917808,2.506849315,2.583561644,2.668493151,2.750684932,2.835616438,2.917808219,3.002739726 };
	double arr_q[] = { 0.00000,0.00000,0.00000,2.57097, 2.09737,1.77112,1.53270, 1.30656 ,0.86631 ,5.16283, 4.11677,3.42318,3.34816,2.92103,2.60139,2.67302,2.43293,2.22630,2.26258, 2.10325,1.96057,3.00733,2.82806 ,2.66837, 2.53449,2.53675,2.41823 ,2.46233 ,2.35680 ,2.25685 ,2.27446 ,2.18831 ,2.10588 ,2.66917,2.57598,2.48871,2.41481,2.42319,2.35115 ,2.38170 ,2.31461 ,2.24914 };
	for (int i = 0; i<42; i++)
		arr_q[i] /= 100.0;

	//double spot=297.22;
	double vol_term[] = { 0.08493150,0.16712330,0.25205480,0.50410960,0.74794520,1.00000000,1.50410960,2.00273970,2.50684930,3.00273970 }; //10terms
	double vol_strike[] = { 178.332,193.193,208.054,222.915,237.776,252.637,267.498,282.359,297.22,312.081,326.942,341.803,356.664,371.525,386.386,401.247,416.108 };//17spot
	Vol volat(10, 17);
	double t0[] = { 0.3095,0.2907,0.2719,0.2528,0.2331,0.2136,0.1816,0.1388,0.1081,0.105,0.1296,0.1606,0.1795,0.1892,0.1978,0.2057,0.2129 };
	double t1[] = { 0.2807,0.2619,0.2431,0.2239,0.2042,0.1845,0.1586,0.1285,0.1095,0.1027,0.1076,0.1306,0.1518,0.1634,0.172,0.1798,0.1869 };
	double t2[] = { 0.2621,0.2448,0.2274,0.2098,0.1916,0.172,0.1505,0.128,0.1131,0.1076,0.1125,0.1264,0.1413,0.1517,0.16,0.1676,0.1745 };
	double t3[] = { 0.2376,0.2219,0.2061,0.1902,0.1741,0.1576,0.141,0.1261,0.1169,0.1161,0.1215,0.1292,0.1371,0.1447,0.1517,0.1583,0.1644 };
	double t4[] = { 0.2286,0.214,0.1997,0.1854,0.1712,0.1571,0.1437,0.1319,0.1238,0.1211,0.1231,0.128,0.1339,0.14,0.146,0.1517,0.1571 };
	double t5[] = { 0.221,0.2077,0.1946,0.1819,0.1694,0.1574,0.1462,0.1366,0.1295,0.1261,0.1261,0.1287,0.1327,0.1374,0.1422,0.1469,0.1516 };
	double t6[] = { 0.2063,0.1949,0.1841,0.1737,0.1639,0.1549,0.1469,0.1403,0.1355,0.1329,0.1322,0.1331,0.1352,0.1379,0.1409,0.1442,0.1475 };
	double t7[] = { 0.1993,0.1897,0.1806,0.1721,0.1642,0.157,0.1506,0.1453,0.1411,0.1383,0.1368,0.1364,0.1369,0.1381,0.1398,0.1418,0.144 };
	double t8[] = { 0.1927,0.1844,0.1768,0.1696,0.1631,0.1572,0.1521,0.1477,0.1443,0.1418,0.1402,0.1394,0.1393,0.1398,0.1407,0.1419,0.1434 };
	double t9[] = { 0.1892,0.182,0.1754,0.1692,0.1637,0.1586,0.1542,0.1505,0.1474,0.145,0.1432,0.1421,0.1415,0.1414,0.1417,0.1423,0.1431 };

	volat.set_vol_strike(vol_strike, 17);
	volat.set_vol_term(vol_term, 10);

	volat.set_volsurface_by_term(t0, 0, 17);
	volat.set_volsurface_by_term(t1, 1, 17);
	volat.set_volsurface_by_term(t2, 2, 17);
	volat.set_volsurface_by_term(t3, 3, 17);
	volat.set_volsurface_by_term(t4, 4, 17);
	volat.set_volsurface_by_term(t5, 5, 17);
	volat.set_volsurface_by_term(t6, 6, 17);
	volat.set_volsurface_by_term(t7, 7, 17);
	volat.set_volsurface_by_term(t8, 8, 17);
	volat.set_volsurface_by_term(t9, 9, 17);

	std::vector<double> v_rts(std::begin(arr_rts), std::end(arr_rts));
	std::vector<double> v_r(std::begin(arr_r), std::end(arr_r));
	std::vector<double> v_qts(std::begin(arr_qts), std::end(arr_qts));
	std::vector<double> v_q(std::begin(arr_q), std::end(arr_q));

	Rate r(v_r, v_rts);
	Rate q(v_q, v_qts);

	double refprice = 297.22;
	double rf_for_bs = r.getIntpRate(1.0);
	double div_for_bs = q.getIntpRate(1.0);
	double putstrike = 297.22*0.9;
	double callstrike = 297.22*1.0;
	double upbarrier = 297.22*1.2;
	double intpvol = volat.getInpVol(1.0, putstrike);
	double rebate = 0.0;

	double const_vol = volat.getBSVol(1.0,297.22);
	double const_rf = r.getIntpRate(1.0);
	double const_q = q.getIntpRate(1.0);

	volat.set_const_vol(const_vol);
	r.set_const_rate(const_rf);
	q.set_const_rate(const_q);

	MarketParam para(0, spot, volat, r, q);

	PayoffBarCUO cuo(callstrike,upbarrier,rebate);
	
	BarrierOption Bar_cuo(refprice,365,cuo);

	Bar_cuo.Calc(para);

	//std::vector<double> rs = EurPut.GetResult();
	std::vector<double> rs_bar = Bar_cuo.GetResult();
	
	std::cout.precision(8);
	std::cout << std::fixed;

	std::cout << "pv " << rs_bar[0] << std::endl;
	std::cout << "delta " << rs_bar[1] << std::endl;
	std::cout << "gamma " << rs_bar[2] << std::endl;
	std::cout << "spot " << rs_bar[5] << std::endl;
	std::cout << "atm vol at ref " << volat.getBSVol(1.0, refprice) << std::endl;
	
	const double ds = 297.22*0.01;
	double clf_pv = k_fmla_Barrier(0, 0, 1, 297.22, callstrike, refprice*1.205355, const_rf, const_q, const_vol, 1.0, rebate);
	double clf_pv_up = k_fmla_Barrier(0, 0, 1, 297.22+ ds, callstrike, refprice*1.205355, const_rf, const_q, const_vol, 1.0, rebate);
	double clf_pv_down = k_fmla_Barrier(0, 0, 1, 297.22- ds, callstrike, refprice*1.205355, const_rf, const_q, const_vol, 1.0, rebate);

	std::cout << "closed form " << clf_pv << std::endl;
	std::cout << "closed form delta" << (clf_pv_up-clf_pv_down) /(ds*2.0) << std::endl;
	std::cout << "closed form gamma" << (clf_pv_up -2.0*clf_pv + clf_pv_down) /ds/ds << std::endl;

	std::cout << "\n";

	Vol volat_up(volat);
	volat_up.Vol_up(0.01);
	MarketParam para_volup(0, spot, volat_up, r, q);
	BarrierOption Bar_cuo_vega(refprice, 365, cuo);
	Bar_cuo_vega.Calc(para_volup);

	std::vector<double> rs_vega = Bar_cuo_vega.GetResult();
	std::cout << "\npv(for vega)" << rs_vega[0] << std::endl;
	std::cout << "delta(for vega) " << rs_vega[1] << std::endl;
	std::cout << "gamma(for vega) " << rs_vega[2] << std::endl;
	std::cout << "spot(for vega) " << rs_vega[5] << std::endl;
	std::cout << "BS vol(for vega)=" << volat_up.getBSVol(1.0, callstrike) << std::endl;
	std::cout << "\n";

	std::cout << "vega(closed, Flatvol)=" << k_fmla_Barrier(0, 0, 1, 297.22, callstrike, refprice*1.205355, const_rf, const_q, const_vol + 0.01, 1.0, rebate) - k_fmla_Barrier(0, 0, 1, 297.22, callstrike, refprice*1.205355, const_rf, const_q, const_vol, 1.0, rebate) << std::endl;
	std::cout << "vega(for vega) " << rs_vega[0] - rs_bar[0] << std::endl;


}

void test_barrier_LV(double spot)
{
	double arr_rts[] = { 0.002739726,0.25,0.5,0.75,1,1.5,2,2.5,3,4,5,7,10,15,20 };
	double arr_r[] = { 1.54,1.64,1.72,1.77,1.8,1.85,1.88,1.90,1.92,1.92,1.92,1.92,1.92,1.92,1.92 };
	for (int i = 0; i<15; i++)
		arr_r[i] /= 100.0;

	double arr_qts[] = { 0.0191780821917808,0.038356164,0.057534247,0.084931507,0.104109589,0.123287671,0.142465753,0.167123288,0.252054795,0.334246575,0.419178082,0.504109589,0.580821918,0.665753425,0.747945205,0.832876712,0.915068493,1,1.084931507,1.167123288,1.252054795,1.334246575,1.419178082,1.504109589,1.583561644,1.668493151,1.750684932,1.835616438,1.917808219,	2.002739726	,2.087671233,2.169863014,2.254794521,2.336986301,2.421917808,2.506849315,2.583561644,2.668493151,2.750684932,2.835616438,2.917808219,3.002739726 };
	double arr_q[] = { 0.00000,0.00000,0.00000,2.57097, 2.09737,1.77112,1.53270, 1.30656 ,0.86631 ,5.16283, 4.11677,3.42318,3.34816,2.92103,2.60139,2.67302,2.43293,2.22630,2.26258, 2.10325,1.96057,3.00733,2.82806 ,2.66837, 2.53449,2.53675,2.41823 ,2.46233 ,2.35680 ,2.25685 ,2.27446 ,2.18831 ,2.10588 ,2.66917,2.57598,2.48871,2.41481,2.42319,2.35115 ,2.38170 ,2.31461 ,2.24914 };
	for (int i = 0; i<42; i++)
		arr_q[i] /= 100.0;

	//double spot=297.22;
	double vol_term[] = { 0.08493150,0.16712330,0.25205480,0.50410960,0.74794520,1.00000000,1.50410960,2.00273970,2.50684930,3.00273970 }; //10terms
	double vol_strike[] = { 178.332,193.193,208.054,222.915,237.776,252.637,267.498,282.359,297.22,312.081,326.942,341.803,356.664,371.525,386.386,401.247,416.108 };//17spot
	Vol volat(10, 17);
	double t0[] = { 0.3095,0.2907,0.2719,0.2528,0.2331,0.2136,0.1816,0.1388,0.1081,0.105,0.1296,0.1606,0.1795,0.1892,0.1978,0.2057,0.2129 };
	double t1[] = { 0.2807,0.2619,0.2431,0.2239,0.2042,0.1845,0.1586,0.1285,0.1095,0.1027,0.1076,0.1306,0.1518,0.1634,0.172,0.1798,0.1869 };
	double t2[] = { 0.2621,0.2448,0.2274,0.2098,0.1916,0.172,0.1505,0.128,0.1131,0.1076,0.1125,0.1264,0.1413,0.1517,0.16,0.1676,0.1745 };
	double t3[] = { 0.2376,0.2219,0.2061,0.1902,0.1741,0.1576,0.141,0.1261,0.1169,0.1161,0.1215,0.1292,0.1371,0.1447,0.1517,0.1583,0.1644 };
	double t4[] = { 0.2286,0.214,0.1997,0.1854,0.1712,0.1571,0.1437,0.1319,0.1238,0.1211,0.1231,0.128,0.1339,0.14,0.146,0.1517,0.1571 };
	double t5[] = { 0.221,0.2077,0.1946,0.1819,0.1694,0.1574,0.1462,0.1366,0.1295,0.1261,0.1261,0.1287,0.1327,0.1374,0.1422,0.1469,0.1516 };
	double t6[] = { 0.2063,0.1949,0.1841,0.1737,0.1639,0.1549,0.1469,0.1403,0.1355,0.1329,0.1322,0.1331,0.1352,0.1379,0.1409,0.1442,0.1475 };
	double t7[] = { 0.1993,0.1897,0.1806,0.1721,0.1642,0.157,0.1506,0.1453,0.1411,0.1383,0.1368,0.1364,0.1369,0.1381,0.1398,0.1418,0.144 };
	double t8[] = { 0.1927,0.1844,0.1768,0.1696,0.1631,0.1572,0.1521,0.1477,0.1443,0.1418,0.1402,0.1394,0.1393,0.1398,0.1407,0.1419,0.1434 };
	double t9[] = { 0.1892,0.182,0.1754,0.1692,0.1637,0.1586,0.1542,0.1505,0.1474,0.145,0.1432,0.1421,0.1415,0.1414,0.1417,0.1423,0.1431 };

	volat.set_vol_strike(vol_strike, 17);
	volat.set_vol_term(vol_term, 10);

	volat.set_volsurface_by_term(t0, 0, 17);
	volat.set_volsurface_by_term(t1, 1, 17);
	volat.set_volsurface_by_term(t2, 2, 17);
	volat.set_volsurface_by_term(t3, 3, 17);
	volat.set_volsurface_by_term(t4, 4, 17);
	volat.set_volsurface_by_term(t5, 5, 17);
	volat.set_volsurface_by_term(t6, 6, 17);
	volat.set_volsurface_by_term(t7, 7, 17);
	volat.set_volsurface_by_term(t8, 8, 17);
	volat.set_volsurface_by_term(t9, 9, 17);

	std::vector<double> v_rts(std::begin(arr_rts), std::end(arr_rts));
	std::vector<double> v_r(std::begin(arr_r), std::end(arr_r));
	std::vector<double> v_qts(std::begin(arr_qts), std::end(arr_qts));
	std::vector<double> v_q(std::begin(arr_q), std::end(arr_q));

	Rate r(v_r, v_rts);
	Rate q(v_q, v_qts);

	double refprice = 297.22;
	double rf_for_bs = r.getIntpRate(1.0);
	double div_for_bs = q.getIntpRate(1.0);
	double putstrike = 297.22*0.9;
	double callstrike = 297.22*1.0;
	double upbarrier = 297.22*1.2;
	double intpvol = volat.getInpVol(1.0, putstrike);
	double rebate = 0.0;

	double const_vol = volat.getBSVol(1.0, 297.22);
	double const_rf = r.getIntpRate(1.0);
	double const_q = q.getIntpRate(1.0);

	MarketParam para(0, spot, volat, r, q);

	PayoffBarCUO cuo(callstrike, upbarrier, rebate);

	BarrierOption Bar_cuo(refprice, 365, cuo);

	Bar_cuo.Calc(para);

	std::vector<double> rs_bar = Bar_cuo.GetResult();

	std::cout.precision(8);
	std::cout << std::fixed;

	std::cout << "\npv " << rs_bar[0] << std::endl;
	std::cout << "delta " << rs_bar[1] << std::endl;
	std::cout << "gamma " << rs_bar[2] << std::endl;
	std::cout << "spot " << rs_bar[5] << std::endl;
	std::cout << "atm vol at ref" << volat.getBSVol(1.0, refprice) << std::endl;

	const double ds = 297.22*0.01;
	double clf_pv = k_fmla_Barrier(0, 0, 1, 297.22, callstrike, refprice*1.205355, const_rf, const_q, const_vol, 1.0, rebate);
	double clf_pv_up = k_fmla_Barrier(0, 0, 1, 297.22 + ds, callstrike, refprice*1.205355, const_rf, const_q, const_vol, 1.0, rebate);
	double clf_pv_down = k_fmla_Barrier(0, 0, 1, 297.22 - ds, callstrike, refprice*1.205355, const_rf, const_q, const_vol, 1.0, rebate);

	std::cout << "closed form " << clf_pv << std::endl;
	std::cout << "closed form delta" << (clf_pv_up - clf_pv_down) / (ds*2.0) << std::endl;
	std::cout << "closed form gamma" << (clf_pv_up - 2.0*clf_pv + clf_pv_down) / ds / ds << std::endl;
	std::cout << "\n";

	Vol volat_up(volat);
	volat_up.Vol_up(0.01);
	MarketParam para_volup(0, spot, volat_up, r, q);
	BarrierOption Bar_cuo_vega(refprice, 365, cuo);
	Bar_cuo_vega.Calc(para_volup);

	std::vector<double> rs_vega = Bar_cuo_vega.GetResult();
	std::cout << "pv(for vega)" << rs_vega[0] << std::endl;
	std::cout << "delta(for vega) " << rs_vega[1] << std::endl;
	std::cout << "gamma(for vega) " << rs_vega[2] << std::endl;
	std::cout << "spot(for vega) " << rs_vega[5] << std::endl;
	std::cout << "BS vol(for vega)=" << volat_up.getBSVol(1.0, callstrike) << std::endl;
	std::cout << "\n";

	std::cout << "vega(closed, Flatvol)=" << k_fmla_Barrier(0, 0, 1, 297.22, callstrike, refprice*1.205355, const_rf, const_q, const_vol+0.01, 1.0, rebate)- k_fmla_Barrier(0, 0, 1, 297.22, callstrike, refprice*1.205355, const_rf, const_q, const_vol, 1.0, rebate) << std::endl;
	std::cout << "vega(for vega) " << rs_vega[0] - rs_bar[0] << std::endl;

}

void test_autocall_simple(double spot, int hitflag = 0, FvsS fvss = surface, CalcMode calcmode = calc1)
{
	double arr_rts[] = { 0.002739726,0.25,0.5,0.75,1,1.5,2,2.5,3,4,5,7,10,15,20 };
	//double arr_r[] = { 1.54,1.64,1.72,1.77,1.8,1.85,1.88,1.90,1.92,1.92,1.92,1.92,1.92,1.92,1.92 };
	double arr_r[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	for (int i = 0; i<15; i++)
		arr_r[i] /= 100.0;

	double arr_qts[] = { 0.0191780821917808,0.038356164,0.057534247,0.084931507,0.104109589,0.123287671,0.142465753,0.167123288,0.252054795,0.334246575,0.419178082,0.504109589,0.580821918,0.665753425,0.747945205,0.832876712,0.915068493,1,1.084931507,1.167123288,1.252054795,1.334246575,1.419178082,1.504109589,1.583561644,1.668493151,1.750684932,1.835616438,1.917808219,	2.002739726	,2.087671233,2.169863014,2.254794521,2.336986301,2.421917808,2.506849315,2.583561644,2.668493151,2.750684932,2.835616438,2.917808219,3.002739726 };
	double arr_q[] = { 0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	for (int i = 0; i<42; i++)
		arr_q[i] /= 100.0;

	//double spot=297.22;
	double vol_term[] = { 0.08493150,0.16712330,0.25205480,0.50410960,0.74794520,1.00000000,1.50410960,2.00273970,2.50684930,3.00273970 }; //10terms
	double vol_strike[] = { 178.332,193.193,208.054,222.915,237.776,252.637,267.498,282.359,297.22,312.081,326.942,341.803,356.664,371.525,386.386,401.247,416.108 };//17spot
	Vol volat(10, 17);
	double t0[] = { 0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15 };
	double t1[] = { 0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15 };
	double t2[] = { 0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15 };
	double t3[] = { 0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15 };
	double t4[] = { 0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15 };
	double t5[] = { 0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15 };
	double t6[] = { 0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15 };
	double t7[] = { 0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15 };
	double t8[] = { 0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15 };
	double t9[] = { 0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15,0.15 };

	volat.set_vol_strike(vol_strike, 17);
	volat.set_vol_term(vol_term, 10);

	volat.set_volsurface_by_term(t0, 0, 17);
	volat.set_volsurface_by_term(t1, 1, 17);
	volat.set_volsurface_by_term(t2, 2, 17);
	volat.set_volsurface_by_term(t3, 3, 17);
	volat.set_volsurface_by_term(t4, 4, 17);
	volat.set_volsurface_by_term(t5, 5, 17);
	volat.set_volsurface_by_term(t6, 6, 17);
	volat.set_volsurface_by_term(t7, 7, 17);
	volat.set_volsurface_by_term(t8, 8, 17);
	volat.set_volsurface_by_term(t9, 9, 17);

	std::vector<double> v_rts(std::begin(arr_rts), std::end(arr_rts));
	std::vector<double> v_r(std::begin(arr_r), std::end(arr_r));
	std::vector<double> v_qts(std::begin(arr_qts), std::end(arr_qts));
	std::vector<double> v_q(std::begin(arr_q), std::end(arr_q));

	Rate r(v_r, v_rts);
	Rate q(v_q, v_qts);

	double refprice = 297.22;


	double rf_for_bs = r.getIntpRate(1.0);
	double div_for_bs = q.getIntpRate(1.0);
	double putstrike = 297.22*0.9;
	double callstrike = 297.22*1.0;
	double upbarrier = 297.22*1.2;
	double intpvol = volat.getInpVol(1.0, putstrike);
	double rebate = 0.0;

	double const_vol = volat.getBSVol(1.0, 297.22);
	double const_rf = r.getIntpRate(1.0);
	double const_q = q.getIntpRate(1.0);

	if (fvss == flat) {
		volat.set_const_vol(const_vol);
		r.set_const_rate(const_rf);
		q.set_const_rate(const_q);
	}

	MarketParam para(43340, spot, volat, r, q);

	signed int expiryDate = 44436;
	int nb_autocall = 6;
	signed int auto_date[7] = { -1,43524,43705,43889,44071,44255,44436 };
	double auto_strike[7] = { -1,297.22,	297.22,	282.359	,282.359,	267.498,	267.498 };
	double auto_coupon[7] = { -1, 0.0230,	0.0460,	0.0690,	0.0920,	0.1150,	0.1380 };
	double auto_ki_barrier = refprice*0.6;
	double auto_dummy_coupon = auto_coupon[6];

	double auto_put_strike;
	auto_put_strike = refprice*1.0; // if put_strike=0, notional protected

	PayoffAutocallStd autoPayoff(nb_autocall, auto_date, auto_strike, auto_coupon, auto_ki_barrier, auto_put_strike, auto_dummy_coupon, refprice);
	AutocallOption AutoKOSPI(refprice, expiryDate, autoPayoff, hitflag);

	AutoKOSPI.Calc(para);

	std::vector<double> rs_auto = AutoKOSPI.GetResult();

	std::cout.precision(8);
	std::cout << std::fixed;

	std::cout << "spot " << rs_auto[5] << std::endl;
	std::cout << "pv " << (hitflag ? "hitted  " : "not hit  ") << rs_auto[0] << std::endl;
	std::cout << "delta " << rs_auto[1] << std::endl;
	std::cout << "gamma " << rs_auto[2] << std::endl;

}

void test_autocall_Cal2_vs_paras()
{
	double arr_rts[] = { 0.002739726,0.25,0.5,0.75,1,1.5,2,2.5,3,4,5,7,10,15,20 };
	double arr_r[] = { 1.79000,1.81000,1.74000,1.69000,1.65000,1.59000,1.55000,1.52000,1.50000,1.50000,1.50000,1.50000,1.50000,1.50000 ,1.50000 };

	for (int i = 0; i<15; i++)
		arr_r[i] /= 100.0;

	double arr_qts[] = { 0.01918000,0.03836000,0.05753000,0.08493000,0.10411000,0.12329000,0.14247000,0.16712000,0.25205000,0.33425000,0.41918000,0.50411000,0.58082000,0.66575000,0.74795000,0.83288000,0.91507000,1.00000000,1.08493000,1.16712000,1.25205000,1.33425000,1.41918000,1.50411000,1.58356000,1.66849000,1.75068000,1.83562000,1.91781000,2.00274000,2.08767000,2.16986000,2.25479000,2.33699000,2.42192000,2.50685000,2.58356000,2.66849000,2.75068000,2.83562000,2.91781000,3.00274000 };
	double arr_q[] = { 0.00000,0.00000,0.00000,2.63440,2.14907,1.81475,1.570440,1.338800,0.887680,5.313760,4.237140,3.523290,3.057960,2.997860,2.668400,2.396300,2.486970,2.275760,2.303830,2.141590,1.996320,1.886950,2.912220,2.747780,2.609920,2.609350,2.486850,2.371770,2.421350,2.318670,2.224340,2.243670,2.159160,2.090990,2.701420,2.609900,2.532410,2.534520,2.458790,2.385140,2.417350,2.348970 };
	for (int i = 0; i<42; i++)
		arr_q[i] /= 100.0;

	//double spot=297.22;
	double vol_term[] = { 0.08333,0.16667,0.250000,0.500000,0.750000,1.000000,1.500000,2.000000,2.500000,3.000000 };
	double vol_strike[] = { 178.332,193.193,208.054,222.915,237.776,252.637,267.498,282.359,297.22,312.081,326.942,341.803,356.664,371.525,386.386,401.247,416.108 };//17spot
	Vol volat(10, 17);
	double t0[] = { 0.3484,	0.3310,	0.3135,	0.2960,	0.2778,	0.2577, 0.2279, 0.1924, 0.1641, 0.1532, 0.1618, 0.1729, 0.1807, 0.1883, 0.1968,	0.2034,	0.2090 };
	double t1[] = { 0.3182,	0.3002,	0.2822,	0.2640,	0.2454,	0.2252,	0.2010,	0.1770,	0.1568,	0.1452,	0.1429,	0.1484,	0.1565,	0.1645,	0.1722,	0.1792,	0.1855 };
	double t2[] = { 0.3069,	0.2883,	0.2697,	0.2509,	0.2318,	0.2118,	0.1907,	0.1699,	0.1527,	0.1432,	0.1423,	0.1476,	0.1554,	0.1634,	0.1711,	0.1782,	0.1849 };
	double t3[] = { 0.2709,	0.2543,	0.2380,	0.2218,	0.2058,	0.1900,	0.1748,	0.1607,	0.1491,	0.1418,	0.1395,	0.1411,	0.1451,	0.1502,	0.1555,	0.1609,	0.1662 };
	double t4[] = { 0.2441,	0.2300,	0.2163,	0.2029,	0.1901,	0.1778,	0.1664,	0.1562,	0.1479,	0.1423,	0.1396,	0.1394,	0.1410,	0.1437,	0.1470,	0.1506,	0.1543 };
	double t5[] = { 0.2277,	0.2156,	0.2041,	0.1930,	0.1826,	0.1727,	0.1636,	0.1556,	0.1490,	0.1440,	0.1409,	0.1395,	0.1396,	0.1406,	0.1424,	0.1446,	0.1471 };
	double t6[] = { 0.2073, 0.1981,	0.1895,	0.1814,	0.1739,	0.1669,	0.1606,	0.1550,	0.1503,	0.1465,	0.1436,	0.1416,	0.1404,	0.1400,	0.1401,	0.1407,	0.1417 };
	double t7[] = { 0.1944,	0.1871,	0.1802,	0.1739,	0.1681,	0.1629,	0.1581,	0.1539,	0.1503,	0.1473,	0.1449,	0.1431,	0.1418,	0.1410,	0.1406,	0.1405,	0.1407 };
	double t8[] = { 0.1876,	0.1815,	0.1760,	0.1709,	0.1662,	0.1619,	0.1580,	0.1546,	0.1516,	0.1490,	0.1468,	0.1451,	0.1436,	0.1426,	0.1418,	0.1413,	0.1410 };
	double t9[] = { 0.1821,	0.1770,	0.1723,	0.1680,	0.1641,	0.1606,	0.1573,	0.1545,	0.1520,	0.1497,	0.1478,	0.1462,	0.1449,	0.1438,	0.1429,	0.1422,	0.1418 };

	volat.set_vol_strike(vol_strike, 17);
	volat.set_vol_term(vol_term, 10);

	volat.set_volsurface_by_term(t0, 0, 17);
	volat.set_volsurface_by_term(t1, 1, 17);
	volat.set_volsurface_by_term(t2, 2, 17);
	volat.set_volsurface_by_term(t3, 3, 17);
	volat.set_volsurface_by_term(t4, 4, 17);
	volat.set_volsurface_by_term(t5, 5, 17);
	volat.set_volsurface_by_term(t6, 6, 17);
	volat.set_volsurface_by_term(t7, 7, 17);
	volat.set_volsurface_by_term(t8, 8, 17);
	volat.set_volsurface_by_term(t9, 9, 17);

	std::vector<double> v_rts(std::begin(arr_rts), std::end(arr_rts));
	std::vector<double> v_r(std::begin(arr_r), std::end(arr_r));
	std::vector<double> v_qts(std::begin(arr_qts), std::end(arr_qts));
	std::vector<double> v_q(std::begin(arr_q), std::end(arr_q));

	Rate r(v_r, v_rts);
	Rate q(v_q, v_qts);


	//double intpvol = volat.getInpVol(1.0, putstrike);  //flat interpolation use .getBSVol

	//상품타입변경
	//bool is_flat_vol = true;
	//int hitflag = 1;
	//long nM = 30000;

	//***상품타입변경
	double spot = 297.22;
	signed int vd = 43340;
	signed int exd = 44436;
	double refprice = 297.22;
	double auto_ki_barrier = refprice*0.6;
	double auto_put_strike= refprice; // if put_strike=0, notional protected

	double rf_for_bs = r.getIntpRate((exd - vd) / 365.0);
	cout << "rf_for_bs " << rf_for_bs << "\n";
	double div_for_bs = q.getIntpRate((exd - vd) / 365.0);
	cout << "div_for_bs " << div_for_bs << "\n";

	double vol_bs = volat.getBSVol((exd - vd) / 365.0, auto_put_strike);

	
	MarketParam para(43340, spot, volat, r, q);
	MarketParameters paras(43340, spot, volat, r, q);
	
	int nb_autocall = 6;
	signed int auto_date[7] = { -1,43524,43705,43889,44071,44255,44436
	};
	double auto_strike[7] = {-1,297.22,	297.22,	282.359	,282.359,	267.498,	267.498};
	double auto_coupon[7] = {-1, 0.0230,	0.0460,	0.0690,	0.0920,	0.1150,	0.1380};
	
	double auto_dummy_coupon = auto_coupon[6];

	
	

	PayoffAutocallStd autoPayoff(nb_autocall, auto_date, auto_strike, auto_coupon, auto_ki_barrier, auto_put_strike, auto_dummy_coupon, refprice);
	AutocallOption AutoKOSPI(refprice, exd, autoPayoff,0);
	AutocallOption AutoKOSPI_paras(refprice, exd, autoPayoff, 0);
	AutocallOption AutoKOSPI_hit(refprice, exd, autoPayoff, 1);
	AutocallOption AutoKOSPI_paras_hit(refprice, exd, autoPayoff, 1);

	AutoKOSPI.Calc(para);
	AutoKOSPI_paras.Calc(paras);
	AutoKOSPI_hit.Calc(para);
	AutoKOSPI_paras_hit.Calc(paras);


//	AutoKOSPI.Calc(para);

	std::vector<double> rs_auto = AutoKOSPI.GetResult();
	std::vector<double> rs_auto_paras = AutoKOSPI_paras.GetResult();
	std::vector<double> rs_auto_hit = AutoKOSPI_hit.GetResult();
	std::vector<double> rs_auto_paras_hit = AutoKOSPI_paras_hit.GetResult();

	
	std::cout.precision(8);
	std::cout << std::fixed;
	cout << "result : test_autocall_Cal2_vs_paras\n"; 
	std::cout << "spot " << rs_auto[5] << std::endl;
	std::cout << "calc2 para"<<rs_auto[0] << std::endl;
	std::cout << "calc2 para hitted" << rs_auto_hit[0] << std::endl;
	std::cout << "calc2 paras" << rs_auto_paras[0] << std::endl;
	std::cout << "calc2 paras hitted" << rs_auto_paras_hit[0] << std::endl;

	//std::cout << "delta " << rs_auto[1] << std::endl;
	//std::cout << "gamma " << rs_auto[2] << std::endl;

}

void test_autocall_Calc_vs_paras_paramodule(MarketParameters& paras)
{
	//paras input
	double refprice = paras.get_spot();
	double auto_ki_barrier = refprice*0.6;
	double auto_put_strike = refprice; // if put_strike=0, notional protected
	signed int vd = paras.get_vdate();
	signed int exd = 44436;

	int nb_autocall = 6;
	signed int auto_date[7] = { -1,43524,43705,43889,44071,44255,44436};
	double auto_strike[7] = { -1,297.22,	297.22,	282.359	,282.359,	267.498,	267.498 };
	double auto_coupon[7] = { -1, 0.0230,	0.0460,	0.0690,	0.0920,	0.1150,	0.1380 };
	double auto_dummy_coupon = auto_coupon[6];

	PayoffAutocallStd autoPayoff(nb_autocall, auto_date, auto_strike, auto_coupon, auto_ki_barrier, auto_put_strike, auto_dummy_coupon, refprice);
	AutocallOption AutoKOSPI(refprice, exd, autoPayoff, 0);
	AutocallOption AutoKOSPI_paras(refprice, exd, autoPayoff, 0);
	AutocallOption AutoKOSPI_hit(refprice, exd, autoPayoff, 1);
	AutocallOption AutoKOSPI_paras_hit(refprice, exd, autoPayoff, 1);

	AutoKOSPI_paras.Calc(paras);
	AutoKOSPI_paras_hit.Calc(paras);

	std::vector<double> rs_auto_paras = AutoKOSPI_paras.GetResult();
	std::vector<double> rs_auto_paras_hit = AutoKOSPI_paras_hit.GetResult();

	std::cout.precision(8);
	std::cout << std::fixed;
	cout << "\nresult : test_autocall_Calc_vs_paras_paramodule\n";
	std::cout << "calc2 para - para module " << rs_auto_paras[0] << std::endl;
	std::cout << "calc2 paras  hitted - para module " << rs_auto_paras_hit[0] << std::endl;

}


void test_autocall_div(double spot, int hitflag = 0, FvsS fvss = surface, CalcMode calcmode = calc1)
{
	double arr_rts[] = { 0.002739726,0.25,0.5,0.75,1,1.5,2,2.5,3,4,5,7,10,15,20 };
	double arr_r[] = { 1.54,1.64,1.72,1.77,1.8,1.85,1.88,1.90,1.92,1.92,1.92,1.92,1.92,1.92,1.92 };
	for (int i = 0; i<15; i++)
		arr_r[i] /= 100.0;

	double arr_qts[] = { 0.0191780821917808,0.038356164,0.057534247,0.084931507,0.104109589,0.123287671,0.142465753,0.167123288,0.252054795,0.334246575,0.419178082,0.504109589,0.580821918,0.665753425,0.747945205,0.832876712,0.915068493,1,1.084931507,1.167123288,1.252054795,1.334246575,1.419178082,1.504109589,1.583561644,1.668493151,1.750684932,1.835616438,1.917808219,	2.002739726	,2.087671233,2.169863014,2.254794521,2.336986301,2.421917808,2.506849315,2.583561644,2.668493151,2.750684932,2.835616438,2.917808219,3.002739726 };
	double arr_q[] = { 0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,4.5095,3.5958,2.9900,2.9722,2.5930,2.3094,2.4108,2.1943,2.0079,2.0613,1.9162,1.7862,2.8437,2.6742,2.5232,2.3966,2.4059,2.2935,2.3434,2.2429,2.1478,2.1699,2.0877,2.0090,2.5757,2.4858,2.4016,2.330293238,2.341357773,2.27176383,2.304693379,2.239772439,2.176421211 };
	for (int i = 0; i<42; i++)
		arr_q[i] /= 100.0;

	//double spot=297.22;
	double vol_term[] = { 0.08493150,0.16712330,0.25205480,0.50410960,0.74794520,1.00000000,1.50410960,2.00273970,2.50684930,3.00273970 }; //10terms
	double vol_strike[] = { 178.332,193.193,208.054,222.915,237.776,252.637,267.498,282.359,297.22,312.081,326.942,341.803,356.664,371.525,386.386,401.247,416.108 };//17spot
	Vol volat(10, 17);
	double t0[] = { 0.3095,0.2907,0.2719,0.2528,0.2331,0.2136,0.1816,0.1388,0.1081,0.105,0.1296,0.1606,0.1795,0.1892,0.1978,0.2057,0.2129 };
	double t1[] = { 0.2807,0.2619,0.2431,0.2239,0.2042,0.1845,0.1586,0.1285,0.1095,0.1027,0.1076,0.1306,0.1518,0.1634,0.172,0.1798,0.1869 };
	double t2[] = { 0.2621,0.2448,0.2274,0.2098,0.1916,0.172,0.1505,0.128,0.1131,0.1076,0.1125,0.1264,0.1413,0.1517,0.16,0.1676,0.1745 };
	double t3[] = { 0.2376,0.2219,0.2061,0.1902,0.1741,0.1576,0.141,0.1261,0.1169,0.1161,0.1215,0.1292,0.1371,0.1447,0.1517,0.1583,0.1644 };
	double t4[] = { 0.2286,0.214,0.1997,0.1854,0.1712,0.1571,0.1437,0.1319,0.1238,0.1211,0.1231,0.128,0.1339,0.14,0.146,0.1517,0.1571 };
	double t5[] = { 0.221,0.2077,0.1946,0.1819,0.1694,0.1574,0.1462,0.1366,0.1295,0.1261,0.1261,0.1287,0.1327,0.1374,0.1422,0.1469,0.1516 };
	double t6[] = { 0.2063,0.1949,0.1841,0.1737,0.1639,0.1549,0.1469,0.1403,0.1355,0.1329,0.1322,0.1331,0.1352,0.1379,0.1409,0.1442,0.1475 };
	double t7[] = { 0.1993,0.1897,0.1806,0.1721,0.1642,0.157,0.1506,0.1453,0.1411,0.1383,0.1368,0.1364,0.1369,0.1381,0.1398,0.1418,0.144 };
	double t8[] = { 0.1927,0.1844,0.1768,0.1696,0.1631,0.1572,0.1521,0.1477,0.1443,0.1418,0.1402,0.1394,0.1393,0.1398,0.1407,0.1419,0.1434 };
	double t9[] = { 0.1892,0.182,0.1754,0.1692,0.1637,0.1586,0.1542,0.1505,0.1474,0.145,0.1432,0.1421,0.1415,0.1414,0.1417,0.1423,0.1431 };

	volat.set_vol_strike(vol_strike, 17);
	volat.set_vol_term(vol_term, 10);

	volat.set_volsurface_by_term(t0, 0, 17);
	volat.set_volsurface_by_term(t1, 1, 17);
	volat.set_volsurface_by_term(t2, 2, 17);
	volat.set_volsurface_by_term(t3, 3, 17);
	volat.set_volsurface_by_term(t4, 4, 17);
	volat.set_volsurface_by_term(t5, 5, 17);
	volat.set_volsurface_by_term(t6, 6, 17);
	volat.set_volsurface_by_term(t7, 7, 17);
	volat.set_volsurface_by_term(t8, 8, 17);
	volat.set_volsurface_by_term(t9, 9, 17);

	std::vector<double> v_rts(std::begin(arr_rts), std::end(arr_rts));
	std::vector<double> v_r(std::begin(arr_r), std::end(arr_r));
	std::vector<double> v_qts(std::begin(arr_qts), std::end(arr_qts));
	std::vector<double> v_q(std::begin(arr_q), std::end(arr_q));

	Rate r(v_r, v_rts);
	Rate q(v_q, v_qts);

	double refprice = 297.22;


	double rf_for_bs = r.getIntpRate(1.0);
	double div_for_bs = q.getIntpRate(1.0);
	double putstrike = 297.22*0.9;
	double callstrike = 297.22*1.0;
	double upbarrier = 297.22*1.2;
	double intpvol = volat.getInpVol(1.0, putstrike);
	double rebate = 0.0;

	double const_vol = volat.getBSVol(1.0, 297.22);
	double const_rf = r.getIntpRate(1.0);
	double const_q = q.getIntpRate(1.0);

	if (fvss == flat) {
		volat.set_const_vol(const_vol);
		r.set_const_rate(const_rf);
		q.set_const_rate(const_q);
	}

	MarketParam para(43340, spot, volat, r, q);

	signed int expiryDate = 44436;
	int nb_autocall = 6;
	signed int auto_date[7] = { -1,43524,43705,43889,44071,44255,44436 };
	double auto_strike[7] = { -1,297.22,	297.22,	282.359	,282.359,	267.498,	267.498 };
	double auto_coupon[7] = { -1, 0.0230,	0.0460,	0.0690,	0.0920,	0.1150,	0.1380 };
	double auto_ki_barrier = refprice*0.6;
	double auto_dummy_coupon = auto_coupon[6];

	double auto_put_strike;
	auto_put_strike = refprice*1.0; // if put_strike=0, notional protected

	PayoffAutocallStd autoPayoff(nb_autocall, auto_date, auto_strike, auto_coupon, auto_ki_barrier, auto_put_strike, auto_dummy_coupon, refprice);
	AutocallOption AutoKOSPI(refprice, expiryDate, autoPayoff, hitflag);

	AutoKOSPI.Calc(para);

	std::vector<double> rs_auto = AutoKOSPI.GetResult();

	std::cout.precision(8);
	std::cout << std::fixed;

	std::cout << "spot " << rs_auto[5] << std::endl;
	std::cout << "pv " << (hitflag ? "hitted  " : "not hit  ") << rs_auto[0] << std::endl;
	std::cout << "delta " << rs_auto[1] << std::endl;
	std::cout << "gamma " << rs_auto[2] << std::endl;

}


void test_autocall_mc(double spot, int hitflag = 0, long nMC=30000, FvsS fvss=surface)
{
	double arr_rts[] = { 0.002739726,0.25,0.5,0.75,1,1.5,2,2.5,3,4,5,7,10,15,20 };
	double arr_r[] = { 1.54,1.64,1.72,1.77,1.8,1.85,1.88,1.90,1.92,1.92,1.92,1.92,1.92,1.92,1.92 };
	for (int i = 0; i<15; i++)
		arr_r[i] /= 100.0;

	double arr_qts[] = { 0.0191780821917808,0.038356164,0.057534247,0.084931507,0.104109589,0.123287671,0.142465753,0.167123288,0.252054795,0.334246575,0.419178082,0.504109589,0.580821918,0.665753425,0.747945205,0.832876712,0.915068493,1,1.084931507,1.167123288,1.252054795,1.334246575,1.419178082,1.504109589,1.583561644,1.668493151,1.750684932,1.835616438,1.917808219,	2.002739726	,2.087671233,2.169863014,2.254794521,2.336986301,2.421917808,2.506849315,2.583561644,2.668493151,2.750684932,2.835616438,2.917808219,3.002739726 };
	double arr_q[] = { 0.00000,0.00000,0.00000,2.57097, 2.09737,1.77112,1.53270, 1.30656 ,0.86631 ,5.16283, 4.11677,3.42318,3.34816,2.92103,2.60139,2.67302,2.43293,2.22630,2.26258, 2.10325,1.96057,3.00733,2.82806 ,2.66837, 2.53449,2.53675,2.41823 ,2.46233 ,2.35680 ,2.25685 ,2.27446 ,2.18831 ,2.10588 ,2.66917,2.57598,2.48871,2.41481,2.42319,2.35115 ,2.38170 ,2.31461 ,2.24914 };
	for (int i = 0; i<42; i++)
		arr_q[i] /= 100.0;

	//double spot=297.22;
	double vol_term[] = { 0.08493150,0.16712330,0.25205480,0.50410960,0.74794520,1.00000000,1.50410960,2.00273970,2.50684930,3.00273970 }; //10terms
	double vol_strike[] = { 178.332,193.193,208.054,222.915,237.776,252.637,267.498,282.359,297.22,312.081,326.942,341.803,356.664,371.525,386.386,401.247,416.108 };//17spot
	Vol volat(10, 17);
	double t0[] = { 0.3095,0.2907,0.2719,0.2528,0.2331,0.2136,0.1816,0.1388,0.1081,0.105,0.1296,0.1606,0.1795,0.1892,0.1978,0.2057,0.2129 };
	double t1[] = { 0.2807,0.2619,0.2431,0.2239,0.2042,0.1845,0.1586,0.1285,0.1095,0.1027,0.1076,0.1306,0.1518,0.1634,0.172,0.1798,0.1869 };
	double t2[] = { 0.2621,0.2448,0.2274,0.2098,0.1916,0.172,0.1505,0.128,0.1131,0.1076,0.1125,0.1264,0.1413,0.1517,0.16,0.1676,0.1745 };
	double t3[] = { 0.2376,0.2219,0.2061,0.1902,0.1741,0.1576,0.141,0.1261,0.1169,0.1161,0.1215,0.1292,0.1371,0.1447,0.1517,0.1583,0.1644 };
	double t4[] = { 0.2286,0.214,0.1997,0.1854,0.1712,0.1571,0.1437,0.1319,0.1238,0.1211,0.1231,0.128,0.1339,0.14,0.146,0.1517,0.1571 };
	double t5[] = { 0.221,0.2077,0.1946,0.1819,0.1694,0.1574,0.1462,0.1366,0.1295,0.1261,0.1261,0.1287,0.1327,0.1374,0.1422,0.1469,0.1516 };
	double t6[] = { 0.2063,0.1949,0.1841,0.1737,0.1639,0.1549,0.1469,0.1403,0.1355,0.1329,0.1322,0.1331,0.1352,0.1379,0.1409,0.1442,0.1475 };
	double t7[] = { 0.1993,0.1897,0.1806,0.1721,0.1642,0.157,0.1506,0.1453,0.1411,0.1383,0.1368,0.1364,0.1369,0.1381,0.1398,0.1418,0.144 };
	double t8[] = { 0.1927,0.1844,0.1768,0.1696,0.1631,0.1572,0.1521,0.1477,0.1443,0.1418,0.1402,0.1394,0.1393,0.1398,0.1407,0.1419,0.1434 };
	double t9[] = { 0.1892,0.182,0.1754,0.1692,0.1637,0.1586,0.1542,0.1505,0.1474,0.145,0.1432,0.1421,0.1415,0.1414,0.1417,0.1423,0.1431 };

	volat.set_vol_strike(vol_strike, 17);
	volat.set_vol_term(vol_term, 10);

	volat.set_volsurface_by_term(t0, 0, 17);
	volat.set_volsurface_by_term(t1, 1, 17);
	volat.set_volsurface_by_term(t2, 2, 17);
	volat.set_volsurface_by_term(t3, 3, 17);
	volat.set_volsurface_by_term(t4, 4, 17);
	volat.set_volsurface_by_term(t5, 5, 17);
	volat.set_volsurface_by_term(t6, 6, 17);
	volat.set_volsurface_by_term(t7, 7, 17);
	volat.set_volsurface_by_term(t8, 8, 17);
	volat.set_volsurface_by_term(t9, 9, 17);

	std::vector<double> v_rts(std::begin(arr_rts), std::end(arr_rts));
	std::vector<double> v_r(std::begin(arr_r), std::end(arr_r));
	std::vector<double> v_qts(std::begin(arr_qts), std::end(arr_qts));
	std::vector<double> v_q(std::begin(arr_q), std::end(arr_q));

	Rate r(v_r, v_rts);
	Rate q(v_q, v_qts);

	double refprice = 297.22;
	double rf_for_bs = r.getIntpRate(1.0);
	double div_for_bs = q.getIntpRate(1.0);
	double putstrike = 297.22*0.9;
	double callstrike = 297.22*1.0;
	double upbarrier = 297.22*1.2;
	double intpvol = volat.getInpVol(1.0, putstrike);
	double rebate = 0.0;

	double const_vol = volat.getBSVol(1.0, 297.22);
	double const_rf = r.getIntpRate(1.0);
	double const_q = q.getIntpRate(1.0);

	if (fvss == flat) {
		volat.set_const_vol(const_vol);
		r.set_const_rate(const_rf);
		q.set_const_rate(const_q);
	}

	MarketParam para(43340, spot, volat, r, q);
	signed int exd = 44436;
	int nb_autocall = 6;
	signed int auto_date[7] = { -1,43524,43705,43889,44071,44255,44436 };
	double auto_strike[7] = { -1,297.22,	297.22,	282.359	,282.359,	267.498,	267.498 };
	double auto_coupon[7] = { -1, 0.0230,	0.0460,	0.0690,	0.0920,	0.1150,	0.1380 };
	double auto_ki_barrier = refprice*0.6;
	double auto_dummy_coupon = auto_coupon[6];


	double auto_put_strike;
	auto_put_strike = refprice; // if put_strike=0, notional protected

	PayoffAutocallStd autoPayoff(nb_autocall, auto_date, auto_strike, auto_coupon, auto_ki_barrier, auto_put_strike, auto_dummy_coupon, refprice);
	AutocallOption AutoKOSPI(refprice, exd, autoPayoff, hitflag);
	AutoKOSPI.CalcMC(para, nMC);
	std::vector<double> rs_auto = AutoKOSPI.GetResult();

	std::cout.precision(8);
	std::cout << std::fixed;

	std::cout << "\npv(MC) " << (hitflag ? "hitted  " : "not hit  ") << rs_auto[10] << std::endl;

	std::cout << "atm vol at ref " << volat.getBSVol(1.0, refprice) << std::endl;


}





void test_dividend()
{
	vector<signed int> divexdate = { 43278,43370,43461,43554,43645,43736,43823,43828,43920,44011,44103,44192,44194,44285,44376,44468,44557,44559,44650,44741,44833,44922,44924 };
	vector<double> divamount = { 0.827,0.665,4.614,0.653,0.832,0.665,0.054,4.801,0.656,0.862,0.668,0.054,4.922,0.656,0.862,0.668,0.054,4.922,0.656,0.862,0.668,0.054,4.922 };
	Dividend div(divexdate, divamount);
	double arr_rts[] = { 0.002739726,0.25,0.5,0.75,1,1.5,2,2.5,3,4,5,7,10,15,20 };
	std::vector<double> v_rts(std::begin(arr_rts), std::end(arr_rts));
	signed int vd = 43340;
	double spot = 297.22;
	Rate ra = div.getDividendCurve(v_rts,vd,spot);
	ra.print();

	//cout << di.getLumpsum(3, 8);

}

void test_calc2_discrete_localvol()
{


	double arr_rts[] = { 0.002739726,0.25,0.5,0.75,1,1.5,2,2.5,3,4,5,7,10,15,20 };
	double arr_r[] = { 1.79000,1.81000,1.74000,1.69000,1.65000,1.59000,1.55000,1.52000,1.50000,1.50000,1.50000,1.50000,1.50000,1.50000 ,1.50000 };

	for (int i = 0; i<15; i++)
		arr_r[i] /= 100.0;

	double arr_qts[] = { 0.01918000,0.03836000,0.05753000,0.08493000,0.10411000,0.12329000,0.14247000,0.16712000,0.25205000,0.33425000,0.41918000,0.50411000,0.58082000,0.66575000,0.74795000,0.83288000,0.91507000,1.00000000,1.08493000,1.16712000,1.25205000,1.33425000,1.41918000,1.50411000,1.58356000,1.66849000,1.75068000,1.83562000,1.91781000,2.00274000,2.08767000,2.16986000,2.25479000,2.33699000,2.42192000,2.50685000,2.58356000,2.66849000,2.75068000,2.83562000,2.91781000,3.00274000 };
	double arr_q[] = { 0.00000,0.00000,0.00000,2.63440,2.14907,1.81475,1.570440,1.338800,0.887680,5.313760,4.237140,3.523290,3.057960,2.997860,2.668400,2.396300,2.486970,2.275760,2.303830,2.141590,1.996320,1.886950,2.912220,2.747780,2.609920,2.609350,2.486850,2.371770,2.421350,2.318670,2.224340,2.243670,2.159160,2.090990,2.701420,2.609900,2.532410,2.534520,2.458790,2.385140,2.417350,2.348970 };
	for (int i = 0; i<42; i++)
		arr_q[i] /= 100.0;

	//double spot=297.22;
	double vol_term[] = { 0.08333,0.16667,0.250000,0.500000,0.750000,1.000000,1.500000,2.000000,2.500000,3.000000 };
	double vol_strike[] = { 178.332,193.193,208.054,222.915,237.776,252.637,267.498,282.359,297.22,312.081,326.942,341.803,356.664,371.525,386.386,401.247,416.108 };//17spot
	Vol volat(10, 17);
	double t0[] = { 0.3484,	0.3310,	0.3135,	0.2960,	0.2778,	0.2577, 0.2279, 0.1924, 0.1641, 0.1532, 0.1618, 0.1729, 0.1807, 0.1883, 0.1968,	0.2034,	0.2090 };
	double t1[] = { 0.3182,	0.3002,	0.2822,	0.2640,	0.2454,	0.2252,	0.2010,	0.1770,	0.1568,	0.1452,	0.1429,	0.1484,	0.1565,	0.1645,	0.1722,	0.1792,	0.1855 };
	double t2[] = { 0.3069,	0.2883,	0.2697,	0.2509,	0.2318,	0.2118,	0.1907,	0.1699,	0.1527,	0.1432,	0.1423,	0.1476,	0.1554,	0.1634,	0.1711,	0.1782,	0.1849 };
	double t3[] = { 0.2709,	0.2543,	0.2380,	0.2218,	0.2058,	0.1900,	0.1748,	0.1607,	0.1491,	0.1418,	0.1395,	0.1411,	0.1451,	0.1502,	0.1555,	0.1609,	0.1662 };
	double t4[] = { 0.2441,	0.2300,	0.2163,	0.2029,	0.1901,	0.1778,	0.1664,	0.1562,	0.1479,	0.1423,	0.1396,	0.1394,	0.1410,	0.1437,	0.1470,	0.1506,	0.1543 };
	double t5[] = { 0.2277,	0.2156,	0.2041,	0.1930,	0.1826,	0.1727,	0.1636,	0.1556,	0.1490,	0.1440,	0.1409,	0.1395,	0.1396,	0.1406,	0.1424,	0.1446,	0.1471 };
	double t6[] = { 0.2073, 0.1981,	0.1895,	0.1814,	0.1739,	0.1669,	0.1606,	0.1550,	0.1503,	0.1465,	0.1436,	0.1416,	0.1404,	0.1400,	0.1401,	0.1407,	0.1417 };
	double t7[] = { 0.1944,	0.1871,	0.1802,	0.1739,	0.1681,	0.1629,	0.1581,	0.1539,	0.1503,	0.1473,	0.1449,	0.1431,	0.1418,	0.1410,	0.1406,	0.1405,	0.1407 };
	double t8[] = { 0.1876,	0.1815,	0.1760,	0.1709,	0.1662,	0.1619,	0.1580,	0.1546,	0.1516,	0.1490,	0.1468,	0.1451,	0.1436,	0.1426,	0.1418,	0.1413,	0.1410 };
	double t9[] = { 0.1821,	0.1770,	0.1723,	0.1680,	0.1641,	0.1606,	0.1573,	0.1545,	0.1520,	0.1497,	0.1478,	0.1462,	0.1449,	0.1438,	0.1429,	0.1422,	0.1418 };

	volat.set_vol_strike(vol_strike, 17);
	volat.set_vol_term(vol_term, 10);

	volat.set_volsurface_by_term(t0, 0, 17);
	volat.set_volsurface_by_term(t1, 1, 17);
	volat.set_volsurface_by_term(t2, 2, 17);
	volat.set_volsurface_by_term(t3, 3, 17);
	volat.set_volsurface_by_term(t4, 4, 17);
	volat.set_volsurface_by_term(t5, 5, 17);
	volat.set_volsurface_by_term(t6, 6, 17);
	volat.set_volsurface_by_term(t7, 7, 17);
	volat.set_volsurface_by_term(t8, 8, 17);
	volat.set_volsurface_by_term(t9, 9, 17);

	std::vector<double> v_rts(std::begin(arr_rts), std::end(arr_rts));
	std::vector<double> v_r(std::begin(arr_r), std::end(arr_r));
	std::vector<double> v_qts(std::begin(arr_qts), std::end(arr_qts));
	std::vector<double> v_q(std::begin(arr_q), std::end(arr_q));

	Rate r(v_r, v_rts);
	Rate q(v_q, v_qts);


	//double intpvol = volat.getInpVol(1.0, putstrike);  //flat interpolation use .getBSVol

	//상품타입변경
	//bool is_flat_vol = true;
	//int hitflag = 1;
	//long nM = 30000;

	//***상품타입변경
	double spot = 297.22;
	signed int vd = 43340;
	signed int exd = 43705;
	double refprice = 297.22;
	double putstrike = spot;

	double rf_for_bs = r.getIntpRate(1.0);
	double div_for_bs = q.getIntpRate(1.0);
	double intpvol = volat.getInpVol(1.0, putstrike);

	vector<signed int> divexdate = { 43278,43370,43461,43554,43645,43736,43823,43828,43920,44011,44103,44192,44194,44285,44376,44468,44557,44559,44650,44741,44833,44922,44924};
	vector<double> divamount = { 0.827,0.665,4.614,0.653,0.832,0.665,0.054,4.801,0.656,0.862,0.668,0.054,4.922,0.656,0.862,0.668,0.054,4.922,0.656,0.862,0.668,0.054,4.922};
	Dividend div(divexdate, divamount);
	MarketParameters paras(43340, spot, volat, r, div);
	//MarketParam para(43340, spot, volat, r, q);

	PayoffPut putpay(putstrike);
	//PayoffCall callpay(callstrike);
	
	EuropeanOption EurPut_discrete(refprice, 43705, putpay);
	//EuropeanOption EurCall_discrete(refprice, 43705, callpay);

	//EuropeanOptionMC EurPutMC(refprice, 43705, putpay);

	EurPut_discrete.Calc(paras);
	//EurCall_discrete.Calc(paras);

	//EurPutMC.Calc(para, numMC);
	/*EuropeanVanilla Vput(putpay,refprice, ed);

	EuropeanPut eput(refprice,putstrike,365);
	EuropeanCall ecall(refprice,callstrike,365);

	*/
	std::cout.precision(8);
	std::cout << std::fixed;

	//cout << volat.getBSVol(1.0, putstrike);

	//std::cout <<"eput:  s=" <<spot<<" "<< eput.pv(para)<<" vol " << volat.getInpVol(365/365,putstrike)<< "r " << r.getIntpRate(1.0) << " q " << q.getIntpRate(1.0) <<  std::endl;
	//std::cout <<"ecall: "<< ecall.pv(para)<< "s= " <<spot<<" "<<" vol " <<volat.getInpVol(365/365,callstrike)<< " r " << r.getIntpRate(1.0) << " q " << q.getIntpRate(1.0) << std::endl;
	std::vector<double> rs = EurPut_discrete.GetResult();
	//vector<double> rs_call = EurCall_discrete.GetResult();

	std::cout << "pv put " << rs[0] << std::endl;
	std::cout << "delta put " << rs[1] << std::endl;
	std::cout << "gamma put " << rs[2] << std::endl;
	std::cout << "spot " << rs[5] << std::endl;
	std::cout << "BS vol= " << volat.getBSVol(1.0, putstrike) << std::endl;
	std::cout << "implied vol= " << put_iv(spot, putstrike, rf_for_bs, div_for_bs, 0.1, (43705 - 43340) / 365.0, rs[0]) << std::endl;
	cout << "closed form = " << k_fmla_BSPut(spot, putstrike, rf_for_bs, div_for_bs, volat.getBSVol(1.0, putstrike), 1.0) << endl;
	std::cout << "delta(closed, Flatvol)= " << k_fmla_BSPutDelta(spot, putstrike, rf_for_bs, div_for_bs, volat.getBSVol(1.0, putstrike), 1.0) << std::endl;
	std::cout << "gamma(closed, Flatvol)= " << k_fmla_BSGamma(spot, putstrike, rf_for_bs, div_for_bs, volat.getBSVol(1.0, putstrike), 1.0) << std::endl;
	std::cout << "\n";

	//std::cout << "pv call " << rs_call[0] << std::endl;
	//std::cout << "delta call " << rs_call[1] << std::endl;
	//std::cout << "gamma call " << rs_call[2] << std::endl;
	//std::cout << "spot " << rs_call[5] << std::endl;
	//std::cout << "BS vol= " << volat.getBSVol(1.0, callstrike) << std::endl;
	//std::cout << "implied vol call= " << call_iv(spot, callstrike, rf_for_bs, div_for_bs, 0.1, (43705 - 43340) / 365.0, rs_call[0]) << std::endl;
	//cout << "closed form call= " << k_fmla_BSCall(spot, callstrike, rf_for_bs, div_for_bs, volat.getBSVol(1.0, callstrike), 1.0) << endl;
	//std::cout << "delta call(closed, Flatvol)= " << k_fmla_BSCallDelta(spot, callstrike, rf_for_bs, div_for_bs, volat.getBSVol(1.0, callstrike), 1.0) << std::endl;
	//std::cout << "gamma call(closed, Flatvol)= " << k_fmla_BSGamma(spot, callstrike, rf_for_bs, div_for_bs, volat.getBSVol(1.0, callstrike), 1.0) << std::endl;
	//std::cout << "\n";

}

void test_autocall__fd_mc_old(long nM)
{
	double arr_rts[] = { 0.002739726,0.25,0.5,0.75,1,1.5,2,2.5,3,4,5,7,10,15,20 };
	double arr_r[] = { 1.79000,1.81000,1.74000,1.69000,1.65000,1.59000,1.55000,1.52000,1.50000,1.50000,1.50000,1.50000,1.50000,1.50000 ,1.50000 };

	for (int i = 0; i<15; i++)
		arr_r[i] /= 100.0;

	double arr_qts[] = { 0.01918000 ,	0.03836000 ,	0.05753000 ,	0.07671000 	,0.09589000 ,	0.11507000 	,0.13425000 ,	0.16438000 ,	0.24658000, 	0.32877000 ,	0.41096000 	,0.49315000 	,0.57534000 ,	0.65753000 ,	0.73973000 ,	0.82192000 ,	0.90411000 ,	0.98630000 ,	1.06849000 ,	1.15068000 	,1.23288000 ,	1.31507000 ,	1.39726000 ,	1.47945000 	,1.56164000 	,1.64384000 ,	1.72603000 ,	1.80822000 ,	1.89041000 ,	1.97260000 ,	2.05479000 ,2.13699000 	,2.21918000, 	2.30137000 ,	2.38356000 	,2.46575000 ,	2.54795000 ,	2.63014000 ,	2.71233000 ,	2.79452000 ,	2.87671000, 	2.95890000 };
	double arr_q[] = { 0.00000,0.00000,0.00000,0.00000,2.33330,1.94438,1.666590,1.361110,0.907370,0.680540,4.321890,3.601590,3.087090,3.035340,2.698050,2.428250,2.517120,2.307370,2.129880,2.172190,2.027360,1.900660,2.957910,2.793580,2.646550,2.648480,2.522360,2.407710,2.456450,2.354100,2.259940,2.278180,2.193800,2.115460,2.744900,2.653400,2.567800,2.571480,2.493550,2.420220,2.451880,2.383780 };
	for (int i = 0; i<42; i++)
		arr_q[i] /= 100.0;

	//double spot=297.22;
	double vol_term[] = { 0.08333,0.16667,0.250000,0.500000,0.750000,1.000000,1.500000,2.000000,2.500000,3.000000 };
	double vol_strike[] = { 178.332,193.193,208.054,222.915,237.776,252.637,267.498,282.359,297.22,312.081,326.942,341.803,356.664,371.525,386.386,401.247,416.108 };//17spot
	Vol volat(10, 17);
	double t0[] = { 0.3484,	0.3310,	0.3135,	0.2960,	0.2778,	0.2577, 0.2279, 0.1924, 0.1641, 0.1532, 0.1618, 0.1729, 0.1807, 0.1883, 0.1968,	0.2034,	0.2090 };
	double t1[] = { 0.3182,	0.3002,	0.2822,	0.2640,	0.2454,	0.2252,	0.2010,	0.1770,	0.1568,	0.1452,	0.1429,	0.1484,	0.1565,	0.1645,	0.1722,	0.1792,	0.1855 };
	double t2[] = { 0.3069,	0.2883,	0.2697,	0.2509,	0.2318,	0.2118,	0.1907,	0.1699,	0.1527,	0.1432,	0.1423,	0.1476,	0.1554,	0.1634,	0.1711,	0.1782,	0.1849 };
	double t3[] = { 0.2709,	0.2543,	0.2380,	0.2218,	0.2058,	0.1900,	0.1748,	0.1607,	0.1491,	0.1418,	0.1395,	0.1411,	0.1451,	0.1502,	0.1555,	0.1609,	0.1662 };
	double t4[] = { 0.2441,	0.2300,	0.2163,	0.2029,	0.1901,	0.1778,	0.1664,	0.1562,	0.1479,	0.1423,	0.1396,	0.1394,	0.1410,	0.1437,	0.1470,	0.1506,	0.1543 };
	double t5[] = { 0.2277,	0.2156,	0.2041,	0.1930,	0.1826,	0.1727,	0.1636,	0.1556,	0.1490,	0.1440,	0.1409,	0.1395,	0.1396,	0.1406,	0.1424,	0.1446,	0.1471 };
	double t6[] = { 0.2073, 0.1981,	0.1895,	0.1814,	0.1739,	0.1669,	0.1606,	0.1550,	0.1503,	0.1465,	0.1436,	0.1416,	0.1404,	0.1400,	0.1401,	0.1407,	0.1417 };
	double t7[] = { 0.1944,	0.1871,	0.1802,	0.1739,	0.1681,	0.1629,	0.1581,	0.1539,	0.1503,	0.1473,	0.1449,	0.1431,	0.1418,	0.1410,	0.1406,	0.1405,	0.1407 };
	double t8[] = { 0.1876,	0.1815,	0.1760,	0.1709,	0.1662,	0.1619,	0.1580,	0.1546,	0.1516,	0.1490,	0.1468,	0.1451,	0.1436,	0.1426,	0.1418,	0.1413,	0.1410 };
	double t9[] = { 0.1821,	0.1770,	0.1723,	0.1680,	0.1641,	0.1606,	0.1573,	0.1545,	0.1520,	0.1497,	0.1478,	0.1462,	0.1449,	0.1438,	0.1429,	0.1422,	0.1418 };

	volat.set_vol_strike(vol_strike, 17);
	volat.set_vol_term(vol_term, 10);

	volat.set_volsurface_by_term(t0, 0, 17);
	volat.set_volsurface_by_term(t1, 1, 17);
	volat.set_volsurface_by_term(t2, 2, 17);
	volat.set_volsurface_by_term(t3, 3, 17);
	volat.set_volsurface_by_term(t4, 4, 17);
	volat.set_volsurface_by_term(t5, 5, 17);
	volat.set_volsurface_by_term(t6, 6, 17);
	volat.set_volsurface_by_term(t7, 7, 17);
	volat.set_volsurface_by_term(t8, 8, 17);
	volat.set_volsurface_by_term(t9, 9, 17);

	std::vector<double> v_rts(std::begin(arr_rts), std::end(arr_rts));
	std::vector<double> v_r(std::begin(arr_r), std::end(arr_r));
	std::vector<double> v_qts(std::begin(arr_qts), std::end(arr_qts));
	std::vector<double> v_q(std::begin(arr_q), std::end(arr_q));

	Rate r(v_r, v_rts);
	Rate q(v_q, v_qts);


	//double intpvol = volat.getInpVol(1.0, putstrike);  //flat interpolation use .getBSVol

	//상품타입변경
	//bool is_flat_vol = true;
	//int hitflag = 1;

	double refprice = 297.22;


	//상품타입변경
	bool is_flat_vol = false;
	int hitflag = 0;
	
	double spot = 297.22;

	
	MarketParam para(43340, spot, volat, r, q);

	signed int expiryDate = 44436;
	int nb_autocall = 6;
	signed int auto_date[7] = { -1,43524,43705,43889,44071,44255,44436 };
	double auto_strike[7] = { -1,297.22,	297.22,	282.359	,282.359,	267.498,	267.498 };
	double auto_coupon[7] = { -1, 0.0230,	0.0460,	0.0690,	0.0920,	0.1150,	0.1380 };
	double auto_ki_barrier = refprice*0.6;
	double auto_dummy_coupon = auto_coupon[6];

	double auto_put_strike;
	auto_put_strike = refprice*1.0; // if put_strike=0, notional protected

	PayoffAutocallStd autoPayoff(nb_autocall, auto_date, auto_strike, auto_coupon, auto_ki_barrier, auto_put_strike, auto_dummy_coupon, refprice);
	AutocallOption AutoKOSPI(refprice, expiryDate, autoPayoff, hitflag);
	AutocallOption AutoKOSPI_mc(refprice, expiryDate, autoPayoff, hitflag);

	//long nM = 1000;
	AutoKOSPI.Calc(para);
	//AutoKOSPI_mc.CalcMC(para, nM);
	AutoKOSPI_mc.CalcMC_calc2(para, nM);


	//	AutoKOSPI.Calc(para);

	std::vector<double> rs = AutoKOSPI.GetResult();
	std::vector<double> rs_mc = AutoKOSPI_mc.GetResult();

	std::cout.precision(8);
	std::cout << std::fixed;

	cout << "\ntest_autocall__fd_mc\n";
	std::cout << "spot " << rs[5] << std::endl;

	std::cout << "pv(FD) " << (hitflag ? "hitted " : "not hit ")<<(is_flat_vol ? "flatvol" : "surfacevol ") <<"->" << rs[0] << std::endl;
	std::cout << "pv(MC) " << (hitflag ? "hitted " : "not hit ")<<(is_flat_vol ? "flatvol" : "surfacevol ") <<"nMC= " << nM << "-> "<<  rs_mc[0] << std::endl;
}

void test_autocall__fd_mc(MarketParam& para, long nM)
{//setpara
	double spot = para.get_spot();

	//make autocall product
	double refprice = 297.22;
	bool is_flat_vol = false;
	int hitflag = 0;
	signed int expiryDate = 44436;
	int nb_autocall = 6;
	signed int auto_date[7] = { -1,43524,43705,43889,44071,44255,44436 };
	double auto_strike[7] = { -1,297.22,	297.22,	282.359	,282.359,	267.498,	267.498 };
	double auto_coupon[7] = { -1, 0.0230,	0.0460,	0.0690,	0.0920,	0.1150,	0.1380 };
	double auto_ki_barrier = refprice*0.6;
	double auto_dummy_coupon = auto_coupon[6];

	double auto_put_strike=refprice*1.0; // if put_strike=0, notional protected

	PayoffAutocallStd autoPayoff(nb_autocall, auto_date, auto_strike, auto_coupon, auto_ki_barrier, auto_put_strike, auto_dummy_coupon, refprice);
	AutocallOption AutoKOSPI(refprice, expiryDate, autoPayoff, hitflag);
	AutocallOption AutoKOSPI_mc(refprice, expiryDate, autoPayoff, hitflag);

	//long nM = 10000;
	AutoKOSPI.Calc(para);
	//AutoKOSPI_mc.CalcMC(para, nM);
	AutoKOSPI_mc.CalcMC_calc2(para, nM);


	//	AutoKOSPI.Calc(para);

	std::vector<double> rs = AutoKOSPI.GetResult();
	std::vector<double> rs_mc = AutoKOSPI_mc.GetResult();

	std::cout.precision(8);
	std::cout << std::fixed;

	cout << "\ntest_autocall__fd_mc\n";
	std::cout << "spot " << rs[5] << std::endl;

	std::cout << "pv(FD) " << (AutoKOSPI.GetHitFlag() ? "hitted " : "not hit ") << "->" << rs[0] << std::endl;
	std::cout << "pv(MC) " << (AutoKOSPI_mc.GetHitFlag() ? "hitted " : "not hit ")  << "nMC= " << nM << "-> " << rs_mc[0] << std::endl;
}

void test_autocall__fd_mc(MarketParameters& paras, long nM)
{//setpara
	double spot = paras.get_spot();

	//make autocall product
	double refprice = 297.22;
	bool is_flat_vol = false;
	int hitflag = 0;
	signed int expiryDate = 44436;
	int nb_autocall = 6;
	signed int auto_date[7] = { -1,43524,43705,43889,44071,44255,44436 };
	double auto_strike[7] = { -1,297.22,	297.22,	282.359	,282.359,	267.498,	267.498 };
	double auto_coupon[7] = { -1, 0.0230,	0.0460,	0.0690,	0.0920,	0.1150,	0.1380 };
	double auto_ki_barrier = refprice*0.6;
	double auto_dummy_coupon = auto_coupon[6];

	double auto_put_strike = refprice*1.0; // if put_strike=0, notional protected

	PayoffAutocallStd autoPayoff(nb_autocall, auto_date, auto_strike, auto_coupon, auto_ki_barrier, auto_put_strike, auto_dummy_coupon, refprice);
	AutocallOption AutoKOSPI(refprice, expiryDate, autoPayoff, hitflag);
	AutocallOption AutoKOSPI_mc(refprice, expiryDate, autoPayoff, hitflag);

	//long nM = 10000;
	AutoKOSPI.Calc(paras);
	//AutoKOSPI_mc.CalcMC(para, nM);
	AutoKOSPI_mc.CalcMC_calc2(paras, nM);


	//	AutoKOSPI.Calc(para);

	std::vector<double> rs = AutoKOSPI.GetResult();
	std::vector<double> rs_mc = AutoKOSPI_mc.GetResult();

	std::cout.precision(8);
	std::cout << std::fixed;

	std::cout << "spot " << rs[5] << std::endl;

	std::cout << "pv(FD) " << (hitflag ? "hitted " : "not hit ") << (is_flat_vol ? "flatvol" : "surfacevol ") << "->" << rs[0] << std::endl;
	std::cout << "pv(MC) " << (hitflag ? "hitted " : "not hit ") << (is_flat_vol ? "flatvol" : "surfacevol ") << "nMC= " << nM << "-> " << rs_mc[0] << std::endl;
}


void test_autocall__fd_mc_inst(MarketParameters& paras, AutocallOption& autoop, long nM)
{//setpara
	double spot = paras.get_spot();
	//long nM = 10000;
	autoop.Calc(paras);
	std::vector<double> rs = autoop.GetResult();

	autoop.CalcMC_calc2(paras, nM);
	std::vector<double> rs_mc = autoop.GetResult();

	std::cout.precision(8);
	std::cout << std::fixed;

	std::cout << "spot " << rs[5] << std::endl;

	std::cout << "pv(FD) " << (autoop.GetHitFlag() ? "hitted " : "not hit ")  << "->" << rs[0] << std::endl;
	std::cout << "pv(MC) " << (autoop.GetHitFlag() ? "hitted " : "not hit ") << "nMC= " << nM << "-> " << rs_mc[0] << std::endl;
}

void test_autocall__fd_inst_swip_first(MarketParameters& paras, AutocallOption& autoop)
{//setpara
	double spot = paras.get_spot();
	//long nM = 10000;
	autoop.Calc(paras);
	std::vector<double> rs = autoop.GetResult();

	//autoop.CalcMC_calc2(paras, nM);
	//std::vector<double> rs_mc = autoop.GetResult();

	std::cout.precision(8);
	std::cout << std::fixed;

	std::cout << "spot " << rs[5] << std::endl;

	std::cout << "pv(FD) " << (autoop.GetHitFlag() ? "hitted " : "not hit ") << "->" << rs[0] << std::endl;
	//std::cout << "pv(MC) " << (autoop.GetHitFlag() ? "hitted " : "not hit ") << "nMC= " << nM << "-> " << rs_mc[0] << std::endl;
}

void test_autocall_fd_mc_inst_swip_first_time(MarketParameters& paras, AutocallOption& autoop, long nM=1000)
{//setpara
	double spot = paras.get_spot();
	autoop.Calc(paras);
	std::vector<double> rs = autoop.GetResult();

	autoop.CalcMC_calc2(paras, nM);
	std::vector<double> rs_mc = autoop.GetResult();

	std::cout.precision(8);
	std::cout << std::fixed;

	std::cout << "spot " << rs[5] << std::endl;
	std::cout << "spot " << rs_mc[5] << std::endl;

	std::cout << "pv(FD) " << (autoop.GetHitFlag() ? "hitted " : "not hit ") << "->" << rs[0] << std::endl;
	std::cout << "pv(MC) " << (autoop.GetHitFlag() ? "hitted " : "not hit ") << "nMC= " << nM << "-> " << rs_mc[0] << std::endl;
}

void test_autocall__fd_mc_inst_swip_first(MarketParam& para, AutocallOption& autoop)
{
	double spot = para.get_spot();
	//long nM = 10000;
	autoop.Calc(para);
	std::vector<double> rs = autoop.GetResult();

	//autoop.CalcMC_calc2(para, nM);
	std::vector<double> rs_mc = autoop.GetResult();

	std::cout.precision(8);
	std::cout << std::fixed;

	std::cout << "spot " << rs[5] << std::endl;

	std::cout << "pv(FD) " << (autoop.GetHitFlag() ? "hitted " : "not hit ") << "->" << rs[0] << std::endl;
	//std::cout << "pv(MC) " << (autoop.GetHitFlag() ? "hitted " : "not hit ") << "nMC= " << nM << "-> " << rs_mc[0] << std::endl;
}

void test_autocall_calc2()
{
	double arr_rts[] = { 0.002739726,0.25,0.5,0.75,1,1.5,2,2.5,3,4,5,7,10,15,20 };
	double arr_r[] = { 1.54,1.64,1.72,1.77,1.8,1.85,1.88,1.90,1.92,1.92,1.92,1.92,1.92,1.92,1.92 };
	for (int i = 0; i<15; i++)
		arr_r[i] /= 100.0;

	double arr_qts[] = { 0.0191780821917808,0.038356164,0.057534247,0.084931507,0.104109589,0.123287671,0.142465753,0.167123288,0.252054795,0.334246575,0.419178082,0.504109589,0.580821918,0.665753425,0.747945205,0.832876712,0.915068493,1,1.084931507,1.167123288,1.252054795,1.334246575,1.419178082,1.504109589,1.583561644,1.668493151,1.750684932,1.835616438,1.917808219,	2.002739726	,2.087671233,2.169863014,2.254794521,2.336986301,2.421917808,2.506849315,2.583561644,2.668493151,2.750684932,2.835616438,2.917808219,3.002739726 };
	double arr_q[] = { 0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,4.5095,3.5958,2.9900,2.9722,2.5930,2.3094,2.4108,2.1943,2.0079,2.0613,1.9162,1.7862,2.8437,2.6742,2.5232,2.3966,2.4059,2.2935,2.3434,2.2429,2.1478,2.1699,2.0877,2.0090,2.5757,2.4858,2.4016,2.330293238,2.341357773,2.27176383,2.304693379,2.239772439,2.176421211 };
	for (int i = 0; i<42; i++)
		arr_q[i] /= 100.0;

	//double spot=297.22;
	double vol_term[] = { 0.08493150,0.16712330,0.25205480,0.50410960,0.74794520,1.00000000,1.50410960,2.00273970,2.50684930,3.00273970 }; //10terms
	double vol_strike[] = { 178.332,193.193,208.054,222.915,237.776,252.637,267.498,282.359,297.22,312.081,326.942,341.803,356.664,371.525,386.386,401.247,416.108 };//17spot
	Vol volat(10, 17);
	double t0[] = { 0.3095,0.2907,0.2719,0.2528,0.2331,0.2136,0.1816,0.1388,0.1081,0.105,0.1296,0.1606,0.1795,0.1892,0.1978,0.2057,0.2129 };
	double t1[] = { 0.2807,0.2619,0.2431,0.2239,0.2042,0.1845,0.1586,0.1285,0.1095,0.1027,0.1076,0.1306,0.1518,0.1634,0.172,0.1798,0.1869 };
	double t2[] = { 0.2621,0.2448,0.2274,0.2098,0.1916,0.172,0.1505,0.128,0.1131,0.1076,0.1125,0.1264,0.1413,0.1517,0.16,0.1676,0.1745 };
	double t3[] = { 0.2376,0.2219,0.2061,0.1902,0.1741,0.1576,0.141,0.1261,0.1169,0.1161,0.1215,0.1292,0.1371,0.1447,0.1517,0.1583,0.1644 };
	double t4[] = { 0.2286,0.214,0.1997,0.1854,0.1712,0.1571,0.1437,0.1319,0.1238,0.1211,0.1231,0.128,0.1339,0.14,0.146,0.1517,0.1571 };
	double t5[] = { 0.221,0.2077,0.1946,0.1819,0.1694,0.1574,0.1462,0.1366,0.1295,0.1261,0.1261,0.1287,0.1327,0.1374,0.1422,0.1469,0.1516 };
	double t6[] = { 0.2063,0.1949,0.1841,0.1737,0.1639,0.1549,0.1469,0.1403,0.1355,0.1329,0.1322,0.1331,0.1352,0.1379,0.1409,0.1442,0.1475 };
	double t7[] = { 0.1993,0.1897,0.1806,0.1721,0.1642,0.157,0.1506,0.1453,0.1411,0.1383,0.1368,0.1364,0.1369,0.1381,0.1398,0.1418,0.144 };
	double t8[] = { 0.1927,0.1844,0.1768,0.1696,0.1631,0.1572,0.1521,0.1477,0.1443,0.1418,0.1402,0.1394,0.1393,0.1398,0.1407,0.1419,0.1434 };
	double t9[] = { 0.1892,0.182,0.1754,0.1692,0.1637,0.1586,0.1542,0.1505,0.1474,0.145,0.1432,0.1421,0.1415,0.1414,0.1417,0.1423,0.1431 };

	volat.set_vol_strike(vol_strike, 17);
	volat.set_vol_term(vol_term, 10);

	volat.set_volsurface_by_term(t0, 0, 17);
	volat.set_volsurface_by_term(t1, 1, 17);
	volat.set_volsurface_by_term(t2, 2, 17);
	volat.set_volsurface_by_term(t3, 3, 17);
	volat.set_volsurface_by_term(t4, 4, 17);
	volat.set_volsurface_by_term(t5, 5, 17);
	volat.set_volsurface_by_term(t6, 6, 17);
	volat.set_volsurface_by_term(t7, 7, 17);
	volat.set_volsurface_by_term(t8, 8, 17);
	volat.set_volsurface_by_term(t9, 9, 17);

	std::vector<double> v_rts(std::begin(arr_rts), std::end(arr_rts));
	std::vector<double> v_r(std::begin(arr_r), std::end(arr_r));
	std::vector<double> v_qts(std::begin(arr_qts), std::end(arr_qts));
	std::vector<double> v_q(std::begin(arr_q), std::end(arr_q));

	Rate r(v_r, v_rts);
	Rate q(v_q, v_qts);

	double refprice = 297.22;


	double rf_for_bs = r.getIntpRate(1.0);
	double div_for_bs = q.getIntpRate(1.0);
	double putstrike = 297.22*0.9;
	double callstrike = 297.22*1.0;
	double upbarrier = 297.22*1.2;
	double intpvol = volat.getInpVol(1.0, putstrike);
	double rebate = 0.0;

	double const_vol = volat.getBSVol(1.0, 297.22);
	double const_rf = r.getIntpRate(1.0);
	double const_q = q.getIntpRate(1.0);

	//상품타입변경
	bool is_flat_vol = false;
	int hitflag = 1;
	long nM = 30000;
	double spot = 297.22;
	//***상품타입변경

	if (is_flat_vol) {
		volat.set_const_vol(const_vol);
		r.set_const_rate(const_rf);
		q.set_const_rate(const_q);
	}


	MarketParam para(43340, spot, volat, r, q);

	signed int expiryDate = 44436;
	int nb_autocall = 6;
	signed int auto_date[7] = { -1,43524,43705,43889,44071,44255,44436 };
	double auto_strike[7] = { -1,297.22,	297.22,	282.359	,282.359,	267.498,	267.498 };
	double auto_coupon[7] = { -1, 0.0230,	0.0460,	0.0690,	0.0920,	0.1150,	0.1380 };
	double auto_ki_barrier = refprice*0.6;
	double auto_dummy_coupon = auto_coupon[6];

	double auto_put_strike;
	auto_put_strike = refprice*1.0; // if put_strike=0, notional protected

	PayoffAutocallStd autoPayoff(nb_autocall, auto_date, auto_strike, auto_coupon, auto_ki_barrier, auto_put_strike, auto_dummy_coupon, refprice);
	AutocallOption AutoKOSPI(refprice, expiryDate, autoPayoff, hitflag);
	AutocallOption AutoKOSPI_mc(refprice, expiryDate, autoPayoff, hitflag);


	AutoKOSPI.Calc(para);
	AutoKOSPI_mc.CalcMC(para, nM);

	//	AutoKOSPI.Calc(para);

	std::vector<double> rs = AutoKOSPI.GetResult();
	std::vector<double> rs_mc = AutoKOSPI_mc.GetResult();

	std::cout.precision(8);
	std::cout << std::fixed;

	std::cout << "spot " << rs[5] << std::endl;

	std::cout << "pv(FD) " << (hitflag ? "hitted " : "not hit ") << (is_flat_vol ? "flatvol" : "surfacevol ") << "->" << rs[0] << std::endl;
	std::cout << "pv(MC) " << (hitflag ? "hitted " : "not hit ") << (is_flat_vol ? "flatvol" : "surfacevol ") << "nMC= " << nM << "-> " << rs_mc[0] << std::endl;


}
void test_Cal1_vs_Calc2()
{
	//Test Clac1 vs Calc2, for loop improvment
	//using namespace std::chrono;
	//double spot = 297.22;
	//auto ms3_1 = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	//test_autocall(spot, 1, surface, calc1);//pv hitted 0.98996 
	//auto  ms4_1 = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	//std::cout << "duration : " << (ms4_1.count() - ms3_1.count()) / 1000.0 << std::endl <<endl;

	//auto ms3_2 = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	//test_autocall(spot, 1, surface, calc2);  //pv hitted 0.98996 
	//auto  ms4_2 = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	//std::cout << "duration : " << (ms4_2.count() - ms3_2.count()) / 1000.0 << std::endl << endl;

	//auto ms11 = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	//test_autocall_mc(spot, 1, 10000, surface);//
	//auto ms22 = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	//std::cout << "duration : " << (ms22.count() - ms11.count()) / 1000.0 << std::endl << endl;
	double arr_rts[] = { 0.002739726,0.25,0.5,0.75,1,1.5,2,2.5,3,4,5,7,10,15,20 };
	double arr_r[] = { 1.54,1.64,1.72,1.77,1.8,1.85,1.88,1.90,1.92,1.92,1.92,1.92,1.92,1.92,1.92 };
	for (int i = 0; i<15; i++)
		arr_r[i] /= 100.0;

	double arr_qts[] = { 0.0191780821917808,0.038356164,0.057534247,0.084931507,0.104109589,0.123287671,0.142465753,0.167123288,0.252054795,0.334246575,0.419178082,0.504109589,0.580821918,0.665753425,0.747945205,0.832876712,0.915068493,1,1.084931507,1.167123288,1.252054795,1.334246575,1.419178082,1.504109589,1.583561644,1.668493151,1.750684932,1.835616438,1.917808219,	2.002739726	,2.087671233,2.169863014,2.254794521,2.336986301,2.421917808,2.506849315,2.583561644,2.668493151,2.750684932,2.835616438,2.917808219,3.002739726 };
	double arr_q[] = { 0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,0.0000,4.5095,3.5958,2.9900,2.9722,2.5930,2.3094,2.4108,2.1943,2.0079,2.0613,1.9162,1.7862,2.8437,2.6742,2.5232,2.3966,2.4059,2.2935,2.3434,2.2429,2.1478,2.1699,2.0877,2.0090,2.5757,2.4858,2.4016,2.330293238,2.341357773,2.27176383,2.304693379,2.239772439,2.176421211 };
	for (int i = 0; i<42; i++)
		arr_q[i] /= 100.0;

	//double spot=297.22;
	double vol_term[] = { 0.08493150,0.16712330,0.25205480,0.50410960,0.74794520,1.00000000,1.50410960,2.00273970,2.50684930,3.00273970 }; //10terms
	double vol_strike[] = { 178.332,193.193,208.054,222.915,237.776,252.637,267.498,282.359,297.22,312.081,326.942,341.803,356.664,371.525,386.386,401.247,416.108 };//17spot
	Vol volat(10, 17);
	double t0[] = { 0.3095,0.2907,0.2719,0.2528,0.2331,0.2136,0.1816,0.1388,0.1081,0.105,0.1296,0.1606,0.1795,0.1892,0.1978,0.2057,0.2129 };
	double t1[] = { 0.2807,0.2619,0.2431,0.2239,0.2042,0.1845,0.1586,0.1285,0.1095,0.1027,0.1076,0.1306,0.1518,0.1634,0.172,0.1798,0.1869 };
	double t2[] = { 0.2621,0.2448,0.2274,0.2098,0.1916,0.172,0.1505,0.128,0.1131,0.1076,0.1125,0.1264,0.1413,0.1517,0.16,0.1676,0.1745 };
	double t3[] = { 0.2376,0.2219,0.2061,0.1902,0.1741,0.1576,0.141,0.1261,0.1169,0.1161,0.1215,0.1292,0.1371,0.1447,0.1517,0.1583,0.1644 };
	double t4[] = { 0.2286,0.214,0.1997,0.1854,0.1712,0.1571,0.1437,0.1319,0.1238,0.1211,0.1231,0.128,0.1339,0.14,0.146,0.1517,0.1571 };
	double t5[] = { 0.221,0.2077,0.1946,0.1819,0.1694,0.1574,0.1462,0.1366,0.1295,0.1261,0.1261,0.1287,0.1327,0.1374,0.1422,0.1469,0.1516 };
	double t6[] = { 0.2063,0.1949,0.1841,0.1737,0.1639,0.1549,0.1469,0.1403,0.1355,0.1329,0.1322,0.1331,0.1352,0.1379,0.1409,0.1442,0.1475 };
	double t7[] = { 0.1993,0.1897,0.1806,0.1721,0.1642,0.157,0.1506,0.1453,0.1411,0.1383,0.1368,0.1364,0.1369,0.1381,0.1398,0.1418,0.144 };
	double t8[] = { 0.1927,0.1844,0.1768,0.1696,0.1631,0.1572,0.1521,0.1477,0.1443,0.1418,0.1402,0.1394,0.1393,0.1398,0.1407,0.1419,0.1434 };
	double t9[] = { 0.1892,0.182,0.1754,0.1692,0.1637,0.1586,0.1542,0.1505,0.1474,0.145,0.1432,0.1421,0.1415,0.1414,0.1417,0.1423,0.1431 };

	volat.set_vol_strike(vol_strike, 17);
	volat.set_vol_term(vol_term, 10);

	volat.set_volsurface_by_term(t0, 0, 17);
	volat.set_volsurface_by_term(t1, 1, 17);
	volat.set_volsurface_by_term(t2, 2, 17);
	volat.set_volsurface_by_term(t3, 3, 17);
	volat.set_volsurface_by_term(t4, 4, 17);
	volat.set_volsurface_by_term(t5, 5, 17);
	volat.set_volsurface_by_term(t6, 6, 17);
	volat.set_volsurface_by_term(t7, 7, 17);
	volat.set_volsurface_by_term(t8, 8, 17);
	volat.set_volsurface_by_term(t9, 9, 17);

	std::vector<double> v_rts(std::begin(arr_rts), std::end(arr_rts));
	std::vector<double> v_r(std::begin(arr_r), std::end(arr_r));
	std::vector<double> v_qts(std::begin(arr_qts), std::end(arr_qts));
	std::vector<double> v_q(std::begin(arr_q), std::end(arr_q));

	Rate r(v_r, v_rts);
	Rate q(v_q, v_qts);

	double refprice = 297.22;


	double rf_for_bs = r.getIntpRate(1.0);
	double div_for_bs = q.getIntpRate(1.0);
	double putstrike = 297.22*0.9;
	double callstrike = 297.22*1.0;
	double upbarrier = 297.22*1.2;
	double intpvol = volat.getInpVol(1.0, putstrike);
	double rebate = 0.0;

	double const_vol = volat.getBSVol(1.0, 297.22);
	double const_rf = r.getIntpRate(1.0);
	double const_q = q.getIntpRate(1.0);

	//상품타입변경
	bool is_flat_vol = true;
	int hitflag = 1;
	long nM = 30000;
	double spot = 297.22;
	//***상품타입변경

	if (is_flat_vol) {
		volat.set_const_vol(const_vol);
		r.set_const_rate(const_rf);
		q.set_const_rate(const_q);
	}

	MarketParam para(43340, spot, volat, r, q);

	signed int expiryDate = 44436;
	int nb_autocall = 6;
	signed int auto_date[7] = { -1,43524,43705,43889,44071,44255,44436 };
	double auto_strike[7] = { -1,297.22,	297.22,	282.359	,282.359,	267.498,	267.498 };
	double auto_coupon[7] = { -1, 0.0230,	0.0460,	0.0690,	0.0920,	0.1150,	0.1380 };
	double auto_ki_barrier = refprice*0.6;
	double auto_dummy_coupon = auto_coupon[6];

	double auto_put_strike;
	auto_put_strike = refprice*1.0; // if put_strike=0, notional protected

	PayoffAutocallStd autoPayoff(nb_autocall, auto_date, auto_strike, auto_coupon, auto_ki_barrier, auto_put_strike, auto_dummy_coupon, refprice);
	AutocallOption AutoKOSPI(refprice, expiryDate, autoPayoff, hitflag);
	AutocallOption AutoKOSPI2(refprice, expiryDate, autoPayoff, hitflag);


	AutoKOSPI.Calc(para);
	AutoKOSPI2.Calc_old(para);


	//	AutoKOSPI.Calc(para);

	std::vector<double> rs = AutoKOSPI.GetResult();
	std::vector<double> rs2 = AutoKOSPI2.GetResult();

	std::cout.precision(8);
	std::cout << std::fixed;

	std::cout << "spot " << rs[5] << std::endl;
	std::cout << "pv " << (hitflag ? "hitted  " : "not hit  ") << (is_flat_vol ? "flatvol" : "surfacevol ") << rs[0] << std::endl;
	std::cout << "pv " << (hitflag ? "hitted  " : "not hit  ") << (is_flat_vol ? "flatvol" : "surfacevol ") << rs2[0] << std::endl;

}

void test_calc2_mc_for_vanilla(long n)
{
	double arr_rts[] = { 0.002739726,0.25,0.5,0.75,1,1.5,2,2.5,3,4,5,7,10,15,20 };
	double arr_r[] = { 1.79000,1.81000,1.74000,1.69000,1.65000,1.59000,1.55000,1.52000,1.50000,1.50000,1.50000,1.50000,1.50000,1.50000 ,1.50000 };

	for (int i = 0; i<15; i++)
		arr_r[i] /= 100.0;

	double arr_qts[] = { 0.01918000 ,	0.03836000 ,	0.05753000 ,	0.07671000 	,0.09589000 ,	0.11507000 	,0.13425000 ,	0.16438000 ,	0.24658000, 	0.32877000 ,	0.41096000 	,0.49315000 	,0.57534000 ,	0.65753000 ,	0.73973000 ,	0.82192000 ,	0.90411000 ,	0.98630000 ,	1.06849000 ,	1.15068000 	,1.23288000 ,	1.31507000 ,	1.39726000 ,	1.47945000 	,1.56164000 	,1.64384000 ,	1.72603000 ,	1.80822000 ,	1.89041000 ,	1.97260000 ,	2.05479000 ,2.13699000 	,2.21918000, 	2.30137000 ,	2.38356000 	,2.46575000 ,	2.54795000 ,	2.63014000 ,	2.71233000 ,	2.79452000 ,	2.87671000, 	2.95890000 };
	double arr_q[] = { 0.00000,0.00000,0.00000,0.00000,2.33330,1.94438,1.666590,1.361110,0.907370,0.680540,4.321890,3.601590,3.087090,3.035340,2.698050,2.428250,2.517120,2.307370,2.129880,2.172190,2.027360,1.900660,2.957910,2.793580,2.646550,2.648480,2.522360,2.407710,2.456450,2.354100,2.259940,2.278180,2.193800,2.115460,2.744900,2.653400,2.567800,2.571480,2.493550,2.420220,2.451880,2.383780 };
	for (int i = 0; i<42; i++)
		arr_q[i] /= 100.0;

	//double spot=297.22;
	double vol_term[] = { 0.08333,0.16667,0.250000,0.500000,0.750000,1.000000,1.500000,2.000000,2.500000,3.000000 };
	double vol_strike[] = { 178.332,193.193,208.054,222.915,237.776,252.637,267.498,282.359,297.22,312.081,326.942,341.803,356.664,371.525,386.386,401.247,416.108 };//17spot
	Vol volat(10, 17);
	double t0[] = { 0.3484,	0.3310,	0.3135,	0.2960,	0.2778,	0.2577, 0.2279, 0.1924, 0.1641, 0.1532, 0.1618, 0.1729, 0.1807, 0.1883, 0.1968,	0.2034,	0.2090 };
	double t1[] = { 0.3182,	0.3002,	0.2822,	0.2640,	0.2454,	0.2252,	0.2010,	0.1770,	0.1568,	0.1452,	0.1429,	0.1484,	0.1565,	0.1645,	0.1722,	0.1792,	0.1855 };
	double t2[] = { 0.3069,	0.2883,	0.2697,	0.2509,	0.2318,	0.2118,	0.1907,	0.1699,	0.1527,	0.1432,	0.1423,	0.1476,	0.1554,	0.1634,	0.1711,	0.1782,	0.1849 };
	double t3[] = { 0.2709,	0.2543,	0.2380,	0.2218,	0.2058,	0.1900,	0.1748,	0.1607,	0.1491,	0.1418,	0.1395,	0.1411,	0.1451,	0.1502,	0.1555,	0.1609,	0.1662 };
	double t4[] = { 0.2441,	0.2300,	0.2163,	0.2029,	0.1901,	0.1778,	0.1664,	0.1562,	0.1479,	0.1423,	0.1396,	0.1394,	0.1410,	0.1437,	0.1470,	0.1506,	0.1543 };
	double t5[] = { 0.2277,	0.2156,	0.2041,	0.1930,	0.1826,	0.1727,	0.1636,	0.1556,	0.1490,	0.1440,	0.1409,	0.1395,	0.1396,	0.1406,	0.1424,	0.1446,	0.1471 };
	double t6[] = { 0.2073, 0.1981,	0.1895,	0.1814,	0.1739,	0.1669,	0.1606,	0.1550,	0.1503,	0.1465,	0.1436,	0.1416,	0.1404,	0.1400,	0.1401,	0.1407,	0.1417 };
	double t7[] = { 0.1944,	0.1871,	0.1802,	0.1739,	0.1681,	0.1629,	0.1581,	0.1539,	0.1503,	0.1473,	0.1449,	0.1431,	0.1418,	0.1410,	0.1406,	0.1405,	0.1407 };
	double t8[] = { 0.1876,	0.1815,	0.1760,	0.1709,	0.1662,	0.1619,	0.1580,	0.1546,	0.1516,	0.1490,	0.1468,	0.1451,	0.1436,	0.1426,	0.1418,	0.1413,	0.1410 };
	double t9[] = { 0.1821,	0.1770,	0.1723,	0.1680,	0.1641,	0.1606,	0.1573,	0.1545,	0.1520,	0.1497,	0.1478,	0.1462,	0.1449,	0.1438,	0.1429,	0.1422,	0.1418 };

	volat.set_vol_strike(vol_strike, 17);
	volat.set_vol_term(vol_term, 10);

	volat.set_volsurface_by_term(t0, 0, 17);
	volat.set_volsurface_by_term(t1, 1, 17);
	volat.set_volsurface_by_term(t2, 2, 17);
	volat.set_volsurface_by_term(t3, 3, 17);
	volat.set_volsurface_by_term(t4, 4, 17);
	volat.set_volsurface_by_term(t5, 5, 17);
	volat.set_volsurface_by_term(t6, 6, 17);
	volat.set_volsurface_by_term(t7, 7, 17);
	volat.set_volsurface_by_term(t8, 8, 17);
	volat.set_volsurface_by_term(t9, 9, 17);

	std::vector<double> v_rts(std::begin(arr_rts), std::end(arr_rts));
	std::vector<double> v_r(std::begin(arr_r), std::end(arr_r));
	std::vector<double> v_qts(std::begin(arr_qts), std::end(arr_qts));
	std::vector<double> v_q(std::begin(arr_q), std::end(arr_q));

	Rate r(v_r, v_rts);
	Rate q(v_q, v_qts);


	//double intpvol = volat.getInpVol(1.0, putstrike);  //flat interpolation use .getBSVol

	//상품타입변경
	//bool is_flat_vol = true;
	//int hitflag = 1;
	//long nM = 30000;

	//***상품타입변경
	double spot = 297.22;
	signed int vd = 43340;
	double refprice = 297.22;
	double putstrike = spot;


	//exd var is for instrument not market para
	signed int exd = 43705;

	double rf_for_bs = r.getIntpRate((exd - vd) / 365.0);
	cout << "rf_for_bs " << rf_for_bs << "\n";
	double div_for_bs = q.getIntpRate((exd - vd) / 365.0);
	cout << "div_for_bs " << div_for_bs << "\n";

	double vol_bs = volat.getBSVol((exd - vd) / 365.0, putstrike);

	MarketParam para(vd, spot, volat, r, q);
	MarketParameters paras(vd, spot, volat, r, q);

	PayoffPut putpay(putstrike);

	EuropeanOption EurPut(refprice, exd, putpay);
	EuropeanOption EurPut2(refprice, exd, putpay);

	EuropeanOptionMC EurPutMC(refprice, exd, putpay);
	EuropeanOptionMC EurPutMC_paras(refprice, exd, putpay);


	//EurPut.Calc(para);
	EurPut2.Calc2(paras);
	//EurPutMC.Calc(para,n);
	EurPutMC_paras.Calc2(paras, n);

	std::cout.precision(8);
	std::cout << std::fixed;

	//std::vector<double> rs = EurPut.GetResult();
	vector<double> rs_fdm_paras_calc2 = EurPut2.GetResult();
	//vector<double> rs3 = EurPutMC.GetResult();
	vector<double> rs_mc_paras_calc2_module= EurPutMC_paras.GetResult();

	double bs_put_value = k_fmla_BSPut(spot, putstrike, rf_for_bs, div_for_bs, vol_bs, (exd - vd) / 365.0);
	double bs_put_vega = k_fmla_BSVega(spot, putstrike, rf_for_bs, div_for_bs, vol_bs, (exd - vd) / 365.0);

	cout << "result : test_calc2_mc_for_vanilla - no module \n ";
	//std::cout << "pv by calc1 localvol r,q term " << rs[0] << "=%price " << rs[0] / spot * 100 << "%" << std::endl;
	std::cout << "pv by calc2 localvol r,q term " << rs_fdm_paras_calc2[0] << "=%price " << rs_fdm_paras_calc2[0] / refprice * 100 << "%" << std::endl;
	//cout << "pv by mc numMC="  <<n << "  "  << rs3[0] << "=%price " << rs3[0] / spot * 100 << "%" << std::endl;
	cout << "pv by mc numMC(paras)=" << n << "  " << rs_mc_paras_calc2_module[0] << "=%price " << rs_mc_paras_calc2_module[0] / refprice * 100 << "%" << std::endl;

	std::cout << "BS vol=" << vol_bs << "=" << vol_bs * 100 << "%" << std::endl;
	//cout << "BS put value(closed) " << bs_put_value << " error/spot " << (rs3[0] - bs_put_value) / spot << "=" << (rs[0] - bs_put_value) / spot * 10000 << " bp" << endl;
	//cout << "BS put vega(%)/spot " << bs_put_vega / 100.0 / spot << "=" << bs_put_vega * 100.0 / spot << " bp" << endl;
	cout << "mc vs calc2 for vanilla : price diff 2bp, 1/10 vega\n";
}

void test_calc1_vs_calc2_for_vanilla()
{

	double arr_rts[] = { 0.002739726,0.25,0.5,0.75,1,1.5,2,2.5,3,4,5,7,10,15,20 };
	double arr_r[] = {1.79000,1.81000,1.74000,1.69000,1.65000,1.59000,1.55000,1.52000,1.50000,1.50000,1.50000,1.50000,1.50000,1.50000 ,1.50000};

	for (int i = 0; i<15; i++)
		arr_r[i] /= 100.0;

	double arr_qts[] = { 0.01918000 ,	0.03836000 ,	0.05753000 ,	0.07671000 	,0.09589000 ,	0.11507000 	,0.13425000 ,	0.16438000 ,	0.24658000, 	0.32877000 ,	0.41096000 	,0.49315000 	,0.57534000 ,	0.65753000 ,	0.73973000 ,	0.82192000 ,	0.90411000 ,	0.98630000 ,	1.06849000 ,	1.15068000 	,1.23288000 ,	1.31507000 ,	1.39726000 ,	1.47945000 	,1.56164000 	,1.64384000 ,	1.72603000 ,	1.80822000 ,	1.89041000 ,	1.97260000 ,	2.05479000 ,2.13699000 	,2.21918000, 	2.30137000 ,	2.38356000 	,2.46575000 ,	2.54795000 ,	2.63014000 ,	2.71233000 ,	2.79452000 ,	2.87671000, 	2.95890000};
	double arr_q[] = { 0.00000,0.00000,0.00000,0.00000,2.33330,1.94438,1.666590,1.361110,0.907370,0.680540,4.321890,3.601590,3.087090,3.035340,2.698050,2.428250,2.517120,2.307370,2.129880,2.172190,2.027360,1.900660,2.957910,2.793580,2.646550,2.648480,2.522360,2.407710,2.456450,2.354100,2.259940,2.278180,2.193800,2.115460,2.744900,2.653400,2.567800,2.571480,2.493550,2.420220,2.451880,2.383780 };
	for (int i = 0; i<42; i++)
		arr_q[i] /= 100.0;

	//double spot=297.22;
	double vol_term[] = {0.08333,0.16667,0.250000,0.500000,0.750000,1.000000,1.500000,2.000000,2.500000,3.000000};
	double vol_strike[] = { 178.332,193.193,208.054,222.915,237.776,252.637,267.498,282.359,297.22,312.081,326.942,341.803,356.664,371.525,386.386,401.247,416.108 };//17spot
	Vol volat(10, 17);
	double t0[] = { 0.3484,	0.3310,	0.3135,	0.2960,	0.2778,	0.2577, 0.2279, 0.1924, 0.1641, 0.1532, 0.1618, 0.1729, 0.1807, 0.1883, 0.1968,	0.2034,	0.2090};
	double t1[] = { 0.3182,	0.3002,	0.2822,	0.2640,	0.2454,	0.2252,	0.2010,	0.1770,	0.1568,	0.1452,	0.1429,	0.1484,	0.1565,	0.1645,	0.1722,	0.1792,	0.1855};
	double t2[] = { 0.3069,	0.2883,	0.2697,	0.2509,	0.2318,	0.2118,	0.1907,	0.1699,	0.1527,	0.1432,	0.1423,	0.1476,	0.1554,	0.1634,	0.1711,	0.1782,	0.1849};
	double t3[] = { 0.2709,	0.2543,	0.2380,	0.2218,	0.2058,	0.1900,	0.1748,	0.1607,	0.1491,	0.1418,	0.1395,	0.1411,	0.1451,	0.1502,	0.1555,	0.1609,	0.1662};
	double t4[] = { 0.2441,	0.2300,	0.2163,	0.2029,	0.1901,	0.1778,	0.1664,	0.1562,	0.1479,	0.1423,	0.1396,	0.1394,	0.1410,	0.1437,	0.1470,	0.1506,	0.1543};
	double t5[] = { 0.2277,	0.2156,	0.2041,	0.1930,	0.1826,	0.1727,	0.1636,	0.1556,	0.1490,	0.1440,	0.1409,	0.1395,	0.1396,	0.1406,	0.1424,	0.1446,	0.1471};
	double t6[] = { 0.2073, 0.1981,	0.1895,	0.1814,	0.1739,	0.1669,	0.1606,	0.1550,	0.1503,	0.1465,	0.1436,	0.1416,	0.1404,	0.1400,	0.1401,	0.1407,	0.1417};
	double t7[] = { 0.1944,	0.1871,	0.1802,	0.1739,	0.1681,	0.1629,	0.1581,	0.1539,	0.1503,	0.1473,	0.1449,	0.1431,	0.1418,	0.1410,	0.1406,	0.1405,	0.1407};
	double t8[] = { 0.1876,	0.1815,	0.1760,	0.1709,	0.1662,	0.1619,	0.1580,	0.1546,	0.1516,	0.1490,	0.1468,	0.1451,	0.1436,	0.1426,	0.1418,	0.1413,	0.1410};
	double t9[] = { 0.1821,	0.1770,	0.1723,	0.1680,	0.1641,	0.1606,	0.1573,	0.1545,	0.1520,	0.1497,	0.1478,	0.1462,	0.1449,	0.1438,	0.1429,	0.1422,	0.1418};

	volat.set_vol_strike(vol_strike, 17);
	volat.set_vol_term(vol_term, 10);

	volat.set_volsurface_by_term(t0, 0, 17);
	volat.set_volsurface_by_term(t1, 1, 17);
	volat.set_volsurface_by_term(t2, 2, 17);
	volat.set_volsurface_by_term(t3, 3, 17);
	volat.set_volsurface_by_term(t4, 4, 17);
	volat.set_volsurface_by_term(t5, 5, 17);
	volat.set_volsurface_by_term(t6, 6, 17);
	volat.set_volsurface_by_term(t7, 7, 17);
	volat.set_volsurface_by_term(t8, 8, 17);
	volat.set_volsurface_by_term(t9, 9, 17);

	std::vector<double> v_rts(std::begin(arr_rts), std::end(arr_rts));
	std::vector<double> v_r(std::begin(arr_r), std::end(arr_r));
	std::vector<double> v_qts(std::begin(arr_qts), std::end(arr_qts));
	std::vector<double> v_q(std::begin(arr_q), std::end(arr_q));

	Rate r(v_r, v_rts);
	Rate q(v_q, v_qts);


	//double intpvol = volat.getInpVol(1.0, putstrike);  //flat interpolation use .getBSVol

	//상품타입변경
	//bool is_flat_vol = true;
	//int hitflag = 1;
	//long nM = 30000;
	
	//***상품타입변경
    double spot = 297.22;
	signed int vd = 43340;
	signed int exd = 43705;
	double refprice = 297.22;
	double putstrike = spot;

	double rf_for_bs = r.getIntpRate((exd-vd)/365.0);
	cout << "rf_for_bs "<<rf_for_bs << "\n";
	double div_for_bs = q.getIntpRate((exd - vd) / 365.0);
	cout << "div_for_bs "<<div_for_bs << "\n";

	
	//double callstrike = spot*1.1;
	double vol_bs = volat.getBSVol((exd - vd) / 365.0, putstrike);

	MarketParam para(vd, spot, volat, r, q);

	PayoffPut putpay(putstrike);
	
	EuropeanOption EurPut(refprice, exd, putpay);
	EuropeanOption EurPut2(refprice, exd, putpay);

	EuropeanOptionMC EurPutMC(refprice, exd, putpay);

	EurPut.Calc(para);
	EurPut2.Calc2(para);
	//EurPutMC.Calc(para, nM);
	/*EuropeanVanilla Vput(putpay,refprice, ed);

	EuropeanPut eput(refprice,putstrike,365);
	EuropeanCall ecall(refprice,callstrike,365);

	*/
	std::cout.precision(8);
	std::cout << std::fixed;

	//std::cout <<"eput:  s=" <<spot<<" "<< eput.pv(para)<<" vol " << volat.getInpVol(365/365,putstrike)<< "r " << r.getIntpRate(1.0) << " q " << q.getIntpRate(1.0) <<  std::endl;
	//std::cout <<"ecall: "<< ecall.pv(para)<< "s= " <<spot<<" "<<" vol " <<volat.getInpVol(365/365,callstrike)<< " r " << r.getIntpRate(1.0) << " q " << q.getIntpRate(1.0) << std::endl;
	std::vector<double> rs = EurPut.GetResult();
	std::vector<double> rs2 = EurPut2.GetResult();
	double bs_put_value = k_fmla_BSPut(spot, putstrike, rf_for_bs, div_for_bs, vol_bs, (exd - vd) / 365.0);
	double bs_put_vega = k_fmla_BSVega(spot, putstrike, rf_for_bs, div_for_bs, vol_bs, (exd - vd) / 365.0);
	cout << "\ntest_calc1_vs_calc2_for_vanilla\n";
	std::cout << "pv by calc1 localvol r,q term " << rs[0] << "=%price " << rs[0]/spot*100 << "%" <<std::endl;
	std::cout << "pv by calc2 localvol r,q term " << rs2[0] << "=%price " << rs2[0] / spot*100 << "%"<< std::endl;
	std::cout << "BS vol=" << vol_bs << "=" << vol_bs*100 << "%" << std::endl;
	cout << "BS put value(closed) " << bs_put_value << " error/spot " << (rs[0] - bs_put_value) / spot << "=" << (rs[0] - bs_put_value) / spot*10000 << " bp" << endl;
	cout << "BS put vega(%)/spot " << bs_put_vega /100.0/spot << "=" << bs_put_vega * 100.0 / spot << " bp"<< endl;
	

	//////////////////////////
	cout << "\n FDM으로 Flat vol pricing\n";
	//double vol_term[] = { 0.08493150,0.16712330,0.25205480,0.50410960,0.74794520,1.00000000,1.50410960,2.00273970,2.50684930,3.00273970 }; //10terms
	//double vol_strike[] = { 178.332,193.193,208.054,222.915,237.776,252.637,267.498,282.359,297.22,312.081,326.942,341.803,356.664,371.525,386.386,401.247,416.108 };//17spot
	Vol volat_flat(10, 17);
	double t0_flat[] = { vol_bs,vol_bs,vol_bs,vol_bs,vol_bs,vol_bs,vol_bs,vol_bs,vol_bs,vol_bs,vol_bs,vol_bs,vol_bs,vol_bs,vol_bs,vol_bs,vol_bs };

	volat_flat.set_vol_strike(vol_strike, 17);
	volat_flat.set_vol_term(vol_term, 10);

	volat_flat.set_volsurface_by_term(t0_flat, 0, 17);
	volat_flat.set_volsurface_by_term(t0_flat, 1, 17);
	volat_flat.set_volsurface_by_term(t0_flat, 2, 17);
	volat_flat.set_volsurface_by_term(t0_flat, 3, 17);
	volat_flat.set_volsurface_by_term(t0_flat, 4, 17);
	volat_flat.set_volsurface_by_term(t0_flat, 5, 17);
	volat_flat.set_volsurface_by_term(t0_flat, 6, 17);
	volat_flat.set_volsurface_by_term(t0_flat, 7, 17);
	volat_flat.set_volsurface_by_term(t0_flat, 8, 17);
	volat_flat.set_volsurface_by_term(t0_flat, 9, 17);

	MarketParam para_flat(vd, spot, volat_flat, r, q);

	EuropeanOption EurPut_flat(refprice, exd, putpay);
	EuropeanOption EurPut2_flat(refprice, exd, putpay);
	EurPut_flat.Calc(para_flat);
	EurPut2_flat.Calc2(para_flat);
	std::vector<double> rs_flat = EurPut_flat.GetResult();
	std::vector<double> rs2_flat = EurPut2_flat.GetResult();
	std::cout << "pv by calc1 flat_vol r,q term struct " << rs_flat[0] << "=%price " << rs_flat[0] / spot * 100 << "%" << std::endl;
	std::cout << "pv by calc2 flat_vol r,q term struct " << rs2_flat[0] << "=%price " << rs2_flat[0] / spot * 100 << "%" << std::endl;
	cout << "flat vs surface 가격차이는 2bp수준으로 양호함\n";

	////////////////////////////////
	cout << "\n FDM으로 Flat vol, flat r,div curve pricing\n";
	//double arr_rts[] = { 0.002739726,0.25,0.5,0.75,1,1.5,2,2.5,3,4,5,7,10,15,20 };
	//double arr_r_flat[] = { rf_for_bs,rf_for_bs,rf_for_bs,rf_for_bs,rf_for_bs,rf_for_bs,rf_for_bs,rf_for_bs,rf_for_bs,rf_for_bs,rf_for_bs,rf_for_bs,rf_for_bs,rf_for_bs,rf_for_bs };
	double arr_r_flat[15];
	fill_n(arr_r_flat, 15, rf_for_bs);

	//double arr_qts[] = { 0.0191780821917808,0.038356164,0.057534247,0.084931507,0.104109589,0.123287671,0.142465753,0.167123288,0.252054795,0.334246575,0.419178082,0.504109589,0.580821918,0.665753425,0.747945205,0.832876712,0.915068493,1,1.084931507,1.167123288,1.252054795,1.334246575,1.419178082,1.504109589,1.583561644,1.668493151,1.750684932,1.835616438,1.917808219,	2.002739726	,2.087671233,2.169863014,2.254794521,2.336986301,2.421917808,2.506849315,2.583561644,2.668493151,2.750684932,2.835616438,2.917808219,3.002739726 };
	//double arr_q_flat[] = { div_for_bs,0.00000,0.00000,2.57097, 2.09737,1.77112,1.53270, 1.30656 ,0.86631 ,5.16283, 4.11677,3.42318,3.34816,2.92103,2.60139,2.67302,2.43293,2.22630,2.26258, 2.10325,1.96057,3.00733,2.82806 ,2.66837, 2.53449,2.53675,2.41823 ,2.46233 ,2.35680 ,2.25685 ,2.27446 ,2.18831 ,2.10588 ,2.66917,2.57598,2.48871,2.41481,2.42319,2.35115 ,2.38170 ,2.31461 ,2.24914 };
	double arr_q_flat[42];
	fill_n(arr_q_flat, 42, div_for_bs);

	//std::vector<double> v_rts(std::begin(arr_rts), std::end(arr_rts));
	std::vector<double> v_r_flat(std::begin(arr_r_flat), std::end(arr_r_flat));
	//std::vector<double> v_qts(std::begin(arr_qts), std::end(arr_qts));
	std::vector<double> v_q_flat(std::begin(arr_q_flat), std::end(arr_q_flat));

	Rate r_flat(v_r_flat, v_rts);
	Rate q_flat(v_q_flat, v_qts);

	MarketParam para_volflat_rqflat(vd, spot, volat_flat, r_flat, q_flat);
	EurPut_flat.Calc(para_volflat_rqflat);
	EurPut2_flat.Calc2(para_volflat_rqflat);

	std::vector<double> rs_flat_rqflat = EurPut_flat.GetResult();
	std::vector<double> rs2_flat_rqflat = EurPut2_flat.GetResult();
	std::cout << "pv by calc1 flat vol,r,q  " << rs_flat_rqflat[0] << "=%price " << rs_flat_rqflat[0] / spot * 100 << "%" << std::endl;
	std::cout << "pv by calc2 flat vol,r,q  " << rs2_flat_rqflat[0] << "=%price " << rs2_flat_rqflat[0] / spot * 100 << "%" << std::endl;
	cout << "vol,r,q,flat vs surface 가격차이는 3bp(2bp from vol, 1bp from rq) 수준으로 양호함\n";

}

void test_calc2_for_vanilla_param_params()
{

	double arr_rts[] = { 0.002739726,0.25,0.5,0.75,1,1.5,2,2.5,3,4,5,7,10,15,20 };
	double arr_r[] = { 1.79000,1.81000,1.74000,1.69000,1.65000,1.59000,1.55000,1.52000,1.50000,1.50000,1.50000,1.50000,1.50000,1.50000 ,1.50000 };

	for (int i = 0; i<15; i++)
		arr_r[i] /= 100.0;

	double arr_qts[] = { 0.01918000 ,	0.03836000 ,	0.05753000 ,	0.07671000 	,0.09589000 ,	0.11507000 	,0.13425000 ,	0.16438000 ,	0.24658000, 	0.32877000 ,	0.41096000 	,0.49315000 	,0.57534000 ,	0.65753000 ,	0.73973000 ,	0.82192000 ,	0.90411000 ,	0.98630000 ,	1.06849000 ,	1.15068000 	,1.23288000 ,	1.31507000 ,	1.39726000 ,	1.47945000 	,1.56164000 	,1.64384000 ,	1.72603000 ,	1.80822000 ,	1.89041000 ,	1.97260000 ,	2.05479000 ,2.13699000 	,2.21918000, 	2.30137000 ,	2.38356000 	,2.46575000 ,	2.54795000 ,	2.63014000 ,	2.71233000 ,	2.79452000 ,	2.87671000, 	2.95890000 };
	double arr_q[] = { 0.00000,0.00000,0.00000,0.00000,2.33330,1.94438,1.666590,1.361110,0.907370,0.680540,4.321890,3.601590,3.087090,3.035340,2.698050,2.428250,2.517120,2.307370,2.129880,2.172190,2.027360,1.900660,2.957910,2.793580,2.646550,2.648480,2.522360,2.407710,2.456450,2.354100,2.259940,2.278180,2.193800,2.115460,2.744900,2.653400,2.567800,2.571480,2.493550,2.420220,2.451880,2.383780 };
	//double arr_q[] = { 0.00000,0.00000,0.00000,0.00000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	for (int i = 0; i<42; i++)
		arr_q[i] /= 100.0;

	//double spot=297.22;
	double vol_term[] = { 0.08333,0.16667,0.250000,0.500000,0.750000,1.000000,1.500000,2.000000,2.500000,3.000000 };
	double vol_strike[] = { 178.332,193.193,208.054,222.915,237.776,252.637,267.498,282.359,297.22,312.081,326.942,341.803,356.664,371.525,386.386,401.247,416.108 };//17spot
	Vol volat(10, 17);
	double t0[] = { 0.3484,	0.3310,	0.3135,	0.2960,	0.2778,	0.2577, 0.2279, 0.1924, 0.1641, 0.1532, 0.1618, 0.1729, 0.1807, 0.1883, 0.1968,	0.2034,	0.2090 };
	double t1[] = { 0.3182,	0.3002,	0.2822,	0.2640,	0.2454,	0.2252,	0.2010,	0.1770,	0.1568,	0.1452,	0.1429,	0.1484,	0.1565,	0.1645,	0.1722,	0.1792,	0.1855 };
	double t2[] = { 0.3069,	0.2883,	0.2697,	0.2509,	0.2318,	0.2118,	0.1907,	0.1699,	0.1527,	0.1432,	0.1423,	0.1476,	0.1554,	0.1634,	0.1711,	0.1782,	0.1849 };
	double t3[] = { 0.2709,	0.2543,	0.2380,	0.2218,	0.2058,	0.1900,	0.1748,	0.1607,	0.1491,	0.1418,	0.1395,	0.1411,	0.1451,	0.1502,	0.1555,	0.1609,	0.1662 };
	double t4[] = { 0.2441,	0.2300,	0.2163,	0.2029,	0.1901,	0.1778,	0.1664,	0.1562,	0.1479,	0.1423,	0.1396,	0.1394,	0.1410,	0.1437,	0.1470,	0.1506,	0.1543 };
	double t5[] = { 0.2277,	0.2156,	0.2041,	0.1930,	0.1826,	0.1727,	0.1636,	0.1556,	0.1490,	0.1440,	0.1409,	0.1395,	0.1396,	0.1406,	0.1424,	0.1446,	0.1471 };
	double t6[] = { 0.2073, 0.1981,	0.1895,	0.1814,	0.1739,	0.1669,	0.1606,	0.1550,	0.1503,	0.1465,	0.1436,	0.1416,	0.1404,	0.1400,	0.1401,	0.1407,	0.1417 };
	double t7[] = { 0.1944,	0.1871,	0.1802,	0.1739,	0.1681,	0.1629,	0.1581,	0.1539,	0.1503,	0.1473,	0.1449,	0.1431,	0.1418,	0.1410,	0.1406,	0.1405,	0.1407 };
	double t8[] = { 0.1876,	0.1815,	0.1760,	0.1709,	0.1662,	0.1619,	0.1580,	0.1546,	0.1516,	0.1490,	0.1468,	0.1451,	0.1436,	0.1426,	0.1418,	0.1413,	0.1410 };
	double t9[] = { 0.1821,	0.1770,	0.1723,	0.1680,	0.1641,	0.1606,	0.1573,	0.1545,	0.1520,	0.1497,	0.1478,	0.1462,	0.1449,	0.1438,	0.1429,	0.1422,	0.1418 };

	volat.set_vol_strike(vol_strike, 17);
	volat.set_vol_term(vol_term, 10);

	volat.set_volsurface_by_term(t0, 0, 17);
	volat.set_volsurface_by_term(t1, 1, 17);
	volat.set_volsurface_by_term(t2, 2, 17);
	volat.set_volsurface_by_term(t3, 3, 17);
	volat.set_volsurface_by_term(t4, 4, 17);
	volat.set_volsurface_by_term(t5, 5, 17);
	volat.set_volsurface_by_term(t6, 6, 17);
	volat.set_volsurface_by_term(t7, 7, 17);
	volat.set_volsurface_by_term(t8, 8, 17);
	volat.set_volsurface_by_term(t9, 9, 17);

	std::vector<double> v_rts(std::begin(arr_rts), std::end(arr_rts));
	std::vector<double> v_r(std::begin(arr_r), std::end(arr_r));
	std::vector<double> v_qts(std::begin(arr_qts), std::end(arr_qts));
	std::vector<double> v_q(std::begin(arr_q), std::end(arr_q));

	Rate r(v_r, v_rts);
	Rate q(v_q, v_qts);


	//double intpvol = volat.getInpVol(1.0, putstrike);  //flat interpolation use .getBSVol

	//상품타입변경
	//bool is_flat_vol = true;
	//int hitflag = 1;
	//long nM = 30000;

	//***상품타입변경
	double spot = 297.22;
	signed int vd = 43340;
	signed int exd = 43705;
	double refprice = 297.22;
	double putstrike = spot;

	double rf_for_bs = r.getIntpRate((exd - vd) / 365.0);
	cout << "rf_for_bs " << rf_for_bs << "\n";
	double div_for_bs = q.getIntpRate((exd - vd) / 365.0);
	cout << "div_for_bs " << div_for_bs << "\n";

	double vol_bs = volat.getBSVol((exd - vd) / 365.0, putstrike);

	//MarketParam para(vd, spot, volat, r, q);
	
	vector<signed int> divexdate = { 43278,43370,	43461,	43554,	43645,	43736,	43823,	43828,	43920,	44011,	44103,	44192,	44194,	44285,	44376,	44468,	44557,	44559,	44650	,44741	,44833	,44922,	44924 };
	vector<double> divamount = { 0.827,	0.665,	4.614	,0.653,	0.832,	0.665	,0.054	,4.801,	0.656	,0.862,	0.668,	0.054	,4.922,	0.656	,0.862	,0.668	,0.054,	4.922,	0.656,	0.862,	0.668,	0.054,	4.922 };
	Dividend div(divexdate, divamount);
	
	Rate r_from_div = div.getDividendCurve(v_rts, vd, spot);

	//MarketParameters paras(43340, spot, volat, r, div); //discrete dividend
	MarketParameters paras(43340, spot, volat, r, q); //conti div. params

	MarketParam para(43340, spot, volat, r, q);


	PayoffPut putpay(putstrike);

	EuropeanOption EurPut_para(refprice, exd, putpay);
	EuropeanOption EurPut_params(refprice, exd, putpay);

	//EuropeanOptionMC EurPutMC(refprice, exd, putpay);

	EurPut_para.Calc2(para);
	EurPut_params.Calc2(paras);

	std::cout.precision(8);
	std::cout << std::fixed;

	//std::cout <<"eput:  s=" <<spot<<" "<< eput.pv(para)<<" vol " << volat.getInpVol(365/365,putstrike)<< "r " << r.getIntpRate(1.0) << " q " << q.getIntpRate(1.0) <<  std::endl;
	//std::cout <<"ecall: "<< ecall.pv(para)<< "s= " <<spot<<" "<<" vol " <<volat.getInpVol(365/365,callstrike)<< " r " << r.getIntpRate(1.0) << " q " << q.getIntpRate(1.0) << std::endl;
	std::vector<double> rs = EurPut_para.GetResult();
	std::vector<double> rs2 = EurPut_params.GetResult();
	double bs_put_value = k_fmla_BSPut(spot, putstrike, rf_for_bs, div_for_bs, vol_bs, (exd - vd) / 365.0);
	double bs_put_vega = k_fmla_BSVega(spot, putstrike, rf_for_bs, div_for_bs, vol_bs, (exd - vd) / 365.0);

	cout << "\nresult : test_calc2_for_vanilla_param_params\n"; 
	std::cout << "pv by calc2 para " << rs[0] << "=%price " << rs[0] / spot * 100 << "%" << std::endl;
	std::cout << "pv by calc2 paras " << rs2[0] << "=%price " << rs2[0] / spot * 100 << "%" << std::endl;
	std::cout << "BS vol=" << vol_bs << "=" << vol_bs * 100 << "%" << std::endl;
	cout << "BS put value(closed) " << bs_put_value << " error/spot " << (rs[0] - bs_put_value) / spot << "=" << (rs[0] - bs_put_value) / spot * 10000 << " bp" << endl;
	cout << "BS put vega(%)/spot " << bs_put_vega / 100.0 / spot << "=" << bs_put_vega * 100.0 / spot << " bp" << endl;

}

void test_calc2_for_vanilla_param_params_module(MarketParameters& paras)
{
	//paras input
	double refprice = paras.get_spot();
	double put_strike = refprice;
	signed int vd = paras.get_vdate();
	signed int exd = 43705; //1year vanilla
	double spot = refprice;

	PayoffPut putpay(put_strike);


	EuropeanOption EurPut_paras_module(refprice, exd, putpay);

	//EuropeanOptionMC EurPutMC(refprice, exd, putpay);

	EurPut_paras_module.Calc2(paras);

	std::cout.precision(8);
	std::cout << std::fixed;

	std::vector<double> rs = EurPut_paras_module.GetResult();

	cout << "\nresult : test_calc2_for_vanilla_paras_module\n";
	std::cout << "pv by calc2 para " << rs[0] << "=%price " << rs[0] / spot * 100 << "%" << std::endl;
}

void test_calc2_for_vanilla_param_params_module_inst(MarketParameters& paras, EuropeanOption& eop)
{
	eop.Calc2(paras);

	std::cout.precision(8);
	std::cout << std::fixed;

	std::vector<double> rs = eop.GetResult();

	double refprice = eop.GetRefPrice();
	cout << "\nresult : test_calc2_for_vanilla_paras_module_instrument\n";
	std::cout << "test_calc2_for_vanilla_param_params_module_inst(MarketParameters& paras, EuropeanOption& eop)\n " << rs[0] << "=%price " << rs[0] / refprice * 100 << "%" << std::endl;
}

void test_calc2_for_vanilla_param_params_module_inst(MarketParam& para, EuropeanOption& eop)
{
	eop.Calc2(para);

	std::cout.precision(8);
	std::cout << std::fixed;

	std::vector<double> rs = eop.GetResult();

	double refprice = eop.GetRefPrice();
	cout << "\nresult : test_calc2_for_vanilla_paras_module_instrument\n";
	std::cout << "test_calc2_for_vanilla_param_params_module_inst(MarketParam& para, EuropeanOption& eop)\n " << rs[0] << "=%price " << rs[0] / refprice * 100 << "%" << std::endl;
}

MarketParameters set_paras()
{
	double arr_rts[] = { 0.002739726,0.25,0.5,0.75,1,1.5,2,2.5,3,4,5,7,10,15,20 };
	double arr_r[] = { 1.79000,1.81000,1.74000,1.69000,1.65000,1.59000,1.55000,1.52000,1.50000,1.50000,1.50000,1.50000,1.50000,1.50000 ,1.50000 };

	for (int i = 0; i<15; i++)
		arr_r[i] /= 100.0;

	double arr_qts[] = { 0.01918000 ,	0.03836000 ,	0.05753000 ,	0.07671000 	,0.09589000 ,	0.11507000 	,0.13425000 ,	0.16438000 ,	0.24658000, 	0.32877000 ,	0.41096000 	,0.49315000 	,0.57534000 ,	0.65753000 ,	0.73973000 ,	0.82192000 ,	0.90411000 ,	0.98630000 ,	1.06849000 ,	1.15068000 	,1.23288000 ,	1.31507000 ,	1.39726000 ,	1.47945000 	,1.56164000 	,1.64384000 ,	1.72603000 ,	1.80822000 ,	1.89041000 ,	1.97260000 ,	2.05479000 ,2.13699000 	,2.21918000, 	2.30137000 ,	2.38356000 	,2.46575000 ,	2.54795000 ,	2.63014000 ,	2.71233000 ,	2.79452000 ,	2.87671000, 	2.95890000 };
	double arr_q[] = { 0.00000,0.00000,0.00000,0.00000,2.33330,1.94438,1.666590,1.361110,0.907370,0.680540,4.321890,3.601590,3.087090,3.035340,2.698050,2.428250,2.517120,2.307370,2.129880,2.172190,2.027360,1.900660,2.957910,2.793580,2.646550,2.648480,2.522360,2.407710,2.456450,2.354100,2.259940,2.278180,2.193800,2.115460,2.744900,2.653400,2.567800,2.571480,2.493550,2.420220,2.451880,2.383780 };
	for (int i = 0; i<42; i++)
		arr_q[i] /= 100.0;

	//double spot=297.22;
	double vol_term[] = { 0.08333,0.16667,0.250000,0.500000,0.750000,1.000000,1.500000,2.000000,2.500000,3.000000 };
	double vol_strike[] = { 178.332,193.193,208.054,222.915,237.776,252.637,267.498,282.359,297.22,312.081,326.942,341.803,356.664,371.525,386.386,401.247,416.108 };//17spot
	Vol volat(10, 17);
	double t0[] = { 0.3484,	0.3310,	0.3135,	0.2960,	0.2778,	0.2577, 0.2279, 0.1924, 0.1641, 0.1532, 0.1618, 0.1729, 0.1807, 0.1883, 0.1968,	0.2034,	0.2090 };
	double t1[] = { 0.3182,	0.3002,	0.2822,	0.2640,	0.2454,	0.2252,	0.2010,	0.1770,	0.1568,	0.1452,	0.1429,	0.1484,	0.1565,	0.1645,	0.1722,	0.1792,	0.1855 };
	double t2[] = { 0.3069,	0.2883,	0.2697,	0.2509,	0.2318,	0.2118,	0.1907,	0.1699,	0.1527,	0.1432,	0.1423,	0.1476,	0.1554,	0.1634,	0.1711,	0.1782,	0.1849 };
	double t3[] = { 0.2709,	0.2543,	0.2380,	0.2218,	0.2058,	0.1900,	0.1748,	0.1607,	0.1491,	0.1418,	0.1395,	0.1411,	0.1451,	0.1502,	0.1555,	0.1609,	0.1662 };
	double t4[] = { 0.2441,	0.2300,	0.2163,	0.2029,	0.1901,	0.1778,	0.1664,	0.1562,	0.1479,	0.1423,	0.1396,	0.1394,	0.1410,	0.1437,	0.1470,	0.1506,	0.1543 };
	double t5[] = { 0.2277,	0.2156,	0.2041,	0.1930,	0.1826,	0.1727,	0.1636,	0.1556,	0.1490,	0.1440,	0.1409,	0.1395,	0.1396,	0.1406,	0.1424,	0.1446,	0.1471 };
	double t6[] = { 0.2073, 0.1981,	0.1895,	0.1814,	0.1739,	0.1669,	0.1606,	0.1550,	0.1503,	0.1465,	0.1436,	0.1416,	0.1404,	0.1400,	0.1401,	0.1407,	0.1417 };
	double t7[] = { 0.1944,	0.1871,	0.1802,	0.1739,	0.1681,	0.1629,	0.1581,	0.1539,	0.1503,	0.1473,	0.1449,	0.1431,	0.1418,	0.1410,	0.1406,	0.1405,	0.1407 };
	double t8[] = { 0.1876,	0.1815,	0.1760,	0.1709,	0.1662,	0.1619,	0.1580,	0.1546,	0.1516,	0.1490,	0.1468,	0.1451,	0.1436,	0.1426,	0.1418,	0.1413,	0.1410 };
	double t9[] = { 0.1821,	0.1770,	0.1723,	0.1680,	0.1641,	0.1606,	0.1573,	0.1545,	0.1520,	0.1497,	0.1478,	0.1462,	0.1449,	0.1438,	0.1429,	0.1422,	0.1418 };

	volat.set_vol_strike(vol_strike, 17);
	volat.set_vol_term(vol_term, 10);

	volat.set_volsurface_by_term(t0, 0, 17);
	volat.set_volsurface_by_term(t1, 1, 17);
	volat.set_volsurface_by_term(t2, 2, 17);
	volat.set_volsurface_by_term(t3, 3, 17);
	volat.set_volsurface_by_term(t4, 4, 17);
	volat.set_volsurface_by_term(t5, 5, 17);
	volat.set_volsurface_by_term(t6, 6, 17);
	volat.set_volsurface_by_term(t7, 7, 17);
	volat.set_volsurface_by_term(t8, 8, 17);
	volat.set_volsurface_by_term(t9, 9, 17);

	std::vector<double> v_rts(std::begin(arr_rts), std::end(arr_rts));
	std::vector<double> v_r(std::begin(arr_r), std::end(arr_r));
	std::vector<double> v_qts(std::begin(arr_qts), std::end(arr_qts));
	std::vector<double> v_q(std::begin(arr_q), std::end(arr_q));

	Rate r(v_r, v_rts);
	Rate q(v_q, v_qts);


	//double intpvol = volat.getInpVol(1.0, putstrike);  //flat interpolation use .getBSVol

	//상품타입변경
	//bool is_flat_vol = true;
	//int hitflag = 1;
	//long nM = 30000;

	//***상품타입변경
	double spot = 297.22;
	signed int vd = 43340;
	double refprice = 297.22;
	double putstrike = spot;

	return MarketParameters(vd, spot, volat, r, q);
}

MarketParameters set_paras_flat()
{
	double arr_rts[] = { 0.002739726,0.25,0.5,0.75,1,1.5,2,2.5,3,4,5,7,10,15,20 };
	double arr_r[] = { 1.79000,1.81000,1.74000,1.69000,1.65000,1.59000,1.55000,1.52000,1.50000,1.50000,1.50000,1.50000,1.50000,1.50000 ,1.50000 };
	//double arr_r[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,0 };

	for (int i = 0; i<15; i++)
		arr_r[i] /= 100.0;

	double arr_qts[] = { 0.01918000 ,	0.03836000 ,	0.05753000 ,	0.07671000 	,0.09589000 ,	0.11507000 	,0.13425000 ,	0.16438000 ,	0.24658000, 	0.32877000 ,	0.41096000 	,0.49315000 	,0.57534000 ,	0.65753000 ,	0.73973000 ,	0.82192000 ,	0.90411000 ,	0.98630000 ,	1.06849000 ,	1.15068000 	,1.23288000 ,	1.31507000 ,	1.39726000 ,	1.47945000 	,1.56164000 	,1.64384000 ,	1.72603000 ,	1.80822000 ,	1.89041000 ,	1.97260000 ,	2.05479000 ,2.13699000 	,2.21918000, 	2.30137000 ,	2.38356000 	,2.46575000 ,	2.54795000 ,	2.63014000 ,	2.71233000 ,	2.79452000 ,	2.87671000, 	2.95890000 };
	//double arr_q[] = { 0.00000,0.00000,0.00000,0.00000,2.33330,1.94438,1.666590,1.361110,0.907370,0.680540,4.321890,3.601590,3.087090,3.035340,2.698050,2.428250,2.517120,2.307370,2.129880,2.172190,2.027360,1.900660,2.957910,2.793580,2.646550,2.648480,2.522360,2.407710,2.456450,2.354100,2.259940,2.278180,2.193800,2.115460,2.744900,2.653400,2.567800,2.571480,2.493550,2.420220,2.451880,2.383780 };
	double arr_q[] = { 0.00000,0.00000,0.00000,0.00000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	for (int i = 0; i<42; i++)
		arr_q[i] /= 100.0;

	//double spot=297.22;
	double vol_term[] = { 0.08333,0.16667,0.250000,0.500000,0.750000,1.000000,1.500000,2.000000,2.500000,3.000000 };
	double vol_strike[] = { 178.332,193.193,208.054,222.915,237.776,252.637,267.498,282.359,297.22,312.081,326.942,341.803,356.664,371.525,386.386,401.247,416.108 };//17spot
	Vol volat(10, 17);
	double t0[] = { 0.3484,	0.3310,	0.3135,	0.2960,	0.2778,	0.2577, 0.2279, 0.1924, 0.1641, 0.1532, 0.1618, 0.1729, 0.1807, 0.1883, 0.1968,	0.2034,	0.2090 };
	double t1[] = { 0.3182,	0.3002,	0.2822,	0.2640,	0.2454,	0.2252,	0.2010,	0.1770,	0.1568,	0.1452,	0.1429,	0.1484,	0.1565,	0.1645,	0.1722,	0.1792,	0.1855 };
	double t2[] = { 0.3069,	0.2883,	0.2697,	0.2509,	0.2318,	0.2118,	0.1907,	0.1699,	0.1527,	0.1432,	0.1423,	0.1476,	0.1554,	0.1634,	0.1711,	0.1782,	0.1849 };
	double t3[] = { 0.2709,	0.2543,	0.2380,	0.2218,	0.2058,	0.1900,	0.1748,	0.1607,	0.1491,	0.1418,	0.1395,	0.1411,	0.1451,	0.1502,	0.1555,	0.1609,	0.1662 };
	double t4[] = { 0.2441,	0.2300,	0.2163,	0.2029,	0.1901,	0.1778,	0.1664,	0.1562,	0.1479,	0.1423,	0.1396,	0.1394,	0.1410,	0.1437,	0.1470,	0.1506,	0.1543 };
	double t5[] = { 0.2277,	0.2156,	0.2041,	0.1930,	0.1826,	0.1727,	0.1636,	0.1556,	0.1490,	0.1440,	0.1409,	0.1395,	0.1396,	0.1406,	0.1424,	0.1446,	0.1471 };
	double t6[] = { 0.2073, 0.1981,	0.1895,	0.1814,	0.1739,	0.1669,	0.1606,	0.1550,	0.1503,	0.1465,	0.1436,	0.1416,	0.1404,	0.1400,	0.1401,	0.1407,	0.1417 };
	double t7[] = { 0.1944,	0.1871,	0.1802,	0.1739,	0.1681,	0.1629,	0.1581,	0.1539,	0.1503,	0.1473,	0.1449,	0.1431,	0.1418,	0.1410,	0.1406,	0.1405,	0.1407 };
	double t8[] = { 0.1876,	0.1815,	0.1760,	0.1709,	0.1662,	0.1619,	0.1580,	0.1546,	0.1516,	0.1490,	0.1468,	0.1451,	0.1436,	0.1426,	0.1418,	0.1413,	0.1410 };
	double t9[] = { 0.1821,	0.1770,	0.1723,	0.1680,	0.1641,	0.1606,	0.1573,	0.1545,	0.1520,	0.1497,	0.1478,	0.1462,	0.1449,	0.1438,	0.1429,	0.1422,	0.1418 };

	volat.set_vol_strike(vol_strike, 17);
	volat.set_vol_term(vol_term, 10);

	volat.set_volsurface_by_term(t0, 0, 17);
	volat.set_volsurface_by_term(t1, 1, 17);
	volat.set_volsurface_by_term(t2, 2, 17);
	volat.set_volsurface_by_term(t3, 3, 17);
	volat.set_volsurface_by_term(t4, 4, 17);
	volat.set_volsurface_by_term(t5, 5, 17);
	volat.set_volsurface_by_term(t6, 6, 17);
	volat.set_volsurface_by_term(t7, 7, 17);
	volat.set_volsurface_by_term(t8, 8, 17);
	volat.set_volsurface_by_term(t9, 9, 17);

	std::vector<double> v_rts(std::begin(arr_rts), std::end(arr_rts));
	std::vector<double> v_r(std::begin(arr_r), std::end(arr_r));
	std::vector<double> v_qts(std::begin(arr_qts), std::end(arr_qts));
	std::vector<double> v_q(std::begin(arr_q), std::end(arr_q));

	Rate r(v_r, v_rts);
	Rate q(v_q, v_qts);


	//double intpvol = volat.getInpVol(1.0, putstrike);  //flat interpolation use .getBSVol

	//상품타입변경
	//bool is_flat_vol = true;
	//int hitflag = 1;
	//long nM = 30000;

	//***상품타입변경
	double spot = 297.22;
	signed int vd = 43340;
	double refprice = 297.22;
	double putstrike = spot;

	return MarketParameters(vd, spot, volat, r, q);
}

MarketParam set_para_flat()
{
	double arr_rts[] = { 0.002739726,0.25,0.5,0.75,1,1.5,2,2.5,3,4,5,7,10,15,20 };
	double arr_r[] = { 1.79000,1.81000,1.74000,1.69000,1.65000,1.59000,1.55000,1.52000,1.50000,1.50000,1.50000,1.50000,1.50000,1.50000 ,1.50000 };
	//double arr_r[] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0 ,0 };

	for (int i = 0; i<15; i++)
		arr_r[i] /= 100.0;

	double arr_qts[] = { 0.01918000 ,	0.03836000 ,	0.05753000 ,	0.07671000 	,0.09589000 ,	0.11507000 	,0.13425000 ,	0.16438000 ,	0.24658000, 	0.32877000 ,	0.41096000 	,0.49315000 	,0.57534000 ,	0.65753000 ,	0.73973000 ,	0.82192000 ,	0.90411000 ,	0.98630000 ,	1.06849000 ,	1.15068000 	,1.23288000 ,	1.31507000 ,	1.39726000 ,	1.47945000 	,1.56164000 	,1.64384000 ,	1.72603000 ,	1.80822000 ,	1.89041000 ,	1.97260000 ,	2.05479000 ,2.13699000 	,2.21918000, 	2.30137000 ,	2.38356000 	,2.46575000 ,	2.54795000 ,	2.63014000 ,	2.71233000 ,	2.79452000 ,	2.87671000, 	2.95890000 };
	//double arr_q[] = { 0.00000,0.00000,0.00000,0.00000,2.33330,1.94438,1.666590,1.361110,0.907370,0.680540,4.321890,3.601590,3.087090,3.035340,2.698050,2.428250,2.517120,2.307370,2.129880,2.172190,2.027360,1.900660,2.957910,2.793580,2.646550,2.648480,2.522360,2.407710,2.456450,2.354100,2.259940,2.278180,2.193800,2.115460,2.744900,2.653400,2.567800,2.571480,2.493550,2.420220,2.451880,2.383780 };
	double arr_q[] = { 0.00000,0.00000,0.00000,0.00000,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

	for (int i = 0; i<42; i++)
		arr_q[i] /= 100.0;

	//double spot=297.22;
	double vol_term[] = { 0.08333,0.16667,0.250000,0.500000,0.750000,1.000000,1.500000,2.000000,2.500000,3.000000 };
	double vol_strike[] = { 178.332,193.193,208.054,222.915,237.776,252.637,267.498,282.359,297.22,312.081,326.942,341.803,356.664,371.525,386.386,401.247,416.108 };//17spot
	Vol volat(10, 17);
	double t0[] = { 0.3484,	0.3310,	0.3135,	0.2960,	0.2778,	0.2577, 0.2279, 0.1924, 0.1641, 0.1532, 0.1618, 0.1729, 0.1807, 0.1883, 0.1968,	0.2034,	0.2090 };
	double t1[] = { 0.3182,	0.3002,	0.2822,	0.2640,	0.2454,	0.2252,	0.2010,	0.1770,	0.1568,	0.1452,	0.1429,	0.1484,	0.1565,	0.1645,	0.1722,	0.1792,	0.1855 };
	double t2[] = { 0.3069,	0.2883,	0.2697,	0.2509,	0.2318,	0.2118,	0.1907,	0.1699,	0.1527,	0.1432,	0.1423,	0.1476,	0.1554,	0.1634,	0.1711,	0.1782,	0.1849 };
	double t3[] = { 0.2709,	0.2543,	0.2380,	0.2218,	0.2058,	0.1900,	0.1748,	0.1607,	0.1491,	0.1418,	0.1395,	0.1411,	0.1451,	0.1502,	0.1555,	0.1609,	0.1662 };
	double t4[] = { 0.2441,	0.2300,	0.2163,	0.2029,	0.1901,	0.1778,	0.1664,	0.1562,	0.1479,	0.1423,	0.1396,	0.1394,	0.1410,	0.1437,	0.1470,	0.1506,	0.1543 };
	double t5[] = { 0.2277,	0.2156,	0.2041,	0.1930,	0.1826,	0.1727,	0.1636,	0.1556,	0.1490,	0.1440,	0.1409,	0.1395,	0.1396,	0.1406,	0.1424,	0.1446,	0.1471 };
	double t6[] = { 0.2073, 0.1981,	0.1895,	0.1814,	0.1739,	0.1669,	0.1606,	0.1550,	0.1503,	0.1465,	0.1436,	0.1416,	0.1404,	0.1400,	0.1401,	0.1407,	0.1417 };
	double t7[] = { 0.1944,	0.1871,	0.1802,	0.1739,	0.1681,	0.1629,	0.1581,	0.1539,	0.1503,	0.1473,	0.1449,	0.1431,	0.1418,	0.1410,	0.1406,	0.1405,	0.1407 };
	double t8[] = { 0.1876,	0.1815,	0.1760,	0.1709,	0.1662,	0.1619,	0.1580,	0.1546,	0.1516,	0.1490,	0.1468,	0.1451,	0.1436,	0.1426,	0.1418,	0.1413,	0.1410 };
	double t9[] = { 0.1821,	0.1770,	0.1723,	0.1680,	0.1641,	0.1606,	0.1573,	0.1545,	0.1520,	0.1497,	0.1478,	0.1462,	0.1449,	0.1438,	0.1429,	0.1422,	0.1418 };

	volat.set_vol_strike(vol_strike, 17);
	volat.set_vol_term(vol_term, 10);

	volat.set_volsurface_by_term(t0, 0, 17);
	volat.set_volsurface_by_term(t1, 1, 17);
	volat.set_volsurface_by_term(t2, 2, 17);
	volat.set_volsurface_by_term(t3, 3, 17);
	volat.set_volsurface_by_term(t4, 4, 17);
	volat.set_volsurface_by_term(t5, 5, 17);
	volat.set_volsurface_by_term(t6, 6, 17);
	volat.set_volsurface_by_term(t7, 7, 17);
	volat.set_volsurface_by_term(t8, 8, 17);
	volat.set_volsurface_by_term(t9, 9, 17);

	std::vector<double> v_rts(std::begin(arr_rts), std::end(arr_rts));
	std::vector<double> v_r(std::begin(arr_r), std::end(arr_r));
	std::vector<double> v_qts(std::begin(arr_qts), std::end(arr_qts));
	std::vector<double> v_q(std::begin(arr_q), std::end(arr_q));

	Rate r(v_r, v_rts);
	Rate q(v_q, v_qts);


	//double intpvol = volat.getInpVol(1.0, putstrike);  //flat interpolation use .getBSVol

	//상품타입변경
	//bool is_flat_vol = true;
	//int hitflag = 1;
	//long nM = 30000;

	//***상품타입변경
	double spot = 297.22;
	signed int vd = 43340;
	double refprice = 297.22;
	double putstrike = spot;

	return MarketParam(vd, spot, volat, r, q);
}

MarketParam set_para()
{
	double arr_rts[] = { 0.002739726,0.25,0.5,0.75,1,1.5,2,2.5,3,4,5,7,10,15,20 };
	double arr_r[] = { 1.79000,1.81000,1.74000,1.69000,1.65000,1.59000,1.55000,1.52000,1.50000,1.50000,1.50000,1.50000,1.50000,1.50000 ,1.50000 };

	for (int i = 0; i<15; i++)
		arr_r[i] /= 100.0;

	double arr_qts[] = { 0.01918000 ,	0.03836000 ,	0.05753000 ,	0.07671000 	,0.09589000 ,	0.11507000 	,0.13425000 ,	0.16438000 ,	0.24658000, 	0.32877000 ,	0.41096000 	,0.49315000 	,0.57534000 ,	0.65753000 ,	0.73973000 ,	0.82192000 ,	0.90411000 ,	0.98630000 ,	1.06849000 ,	1.15068000 	,1.23288000 ,	1.31507000 ,	1.39726000 ,	1.47945000 	,1.56164000 	,1.64384000 ,	1.72603000 ,	1.80822000 ,	1.89041000 ,	1.97260000 ,	2.05479000 ,2.13699000 	,2.21918000, 	2.30137000 ,	2.38356000 	,2.46575000 ,	2.54795000 ,	2.63014000 ,	2.71233000 ,	2.79452000 ,	2.87671000, 	2.95890000 };
	double arr_q[] = { 0.00000,0.00000,0.00000,0.00000,2.33330,1.94438,1.666590,1.361110,0.907370,0.680540,4.321890,3.601590,3.087090,3.035340,2.698050,2.428250,2.517120,2.307370,2.129880,2.172190,2.027360,1.900660,2.957910,2.793580,2.646550,2.648480,2.522360,2.407710,2.456450,2.354100,2.259940,2.278180,2.193800,2.115460,2.744900,2.653400,2.567800,2.571480,2.493550,2.420220,2.451880,2.383780 };
	for (int i = 0; i<42; i++)
		arr_q[i] /= 100.0;

	//double spot=297.22;
	double vol_term[] = { 0.08333,0.16667,0.250000,0.500000,0.750000,1.000000,1.500000,2.000000,2.500000,3.000000 };
	double vol_strike[] = { 178.332,193.193,208.054,222.915,237.776,252.637,267.498,282.359,297.22,312.081,326.942,341.803,356.664,371.525,386.386,401.247,416.108 };//17spot
	Vol volat(10, 17);
	double t0[] = { 0.3484,	0.3310,	0.3135,	0.2960,	0.2778,	0.2577, 0.2279, 0.1924, 0.1641, 0.1532, 0.1618, 0.1729, 0.1807, 0.1883, 0.1968,	0.2034,	0.2090 };
	double t1[] = { 0.3182,	0.3002,	0.2822,	0.2640,	0.2454,	0.2252,	0.2010,	0.1770,	0.1568,	0.1452,	0.1429,	0.1484,	0.1565,	0.1645,	0.1722,	0.1792,	0.1855 };
	double t2[] = { 0.3069,	0.2883,	0.2697,	0.2509,	0.2318,	0.2118,	0.1907,	0.1699,	0.1527,	0.1432,	0.1423,	0.1476,	0.1554,	0.1634,	0.1711,	0.1782,	0.1849 };
	double t3[] = { 0.2709,	0.2543,	0.2380,	0.2218,	0.2058,	0.1900,	0.1748,	0.1607,	0.1491,	0.1418,	0.1395,	0.1411,	0.1451,	0.1502,	0.1555,	0.1609,	0.1662 };
	double t4[] = { 0.2441,	0.2300,	0.2163,	0.2029,	0.1901,	0.1778,	0.1664,	0.1562,	0.1479,	0.1423,	0.1396,	0.1394,	0.1410,	0.1437,	0.1470,	0.1506,	0.1543 };
	double t5[] = { 0.2277,	0.2156,	0.2041,	0.1930,	0.1826,	0.1727,	0.1636,	0.1556,	0.1490,	0.1440,	0.1409,	0.1395,	0.1396,	0.1406,	0.1424,	0.1446,	0.1471 };
	double t6[] = { 0.2073, 0.1981,	0.1895,	0.1814,	0.1739,	0.1669,	0.1606,	0.1550,	0.1503,	0.1465,	0.1436,	0.1416,	0.1404,	0.1400,	0.1401,	0.1407,	0.1417 };
	double t7[] = { 0.1944,	0.1871,	0.1802,	0.1739,	0.1681,	0.1629,	0.1581,	0.1539,	0.1503,	0.1473,	0.1449,	0.1431,	0.1418,	0.1410,	0.1406,	0.1405,	0.1407 };
	double t8[] = { 0.1876,	0.1815,	0.1760,	0.1709,	0.1662,	0.1619,	0.1580,	0.1546,	0.1516,	0.1490,	0.1468,	0.1451,	0.1436,	0.1426,	0.1418,	0.1413,	0.1410 };
	double t9[] = { 0.1821,	0.1770,	0.1723,	0.1680,	0.1641,	0.1606,	0.1573,	0.1545,	0.1520,	0.1497,	0.1478,	0.1462,	0.1449,	0.1438,	0.1429,	0.1422,	0.1418 };

	volat.set_vol_strike(vol_strike, 17);
	volat.set_vol_term(vol_term, 10);

	volat.set_volsurface_by_term(t0, 0, 17);
	volat.set_volsurface_by_term(t1, 1, 17);
	volat.set_volsurface_by_term(t2, 2, 17);
	volat.set_volsurface_by_term(t3, 3, 17);
	volat.set_volsurface_by_term(t4, 4, 17);
	volat.set_volsurface_by_term(t5, 5, 17);
	volat.set_volsurface_by_term(t6, 6, 17);
	volat.set_volsurface_by_term(t7, 7, 17);
	volat.set_volsurface_by_term(t8, 8, 17);
	volat.set_volsurface_by_term(t9, 9, 17);

	std::vector<double> v_rts(std::begin(arr_rts), std::end(arr_rts));
	std::vector<double> v_r(std::begin(arr_r), std::end(arr_r));
	std::vector<double> v_qts(std::begin(arr_qts), std::end(arr_qts));
	std::vector<double> v_q(std::begin(arr_q), std::end(arr_q));

	Rate r(v_r, v_rts);
	Rate q(v_q, v_qts);


	//double intpvol = volat.getInpVol(1.0, putstrike);  //flat interpolation use .getBSVol

	//상품타입변경
	//bool is_flat_vol = true;
	//int hitflag = 1;
	//long nM = 30000;

	//***상품타입변경
	double spot = 297.22;
	signed int vd = 43340;
	double refprice = 297.22;
	double putstrike = spot;

	return MarketParam(vd, spot, volat, r, q);
}
void test_calc2_mc_vanilla_paras_module(MarketParameters& paras, long n)
{
	//instrument(vanilla)
	double refprice = paras.get_spot(); //spot과 동일(거래일 평가)
	double putstrike = refprice; //100%, ATM
	signed int exd = paras.get_vdate()+365; //1y
	PayoffPut putpay(putstrike);

	EuropeanOption EurPut(refprice, exd, putpay);

	EuropeanOptionMC EurPutMC(refprice, exd, putpay);
	EuropeanOptionMC EurPutMC_paras(refprice, exd, putpay);


	//EurPut.Calc(para);
	EurPut.Calc2(paras);
	//EurPutMC.Calc(para,n);
	EurPutMC_paras.Calc2(paras, n);

	std::cout.precision(8);
	std::cout << std::fixed;

	//std::vector<double> rs = EurPut.GetResult();
	vector<double> rs_fdm_paras_module = EurPut.GetResult();
	//vector<double> rs3 = EurPutMC.GetResult();
	vector<double> rs_mc_paras_module = EurPutMC_paras.GetResult();


	cout << "result : test_calc2_mc_vanilla_paras_module \n";
	//std::cout << "pv by calc1 localvol r,q term " << rs[0] << "=%price " << rs[0] / spot * 100 << "%" << std::endl;
	std::cout << "pv by calc2 localvol r,q term module " << rs_fdm_paras_module[0] << "=%price " << rs_fdm_paras_module[0] / refprice * 100 << "%" << std::endl;
	//cout << "pv by mc numMC="  <<n << "  "  << rs3[0] << "=%price " << rs3[0] / spot * 100 << "%" << std::endl;
	cout << "pv by mc paras module numMC(paras)=" << n << "  " << rs_mc_paras_module[0] << "=%price " << rs_mc_paras_module[0] / refprice * 100 << "%" << std::endl;

}

void test_calc2_mc_vanilla_paras_module_inst(MarketParameters& paras, EuropeanOptionMC& eop, long n)
{
	eop.Calc2(paras, n);

	std::cout.precision(8);
	std::cout << std::fixed;
	vector<double> rs_mc_paras_module_inst = eop.GetResult();

	cout << "result : test_calc2_mc_vanilla_paras_module_inst \n ";
	cout << "pv by mc paras module numMC(paras) instrument=" << n << "  " << rs_mc_paras_module_inst[0] << "=%price " << rs_mc_paras_module_inst[0] / eop.GetRefPrice() * 100 << "%" << std::endl;
}

void simulation(PayoffAutocallStd* ThePayoffPtr, MarketParameters& paras)
{
	
	vector<vector<double> > paths;
	long numMC_ = 3;
	signed int vd = 43340;
	int hitFlag=0;
	double refprice = ThePayoffPtr->GetRefPrice();
	
	assert(ThePayoffPtr->GetRefPrice() == paras.get_spot());
	assert(paras.get_vdate() == vd);

	double s0 = paras.get_spot();
	int nb_autocall = ThePayoffPtr->GetNbAutocall();
	paras.calcLV();

	std::vector<signed int> autocall_date;
	autocall_date = ThePayoffPtr->GetAutocall_date();

	double kibarrier = ThePayoffPtr->GetKiBarrier();

	std::vector<double> autocall_strike;
	autocall_strike = ThePayoffPtr->GetAutocall_strike();

	std::vector<double> autocall_coupon;
	autocall_coupon = ThePayoffPtr->GetAutocall_coupon();

	//std::random_device rd;
	std::mt19937 gen(130);
	std::normal_distribution<>ndist(0, 1);
	//double* mcvalues = new double[numMC_];
	vector<double> mcvalues;

	double put_strike = ThePayoffPtr->GetPutStrike();
	double dummy_coupon = ThePayoffPtr->GetDummyCoupon();

	double s_tmp;
	int tmpKIFlag;
	int daydivide_ = 1;
	

	double* tau_p = new double[autocall_date[nb_autocall] - vd + 1];
	double* r_forward_p = new double[autocall_date[nb_autocall] - vd + 1];
	double* r_dc_p = new double[autocall_date[nb_autocall] - vd + 1];
	double* q_forward_p = new double[autocall_date[nb_autocall] - vd + 1];

	for (signed int i = 0; i <= autocall_date[nb_autocall] - vd; i++) {
		tau_p[i] = (i) / 365.0;
		r_forward_p[i] = paras.getForward(tau_p[i]);
		r_dc_p[i] = paras.getIntpRate(tau_p[i]);
		q_forward_p[i] = paras.getDivForward(tau_p[i]);
	}

	double dt = 1 / 365.0;

	int *idxT = new signed int[autocall_date[nb_autocall] - vd + 1];
	for (int tfv = 0; tfv <= autocall_date[nb_autocall] - vd; tfv++) {
		idxT[tfv] = paras.find_index_term(tfv / 365.0);
	}


	for (long i = 0; i<numMC_; i++)
	{
		vector<double> path;
		path.push_back(paras.get_spot());

		s_tmp = s0;
		tmpKIFlag = hitFlag;

		for (int k = 1; k <= nb_autocall; k++) {
			for (signed int t = std::max(autocall_date[k - 1], vd) + 1; t <= autocall_date[k]; t++) {

				//double short_vol = paras.lvol(tau_p[t - vd], s_tmp);
				double short_vol = paras.get_Lvol_hybrid(idxT[t - vd], s_tmp);

				double drift = (r_forward_p[t - vd] - q_forward_p[t - vd] - 0.5*short_vol*short_vol)*dt;
				double diff = short_vol*std::sqrt(dt);

				for (long t2 = 1; t2 <= daydivide_; t2++) {
					s_tmp = s_tmp*std::exp(drift + diff*ndist(gen));
					path.push_back(s_tmp);
				
					if (s_tmp<kibarrier)
						tmpKIFlag = 1;
				}
			}

			if (s_tmp >= autocall_strike[k]) { //check autocallability
				mcvalues.push_back(std::exp(-r_dc_p[autocall_date[k] - vd] * tau_p[autocall_date[k] - vd])*(1.0 + autocall_coupon[k]));
				break; //k loop
			}

			//we are here because it hasn't been autocalled
			if (k == nb_autocall) {

				if (s_tmp >= autocall_strike[k]) {
					mcvalues.push_back(std::exp(-r_dc_p[autocall_date[k] - vd] * tau_p[autocall_date[k] - vd])*(1.0 + autocall_coupon[k]));
				}
				else if (s_tmp >= kibarrier) {
					if (tmpKIFlag == 1) {
						mcvalues.push_back(std::exp(-r_dc_p[autocall_date[k] - vd] * tau_p[autocall_date[k] - vd])*(1.0 - std::max((put_strike - s_tmp) / refprice, 0.0)));
					}
					else if (tmpKIFlag == 0) {
						mcvalues.push_back(std::exp(-r_dc_p[autocall_date[k] - vd] * tau_p[autocall_date[k] - vd])*(1.0 + dummy_coupon));
					}
					else {
						throw std::logic_error("unexpected KIFlag");
					}
				}
				else {
					mcvalues.push_back(std::exp(-r_dc_p[autocall_date[k] - vd] * tau_p[autocall_date[k] - vd])*(1.0 - std::max((put_strike - s_tmp) / refprice, 0.0)));
				}

			} //if k

		}//for k

		paths.push_back(path);
	}//for(i=0..)

	double npv = accumulate(mcvalues.begin(), mcvalues.end(), 0.0) / mcvalues.size();

	/*Calculate delta grid , output : vold, uold*/
	//double s0 = paras.get_spot();
	//signed int vd = paras.get_vdate();

	//int nb_autocall = ThePayoffPtr->GetNbAutocall();

	//paras.calcLV();

	//std::vector<signed int> autocall_date;
	//autocall_date = ThePayoffPtr->GetAutocall_date();

	int maxassetnodeindex = 400;
	double *px = new double[maxassetnodeindex + 1];
	double *dpx = new double[maxassetnodeindex + 1];
	double *alpha = new double[maxassetnodeindex + 1];
	double *alpha_up = new double[maxassetnodeindex + 1];
	double *alpha_down = new double[maxassetnodeindex + 1];

	double *beta = new double[maxassetnodeindex + 1];
	double *vold = new double[maxassetnodeindex + 1];
	double *vold_up = new double[maxassetnodeindex + 1];
	double *vold_down = new double[maxassetnodeindex + 1];

	double *uold = new double[maxassetnodeindex + 1];
	double *uold_up = new double[maxassetnodeindex + 1];
	double *uold_down = new double[maxassetnodeindex + 1];

	double *vold_next = new double[maxassetnodeindex + 1];  //reserve for theta
	double *uold_next = new double[maxassetnodeindex + 1];  //reserve for theta

	double *vnew = new double[maxassetnodeindex + 1];
	double *vnew_up = new double[maxassetnodeindex + 1];
	double *vnew_down = new double[maxassetnodeindex + 1];

	double *unew = new double[maxassetnodeindex + 1];
	double *unew_up = new double[maxassetnodeindex + 1];
	double *unew_down = new double[maxassetnodeindex + 1];

	double *A = new double[maxassetnodeindex + 1];
	double *A_up = new double[maxassetnodeindex + 1];
	double *A_down = new double[maxassetnodeindex + 1];

	double *B = new double[maxassetnodeindex + 1];
	double *B_up = new double[maxassetnodeindex + 1];
	double *B_down = new double[maxassetnodeindex + 1];

	double *C = new double[maxassetnodeindex + 1];
	double *C_up = new double[maxassetnodeindex + 1];
	double *C_down = new double[maxassetnodeindex + 1];

	px[0] = 0.0;
	double tmp_ds = refprice*2.0 / maxassetnodeindex;
	for (int i = 1; i <= maxassetnodeindex; i++)
		px[i] = px[i - 1] + tmp_ds;
	for (int i = 0; i<maxassetnodeindex; i++) //max index of dp is max index of px -1
		dpx[i] = px[i + 1] - px[i];

	ThePayoffPtr->ResetFDGrid(px, dpx, 1, maxassetnodeindex - 1);
	int kiindex = 0;
	kiindex = findlowerindex(px, ThePayoffPtr->GetKiBarrier(), 0, maxassetnodeindex);

	//final BC
	ThePayoffPtr->final_updator(vold, uold, px, 0, maxassetnodeindex);
	for (int i = 0; i <= maxassetnodeindex; i++) {
		vold_up[i] = vold[i];
		vold_down[i] = vold[i];
		uold_up[i] = uold[i];
		uold_down[i] = uold[i];
	}

	
	/*save vold, uold in vector*/
	vector<vector<double> > vgrid;
	vector<vector<double> > ugrid;

	//vector<double> v(vold, vold + (maxassetnodeindex + 1));
	//vector<double> u(uold, uold + (maxassetnodeindex + 1));
	
	vgrid.push_back(vector<double>(vold, vold + (maxassetnodeindex + 1)));
	ugrid.push_back(vector<double>(uold, uold + (maxassetnodeindex + 1)));



	signed int t;
	//double* tau_p = new double[autocall_date[nb_autocall] - vd + 1];
	//double* r_forward_p = new double[autocall_date[nb_autocall] - vd + 1];
	//double* r_dc_p = new double[autocall_date[nb_autocall] - vd + 1];
	//double* q_forward_p = new double[autocall_date[nb_autocall] - vd + 1];

	/*for (signed int i = 0; i <= autocall_date[nb_autocall] - vd; i++) {
		tau_p[i] = (i) / 365.0;
		r_forward_p[i] = paras.getForward(tau_p[i]);
		r_dc_p[i] = paras.getIntpRate(tau_p[i]);
		q_forward_p[i] = paras.getDivForward(tau_p[i]);
	}
*/
	//double dt = 1 / 365.0;

	int *idxS = new int[maxassetnodeindex + 1];
//	int *idxT = new signed int[autocall_date[nb_autocall] - vd + 1];
	for (int i = 0; i <= maxassetnodeindex; i++) {
		idxS[i] = paras.find_index_spot(px[i]);
	}

	//for (int tfv = 0; tfv <= autocall_date[nb_autocall] - vd; tfv++) {
	//	idxT[tfv] = paras.find_index_term(tfv / 365.0);
	//}

	for (int k = nb_autocall; k > 0; k--) {
		for (t = autocall_date[k]; t >= std::max(vd + 1, autocall_date[k - 1] + 1); t--) {

			if (t == vd + 1) {
				for (int i = 0; i <= maxassetnodeindex; i++) {
					vold_next[i] = vold[i];  //theta
					uold_next[i] = uold[i];  //theta
				}
			}

			for (int i = 0; i <= maxassetnodeindex; i++) {
				//double short_vol = paras.lvol(tau_p[t - vd], px[i]);
				//double short_vol_up = paras.lvol_up(tau_p[t - vd], px[i]);
				//double short_vol_down = paras.lvol_down(tau_p[t - vd], px[i]);
				double short_vol = paras.get_Lvol(idxT[t - vd], idxS[i]);
				double short_vol_up = paras.get_Lvol_up(idxT[t - vd], idxS[i]);
				double short_vol_down = paras.get_Lvol_down(idxT[t - vd], idxS[i]);

				alpha[i] = 0.5*short_vol*short_vol*dt;
				alpha_up[i] = 0.5*short_vol_up*short_vol_up*dt;
				alpha_down[i] = 0.5*short_vol_down*short_vol_down*dt;

				beta[i] = (r_forward_p[t - vd] - q_forward_p[t - vd])*dt;
			}

			trimatrix1d(A, B, C, alpha, beta, r_forward_p[t - vd], dt, px, dpx, 1, maxassetnodeindex - 1);
			trimatrix1d(A_up, B_up, C_up, alpha_up, beta, r_forward_p[t - vd], dt, px, dpx, 1, maxassetnodeindex - 1);
			trimatrix1d(A_down, B_down, C_down, alpha_down, beta, r_forward_p[t - vd], dt, px, dpx, 1, maxassetnodeindex - 1);

			trimxsolve1d(A, B, C, vold, vnew, 0, maxassetnodeindex, 0, 0);
			trimxsolve1d(A_up, B_up, C_up, vold_up, vnew_up, 0, maxassetnodeindex, 0, 0);
			trimxsolve1d(A_down, B_down, C_down, vold_down, vnew_down, 0, maxassetnodeindex, 0, 0);


			for (int i = 0; i <= maxassetnodeindex; i++) {
				vold[i] = vnew[i];
				vold_up[i] = vnew_up[i];
				vold_down[i] = vnew_down[i];
			}

			ThePayoffPtr->copy_v_to_u(vnew, unew, kiindex, kiindex);
			ThePayoffPtr->copy_v_to_u(vnew_up, unew_up, kiindex, kiindex);
			ThePayoffPtr->copy_v_to_u(vnew_down, unew_down, kiindex, kiindex);

			trimxsolve1d(A, B, C, uold, unew, kiindex, maxassetnodeindex, 1, 0);
			trimxsolve1d(A_up, B_up, C_up, uold_up, unew_up, kiindex, maxassetnodeindex, 1, 0);
			trimxsolve1d(A_down, B_down, C_down, uold_down, unew_down, kiindex, maxassetnodeindex, 1, 0);

			ThePayoffPtr->copy_v_to_u(unew, uold, 0, maxassetnodeindex);
			ThePayoffPtr->copy_v_to_u(unew_down, uold_down, 0, maxassetnodeindex);
			ThePayoffPtr->copy_v_to_u(unew_up, uold_up, 0, maxassetnodeindex);

			vgrid.push_back(vector<double>(vold, vold + (maxassetnodeindex + 1)));
			ugrid.push_back(vector<double>(uold, uold + (maxassetnodeindex + 1)));

		}//for t

		 //update BC
		if (t == autocall_date[k - 1]) {
			ThePayoffPtr->updator(t, vold, uold, px, 0, maxassetnodeindex);
			ThePayoffPtr->updator(t, vold_up, uold_up, px, 0, maxassetnodeindex);
			ThePayoffPtr->updator(t, vold_down, uold_down, px, 0, maxassetnodeindex);
		}
		if (t == vd)
			break;
	}

	double pv, pv_next, pv_up, pv_down;

	//if (hitflag) { //hitted -> vold 
	//	pv = intp1d(s0, px, vold, 1, maxassetnodeindex - 1);
	//	pv_next = intp1d(s0, px, vold_next, 1, maxassetnodeindex - 1);
	//	pv_up = intp1d(s0*1.01, px, vold_up, 1, maxassetnodeindex - 1);
	//	pv_down = intp1d(s0*0.99, px, vold_down, 1, maxassetnodeindex - 1);
	//}
	//else {
	//	pv = intp1d(s0, px, uold, 1, maxassetnodeindex - 1);
	//	pv_next = intp1d(s0, px, uold_next, 1, maxassetnodeindex - 1);
	//	pv_up = intp1d(s0*1.01, px, uold_up, 1, maxassetnodeindex - 1);
	//	pv_down = intp1d(s0*0.99, px, uold_down, 1, maxassetnodeindex - 1);
	//}

	//result.resize(30, 0.0);
	//result[0] = pv;
	//result[1] = (pv_up - pv_down) / (s0*0.02);
	//result[2] = (pv_up - 2.0*pv + pv_down) / (s0*0.01) / (s0*0.01);
	//result[3] = 0;  //vega

	//if (vd == expiry_date) {
	//	result[4] = 0;
	//}
	//else {
	//	result[4] = pv_next - pv;  //theta
	//}

	//result[5] = s0;

	delete[] px;
	delete[] dpx;
	delete[] alpha;
	delete[] alpha_up;
	delete[] alpha_down;
	delete[] beta;
	delete[] vold;
	delete[] vold_up;
	delete[] vold_down;
	delete[] uold;
	delete[] uold_up;
	delete[] uold_down;
	delete[] vnew;
	delete[] vnew_up;
	delete[] vnew_down;
	delete[] vold_next;
	delete[] unew;
	delete[] unew_up;
	delete[] unew_down;
	delete[] uold_next;

	delete[] A;
	delete[] A_up;
	delete[] A_down;
	delete[] B;
	delete[] B_up;
	delete[] B_down;
	delete[] C;
	delete[] C_up;
	delete[] C_down;

//	delete[] tau_p;
//	delete[] r_forward_p;
//	delete[] r_dc_p;
//	delete[] q_forward_p;
	delete[] idxT;
	delete[] idxS;
	/********************/
	
	//delete[] mcvalues;
	delete[] tau_p;
	delete[] r_forward_p;
	delete[] r_dc_p;
	delete[] q_forward_p;


}

int main()
{	

	using clock = std::chrono::system_clock;
	using sec = std::chrono::duration<double>;

	//market parameter module
	MarketParameters paras;
	MarketParameters paras_flat;
	MarketParam para;
	MarketParam para_flat;

	paras=set_paras();
	para = set_para();
	para_flat = set_para_flat();
	paras_flat = set_paras_flat();
	//instrument 
	unsigned int exd = paras.get_vdate() + 365; //1y vanilla
	double refprice = 297.22;
	double putstrike = refprice;
	PayoffPut putpay(putstrike);
	EuropeanOption EurPut(refprice, exd, putpay);
	EuropeanOptionMC EurPutMC(refprice, exd, putpay);

	//Make autocallable option
	int hitflag = 0;
	signed int exd_3y = 44436; //3y 
	int nb_autocall = 6;
	signed int auto_date[7] = { -1,43524,43705,43889,44071,44255,44436 };
	double auto_strike[7] = { -1,297.22,	297.22,	282.359	,282.359,	267.498,	267.498 };
	double auto_coupon[7] = { -1, 0.0230,	0.0460,	0.0690,	0.0920,	0.1150,	0.1380 };
	double auto_ki_barrier = refprice*0.6;
	double auto_dummy_coupon = auto_coupon[6];
	double auto_put_strike = refprice*1.0; // if put_strike=0, notional protected

	PayoffAutocallStd autoPayoff(nb_autocall, auto_date, auto_strike, auto_coupon, auto_ki_barrier, auto_put_strike, auto_dummy_coupon, refprice);
	AutocallOption AutoKOSPI(refprice, exd_3y, autoPayoff, hitflag);
	AutocallOption AutoKOSPI_mc(refprice, exd_3y, autoPayoff, hitflag);

	AutocallOption AutoKOSPI_KI(refprice, exd_3y, autoPayoff, 1);
	AutocallOption AutoKOSPI_mc_KI(refprice, exd_3y, autoPayoff, 1);


	/*Test*/
	/*test cal1 vs calc2 for vanilla option*/
	//test_calc1_vs_calc2_for_vanilla();

	/*test for mc vanilla	*/
	//test_calc2_mc_for_vanilla(50000);
	//test_calc2_mc_vanilla_paras_module(paras, 50000);
	//test_calc2_mc_vanilla_paras_module_inst(paras, EurPutMC, 50000);

	/*test calc2 for param vs params vanilla	*/
	//test_calc2_for_vanilla_param_params();  //parameter vs parameters -> outdated 
	//test_calc2_for_vanilla_param_params_module(paras); //-> outdated
	//test_calc2_for_vanilla_param_params_module_inst(paras, EurPut);
	//test_calc2_for_vanilla_param_params_module_inst(para, EurPut);
	//cout << "\nnow flat dividend\n";
	//test_calc2_for_vanilla_param_params_module_inst(paras_flat, EurPut);
	//test_calc2_for_vanilla_param_params_module_inst(para_flat, EurPut);

	/*test for div transform*/
	//test_dividend();

	/*test for discrete div*/
	//test_calc2_discrete_localvol();
	/*Test autocall Calc2 vs Calc2_paras*/
	//test_autocall_Cal2_vs_paras();
	//test_autocall_Calc_vs_paras_paramodule(paras);



	/****Value autocall analysis between FD and MC*/ 
	auto before = clock::now();
	//test_autocall__fd_mc_old(10000);
	sec duration = clock::now() - before;
	//std::cout << "test_autocall__fd_mc_old(): It took " << duration.count() << "s" << std::endl;

	//before = clock::now();
	//test_autocall__fd_mc(para, 10000);
	//duration = clock::now() - before;
	//std::cout << "test_autocall__fd_mc(para) :It took " << duration.count() << "s" << std::endl;

	//before = clock::now();
	//test_autocall__fd_mc(paras, 1000);
	//duration = clock::now() - before;
	//std::cout << "test_autocall__fd_mc(paras) : It took " << duration.count() << "s" << std::endl;
	//
	//before = clock::now();
	//test_autocall__fd_mc_inst(paras, AutoKOSPI, 1000);
	//duration = clock::now() - before;
	//std::cout << "test_autocall__fd_mc_inst(paras, AutoKOSPI) : It took " << duration.count() << "s" << std::endl;

	//
	//before = clock::now();
	//test_autocall__fd_inst_swip_first(paras, AutoKOSPI);
	//duration = clock::now() - before;
	//std::cout << "test_autocall__fd_inst_swip_first(paras, AutoKOSPI, 1000); : It took " << duration.count() << "s" << std::endl;

	/*long nM = 10000;
	before = clock::now();
	test_autocall_fd_mc_inst_swip_first_time(paras, AutoKOSPI,nM);
	duration = clock::now() - before;
	std::cout << "test_autocall__mc_inst_swip_first_time(paras, AutoKOSPI,nM); nM="<<nM << " It took "  << duration.count() << "s" << std::endl;

	before = clock::now();
	test_autocall_fd_mc_inst_swip_first_time(paras, AutoKOSPI_KI, nM);
	duration = clock::now() - before;
	std::cout << "test_autocall__mc_inst_swip_first_time(paras, AutoKOSPI_KI, nM); nM=" << nM << " It took " << duration.count() << "s" << std::endl;

    */

	//before = clock::now();
	//test_autocall__fd_mc_inst_swip_first(para, AutoKOSPI);
	//duration = clock::now() - before;
	//std::cout << "test_autocall__fd_mc_inst(paras, AutoKOSPI) : It took " << duration.count() << "s" << std::endl;

	/*para vs paras for flat*/
	//before = clock::now();
	//test_autocall__fd_mc(para_flat);
	//duration = clock::now() - before;
	//std::cout << "test_autocall__fd_mc(para_flat) : It took " << duration.count() << "s" << std::endl;

	//before = clock::now();
	//test_autocall__fd_mc(paras_flat);
	//duration = clock::now() - before;
	//std::cout << "test_autocall__fd_mc(paras_flat) : It took " << duration.count() << "s" << std::endl;

	/*hedge simulation */
	simulation(&autoPayoff, paras);
	






	//long numMc = strtol(argv[1],nullptr, 10);
	//test_dividend();
	//test_european_discrete_localvol(spot);
	//test_asian(spot);
	//test_europeanMC(spot);
	//test_european(spot, numMc);
	//test_asian(spot, numMc);
	//test_american(spot);
	//test_european(spot);
	//test_european(spot*1.05);
	//test_barrier_flat(spot);
	//test_barrier_LV(spot);
	//test_autocall(spot,1);
	//test_autocall_div(spot, 1);
	//auto ms1 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	//test_autocall(spot,0,surface,calc1);  //pv hitted 0.98996 
	//auto  ms2 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	//std::cout << ms2 - ms1 << " calc1 millisecons last." << std::endl;
	//auto ms3 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	//test_autocall(spot+6.0, 0, surface,calc1);  //pv hitted 0.98996 
	//auto  ms4 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	//std::cout << ms4 - ms3 << " calc1 millisecons last." << std::endl;
	//auto ms1_2 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	//test_autocall(spot, 0, surface,calc2);  //pv hitted 0.98996 
	//auto  ms2_2 = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
	//std::cout << ms2_2 - ms1_2 << "calc2 millisecons last." << std::endl;
	//auto ms5 = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	//test_autocall_mc(spot, 1, 300000, surface);//
	//auto ms6 = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	//std::cout <<"duration : " << (ms6.count() - ms5.count())/1000.0 <<std::endl;
	//auto ms7 = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	//test_autocall_mc2(spot+6.0,1, 20000, surface);//
	//auto ms8 = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
	//std::cout << ms8.count() - ms7.count() << "millisecons last." << std::endl;
	//test_autocall_flatvol(spot, 1);
	//test_autocall_mc_flatvol(spot,1,30000);
	//PayoffBarCUO c(100, 100, 1);
	//TEST 2019.8.19
	//FDM interpolation optimization calc vs calc_old
	//simple market param
	//test_autocall_simple(spot, 1, surface, calc)(

	return 0;
}