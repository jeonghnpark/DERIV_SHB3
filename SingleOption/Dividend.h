#pragma once
#include <vector>
#include "Rate.h"

using namespace std;

class Dividend {
public:
	Dividend() {}
	Dividend(vector<signed int> _exdate, vector<double> _amount) :exdate(_exdate), amount(_amount) {};
	double getLumpsum(signed int start, signed int end) const;
	Rate getDividendCurve(std::vector<double>& rts, signed int vd, double spot);
private:
	vector<signed int> exdate;
	vector<double> amount;

};