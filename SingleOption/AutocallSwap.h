#pragma once
#include "AutocallOption.h"
#include "PayoffAutocallSwap.h"
class AutocallSwap :public AutocallOption {
public:
	virtual ~AutocallSwap();
	AutocallSwap(char* csvfile);
};