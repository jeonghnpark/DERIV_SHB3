#pragma once
class PayoffAutocall {
public:
	PayoffAutocall() {};
	//virtual double operator()(double spot) const = 0;
	virtual void updator(signed int td, double* vold, double *px, int mini, int maxi) const = 0;
	virtual void final_updator(double* vold, double *px,int mini, int maxi) const = 0;

	virtual ~PayoffAutocall() {}
	virtual PayoffAutocall* clone() const = 0;
	virtual void ResetFDGrid(double* px, double* dpx, int minnode, int maxnode) const {}

protected:

};