#include <algorithm>
#include <random>
#include <cassert>
#include <cmath>
#include <fstream>
#include <string>
#include <sstream>
#include "j_fd.h"
#include "AutocallSwap.h"
#include "k_miscellaneous.hpp"

AutocallSwap::~AutocallSwap()
{
	delete ThePayoffPtr;
}

AutocallSwap::AutocallSwap(char * csvfile)
{
	ifstream infile(csvfile); // for example

	string line = "";
	string word = "";
	for (int i = 0; i < 14; i++) {
		getline(infile, line); //skip first 14 lines
	}

	getline(infile, line); //line15
	stringstream strstr(line);
	getline(strstr, word, ','); //cells(15,1)
	getline(strstr, word, ','); //cells(15,2)
	this->refprice = stod(word);
	while (getline(strstr, word, ',')) {
	}

	getline(infile, line); //line16
	strstr = stringstream(line);
	getline(strstr, word, ','); //cells(16,1)
	getline(strstr, word, ','); //cells(16,2)
	double spot = stod(word);
	while (getline(strstr, word, ',')) {
	}

	getline(infile, line); //line17
	getline(infile, line); //line18
	getline(infile, line); //line19
	strstr = stringstream(line);
	getline(strstr, word, ','); //cells(19,1)
	getline(strstr, word, ','); //cells(19,2)
	long nM = stoul(word);
	while (getline(strstr, word, ',')) {
	}

	for (int i = 0; i < 12; i++) {
		getline(infile, line); //skip first 22 lines
	}

	getline(infile, line); //line32
	strstr = stringstream(line);
	getline(strstr, word, ','); //cells(32,1)
	getline(strstr, word, ','); //cells(32,2)
	signed int vd = stoul(word);
	while (getline(strstr, word, ',')) {
	}

	getline(infile, line); //line33
	strstr = stringstream(line);
	getline(strstr, word, ','); //skip first col
	getline(strstr, word, ','); //second col
	this->expiry_date = stoul(word);
	while (getline(strstr, word, ',')) {
	}

	//getline(infile, line);
	getline(infile, line); //line34
	strstr = stringstream(line);
	getline(strstr, word, ','); //skip first col
	getline(strstr, word, ','); //second col
	int nokiflag = stoi(word);
	while (getline(strstr, word, ',')) {
	}

	//getline(infile, line); 
	getline(infile, line); //line35
	strstr = stringstream(line);
	getline(strstr, word, ','); //skip first col
	getline(strstr, word, ','); //second col
	this->hitflag = stoi(word);
	while (getline(strstr, word, ',')) {
	}


	//getline(infile, line); 
	getline(infile, line); //line36
	strstr = stringstream(line);
	getline(strstr, word, ','); //skip first col
	getline(strstr, word, ','); //second col
	double notional_protection = stod(word);
	double put_strike = refprice*(1.0 - notional_protection);
	while (getline(strstr, word, ',')) {
	}

	getline(infile, line); //line40
	strstr = stringstream(line);
	getline(strstr, word, ','); //skip first col
	getline(strstr, word, ','); //second col
	double kilevel = stod(word);
	double kibarrier = kilevel*refprice;
	while (getline(strstr, word, ',')) {
	}

	getline(infile, line); //line41
	strstr = stringstream(line);
	getline(strstr, word, ','); //skip first col
	getline(strstr, word, ','); //second col
	double dummycoupon = stod(word);
	while (getline(strstr, word, ',')) {
	}

	vector<signed int> vdates;
	vdates.push_back(-1);
	for (int i = 0; i < 12; i++) {
		getline(infile, line);
		strstr = stringstream(line);
		getline(strstr, word, ','); //skip first col
		getline(strstr, word, ','); //second col
		if (!word.empty()) {
			if (stod(word) > 0)
				vdates.push_back(stoul(word));
		}
		while (getline(strstr, word, ',')) {
		}
	}

	int	nb_autocall = vdates.size() - 1;

	vector<double> vstrikes;
	vstrikes.push_back(-1);

	for (int i = 0; i < 12; i++) {
		getline(infile, line);
		strstr = stringstream(line);
		getline(strstr, word, ','); //skip first col
		getline(strstr, word, ','); //second col
		if (!word.empty()) {
			if (stod(word) > 0)
				vstrikes.push_back(stod(word)*refprice);
		}
		while (getline(strstr, word, ',')) {
		}
	}

	vector<double> vcoupons;
	vcoupons.push_back(-1);
	for (int i = 0; i < 12; i++) {
		getline(infile, line);
		strstr = stringstream(line);
		getline(strstr, word, ','); //skip first col
		getline(strstr, word, ','); //second col
		if (!word.empty()) {
			if (stod(word) > 0)
				vcoupons.push_back(stod(word));
		}
		while (getline(strstr, word, ',')) {
		}
	}

	vector<signed int> vffd; //vector of floating fixing dates
	vffd.push_back(-1);
	for (int i = 0; i < 20; i++) { //from 75th row to 94th row
		getline(infile, line);
		strstr = stringstream(line);
		getline(strstr, word, ','); //skip first col
		getline(strstr, word, ','); //second col
		if (!word.empty()) {
			if (stoul(word) > 0)
				vffd.push_back(stoul(word));
		}
		while (getline(strstr, word, ',')) {
		}
	}

	vector<signed int> vfpd; //vector of floating paying dates
	vfpd.push_back(-1);
	for (int i = 0; i < 20; i++) {//from 95th row to 114th row
		getline(infile, line);
		strstr = stringstream(line);
		getline(strstr, word, ','); //skip first col
		getline(strstr, word, ','); //second col
		if (!word.empty()) {
			if (stoul(word) > 0)
				vfpd.push_back(stoul(word));
		}
		while (getline(strstr, word, ',')) {
		}
	}

	ThePayoffPtr = new PayoffAutocallSwap(nb_autocall, vdates, vstrikes, vcoupons, kibarrier, put_strike, dummycoupon, refprice,vffd,vfpd);
	infile.close();
}

