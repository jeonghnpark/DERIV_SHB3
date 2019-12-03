#include "EuropeanVanilla.h"

double EuropeanVanilla::GetReferencePrice() const
{
	return refprice;
}

EuropeanVanilla::~EuropeanVanilla()
{
	delete ThePayoffPtr;
}

EuropeanVanilla::EuropeanVanilla(const Payoff& ThePayoff_, double refprice_,signed int expiryd_):refprice(refprice_), expiryd(expiryd_)
{
	ThePayoffPtr=ThePayoff_.clone();
}

signed int EuropeanVanilla::GetExpiryd() const
{
	return expiryd;
}

double EuropeanVanilla::OptionPayoff(double spot) const
{
	return (*ThePayoffPtr)(spot);
}

EuropeanVanilla::EuropeanVanilla(const EuropeanVanilla& original)
{
	expiryd=original.expiryd;
	ThePayoffPtr=original.ThePayoffPtr->clone();
}

EuropeanVanilla& EuropeanVanilla::operator=(const EuropeanVanilla& original)
{
	if(this!= &original){
		expiryd=original.expiryd;
		delete ThePayoffPtr;
		ThePayoffPtr=original.ThePayoffPtr->clone();
	}
	return *this;
}
