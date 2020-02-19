#include "PayoffAutocallSwap.h"
#include <algorithm>

void PayoffAutocallSwap::final_updator(double * vold, double * uold, double * px, int mini, int maxi) const
{
	//knock-in
	for (int i = mini; i <= maxi; i++) {
		if (px[i] > strike.back()) {
			vold[i] = 0.0 + autocall_coupon.back();
		}
		else {
			vold[i] = 0.0 - std::max(put_strike - px[i], 0.0) / refprice;
		}
	}

	//not knocked-in
	for (int i = mini; i <= maxi; i++) {
		if (px[i] > ki_barier) {
			//			uold[i] = 1.0 + autocall_coupon.back();
			uold[i] = 0.0 + dummy_coupon;
		}
		else {
			uold[i] = 0.0 - std::max(put_strike - px[i], 0.0) / refprice;
		}
	}
}

void PayoffAutocallSwap::updator(signed int td, double * vold, double * uold, double * px, int mini, int maxi) const
{
	auto iterX = strike.begin();
	auto iterC = autocall_coupon.begin();

	for (auto iter = autocall_date.begin(); iter != autocall_date.end(); ++iter) {
		if (*iter == td) {
			for (int i = mini; i <= maxi; i++) {
				if (px[i] > *iterX) {
					vold[i] = 0.0 + *iterC;
					uold[i] = 0.0 + *iterC;
				}
			}
			//break;
		}
		++iterX;
		++iterC;
	}
}

PayoffAutocallStd * PayoffAutocallSwap::clone() const
{
	return new PayoffAutocallSwap(*this);
}
