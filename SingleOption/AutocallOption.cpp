#include "AutocallOption.h"
#include <algorithm>
#include <random>
#include "j_fd.h"
#include "k_miscellaneous.hpp"

AutocallOption::AutocallOption(double refprice_, signed int expiryd_, const PayoffAutocallStd & ThePayoff_, int hitflag_)
	:refprice(refprice_), expiry_date(expiryd_),hitflag(hitflag_)
{
	ThePayoffPtr = ThePayoff_.clone();
	result = std::vector<double>(30);
}


AutocallOption::~AutocallOption()
{
	delete ThePayoffPtr;
}



double AutocallOption::Calc_old(MarketParam & para)
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
	int maxassetnodeindex = 300;
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
	int kiindex=0;
	kiindex = findlowerindex(px, ThePayoffPtr->GetKiBarrier() , 0, maxassetnodeindex);
	
	//final BC
	ThePayoffPtr->final_updator(vold, uold, px, 0, maxassetnodeindex);
	for (int i = 0; i <= maxassetnodeindex; i++) {
		vold_up[i] = vold[i];
		vold_down[i] = vold[i];
		uold_up[i] = uold[i];
		uold_down[i] = uold[i];
	}

	signed int t;
	for (int k = nb_autocall; k > 0; k--) {
		for (t = autocall_date[k]; t >= std::max(vd + 1, autocall_date[k - 1] + 1);t--) {
			double tfv = (t - vd) / 365.0;
			double dt = 1 / 365.0;

			double r_forward = R.getForward(tfv);
			double q_forward = Q.getForward(tfv);

			if (t == vd + 1) {
				for (int i = 0; i <= maxassetnodeindex; i++) {
					vold_next[i] = vold[i];  //theta
					uold_next[i] = uold[i];  //theta
				}
			}

			for (int i = 0; i <= maxassetnodeindex; i++) {
				double short_vol = vol.lvol(tfv, px[i]);
				double short_vol_up = vol.lvol_up(tfv, px[i]);
				double short_vol_down = vol.lvol_down(tfv, px[i]);

				alpha[i] = 0.5*short_vol*short_vol*dt;
				alpha_up[i] = 0.5*short_vol_up*short_vol_up*dt;
				alpha_down[i] = 0.5*short_vol_down*short_vol_down*dt;
				beta[i] = (r_forward - q_forward)*dt;
			}

			trimatrix1d(A, B, C, alpha, beta, r_forward, dt, px, dpx, 1, maxassetnodeindex - 1);
			trimatrix1d(A_up, B_up, C_up, alpha_up, beta, r_forward, dt, px, dpx, 1, maxassetnodeindex - 1);
			trimatrix1d(A_down, B_down, C_down, alpha_down, beta, r_forward, dt, px, dpx, 1, maxassetnodeindex - 1);

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

		}//for t

		//update BC
		if (t == autocall_date[k - 1]) {
			ThePayoffPtr->updator(t, vold, uold, px, 0, maxassetnodeindex);
			ThePayoffPtr->updator(t, vold_up,uold_up, px, 0, maxassetnodeindex);
			ThePayoffPtr->updator(t, vold_down,uold_down, px, 0, maxassetnodeindex);
		}
		if (t == vd)
			break;
	}

	double pv, pv_next, pv_up, pv_down;

	if (hitflag) { //hitted -> vold 
		pv = intp1d(s0, px, vold, 1, maxassetnodeindex - 1);
		pv_next = intp1d(s0, px, vold_next, 1, maxassetnodeindex - 1);
		pv_up = intp1d(s0*1.01, px, vold_up, 1, maxassetnodeindex - 1);
		pv_down = intp1d(s0*0.99, px, vold_down, 1, maxassetnodeindex - 1);
	}
	else {
		pv = intp1d(s0, px, uold, 1, maxassetnodeindex - 1);
		pv_next = intp1d(s0, px, uold_next, 1, maxassetnodeindex - 1);
		pv_up = intp1d(s0*1.01, px, uold_up, 1, maxassetnodeindex - 1);
		pv_down = intp1d(s0*0.99, px, uold_down, 1, maxassetnodeindex - 1);
	}

	result.resize(30, 0.0);
	result[0] = pv;
	result[1] = (pv_up - pv_down) / (s0*0.02);
	result[2] = (pv_up - 2.0*pv + pv_down) / (s0*0.01) / (s0*0.01);
	result[3] = 0;  //vega
	
	if (vd == expiry_date) {
		result[4] = 0;
	}else{
		result[4] = pv_next - pv;  //theta
	}

	result[5] = s0;

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

	return pv;
}


