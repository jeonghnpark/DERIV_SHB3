#include "EuropeanOptionMC.h"
#include <random>

using namespace std;
double EuropeanOptionMC::Calc(MarketParam & para, long numMc)
{
	double s0 = para.get_spot();
	Rate R = para.get_rfrate();
	Rate Q = para.get_q();
	signed int vd = para.get_vdate();
	signed int expiryd = GetExpiryd();

	Vol vol = para.get_vol();
	vol.calcLv(s0, R, Q);
	//	std::vector<signed int> obs

	std::mt19937 gen(130);
	std::normal_distribution<>ndist(0, 1);
	double* mcvalues = new double[numMc];

	double s_tmp;
//	double s_avg;

	int daydivide_ = 1;

	double* tau_p = new double[expiryd - vd + 1];
	double* r_forward_p = new double[expiryd - vd + 1];
	double* r_dc_p = new double[expiryd - vd + 1];
	double* q_forward_p = new double[expiryd - vd + 1];
	bool* is_obsDay = new bool[expiryd - vd + 1];

	for (signed int i = 0; i <= expiryd - vd; i++) {
		tau_p[i] = (i) / 365.0;
		r_forward_p[i] = R.getForward(tau_p[i]);
		r_dc_p[i] = R.getIntpRate(tau_p[i]);
		q_forward_p[i] = Q.getForward(tau_p[i]);
	}

	double dt = 1 / 365.0;

	for (long i = 0; i<numMc; i++)
	{
		s_tmp = s0;
	
		//make stock price dailiy 
		for (signed int t = vd + 1; t <= expiryd; t++) {
			double short_vol = vol.lvol(tau_p[t - vd], s_tmp);
			double drift = (r_forward_p[t - vd] - q_forward_p[t - vd] - 0.5*short_vol*short_vol)*dt;
			double diff = short_vol*std::sqrt(dt);
			s_tmp = s_tmp*std::exp(drift + diff*ndist(gen));
		}

		mcvalues[i] = std::exp(-r_dc_p[expiryd - vd] * tau_p[expiryd - vd])*((*ThePayoffPtr)(s_tmp));
	}//for(i=0..)

	double npv = 0.0;
	for (long i = 0; i<numMc; i++)
		npv += mcvalues[i];

	npv /= numMc;

	result[0] = npv;
	result[5] = s0;

	delete[] mcvalues;

	delete[] tau_p;
	delete[] r_forward_p;
	delete[] r_dc_p;
	delete[] q_forward_p;
	delete[] is_obsDay;

	return npv;
}

EuropeanOptionMC::~EuropeanOptionMC()
{
	//여기서 thePayoffPtr을 해제해서는 안됨. 부모클래스에서 해제함
}
