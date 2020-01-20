#include <cassert>
#include <random>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include "EuropeanOption.h"
#include "j_fd.h"
#include "k_miscellaneous.hpp"

using namespace std;
string getFnameTimeStartingWith(string init_str);

unsigned int EuropeanOption::getIndex(double target, double * px, int i_min, int i_max) const
{
	if (target <= px[0])
		return (init_i = 0);
	if (target >= px[i_max])
		return (init_i = i_max);

	if (px[init_i] <= target && target <px[init_i + 1]) {
		if (target - px[init_i] <px[init_i + 1] - target) {
			return init_i;
		}
		else {
			return (init_i = init_i + 1);
		}
	}

	int i = init_i;
	while (1) {   //향후 이부분 개선 
		i = i + 1;
		if (i < i_max) {
			if (px[i] <= target && target < px[i + 1]) {
				if (target - px[i] < px[i + 1] - target) {
					return (init_i = i);
				}
				else {
					return (init_i = i + 1);
				}
			}
		}

		int j = init_i - (i - init_i);
		if (j >= 0) {
			if (px[j] <= target && target <px[j + 1]) {
				if (target - px[j] < px[j + 1] - target) {
					return (init_i = j);
				}
				else {
					return (init_i = j + 1);
				}
			}
		}
	}

	throw std::logic_error("find_index_spot2 - interpolaton fail :findnearestindex, vol strike");
	return -1;

}

double EuropeanOption::Calc(MarketParam& para)
{
	double s0=para.get_spot();
	//double rfrate=para.get_rfrate();
	Rate R=para.get_rfrate();
	Rate Q=para.get_q();
	
	Vol vol=para.get_vol();
	//vol.calcLv(para.get_spot(), para.get_rfrate(), para.get_q());
	vol.calcLv(s0,R,Q);
	//vol.foutLvol();
	//vol.print();
	//vol.printLV();

	int maxassetnodeindex=300;
	double *px=new double[maxassetnodeindex+1];
	double *dpx=new double[maxassetnodeindex+1];
	double *alpha =new double[maxassetnodeindex+1];
	double *alpha_up =new double[maxassetnodeindex+1];
	double *alpha_down =new double[maxassetnodeindex+1];

	double *beta = new double[maxassetnodeindex+1];
	double *vold=new double[maxassetnodeindex+1];
	double *vold_up=new double[maxassetnodeindex+1];
	double *vold_down=new double[maxassetnodeindex+1];

	double *vold_next=new double[maxassetnodeindex+1];  //reserve for theta
	
	double *vnew=new double[maxassetnodeindex+1];
	double *vnew_up=new double[maxassetnodeindex+1];
	double *vnew_down=new double[maxassetnodeindex+1];
	
	double *A=new double[maxassetnodeindex+1];
	double *A_up=new double[maxassetnodeindex+1];
	double *A_down=new double[maxassetnodeindex+1];

	double *B=new double[maxassetnodeindex+1];
	double *B_up=new double[maxassetnodeindex+1];
	double *B_down=new double[maxassetnodeindex+1];

	double *C=new double[maxassetnodeindex+1];
	double *C_up=new double[maxassetnodeindex+1];
	double *C_down=new double[maxassetnodeindex+1];
	
	//int kiindex=0;
	px[0]=0.0;
	double tmp_ds=refprice*3.0/maxassetnodeindex;
	for(int i=1;i<=maxassetnodeindex;i++)
		px[i]=px[i-1]+tmp_ds;
	for(int i=0;i<maxassetnodeindex;i++) //max index of dp is max index of px -1
		dpx[i]=px[i+1]-px[i];
	ThePayoffPtr->ResetFDGrid(px,dpx,1,maxassetnodeindex-1);
	//gridcontrol(px, dpx, 1, maxassetnodeindex-1,strike,0);
	
	for(signed int t=expiry_date;t>=para.get_vdate();t--){
		if(t==expiry_date){  //b.c, expiry date
			for(int i=0;i<=maxassetnodeindex;i++){
				//vold[i]=payoff_at_maturity(px[i]);
				vold[i]=(*ThePayoffPtr)(px[i]); 
				vold_up[i]=vold[i];
				vold_down[i]=vold[i];
			}

		}

		if(t==(para.get_vdate()+1)){  //for theta, if vd==expiry date ? 
			for(int i=0;i<=maxassetnodeindex;i++)
					vold_next[i]=vold[i];
		}

		double tau=(t-para.get_vdate())/365.0; //time from vdate
		double dt=1/365.0;

		//double r_forward=getforward((t-md)/365.0,rfrate,rfrate_term,numrfrate);
		double r_forward=R.getForward(tau);
		double q_forward=Q.getForward(tau);
		//double q_forward=0.0;
		/*if(t==360){
			q_forward=2.2263/100/dt;
		}*/


		for(int i=0;i<=maxassetnodeindex;i++){	
			double short_vol=vol.lvol(tau,px[i]);
			double short_vol_up=vol.lvol_up(tau,px[i]);
			double short_vol_down=vol.lvol_down(tau,px[i]);

			alpha[i]=0.5*short_vol*short_vol*dt;
			alpha_up[i]=0.5*short_vol_up*short_vol_up*dt;
			alpha_down[i]=0.5*short_vol_down*short_vol_down*dt;

			beta[i]=(r_forward-q_forward)*dt;
		}

		trimatrix1d(A,B,C,alpha,beta, r_forward, dt, px, dpx,1, maxassetnodeindex-1);
		trimatrix1d(A_up,B_up,C_up,alpha_up,beta, r_forward, dt, px, dpx,1, maxassetnodeindex-1);
		trimatrix1d(A_down,B_down,C_down,alpha_down,beta, r_forward, dt, px, dpx,1, maxassetnodeindex-1);
	
		trimxsolve1d(A,B, C, vold,  vnew, 0, maxassetnodeindex, 0,0);
		trimxsolve1d(A_up,B_up, C_up, vold_up,  vnew_up, 0, maxassetnodeindex, 0,0);
		trimxsolve1d(A_down,B_down, C_down, vold_down,  vnew_down, 0, maxassetnodeindex, 0,0);
		


		for(int i=0;i<=maxassetnodeindex;i++){
			vold[i]=vnew[i];
			vold_up[i]=vnew_up[i];
			vold_down[i]=vnew_down[i];

		}

	}

	double pv=intp1d(s0,px,vold,1,maxassetnodeindex-1);
	double pv_next=intp1d(s0,px,vold_next, 1,maxassetnodeindex-1);
	double pv_up=intp1d(s0*1.01, px, vold_up, 1, maxassetnodeindex-1);
	double pv_down=intp1d(s0*0.99, px, vold_down, 1, maxassetnodeindex-1);

	delete[] px;
	delete[] dpx;
	delete[] alpha;
	delete[] alpha_up;
	delete[] alpha_down;

	delete[] beta;
	delete[] vold;
	delete[] vold_up;
	delete[] vold_down;

	delete[] vnew;
	delete[] vnew_up;
	delete[] vnew_down;
	
	delete[] vold_next;
	delete[] A;
	delete[] A_up;
	delete[] A_down;

	delete[] B;
	delete[] B_up;
	delete[] B_down;


	delete[] C;
	delete[] C_up;
	delete[] C_down;

	result.resize(30,0.0);
	result[0]=pv;
	result[1]=(pv_up-pv_down)/(s0*0.02); //delta
	result[2]=(pv_up-2.0*pv+pv_down)/(s0*0.01)/(s0*0.01); //gamma
	result[3]=0;  //vega
	result[4]=pv_next-pv;  //theta
	result[5]=s0;
	return pv;

	
}

