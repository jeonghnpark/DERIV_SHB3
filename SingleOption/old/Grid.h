#ifndef GRID_H
#define GRID_H
#include "EuropeanVanilla.h"
#include "MarketParam.h"


class Grid{
public:
	Grid(int maxi_);
	~Grid();
	void init(const EuropeanVanilla& ev,  const MarketParam& para);
	void finalPayoff(const EuropeanVanilla& ev);
	void backward(signed int d, const MarketParam& para);
	double getpv(double s0) const;
private:
	int maxassetnodeindex;
	double *px;
	double *dpx;
	double *alpha;
	double *beta;
	double *vold;
	double *vold_next;  //reserve for theta
	double *vnew;
	double *A;
	double *B;
	double *C;
};
#endif