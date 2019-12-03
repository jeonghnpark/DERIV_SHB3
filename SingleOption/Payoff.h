#ifndef PAYOFF_H
#define PAYOFF_H
#include <string>
class Payoff{
public:
	Payoff(){};
	virtual double operator()(double spot) const=0;
	virtual ~Payoff(){}
	virtual Payoff* clone() const=0;
	virtual void ResetFDGrid(double* px, double* dpx, int minnode, int maxnode) const {}

private:
	
};

#endif