double AutocallOption::Calc(MarketParam & para)
{
	//Calc2: speed improvemnet for local vol interpoation
	double s0 = para.get_spot();
	Rate R = para.get_rfrate();
	Rate Q = para.get_q();
	signed int vd = para.get_vdate();
	int nb_autocall = ThePayoffPtr->GetNbAutocall();

	Vol vol = para.get_vol();
	vol.calcLv(s0, R, Q);

	std::vector<signed int> autocall_date;
	autocall_date = ThePayoffPtr->GetAutocall_date();

	int maxassetnodeindex = 300;
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

	signed int t;
	double* tau_p = new double[autocall_date[nb_autocall] - vd + 1];
	double* r_forward_p = new double[autocall_date[nb_autocall] - vd + 1];
	double* r_dc_p = new double[autocall_date[nb_autocall] - vd + 1];
	double* q_forward_p = new double[autocall_date[nb_autocall] - vd + 1];

	for (signed int i = 0; i <= autocall_date[nb_autocall] - vd; i++) {
		tau_p[i] = (i) / 365.0;
		r_forward_p[i] = R.getForward(tau_p[i]);
		r_dc_p[i] = R.getIntpRate(tau_p[i]);
		q_forward_p[i] = Q.getForward(tau_p[i]);
	}

	double dt = 1 / 365.0;
	int *idxS = new int[maxassetnodeindex + 1];
	int *idxT = new signed int[autocall_date[nb_autocall]-vd + 1];
	for (int i = 0; i <= maxassetnodeindex; i++) {
		idxS[i] = vol.find_index_spot(px[i]);
	}

	for (int tfv = 0; tfv <= autocall_date[nb_autocall] - vd; tfv++) {
		idxT[tfv] = vol.find_index_term(tfv/365.0);
	}


	for (int k = nb_autocall; k > 0; k--) {
		for (t = autocall_date[k]; t >= std::max(vd + 1, autocall_date[k - 1] + 1); t--) {
			//double tau = (t - vd) / 365.0; //time from vdate
			//double tfv = (t - vd) / 365.0;
			//double dt = 1 / 365.0;

			//double r_forward = R.getForward(tfv);
			//double q_forward = Q.getForward(tfv);

			if (t == vd + 1) {
				for (int i = 0; i <= maxassetnodeindex; i++) {
					vold_next[i] = vold[i];  //theta
					uold_next[i] = uold[i];  //theta
				}
			}

			for (int i = 0; i <= maxassetnodeindex; i++) {
				//double short_vol = vol.lvol(tau_p[t-vd], px[i]);
				//double short_vol_up = vol.lvol_up(tau_p[t - vd], px[i]);
				//double short_vol_down = vol.lvol_down(tau_p[t - vd], px[i]);
				double short_vol = vol.get_Lvol(idxT[t - vd], idxS[i]);
				double short_vol_up = vol.get_Lvol_up(idxT[t - vd], idxS[i]);
				double short_vol_down = vol.get_Lvol_down(idxT[t - vd], idxS[i]);

				alpha[i] = 0.5*short_vol*short_vol*dt;
				alpha_up[i] = 0.5*short_vol_up*short_vol_up*dt;
				alpha_down[i] = 0.5*short_vol_down*short_vol_down*dt;

				beta[i] = (r_forward_p[t-vd] - q_forward_p[t-vd])*dt;
			}

			trimatrix1d(A, B, C, alpha, beta, r_forward_p[t-vd], dt, px, dpx, 1, maxassetnodeindex - 1);
			trimatrix1d(A_up, B_up, C_up, alpha_up, beta, r_forward_p[t-vd], dt, px, dpx, 1, maxassetnodeindex - 1);
			trimatrix1d(A_down, B_down, C_down, alpha_down, beta, r_forward_p[t-vd], dt, px, dpx, 1, maxassetnodeindex - 1);

			trimxsolve1d(A, B, C, vold, vnew, 0, maxassetnodeindex, 0, 0);
			trimxsolve1d(A_up, B_up, C_up, vold_up, vnew_up, 0, maxassetnodeindex, 0, 0);
			trimxsolve1d(A_down, B_down, C_down, vold_down, vnew_down, 0, maxassetnodeindex, 0, 0);


			for (int i = 0; i <= maxassetnodeindex; i++) {
				vold[i] = vnew[i];
				vold_up[i] = vnew_up[i];
				vold_down[i] = vnew_down[i];
				//uold[i] = unew[i];
				//uold_up[i] = unew_up[i];
				//uold_down[i] = unew_down[i];
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

	if (hitflag) { //hitted -> vold 
		pv = intp1d(s0, px, vold, 1, maxassetnodeindex - 1);
		pv_next = intp1d(s0, px, vold_next, 1, maxassetnodeindex - 1);
		pv_up = intp1d(s0*1.01, px, vold_up, 1, maxassetnodeindex - 1);
		pv_down = intp1d(s0*0.99, px, vold_down, 1, maxassetnodeindex - 1);
	}
	else {
		pv = intp1d(s0, px, uold, 1, maxassetnodeindex - 1);
		pv_next = intp1d(s0, px, uold_next, 1, maxassetnodeindex - 1);
		pv_up = intp1d(s0*1.01, px, uold_up, 1, maxassetnodeindex - 1);
		pv_down = intp1d(s0*0.99, px, uold_down, 1, maxassetnodeindex - 1);
	}

	result.resize(30, 0.0);
	result[0] = pv;
	result[1] = (pv_up - pv_down) / (s0*0.02);
	result[2] = (pv_up - 2.0*pv + pv_down) / (s0*0.01) / (s0*0.01);
	result[3] = 0;  //vega

	if (vd == expiry_date) {
		result[4] = 0;
	}
	else {
		result[4] = pv_next - pv;  //theta
	}

	result[5] = s0;

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

	delete[] tau_p; 
	delete[] r_forward_p;
	delete[] r_dc_p;
	delete[] q_forward_p; 
	delete[] idxT;
	delete[] idxS;

	return pv;

}

double AutocallOption::Calc(MarketParameters & paras)
{
	double s0 = paras.get_spot();
	signed int vd = paras.get_vdate();
	
	int nb_autocall = ThePayoffPtr->GetNbAutocall();

	paras.calcLV();

	std::vector<signed int> autocall_date;
	autocall_date = ThePayoffPtr->GetAutocall_date();

	int maxassetnodeindex = 300;
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

	signed int t;
	double* tau_p = new double[autocall_date[nb_autocall] - vd + 1];
	double* r_forward_p = new double[autocall_date[nb_autocall] - vd + 1];
	double* r_dc_p = new double[autocall_date[nb_autocall] - vd + 1];
	double* q_forward_p = new double[autocall_date[nb_autocall] - vd + 1];

	
	for (signed int i = 0; i <= autocall_date[nb_autocall] - vd; i++) {
		tau_p[i] = (i) / 365.0;
		r_forward_p[i] = paras.getForward(tau_p[i]);
		r_dc_p[i] = paras.getIntpRate(tau_p[i]);
		q_forward_p[i] = paras.getDivForward(tau_p[i]);
		//temp test
		//r_forward_p[i] = 0.0;
		//r_dc_p[i] = 0.0;
		//q_forward_p[i] = 0.0;

	}

	double dt = 1 / 365.0; 
	int *idxS = new int[maxassetnodeindex + 1];
	int *idxT = new signed int[autocall_date[nb_autocall] - vd + 1];
	for (int i = 0; i <= maxassetnodeindex; i++) {
		idxS[i] = paras.find_index_spot(px[i]);
	}

	for (int tfv = 0; tfv <= autocall_date[nb_autocall] - vd; tfv++) {
		idxT[tfv] = paras.find_index_term(tfv / 365.0);
	}

	for (int k = nb_autocall; k > 0; k--) {
		for (t = autocall_date[k]; t >= std::max(vd + 1, autocall_date[k - 1] + 1); t--) {

			if (t == vd + 1) {
				for (int i = 0; i <= maxassetnodeindex; i++) {
					vold_next[i] = vold[i];  //theta
					uold_next[i] = uold[i];  //theta
				}
			}

			for (int i = 0; i <= maxassetnodeindex; i++) {
				double short_vol = paras.lvol(idxT[t - vd], idxS[i]);
				double short_vol_up = paras.lvol_up(idxT[t - vd], idxS[i]);
				double short_vol_down = paras.lvol_down(idxT[t - vd], idxS[i]);

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

	if (hitflag) { //hitted -> vold 
		pv = intp1d(s0, px, vold, 1, maxassetnodeindex - 1);
		pv_next = intp1d(s0, px, vold_next, 1, maxassetnodeindex - 1);
		pv_up = intp1d(s0*1.01, px, vold_up, 1, maxassetnodeindex - 1);
		pv_down = intp1d(s0*0.99, px, vold_down, 1, maxassetnodeindex - 1);
	}
	else {
		pv = intp1d(s0, px, uold, 1, maxassetnodeindex - 1);
		pv_next = intp1d(s0, px, uold_next, 1, maxassetnodeindex - 1);
		pv_up = intp1d(s0*1.01, px, uold_up, 1, maxassetnodeindex - 1);
		pv_down = intp1d(s0*0.99, px, uold_down, 1, maxassetnodeindex - 1);
	}

	result.resize(30, 0.0);
	result[0] = pv;
	result[1] = (pv_up - pv_down) / (s0*0.02);
	result[2] = (pv_up - 2.0*pv + pv_down) / (s0*0.01) / (s0*0.01);
	result[3] = 0;  //vega

	if (vd == expiry_date) {
		result[4] = 0;
	}
	else {
		result[4] = pv_next - pv;  //theta
	}

	result[5] = s0;

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

	delete[] tau_p;
	delete[] r_forward_p;
	delete[] r_dc_p;
	delete[] q_forward_p;
	delete[] idxT;
	delete[] idxS;

	return pv;
}

double AutocallOption::CalcMC2(MarketParam & para, long numMC_)
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

	double kibarrier = ThePayoffPtr->GetKiBarrier();

	std::vector<double> autocall_strike;
	autocall_strike = ThePayoffPtr->GetAutocall_strike();

	std::vector<double> autocall_coupon;
	autocall_coupon = ThePayoffPtr->GetAutocall_coupon();

	//std::random_device rd;
	std::mt19937 gen(130);
	std::normal_distribution<>ndist(0, 1);
	double* mcvalues = new double[numMC_];

	double put_strike = ThePayoffPtr->GetPutStrike();
	double dummy_coupon = ThePayoffPtr->GetDummyCoupon();

	double s_tmp;
	int tmpKIFlag;
	int daydivide_ = 1;

	double* tau_p = new double[autocall_date[nb_autocall]-vd+1];
	double* r_forward_p=new double[autocall_date[nb_autocall]-vd+1];
	double* r_dc_p = new double[autocall_date[nb_autocall] - vd + 1];
	double* q_forward_p = new double[autocall_date[nb_autocall] - vd+1];

	for (signed int i = 0; i <= autocall_date[nb_autocall]-vd; i++) {
		tau_p[i] = (i) / 365.0;
		r_forward_p[i] = R.getForward(tau_p[i]);
		r_dc_p[i] = R.getIntpRate(tau_p[i]);
		q_forward_p[i] = Q.getForward(tau_p[i]);
	}

	double dt = 1 / 365.0;

	for (long i = 0; i<numMC_; i++)
	{
		s_tmp = s0;
		tmpKIFlag = hitflag;

		for (int k = 1; k <= nb_autocall; k++) {
			for (signed int t = std::max(autocall_date[k - 1], vd) + 1; t <= autocall_date[k]; t++) {

				double short_vol = vol.lvol(tau_p[t-vd], s_tmp);

				double drift = (r_forward_p[t-vd] - q_forward_p[t-vd] - 0.5*short_vol*short_vol)*dt;
				double diff = short_vol*std::sqrt(dt);

				for (long t2 = 1; t2 <= daydivide_; t2++) {
					s_tmp = s_tmp*std::exp(drift + diff*ndist(gen));
					if (s_tmp<kibarrier)
						tmpKIFlag = 1;
				}
			}

			if (s_tmp >= autocall_strike[k]) { //check autocallability
				mcvalues[i] = std::exp(-r_dc_p[autocall_date[k]-vd]*tau_p[autocall_date[k] - vd])*(1.0 + autocall_coupon[k]);
				break; //k loop
			}

			//we are here because it is not autocalled at maturity
			if (k == nb_autocall) {

				if (s_tmp >= autocall_strike[k]) {
					mcvalues[i] = std::exp(-r_dc_p[autocall_date[k] - vd] * tau_p[autocall_date[k] - vd])*(1.0 + autocall_coupon[k]);
				}
				else if (s_tmp >= kibarrier) {
					if (tmpKIFlag == 1) {
						mcvalues[i] = std::exp(-r_dc_p[autocall_date[k] - vd] * tau_p[autocall_date[k] - vd])*(1.0 - std::max((put_strike - s_tmp) / refprice, 0.0));
					}
					else if (tmpKIFlag == 0) {
						mcvalues[i] = std::exp(-r_dc_p[autocall_date[k] - vd] * tau_p[autocall_date[k] - vd])*(1.0 + dummy_coupon);
					}
					else {
						throw std::logic_error("unexpected KIFlag");
					}
				}
				else {
					mcvalues[i] = std::exp(-r_dc_p[autocall_date[k] - vd] * tau_p[autocall_date[k] - vd])*(1.0 - std::max((put_strike - s_tmp) / refprice, 0.0));
				}

			} //if k

		}//for k

	}//for(i=0..)

	double npv = 0.0;
	for (long i = 0; i<numMC_; i++)
		npv += mcvalues[i];
	npv /= numMC_;

	result[10] = npv;
	result[5] = s0;

	delete[] mcvalues;

	delete[] tau_p;
	delete[] r_forward_p;
	delete[] r_dc_p;
	delete[] q_forward_p;

	return npv;
}

double AutocallOption::CalcMC(MarketParam & para, long numMC_)
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

	double kibarrier = ThePayoffPtr->GetKiBarrier();

	std::vector<double> autocall_strike;
	autocall_strike = ThePayoffPtr->GetAutocall_strike();

	std::vector<double> autocall_coupon;
	autocall_coupon = ThePayoffPtr->GetAutocall_coupon();

//	std::random_device rd;
//	std::mt19937 gen(rd());
	std::mt19937 gen(130);
	std::normal_distribution<>ndist(0, 1);
	//signed int numMC_ = 30000;
	double* mcvalues = new double[numMC_];
	
	double put_strike = ThePayoffPtr->GetPutStrike();
	double dummy_coupon = ThePayoffPtr->GetDummyCoupon();

	double s_tmp;
	int tmpKIFlag;
	int daydivide_ = 1;


	for (long i = 0; i<numMC_; i++)
	{
		s_tmp = s0;
	
		tmpKIFlag = hitflag;

		for (int k = 1; k <= nb_autocall; k++) {
			for (signed int t = std::max(autocall_date[k - 1], vd) + 1; t <= autocall_date[k]; t++) {
				double tau = (t - vd) / 365.0;
				double dt = 1 / 365.0;

				double short_vol = vol.lvol(tau, s_tmp);
				double r_forward = R.getForward(tau);
				double q_forward = Q.getForward(tau);

				double drift = (r_forward - q_forward - 0.5*short_vol*short_vol)*dt;
				double diff = short_vol*std::sqrt(dt);

				for (long t2 = 1; t2 <= daydivide_; t2++){
					s_tmp = s_tmp*std::exp(drift + diff*ndist(gen));
					if (s_tmp<kibarrier)
						tmpKIFlag = 1;
				}
			}

			if (s_tmp >= autocall_strike[k]){ //check autocallability
				double tau = (autocall_date[k] - vd) / 365.0;
				double dt = 1 / 365.0;

				double r = R.getIntpRate(tau);
				//double r = intp1d((autocalldates_[k] - md) / 365.0, rfrate_term, rfrate, 0, numrfrate - 1);
				//double tau = (autocalldates_[k] - vd) / 365.0;
				mcvalues[i] = std::exp(-r*tau)*(1.0 + autocall_coupon[k]);
				break; //k loop
			}

			//we are here because it is not autocalled at maturity
			if (k == nb_autocall) {
				//on expiry date
				double tau = (autocall_date[k] - vd) / 365.0;
				double dt = 1 / 365.0;
				double r = R.getIntpRate(tau);

				//in case of having knock-in feature
				if (s_tmp >= autocall_strike[k]) {
					mcvalues[i] = std::exp(-r*tau)*(1.0 + autocall_coupon[k]);
				}else if (s_tmp >= kibarrier) {
					if (tmpKIFlag == 1) {
						mcvalues[i] = std::exp(-r*tau)*(1.0 - std::max((put_strike- s_tmp) /refprice, 0.0));
					}
					else if (tmpKIFlag == 0) {
						mcvalues[i] = std::exp(-r*tau)*(1.0 + dummy_coupon);
					}
					else {
						throw std::logic_error("unexpected KIFlag");
					}
				}else {
					mcvalues[i] = std::exp(-r*tau)*(1.0 - std::max((put_strike - s_tmp) / refprice, 0.0));
				}

			} //if k

		}//for k

	}//for(i=0..)

	double npv=0.0;
	for (long i = 0; i<numMC_; i++)
		npv += mcvalues[i];
	npv /= numMC_;

	result[0] = npv; 

	delete[] mcvalues;

	return npv;
}

signed int AutocallOption::GetExpiryd() const
{
	return expiry_date;
}

double AutocallOption::GetRefPrice() const
{
	return refprice;
}

std::vector<double> AutocallOption::GetResult() const
{
	return result;
}
