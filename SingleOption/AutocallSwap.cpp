#include "AutocallSwap.h"
#include <fstream>
#include <string>
#include <sstream>

AutocallSwap::~AutocallSwap()
{
	delete ThePayoffPtr;
}

AutocallSwap::AutocallSwap(char * csvfile)
{
	ifstream infile(csvfile); // for example

	string line = "";
	string word = "";
	for (int i = 0; i < 14; i++) {
		getline(infile, line); //skip first 14 lines
	}

	getline(infile, line); //line15
	stringstream strstr(line);
	getline(strstr, word, ','); //cells(15,1)
	getline(strstr, word, ','); //cells(15,2)
	this->refprice = stod(word);
	while (getline(strstr, word, ',')) {
	}

	getline(infile, line); //line16
	strstr = stringstream(line);
	getline(strstr, word, ','); //cells(16,1)
	getline(strstr, word, ','); //cells(16,2)
	double spot = stod(word);
	while (getline(strstr, word, ',')) {
	}

	getline(infile, line); //line17
	getline(infile, line); //line18
	getline(infile, line); //line19
	strstr = stringstream(line);
	getline(strstr, word, ','); //cells(19,1)
	getline(strstr, word, ','); //cells(19,2)
	long nM = stoul(word);
	while (getline(strstr, word, ',')) {
	}

	for (int i = 0; i < 12; i++) {
		getline(infile, line); //skip first 22 lines
	}

	getline(infile, line); //line32
	strstr = stringstream(line);
	getline(strstr, word, ','); //cells(32,1)
	getline(strstr, word, ','); //cells(32,2)
	signed int vd = stoul(word);
	while (getline(strstr, word, ',')) {
	}

	getline(infile, line); //line33
	strstr = stringstream(line);
	getline(strstr, word, ','); //skip first col
	getline(strstr, word, ','); //second col
	this->expiry_date = stoul(word);
	while (getline(strstr, word, ',')) {
	}

	getline(infile, line); //line34
	getline(infile, line); //line35
	strstr = stringstream(line);
	getline(strstr, word, ','); //skip first col
	getline(strstr, word, ','); //second col
	int nokiflag = stoi(word);
	while (getline(strstr, word, ',')) {
	}

	getline(infile, line); //line36
	getline(infile, line); //line37
	strstr = stringstream(line);
	getline(strstr, word, ','); //skip first col
	getline(strstr, word, ','); //second col
	this->hitflag = stoi(word);
	while (getline(strstr, word, ',')) {
	}


	getline(infile, line); //line38
	getline(infile, line); //line39
	strstr = stringstream(line);
	getline(strstr, word, ','); //skip first col
	getline(strstr, word, ','); //second col
	double notional_protection = stod(word);
	double put_strike = refprice*(1.0 - notional_protection);
	while (getline(strstr, word, ',')) {
	}

	getline(infile, line); //line40
	strstr = stringstream(line);
	getline(strstr, word, ','); //skip first col
	getline(strstr, word, ','); //second col
	double kilevel = stod(word);
	double kibarrier = kilevel*refprice;
	while (getline(strstr, word, ',')) {
	}

	getline(infile, line); //line41
	strstr = stringstream(line);
	getline(strstr, word, ','); //skip first col
	getline(strstr, word, ','); //second col
	double dummycoupon = stod(word);
	while (getline(strstr, word, ',')) {
	}

	vector<signed int> vdates;
	vdates.push_back(-1);
	for (int i = 0; i < 12; i++) {
		getline(infile, line);
		strstr = stringstream(line);
		getline(strstr, word, ','); //skip first col
		getline(strstr, word, ','); //second col
		if (!word.empty()) {
			if (stod(word) > 0)
				vdates.push_back(stoul(word));
		}
		while (getline(strstr, word, ',')) {
		}
	}

	int	nb_autocall = vdates.size() - 1;

	vector<double> vstrikes;
	vstrikes.push_back(-1);

	for (int i = 0; i < 12; i++) {
		getline(infile, line);
		strstr = stringstream(line);
		getline(strstr, word, ','); //skip first col
		getline(strstr, word, ','); //second col
		if (!word.empty()) {
			if (stod(word) > 0)
				vstrikes.push_back(stod(word)*refprice);
		}
		while (getline(strstr, word, ',')) {
		}
	}

	vector<double> vcoupons;
	vcoupons.push_back(-1);
	for (int i = 0; i < 12; i++) {
		getline(infile, line);
		strstr = stringstream(line);
		getline(strstr, word, ','); //skip first col
		getline(strstr, word, ','); //second col
		if (!word.empty()) {
			if (stod(word) > 0)
				vcoupons.push_back(stod(word));
		}
		while (getline(strstr, word, ',')) {
		}
	}

	ThePayoffPtr = new PayoffAutocallSwap(nb_autocall, vdates, vstrikes, vcoupons, kibarrier, put_strike, dummycoupon, refprice);
	infile.close();

}
