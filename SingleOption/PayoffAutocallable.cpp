#include "PayoffAutocallable.h"
#include <algorithm>
#include "j_fd.h"

PayoffAutocallable::PayoffAutocallable(int nb_autocall_, signed int * date_, double * strike_, double * coupon, double ki_barrier_, double put_strike_, double dummy_coupon, double refprice_, int maxi_)
{
	//maxassetnodeindex = 300;
	px = new double[maxassetnodeindex + 1];
	dpx = new double[maxassetnodeindex + 1];

	vold = new double[maxassetnodeindex + 1];
	vold_up = new double[maxassetnodeindex + 1];
	vold_down = new double[maxassetnodeindex + 1];

	uold = new double[maxassetnodeindex + 1];
	uold_up = new double[maxassetnodeindex + 1];
	uold_down = new double[maxassetnodeindex + 1];

	vold_next = new double[maxassetnodeindex + 1];  //reserve for theta
	uold_next = new double[maxassetnodeindex + 1];  //reserve for theta

	vnew = new double[maxassetnodeindex + 1];
	vnew_up = new double[maxassetnodeindex + 1];
	vnew_down = new double[maxassetnodeindex + 1];


	unew = new double[maxassetnodeindex + 1];
	unew_up = new double[maxassetnodeindex + 1];
	unew_down = new double[maxassetnodeindex + 1];
}

void PayoffAutocallable::updator(signed int td) const
{
	auto iterX = strike.begin();
	auto iterC = autocall_coupon.begin();

	for (auto iter = autocall_date.begin(); iter != autocall_date.end(); ++iter) {
		if (*iter == td) {
			for (int i = 0; i <= maxassetnodeindex; i++) {
				if (px[i] > *iterX) {
					vold[i] = 1.0 + *iterC;
					uold[i] = 1.0 + *iterC;
				}
			}
			//break;
		}
		++iterX;
		++iterC;
	}

}

void PayoffAutocallable::final_updator() const
{
	for (int i = 0; i <= maxassetnodeindex; i++) {
		if (px[i] > strike.back()) {
			vold[i] = 1.0 + autocall_coupon.back();
		}
		else {
			vold[i] = 1.0 - std::max(put_strike - px[i], 0.0) / refprice;
		}
	}

	for (int i = 0; i <= maxassetnodeindex; i++) {
		if (px[i] > ki_barier) {
			uold[i] = 1.0 + autocall_coupon.back();
		}
		else {
			uold[i] = 1.0 - std::max(put_strike - px[i], 0.0) / refprice;
		}
	}

	for (int i = 0; i <= maxassetnodeindex; i++) {
		vold_up[i] = vold[i];
		vold_down[i] = vold[i];
		uold_up[i] = uold[i];
		uold_down[i] = uold[i];
	}


}

void PayoffAutocallable::copy_v_to_u(double * v, double * u, int mini, int maxi) const
{
	for (int i = 0; i <= maxassetnodeindex; i++)
		u[i] = v[i];

}

void PayoffAutocallable::copy_for_theta() const
{
	for (int i = 0; i <= maxassetnodeindex; i++) {
		vold_next[i] = vold[i];
		uold_next[i] = uold[i];
	}
}

PayoffAutocallable::~PayoffAutocallable()
{
	delete[] px;
	delete[] dpx;
	delete[] px;
	delete[] dpx;

	delete[] vold;
	delete[] vold_up;
	delete[] vold_down;

	delete[] uold;
	delete[] uold_up;
	delete[] uold_down;

	delete[] vold_next;  //reserve for theta
	delete[] uold_next;  //reserve for theta

	delete[] vnew;
	delete[] vnew_up;
	delete[] vnew_down;


	delete[] unew;
	delete[] unew_up;
	delete[] unew_down;
}

PayoffAutocallable * PayoffAutocallable::clone() const
{
	return new PayoffAutocallable(*this);
}

void PayoffAutocallable::ResetFDGrid() const
{
	for (int i = 0; i <= maxassetnodeindex; i++)
	{
		if (ki_barier >= px[i] && ki_barier <= px[i + 1])
		{

			if (ki_barier - px[i] <= dpx[i] / 2.0)
				px[i] = ki_barier;
			else
				px[i + 1] = ki_barier;

			dpx[i + 1] = px[i + 2] - px[i + 1];
			dpx[i] = px[i + 1] - px[i];
			dpx[i - 1] = px[i] - px[i - 1];
			break;
		}
	}

	//need grid control for strike.....
	
	
}

int PayoffAutocallable::GetNbAutocall() const
{
	return nb_autocall;
}

std::vector<signed int> PayoffAutocallable::GetAutocall_date() const
{
	return autocall_date;
}

double PayoffAutocallable::GetKiBarrier() const
{
	return ki_barier;
}

std::vector<double> PayoffAutocallable::GetAutocall_strike() const
{
	return strike;
}

std::vector<double> PayoffAutocallable::GetAutocall_coupon() const
{
	return autocall_coupon;
}

double PayoffAutocallable::GetPutStrike() const
{
	return put_strike;
}

double PayoffAutocallable::GetDummyCoupon() const
{
	return dummy_coupon;
}

void PayoffAutocallable::init(double refprice, signed int vd, Vol vol)
{
	px[0] = 0.0;
	double tmp_ds = refprice*2.0 / maxassetnodeindex;
	for (int i = 1; i <= maxassetnodeindex; i++)
		px[i] = px[i - 1] + tmp_ds;
	for (int i = 0; i<maxassetnodeindex; i++) //max index of dp is max index of px -1
		dpx[i] = px[i + 1] - px[i];
	
	ResetFDGrid();

	//int kiindex = 0;
	kiindex = findlowerindex(px, ki_barier, 0, maxassetnodeindex);

	idxS = new int[maxassetnodeindex + 1];
	idxT = new signed int[autocall_date[nb_autocall] - vd + 1];
	for (int i = 0; i <= maxassetnodeindex; i++) {
		idxS[i] = vol.find_index_spot(px[i]);
	}

	for (int tfv = 0; tfv <= autocall_date[nb_autocall] - vd; tfv++) {
		idxT[tfv] = vol.find_index_term(tfv / 365.0);
	}
}



void PayoffAutocallable::backward(MarketParam& para, int tau)
{
	//for (int i = 0; i <= maxassetnodeindex; i++) {
	//
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
}