double EuropeanOption::Calc2(MarketParam & para)
{
//Calc2 : improved for-loop for r, div
	double s0 = para.get_spot();

	Rate R = para.get_rfrate();
	Rate Q = para.get_q();

	Vol vol = para.get_vol();

	vol.calcLv(s0, R, Q);


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

	double *vold_next = new double[maxassetnodeindex + 1];  //reserve for theta

	double *vnew = new double[maxassetnodeindex + 1];
	double *vnew_up = new double[maxassetnodeindex + 1];
	double *vnew_down = new double[maxassetnodeindex + 1];

	double *A = new double[maxassetnodeindex + 1];
	double *A_up = new double[maxassetnodeindex + 1];
	double *A_down = new double[maxassetnodeindex + 1];

	double *B = new double[maxassetnodeindex + 1];
	double *B_up = new double[maxassetnodeindex + 1];
	double *B_down = new double[maxassetnodeindex + 1];

	double *C = new double[maxassetnodeindex + 1];
	double *C_up = new double[maxassetnodeindex + 1];
	double *C_down = new double[maxassetnodeindex + 1];

	//int kiindex=0;
	px[0] = 0.0;
	double tmp_ds = refprice*3.0 / maxassetnodeindex;
	for (int i = 1; i <= maxassetnodeindex; i++)
		px[i] = px[i - 1] + tmp_ds;
	for (int i = 0; i<maxassetnodeindex; i++) //max index of dp is max index of px -1
		dpx[i] = px[i + 1] - px[i];
	ThePayoffPtr->ResetFDGrid(px, dpx, 1, maxassetnodeindex - 1);

	signed int vd = para.get_vdate();
	double* tau_p = new double[expiry_date - vd + 1];
	double* r_forward_p = new double[expiry_date - vd + 1];
	double* r_dc_p = new double[expiry_date - vd + 1];
	double* q_forward_p = new double[expiry_date - vd + 1];

	for (signed int i = 0; i <= expiry_date - vd; i++) {
		tau_p[i] = (i) / 365.0;
		r_forward_p[i] = R.getForward(tau_p[i]);
		r_dc_p[i] = R.getIntpRate(tau_p[i]);
		q_forward_p[i] = Q.getForward(tau_p[i]);
	}


	for (signed int t = expiry_date; t >= vd; t--) {
		if (t == expiry_date) {  //b.c, expiry date
			for (int i = 0; i <= maxassetnodeindex; i++) {
				vold[i] = (*ThePayoffPtr)(px[i]);
				vold_up[i] = vold[i];
				vold_down[i] = vold[i];
			}

		}

		if (t == (vd + 1)) {  //for theta, if vd==expiry date ? 
			for (int i = 0; i <= maxassetnodeindex; i++)
				vold_next[i] = vold[i];
		}

		double dt = 1 / 365.0;
		for (int i = 0; i <= maxassetnodeindex; i++) {
			double short_vol = vol.lvol(tau_p[t-vd], px[i]);
			double short_vol_up = vol.lvol_up(tau_p[t - vd], px[i]);
			double short_vol_down = vol.lvol_down(tau_p[t - vd], px[i]);

			alpha[i] = 0.5*short_vol*short_vol*dt;
			alpha_up[i] = 0.5*short_vol_up*short_vol_up*dt;
			alpha_down[i] = 0.5*short_vol_down*short_vol_down*dt;

			beta[i] = (r_forward_p[t-vd] - q_forward_p[t-vd])*dt;
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

	}

	double pv = intp1d(s0, px, vold, 1, maxassetnodeindex - 1);
	double pv_next = intp1d(s0, px, vold_next, 1, maxassetnodeindex - 1);
	double pv_up = intp1d(s0*1.01, px, vold_up, 1, maxassetnodeindex - 1);
	double pv_down = intp1d(s0*0.99, px, vold_down, 1, maxassetnodeindex - 1);

	delete[] px;
	delete[] dpx;
	delete[] alpha;
	delete[] alpha_up;
	delete[] alpha_down;

	delete[] beta;
	delete[] vold;
	delete[] vold_up;
	delete[] vold_down;

	delete[] vnew;
	delete[] vnew_up;
	delete[] vnew_down;

	delete[] vold_next;
	delete[] A;
	delete[] A_up;
	delete[] A_down;

	delete[] B;
	delete[] B_up;
	delete[] B_down;


	delete[] C;
	delete[] C_up;
	delete[] C_down;

	result.resize(30, 0.0);
	result[0] = pv;
	result[1] = (pv_up - pv_down) / (s0*0.02); //delta
	result[2] = (pv_up - 2.0*pv + pv_down) / (s0*0.01) / (s0*0.01); //gamma
	result[3] = 0;  //vega
	result[4] = pv_next - pv;  //theta
	result[5] = s0;
	return pv;


}

double EuropeanOption::CalcMC(MarketParam & para, long numMc)
{
	double s0 = para.get_spot();

	Rate R = para.get_rfrate();
	Rate Q = para.get_q();

	Vol vol = para.get_vol();

	vol.calcLv(s0, R, Q);


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

	double *vold_next = new double[maxassetnodeindex + 1];  //reserve for theta

	double *vnew = new double[maxassetnodeindex + 1];
	double *vnew_up = new double[maxassetnodeindex + 1];
	double *vnew_down = new double[maxassetnodeindex + 1];

	double *A = new double[maxassetnodeindex + 1];
	double *A_up = new double[maxassetnodeindex + 1];
	double *A_down = new double[maxassetnodeindex + 1];

	double *B = new double[maxassetnodeindex + 1];
	double *B_up = new double[maxassetnodeindex + 1];
	double *B_down = new double[maxassetnodeindex + 1];

	double *C = new double[maxassetnodeindex + 1];
	double *C_up = new double[maxassetnodeindex + 1];
	double *C_down = new double[maxassetnodeindex + 1];

	//int kiindex=0;
	px[0] = 0.0;
	double tmp_ds = refprice*3.0 / maxassetnodeindex;
	for (int i = 1; i <= maxassetnodeindex; i++)
		px[i] = px[i - 1] + tmp_ds;
	for (int i = 0; i<maxassetnodeindex; i++) //max index of dp is max index of px -1
		dpx[i] = px[i + 1] - px[i];
	ThePayoffPtr->ResetFDGrid(px, dpx, 1, maxassetnodeindex - 1);

	signed int vd = para.get_vdate();
	double* tau_p = new double[expiry_date - vd + 1];
	double* r_forward_p = new double[expiry_date - vd + 1];
	double* r_dc_p = new double[expiry_date - vd + 1];
	double* q_forward_p = new double[expiry_date - vd + 1];

	for (signed int i = 0; i <= expiry_date - vd; i++) {
		tau_p[i] = (i) / 365.0;
		r_forward_p[i] = R.getForward(tau_p[i]);
		r_dc_p[i] = R.getIntpRate(tau_p[i]);
		q_forward_p[i] = Q.getForward(tau_p[i]);
	}


	for (signed int t = expiry_date; t >= vd; t--) {
		if (t == expiry_date) {  //b.c, expiry date
			for (int i = 0; i <= maxassetnodeindex; i++) {
				vold[i] = (*ThePayoffPtr)(px[i]);
				vold_up[i] = vold[i];
				vold_down[i] = vold[i];
			}

		}

		if (t == (vd + 1)) {  //for theta, if vd==expiry date ? 
			for (int i = 0; i <= maxassetnodeindex; i++)
				vold_next[i] = vold[i];
		}

		double dt = 1 / 365.0;
		for (int i = 0; i <= maxassetnodeindex; i++) {
			double short_vol = vol.lvol(tau_p[t - vd], px[i]);
			double short_vol_up = vol.lvol_up(tau_p[t - vd], px[i]);
			double short_vol_down = vol.lvol_down(tau_p[t - vd], px[i]);

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

	}

	double pv = intp1d(s0, px, vold, 1, maxassetnodeindex - 1);
	double pv_next = intp1d(s0, px, vold_next, 1, maxassetnodeindex - 1);
	double pv_up = intp1d(s0*1.01, px, vold_up, 1, maxassetnodeindex - 1);
	double pv_down = intp1d(s0*0.99, px, vold_down, 1, maxassetnodeindex - 1);

	delete[] px;
	delete[] dpx;
	delete[] alpha;
	delete[] alpha_up;
	delete[] alpha_down;

	delete[] beta;
	delete[] vold;
	delete[] vold_up;
	delete[] vold_down;

	delete[] vnew;
	delete[] vnew_up;
	delete[] vnew_down;

	delete[] vold_next;
	delete[] A;
	delete[] A_up;
	delete[] A_down;

	delete[] B;
	delete[] B_up;
	delete[] B_down;


	delete[] C;
	delete[] C_up;
	delete[] C_down;

	result.resize(30, 0.0);
	result[0] = pv;
	result[1] = (pv_up - pv_down) / (s0*0.02); //delta
	result[2] = (pv_up - 2.0*pv + pv_down) / (s0*0.01) / (s0*0.01); //gamma
	result[3] = 0;  //vega
	result[4] = pv_next - pv;  //theta
	result[5] = s0;
	return pv;

}

double EuropeanOption::Calc(MarketParameters & paras)
{
	//Calc2 : improved for-loop for r, div
	//marketparams, conti div
	signed int vd = paras.get_vdate();
	double s0 = paras.get_spot();
	paras.calcLV();

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

	double *vold_next = new double[maxassetnodeindex + 1];  //reserve for theta

	double *vnew = new double[maxassetnodeindex + 1];
	double *vnew_up = new double[maxassetnodeindex + 1];
	double *vnew_down = new double[maxassetnodeindex + 1];

	double *A = new double[maxassetnodeindex + 1];
	double *A_up = new double[maxassetnodeindex + 1];
	double *A_down = new double[maxassetnodeindex + 1];

	double *B = new double[maxassetnodeindex + 1];
	double *B_up = new double[maxassetnodeindex + 1];
	double *B_down = new double[maxassetnodeindex + 1];

	double *C = new double[maxassetnodeindex + 1];
	double *C_up = new double[maxassetnodeindex + 1];
	double *C_down = new double[maxassetnodeindex + 1];

	//int kiindex=0;
	px[0] = 0.0;
	double tmp_ds = refprice*2.0 / maxassetnodeindex;
	for (int i = 1; i <= maxassetnodeindex; i++)
		px[i] = px[i - 1] + tmp_ds;
	for (int i = 0; i<maxassetnodeindex; i++) //max index of dp is max index of px -1
		dpx[i] = px[i + 1] - px[i];
	ThePayoffPtr->ResetFDGrid(px, dpx, 1, maxassetnodeindex - 1);

	double* tau_p = new double[expiry_date - vd + 1];
	double* r_forward_p = new double[expiry_date - vd + 1];
	double* r_dc_p = new double[expiry_date - vd + 1];
	double* q_forward_p = new double[expiry_date - vd + 1];

	for (signed int i = 0; i <= expiry_date - vd; i++) {
		tau_p[i] = (i) / 365.0;
		r_forward_p[i] = paras.getForward(tau_p[i]);
		r_dc_p[i] = paras.getIntpRate(tau_p[i]);
		q_forward_p[i] = paras.getDivForward(tau_p[i]);	
	}


	for (signed int t = expiry_date; t >= vd; t--) {
		if (t == expiry_date) {  //b.c, expiry date
			for (int i = 0; i <= maxassetnodeindex; i++) {
				vold[i] = (*ThePayoffPtr)(px[i]);
				vold_up[i] = vold[i];
				vold_down[i] = vold[i];
			}

		}

		if (t == (vd + 1)) {  //for theta, if vd==expiry date ? 
			for (int i = 0; i <= maxassetnodeindex; i++)
				vold_next[i] = vold[i];
		}

		double dt = 1 / 365.0;

		for (int i = 0; i <= maxassetnodeindex; i++) {
			double short_vol = paras.lvol(tau_p[t - vd], px[i]);
			double short_vol_up = paras.lvol_up(tau_p[t - vd], px[i]);
			double short_vol_down = paras.lvol_down(tau_p[t - vd], px[i]);

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

	}

	double pv = intp1d(s0, px, vold, 1, maxassetnodeindex - 1);
	double pv_next = intp1d(s0, px, vold_next, 1, maxassetnodeindex - 1);
	double pv_up = intp1d(s0*1.01, px, vold_up, 1, maxassetnodeindex - 1);
	double pv_down = intp1d(s0*0.99, px, vold_down, 1, maxassetnodeindex - 1);

	delete[] px;
	delete[] dpx;
	delete[] alpha;
	delete[] alpha_up;
	delete[] alpha_down;

	delete[] beta;
	delete[] vold;
	delete[] vold_up;
	delete[] vold_down;

	delete[] vnew;
	delete[] vnew_up;
	delete[] vnew_down;

	delete[] vold_next;
	delete[] A;
	delete[] A_up;
	delete[] A_down;

	delete[] B;
	delete[] B_up;
	delete[] B_down;


	delete[] C;
	delete[] C_up;
	delete[] C_down;

	result.resize(30, 0.0);
	result[0] = pv;
	result[1] = (pv_up - pv_down) / (s0*0.02); //delta
	result[2] = (pv_up - 2.0*pv + pv_down) / (s0*0.01) / (s0*0.01); //gamma
	result[3] = 0;  //vega
	result[4] = pv_next - pv;  //theta
	result[5] = s0;
	return pv;


}

double EuropeanOption::Simulation3(MarketParameters& paras, std::vector<double>& apath, bool db)
{
	double s0 = paras.get_spot();
	signed int vd = paras.get_vdate();

	paras.calcLV();

	vector<vector<double> > paths;
	double refprice = this->refprice;

	assert(refprice == paras.get_spot());
	assert(paras.get_vdate() == vd);

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

	double *vold_next = new double[maxassetnodeindex + 1];  //reserve for theta

	double *vnew = new double[maxassetnodeindex + 1];
	double *vnew_up = new double[maxassetnodeindex + 1];
	double *vnew_down = new double[maxassetnodeindex + 1];

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

	double s_tmp;
	int daydivide_ = 1;

	double* tau_p = new double[expiry_date - vd + 1];
	double* r_forward_p = new double[expiry_date - vd + 1];
	double* r_dc_p = new double[expiry_date - vd + 1];
	double* q_forward_p = new double[expiry_date - vd + 1];

	for (signed int i = 0; i <= expiry_date - vd; i++) {
		tau_p[i] = (i) / 365.0;
		r_forward_p[i] = paras.getForward(tau_p[i]);
		r_dc_p[i] = paras.getIntpRate(tau_p[i]);
		q_forward_p[i] = paras.getDivForward(tau_p[i]);
	}

	/*save vold, uold in vector*/
	vector<vector<double> > vgrid;

	double dt = 1 / 365.0;

	int *idxS = new int[maxassetnodeindex + 1];
	for (int i = 0; i <= maxassetnodeindex; i++) {
		idxS[i] = paras.find_index_spot(px[i]);
	}

	int *idxT = new signed int[expiry_date - vd + 1];
	for (int tfv = 0; tfv <= expiry_date - vd; tfv++) {
		idxT[tfv] = paras.find_index_term(tfv / 365.0);
	}

	for (signed int t = expiry_date; t >= vd; t--) {
		if (t == expiry_date) {  //b.c, expiry date
			for (int i = 0; i <= maxassetnodeindex; i++) {
				vold[i] = (*ThePayoffPtr)(px[i]);
				vold_up[i] = vold[i];
				vold_down[i] = vold[i];
			}
			vgrid.push_back(vector<double>(vold, vold + (maxassetnodeindex + 1)));
		}

		if (t == (vd + 1)) {  //for theta, if vd==expiry date ? 
			for (int i = 0; i <= maxassetnodeindex; i++)
				vold_next[i] = vold[i];
		}

		for (int i = 0; i <= maxassetnodeindex; i++) {
			double short_vol = paras.lvol(tau_p[t - vd], px[i]);
			double short_vol_up = paras.lvol_up(tau_p[t - vd], px[i]);
			double short_vol_down = paras.lvol_down(tau_p[t - vd], px[i]);

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
		vgrid.push_back(vector<double>(vold, vold + (maxassetnodeindex + 1)));
	}

	//double pv_fd;
	//pv_fd = intp1d(s0, px, vold, 1, maxassetnodeindex - 1);
	////doule check
	//auto it_vgrid = vgrid.rbegin();

	//pv_fd = inpt1d(s0, px, *it_vgrid, 0, maxassetnodeindex, 0);
	

	//vector<double> path;
	vector<double> aPL;
	//path.push_back(paras.get_spot());

	auto riter_vgrid = vgrid.rbegin();
	//auto riter_ugrid = ugrid.rbegin();

	s_tmp = s0;
	//tmpKIFlag = hitFlag;

	unsigned int init_i = 0;
	double cash = 0.0;
	double pv = 0.0;
	double delta = 0.0;
	double delta_new = 0.0;
	double PL = 0.0;

	unsigned int spot_idx = getIndex(s_tmp, px, 0, maxassetnodeindex);


	if (spot_idx == 0) {
		delta = ((*riter_vgrid)[spot_idx + 1] - (*riter_vgrid)[spot_idx]) / (px[spot_idx + 1] - px[spot_idx]);
		pv = inpt1d(s_tmp, px, *riter_vgrid, 0, maxassetnodeindex, 0);
	}
	else if (spot_idx == maxassetnodeindex) {
		delta = ((*riter_vgrid)[spot_idx] - (*riter_vgrid)[spot_idx - 1]) / (px[spot_idx] - px[spot_idx - 1]);
		pv = inpt1d(s_tmp, px, *riter_vgrid, 0, maxassetnodeindex, 0);
	}
	else {
		delta = ((*riter_vgrid)[spot_idx + 1] - (*riter_vgrid)[spot_idx - 1]) / (px[spot_idx + 1] - px[spot_idx - 1]);
		pv = inpt1d(s_tmp, px, *riter_vgrid, 0, maxassetnodeindex, 0);
	}

	cash += pv - s_tmp*delta;
	PL = cash - pv + s_tmp*delta;
	aPL.push_back(PL);

	for (signed int t = vd + 1; t <= expiry_date; t++) {
		//double short_vol = paras.get_Lvol_hybrid(idxT[t - vd], s_tmp);
		//double drift = (r_forward_p[t - vd] - q_forward_p[t - vd] - 0.5*short_vol*short_vol)*dt;
		//double diff = short_vol*std::sqrt(dt);
		s_tmp = apath[t - vd];

		spot_idx = getIndex(s_tmp, px, 0, maxassetnodeindex);
		riter_vgrid++;

		if (spot_idx == 0) {
			delta_new = ((*riter_vgrid)[spot_idx + 1] - (*riter_vgrid)[spot_idx]) / (px[spot_idx + 1] - px[spot_idx]);
			pv = inpt1d(s_tmp, px, *riter_vgrid, 0, maxassetnodeindex, 0);
		}
		else if (spot_idx == maxassetnodeindex) {
			delta_new = ((*riter_vgrid)[spot_idx] - (*riter_vgrid)[spot_idx - 1]) / (px[spot_idx] - px[spot_idx - 1]);
			pv = inpt1d(s_tmp, px, *riter_vgrid, 0, maxassetnodeindex, 0);
		}
		else {
			delta_new = ((*riter_vgrid)[spot_idx + 1] - (*riter_vgrid)[spot_idx - 1]) / (px[spot_idx + 1] - px[spot_idx - 1]);
			pv = inpt1d(s_tmp, px, *riter_vgrid, 0, maxassetnodeindex, 0);
		}

		cash *= std::exp(r_forward_p[t - vd] * dt);
		cash -= s_tmp*(delta_new - delta);
		cash += s_tmp*delta*(std::exp(q_forward_p[t - vd] * dt) - 1.0);

		delta = delta_new;
		PL = cash - pv + s_tmp*delta;
		aPL.push_back(PL);
	}/*for t*/

	//mcvalues.push_back(std::exp(-r_dc_p[expiry_date - vd] * tau_p[expiry_date - vd])*((*ThePayoffPtr)(s_tmp)));
	//pv = mcvalues.back();
	//pvs.push_back(pv);
	//PL = cash - pv + s_tmp*delta;
	//aPL.back() = PL;
	//if (db)
	//	fout_ts << "expired," << s_tmp << "," << cash << "," << delta << "," << pv << ",,," << PL << endl;

	//paths.push_back(path);
	//PLs.push_back(aPL);



	
	return aPL.back();
}
void EuropeanOption::Simulation2(MarketParameters & paras, long numMC_, bool db)
{
	double s0 = paras.get_spot();
	signed int vd = paras.get_vdate();

	paras.calcLV();

	vector<vector<double> > paths;
	double refprice = this->refprice;

	assert(refprice == paras.get_spot());
	assert(paras.get_vdate() == vd);

	std::mt19937 gen(130);
	std::normal_distribution<>ndist(0, 1);
	vector<double> mcvalues;
	vector<double> pvs;
	vector<vector<double> > PLs;

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

	double *vold_next = new double[maxassetnodeindex + 1];  //reserve for theta

	double *vnew = new double[maxassetnodeindex + 1];
	double *vnew_up = new double[maxassetnodeindex + 1];
	double *vnew_down = new double[maxassetnodeindex + 1];

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

	double s_tmp;
	int daydivide_ = 1;

	double* tau_p = new double[expiry_date - vd + 1];
	double* r_forward_p = new double[expiry_date - vd + 1];
	double* r_dc_p = new double[expiry_date - vd + 1];
	double* q_forward_p = new double[expiry_date - vd + 1];

	for (signed int i = 0; i <= expiry_date - vd; i++) {
		tau_p[i] = (i) / 365.0;
		r_forward_p[i] = paras.getForward(tau_p[i]);
		r_dc_p[i] = paras.getIntpRate(tau_p[i]);
		q_forward_p[i] = paras.getDivForward(tau_p[i]);
	}

	/*save vold, uold in vector*/
	vector<vector<double> > vgrid;

	double dt = 1 / 365.0;

	int *idxS = new int[maxassetnodeindex + 1];
	for (int i = 0; i <= maxassetnodeindex; i++) {
		idxS[i] = paras.find_index_spot(px[i]);
	}

	int *idxT = new signed int[expiry_date - vd + 1];
	for (int tfv = 0; tfv <= expiry_date - vd; tfv++) {
		idxT[tfv] = paras.find_index_term(tfv / 365.0);
	}

	for (signed int t = expiry_date; t >= vd; t--) {
		if (t == expiry_date) {  //b.c, expiry date
			for (int i = 0; i <= maxassetnodeindex; i++) {
				vold[i] = (*ThePayoffPtr)(px[i]);
				vold_up[i] = vold[i];
				vold_down[i] = vold[i];
			}
			vgrid.push_back(vector<double>(vold, vold + (maxassetnodeindex + 1)));
		}

		if (t == (vd + 1)) {  //for theta, if vd==expiry date ? 
			for (int i = 0; i <= maxassetnodeindex; i++)
				vold_next[i] = vold[i];
		}

		for (int i = 0; i <= maxassetnodeindex; i++) {
			double short_vol = paras.lvol(tau_p[t - vd], px[i]);
			double short_vol_up = paras.lvol_up(tau_p[t - vd], px[i]);
			double short_vol_down = paras.lvol_down(tau_p[t - vd], px[i]);

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
		vgrid.push_back(vector<double>(vold, vold + (maxassetnodeindex + 1)));
	}

	double pv_fd;
	pv_fd = intp1d(s0, px, vold, 1, maxassetnodeindex - 1);
	//doule check
	auto it_vgrid = vgrid.rbegin();

	pv_fd = inpt1d(s0, px, *it_vgrid, 0, maxassetnodeindex, 0);

	cout << "npv_fd " << pv_fd << endl;


	ofstream fvold("vold.csv");

	fvold << "PX, vold" << endl;

	for (int i = 0; i <= maxassetnodeindex; i++) {

		fvold << px[i] << "," << (*it_vgrid)[i] << endl;
	}

	fvold.close();

	string fn = getFnameTimeStartingWith(string("ts_vanilla"));
	ofstream fout_ts(fn.c_str());
	fout_ts << "tau,s_tmp,cash,delta,pv,r_for,q,PL" << endl;

	for (long i = 0; i<numMC_; i++)
	{
		vector<double> path;
		vector<double> aPL;
		path.push_back(paras.get_spot());

		auto riter_vgrid = vgrid.rbegin();

		s_tmp = s0;

		unsigned int init_i = 0;
		double cash = 0.0;
		double pv = 0.0;
		double delta = 0.0;
		double delta_new = 0.0;
		double PL = 0.0;

		unsigned int spot_idx = getIndex(s_tmp, px, 0, maxassetnodeindex);

		if (spot_idx == 0) {
			delta = ((*riter_vgrid)[spot_idx + 1] - (*riter_vgrid)[spot_idx]) / (px[spot_idx + 1] - px[spot_idx]);
			pv = inpt1d(s_tmp, px, *riter_vgrid, 0, maxassetnodeindex, 0);
		}
		else if (spot_idx == maxassetnodeindex) {
			delta = ((*riter_vgrid)[spot_idx] - (*riter_vgrid)[spot_idx - 1]) / (px[spot_idx] - px[spot_idx - 1]);
			pv = inpt1d(s_tmp, px, *riter_vgrid, 0, maxassetnodeindex, 0);
		}
		else {
			delta = ((*riter_vgrid)[spot_idx + 1] - (*riter_vgrid)[spot_idx - 1]) / (px[spot_idx + 1] - px[spot_idx - 1]);
			pv = inpt1d(s_tmp, px, *riter_vgrid, 0, maxassetnodeindex, 0);
		}

		cash += pv - s_tmp*delta;
		PL = cash - pv + s_tmp*delta;
		aPL.push_back(PL);
		if (db)
			fout_ts << 0 << "," << s_tmp << "," << cash << "," << delta << "," << pv << ",,," << PL << endl;

		for (signed int t = vd + 1; t <= expiry_date; t++) {
			double short_vol = paras.get_Lvol_hybrid(idxT[t - vd], s_tmp);
			double drift = (r_forward_p[t - vd] - q_forward_p[t - vd] - 0.5*short_vol*short_vol)*dt;
			double diff = short_vol*std::sqrt(dt);
			s_tmp = s_tmp*std::exp(drift + diff*ndist(gen));

			spot_idx = getIndex(s_tmp, px, 0, maxassetnodeindex);
			riter_vgrid++;

			if (spot_idx == 0) {
				delta_new = ((*riter_vgrid)[spot_idx + 1] - (*riter_vgrid)[spot_idx]) / (px[spot_idx + 1] - px[spot_idx]);
				pv = inpt1d(s_tmp, px, *riter_vgrid, 0, maxassetnodeindex, 0);
			} else if (spot_idx == maxassetnodeindex) {
				delta_new = ((*riter_vgrid)[spot_idx] - (*riter_vgrid)[spot_idx - 1]) / (px[spot_idx] - px[spot_idx - 1]);
				pv = inpt1d(s_tmp, px, *riter_vgrid, 0, maxassetnodeindex, 0);
			}else {
				delta_new = ((*riter_vgrid)[spot_idx + 1] - (*riter_vgrid)[spot_idx - 1]) / (px[spot_idx + 1] - px[spot_idx - 1]);
				pv = inpt1d(s_tmp, px, *riter_vgrid, 0, maxassetnodeindex, 0);
			}

			cash *= std::exp(r_forward_p[t - vd] * dt);
			cash -= s_tmp*(delta_new - delta);
			cash += s_tmp*delta*(std::exp(q_forward_p[t - vd] * dt) - 1.0);

			delta = delta_new;
			PL = cash - pv + s_tmp*delta;
			aPL.push_back(PL);
			if (db)
				fout_ts << t - vd << "," << s_tmp << "," << cash << "," << delta << "," << pv << "," << r_forward_p[t - vd] << "," << q_forward_p[t - vd] << "," << PL << endl;
		}/*for t*/

		mcvalues.push_back(std::exp(-r_dc_p[expiry_date - vd] * tau_p[expiry_date - vd])*((*ThePayoffPtr)(s_tmp)));
		pv = mcvalues.back();
		pvs.push_back(pv);
		PL = cash - pv + s_tmp*delta;
		aPL.back() = PL;
		if (db)
			fout_ts << "expired," << s_tmp << "," << cash << "," << delta << "," << pv << ",,," << PL << endl;

		paths.push_back(path);
		PLs.push_back(aPL);
	}//for(i=0..)

	fout_ts.close();

	string fn2 = getFnameTimeStartingWith(string("PL_vanilla"));
	ofstream fout(fn2.c_str());
	auto it = PLs.begin();
	fout << "numMC,PL" << endl;
	for (auto iter = PLs.begin(); iter != PLs.end(); iter++)
		fout << iter - it << "," << (*iter).back() << endl;

	fout.close();

	delete[] px;
	delete[] dpx;
	delete[] alpha;
	delete[] alpha_up;
	delete[] alpha_down;
	delete[] beta;
	delete[] vold;
	delete[] vold_up;
	delete[] vold_down;

	delete[] vnew;
	delete[] vnew_up;
	delete[] vnew_down;
	delete[] vold_next;

	delete[] A;
	delete[] A_up;
	delete[] A_down;
	delete[] B;
	delete[] B_up;
	delete[] B_down;
	delete[] C;
	delete[] C_up;
	delete[] C_down;

	delete[] idxT;
	delete[] idxS;
	delete[] tau_p;
	delete[] r_forward_p;
	delete[] r_dc_p;
	delete[] q_forward_p;
}

double EuropeanOption::Calc_discrete(MarketParameters & paras)
//MarketParameters : discrete dividend 2019.12.03
{
	signed int vd = paras.get_vdate();

	double s0 = paras.get_spot();

	paras.calcLV();
	
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

	double *vold_next = new double[maxassetnodeindex + 1];  //reserve for theta

	double *vnew = new double[maxassetnodeindex + 1];
	double *vnew_up = new double[maxassetnodeindex + 1];
	double *vnew_down = new double[maxassetnodeindex + 1];

	double *A = new double[maxassetnodeindex + 1];
	double *A_up = new double[maxassetnodeindex + 1];
	double *A_down = new double[maxassetnodeindex + 1];

	double *B = new double[maxassetnodeindex + 1];
	double *B_up = new double[maxassetnodeindex + 1];
	double *B_down = new double[maxassetnodeindex + 1];

	double *C = new double[maxassetnodeindex + 1];
	double *C_up = new double[maxassetnodeindex + 1];
	double *C_down = new double[maxassetnodeindex + 1];

	//int kiindex=0;
	px[0] = 0.0;
	double tmp_ds = refprice*3.0 / maxassetnodeindex;
	for (int i = 1; i <= maxassetnodeindex; i++)
		px[i] = px[i - 1] + tmp_ds;
	for (int i = 0; i<maxassetnodeindex; i++) //max index of dp is max index of px -1
		dpx[i] = px[i + 1] - px[i];
	ThePayoffPtr->ResetFDGrid(px, dpx, 1, maxassetnodeindex - 1);
	//gridcontrol(px, dpx, 1, maxassetnodeindex-1,strike,0);

	for (signed int t = expiry_date; t >= vd; t--) {
		if (t == expiry_date) {  //b.c, expiry date
			for (int i = 0; i <= maxassetnodeindex; i++) {
				//vold[i]=payoff_at_maturity(px[i]);
				vold[i] = (*ThePayoffPtr)(px[i]);
				vold_up[i] = vold[i];
				vold_down[i] = vold[i];
			}

		}

		if (t == (vd + 1)) {  //for theta, if vd==expiry date ? 
			for (int i = 0; i <= maxassetnodeindex; i++)
				vold_next[i] = vold[i];
		}

		double tau = (t - vd) / 365.0; //time from vdate
		double dt = 1 / 365.0;

		//double r_forward=getforward((t-md)/365.0,rfrate,rfrate_term,numrfrate);
		//double r_forward = R.getForward(tau);
		double r_forward = paras.getForward(tau);


		double q_forward = paras.getTodayDivAmount(t)/s0/dt;  //caution: t instead of tau 

		//double q_forward=0.0;
		/*if(t==360){
		q_forward=2.2263/100/dt;
		}*/

		//if (q_forward > 0)
		//	cout << "positive q at " << t << endl;

		for (int i = 0; i <= maxassetnodeindex; i++) {
			double short_vol = paras.lvol(tau, px[i]);
			double short_vol_up = paras.lvol_up(tau, px[i]);
			double short_vol_down = paras.lvol_down(tau, px[i]);

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

	}

	double pv = intp1d(s0, px, vold, 1, maxassetnodeindex - 1);
	double pv_next = intp1d(s0, px, vold_next, 1, maxassetnodeindex - 1);
	double pv_up = intp1d(s0*1.01, px, vold_up, 1, maxassetnodeindex - 1);
	double pv_down = intp1d(s0*0.99, px, vold_down, 1, maxassetnodeindex - 1);

	delete[] px;
	delete[] dpx;
	delete[] alpha;
	delete[] alpha_up;
	delete[] alpha_down;

	delete[] beta;
	delete[] vold;
	delete[] vold_up;
	delete[] vold_down;

	delete[] vnew;
	delete[] vnew_up;
	delete[] vnew_down;

	delete[] vold_next;
	delete[] A;
	delete[] A_up;
	delete[] A_down;

	delete[] B;
	delete[] B_up;
	delete[] B_down;


	delete[] C;
	delete[] C_up;
	delete[] C_down;

	result.resize(30, 0.0);
	result[0] = pv;
	result[1] = (pv_up - pv_down) / (s0*0.02); //delta
	result[2] = (pv_up - 2.0*pv + pv_down) / (s0*0.01) / (s0*0.01); //gamma
	result[3] = 0;  //vega
	result[4] = pv_next - pv;  //theta
	result[5] = s0;
	return pv;


}
double EuropeanOption::CalcMC(MarketParameters & paras, long numMc)
{
	//paras, conti div
	signed int vd = paras.get_vdate();
	double s0 = paras.get_spot();
	paras.calcLV();

	signed int expiryd = GetExpiryd();

	//Vol vol = para.get_vol();
	//vol.calcLv(s0, R, Q);
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
		r_forward_p[i] = paras.getForward(tau_p[i]);
		r_dc_p[i] = paras.getIntpRate(tau_p[i]);
		q_forward_p[i] = paras.getDivForward(tau_p[i]);
	}

	double dt = 1 / 365.0;

	int *idxT = new signed int[expiryd - vd + 1];
	for (int tfv = 0; tfv <= expiryd - vd; tfv++) {
		idxT[tfv] = paras.find_index_term(tfv / 365.0);
	}

	for (long i = 0; i<numMc; i++)
	{
		s_tmp = s0;
		//make stock price dailiy 
		for (signed int t = vd + 1; t <= expiryd; t++) {
			//double short_vol = paras.lvol(tau_p[t - vd], s_tmp);
			//fast search algorithm
			double short_vol = paras.get_Lvol_hybrid(idxT[t - vd], s_tmp);
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
double EuropeanOption::GetRefPrice() const
{
	return refprice;
}
signed int EuropeanOption::GetExpiryd() const
{
	return expiry_date;
}
EuropeanOption::EuropeanOption(double _refprice, signed int _expiryd,const Payoff& ThePayoff_)
	:refprice(_refprice),expiry_date(_expiryd)
{
	ThePayoffPtr=ThePayoff_.clone();
	result=std::vector<double>(30);
}

std::vector<double> EuropeanOption::GetResult() const
{
	return result;
}

EuropeanOption::~EuropeanOption()
{
	delete ThePayoffPtr;
}