#ifndef PAYOFFDIGITALCALL_H
#define PAYOFFDIGITALCALL_H
#include "Payoff.h"
#include <string>

class PayoffDigitalCall:public Payoff {
public: 
	PayoffDigitalCall(double strike_);
	virtual ~PayoffDigitalCall(){}
	virtual double operator()(double spot) const;
	virtual Payoff* clone() const;
	virtual void ResetFDGrid(double* px, double* dpx, int minnode, int maxnode) const;
private:
	double strike;
	std::string name;
};
#endif