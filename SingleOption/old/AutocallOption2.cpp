//AutocallOption2 : time grid를 객체화하는 방식, 공사중..  20190410 

#include "AutocallOption2.h"
#include <algorithm>

AutocallOption2::AutocallOption2(double refprice_, signed int expiryd_, const PayoffAutocallable & ThePayoff_)
	:refprice(refprice_), expiry_date(expiryd_)
{
	ThePayoffPtr = ThePayoff_.clone();
	result = std::vector<double>(30);
}

AutocallOption2::~AutocallOption2()
{
	delete ThePayoffPtr;
}

double AutocallOption2::Calc2(MarketParam & para)
//underconstruction!!
{
	double s0 = para.get_spot();
	Rate R = para.get_rfrate();
	Rate Q = para.get_q();
	signed int vd = para.get_vdate();
	int nb_autocall = ThePayoffPtr->GetNbAutocall();

	Vol vol = para.get_vol();
	vol.calcLv(s0, R, Q);

	std::vector<signed int> autocall_date;
	autocall_date = ThePayoffPtr->GetAutocall_date();

	ThePayoffPtr->init(refprice,vd,vol);
	//ThePayoffPtr->init(vol);

	ThePayoffPtr->final_updator();

	signed int t;
	
	//double* tau_p = new double[autocall_date[nb_autocall] - vd + 1];
	//double* r_forward_p = new double[autocall_date[nb_autocall] - vd + 1];
	//double* r_dc_p = new double[autocall_date[nb_autocall] - vd + 1];
	//double* q_forward_p = new double[autocall_date[nb_autocall] - vd + 1];

	//for (signed int i = 0; i <= autocall_date[nb_autocall] - vd; i++) {
	//	tau_p[i] = (i) / 365.0;
	//	r_forward_p[i] = R.getForward(tau_p[i]);
	//	r_dc_p[i] = R.getIntpRate(tau_p[i]);
	//	q_forward_p[i] = Q.getForward(tau_p[i]);
	//}

	//double dt = 1 / 365.0;
	//int *idxS = new int[maxassetnodeindex + 1];
	//int *idxT = new signed int[autocall_date[nb_autocall] - vd + 1];
	//for (int i = 0; i <= maxassetnodeindex; i++) {
	//	idxS[i] = vol.find_index_spot(px[i]);
	//}

	//for (int tfv = 0; tfv <= autocall_date[nb_autocall] - vd; tfv++) {
	//	idxT[tfv] = vol.find_index_term(tfv / 365.0);
	//}


	for (int k = nb_autocall; k > 0; k--) {
		for (t = autocall_date[k]; t >= std::max(vd + 1, autocall_date[k - 1] + 1); t--) {
			//double tau = (t - vd) / 365.0; //time from vdate
			//double tfv = (t - vd) / 365.0;
			//double dt = 1 / 365.0;

			//double r_forward = R.getForward(tfv);
			//double q_forward = Q.getForward(tfv);

			if (t == vd + 1) 
				ThePayoffPtr->copy_for_theta();

			ThePayoffPtr->backward(para,t-vd);

			//for (int i = 0; i <= maxassetnodeindex; i++) {
			//	//double short_vol = vol.lvol(tau_p[t-vd], px[i]);
			//	//double short_vol_up = vol.lvol_up(tau_p[t - vd], px[i]);
			//	//double short_vol_down = vol.lvol_down(tau_p[t - vd], px[i]);
			//	double short_vol = vol.get_Lvol(idxT[t - vd], idxS[i]);
			//	double short_vol_up = vol.get_Lvol_up(idxT[t - vd], idxS[i]);
			//	double short_vol_down = vol.get_Lvol_down(idxT[t - vd], idxS[i]);

			//	alpha[i] = 0.5*short_vol*short_vol*dt;
			//	alpha_up[i] = 0.5*short_vol_up*short_vol_up*dt;
			//	alpha_down[i] = 0.5*short_vol_down*short_vol_down*dt;

			//	beta[i] = (r_forward_p[t - vd] - q_forward_p[t - vd])*dt;
			//}

			//trimatrix1d(A, B, C, alpha, beta, r_forward_p[t - vd], dt, px, dpx, 1, maxassetnodeindex - 1);
			//trimatrix1d(A_up, B_up, C_up, alpha_up, beta, r_forward_p[t - vd], dt, px, dpx, 1, maxassetnodeindex - 1);
			//trimatrix1d(A_down, B_down, C_down, alpha_down, beta, r_forward_p[t - vd], dt, px, dpx, 1, maxassetnodeindex - 1);

			//trimxsolve1d(A, B, C, vold, vnew, 0, maxassetnodeindex, 0, 0);
			//trimxsolve1d(A_up, B_up, C_up, vold_up, vnew_up, 0, maxassetnodeindex, 0, 0);
			//trimxsolve1d(A_down, B_down, C_down, vold_down, vnew_down, 0, maxassetnodeindex, 0, 0);


			//for (int i = 0; i <= maxassetnodeindex; i++) {
			//	vold[i] = vnew[i];
			//	vold_up[i] = vnew_up[i];
			//	vold_down[i] = vnew_down[i];
			//	//uold[i] = unew[i];
			//	//uold_up[i] = unew_up[i];
			//	//uold_down[i] = unew_down[i];
			//}

			//ThePayoffPtr->copy_v_to_u(vnew, unew, kiindex, kiindex);
			//ThePayoffPtr->copy_v_to_u(vnew_up, unew_up, kiindex, kiindex);
			//ThePayoffPtr->copy_v_to_u(vnew_down, unew_down, kiindex, kiindex);

			//trimxsolve1d(A, B, C, uold, unew, kiindex, maxassetnodeindex, 1, 0);
			//trimxsolve1d(A_up, B_up, C_up, uold_up, unew_up, kiindex, maxassetnodeindex, 1, 0);
			//trimxsolve1d(A_down, B_down, C_down, uold_down, unew_down, kiindex, maxassetnodeindex, 1, 0);

			//ThePayoffPtr->copy_v_to_u(unew, uold, 0, maxassetnodeindex);
			//ThePayoffPtr->copy_v_to_u(unew_down, uold_down, 0, maxassetnodeindex);
			//ThePayoffPtr->copy_v_to_u(unew_up, uold_up, 0, maxassetnodeindex);


		}//for t

		 //update BC
	/*	if (t == autocall_date[k - 1]) {
			ThePayoffPtr->updator(t, vold, uold, px, 0, maxassetnodeindex);
			ThePayoffPtr->updator(t, vold_up, uold_up, px, 0, maxassetnodeindex);
			ThePayoffPtr->updator(t, vold_down, uold_down, px, 0, maxassetnodeindex);

		}*/
		if (t == vd)
			break;

	}

//	double pv, pv_next, pv_up, pv_down;

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

	//return pv;
	return 0;

}

double AutocallOption2::CalcMC2(MarketParam & para, long nMC)
{
	return 0.0;
}

signed int AutocallOption2::GetExpiryd() const
{
	return expiry_date;
}

double AutocallOption2::GetRefPrice() const
{
	return refprice;
}

std::vector<double> AutocallOption2::GetResult() const
{
	return result;
}

//void AutocallOption2::init_param(MarketParam & para)
//{
//
//}