double AutocallSwap::Calc(MarketParameters & paras)
{
	//AutocallOption::Calc(paras);
	//매수자입장 swap가격=(note가격-1)-경과이자 
	//경과이자: 가장 최근 payment date로부터 오늘까지의 누적이자
	//note가격 98, 경과이자 0.002(20bp), swap=0.98-1-0.002=-0.022

	//Here, copy of AutocallOption::Calc()

	double s0 = paras.get_spot();
	signed int vd = paras.get_vdate();

	int nb_autocall = ThePayoffPtr->GetNbAutocall();

	paras.calcLV();

	std::vector<signed int> autocall_date;
	autocall_date = ThePayoffPtr->GetAutocall_date();

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

			ThePayoffPtr->copy_v_to_u(vnew, unew, 0, kiindex);
			ThePayoffPtr->copy_v_to_u(vnew_up, unew_up, 0, kiindex);
			ThePayoffPtr->copy_v_to_u(vnew_down, unew_down, 0, kiindex);

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

	double pv, pv_next, pv_up, pv_down, delta1, gamma1;

	if (hitflag) { //hitted -> vold 
		pv = intp1d(s0, px, vold, 1, maxassetnodeindex - 1);
		pv_next = intp1d(s0, px, vold_next, 1, maxassetnodeindex - 1);
		pv_up = intp1d(s0*1.01, px, vold_up, 1, maxassetnodeindex - 1);
		pv_down = intp1d(s0*0.99, px, vold_down, 1, maxassetnodeindex - 1);
		delta1 = (intp1d(s0*1.01, px, vold, 1, maxassetnodeindex - 1) - intp1d(s0*0.99, px, vold, 1, maxassetnodeindex - 1)) / (s0*1.01 - s0*0.99);
		gamma1 = (intp1d(s0*1.01, px, vold, 1, maxassetnodeindex - 1) - 2 * intp1d(s0, px, vold, 1, maxassetnodeindex - 1) + intp1d(s0*0.99, px, vold, 1, maxassetnodeindex - 1)) / (s0*s0*0.01*0.01);
	}
	else {
		pv = intp1d(s0, px, uold, 1, maxassetnodeindex - 1);
		pv_next = intp1d(s0, px, uold_next, 1, maxassetnodeindex - 1);
		pv_up = intp1d(s0*1.01, px, uold_up, 1, maxassetnodeindex - 1);
		pv_down = intp1d(s0*0.99, px, uold_down, 1, maxassetnodeindex - 1);
		delta1 = (intp1d(s0*1.01, px, uold, 1, maxassetnodeindex - 1) - intp1d(s0*0.99, px, uold, 1, maxassetnodeindex - 1)) / (s0*1.01 - s0*0.99);
		gamma1 = (intp1d(s0*1.01, px, uold, 1, maxassetnodeindex - 1) - 2 * intp1d(s0, px, uold, 1, maxassetnodeindex - 1) + intp1d(s0*0.99, px, uold, 1, maxassetnodeindex - 1)) / (s0*s0*0.01*0.01);
	}

	result.resize(30, 0.0);
	for (auto iter = result.begin(); iter != result.end(); iter++)
		*iter = 0.0;

	//past interest 
	double past_interest = 0;
	//past_interest = ThePayoffPtr->floating_fixing_date[2];
	result[0] = pv;
	result[1] = (pv_up - pv_down) / (s0*0.02);
	result[2] = (pv_up - 2.0*pv + pv_down) / (s0*0.01) / (s0*0.01);

	if (vd == expiry_date) {
		result[4] = 0;
	}
	else {
		result[4] = pv_next - pv;  //theta
	}

	result[6] = delta1;
	result[7] = gamma1;

	//pure delta


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
	//return 0.0;
}

double AutocallSwap::Calc_floating_leg(MarketParam & paras) const
{
	signed int vd = paras.get_vdate();
	//vector<signed int> autocall_date = ThePayoffPtr->GetAutocall_date();
	//vector<signed int>
	return 0.0;
}
