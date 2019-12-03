#pragma once
#include "FDM.h"
class UpdateAutocall {
private:
	signed int t;
	double strike;
	double kibarrier;
public:
	void update(FDM& fdm) const;
};