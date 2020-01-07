#include "MarketParameters.h"
#include "Rate.h"
#include <vector>

using namespace std;

double MarketParameters::getTodayDivAmount(signed int n_t) const
{
	
	return div.getLumpsum(n_t, n_t);

}

void MarketParameters::calcLV()
{
	//make div-curve from discreter divdend UNDER CONSTRUCTION !
	//vector<double> div_rate;
	//vector<double> tenor = {7/360.0, 14/360.0, 21/360.0, 30/360.0, 60/360.0,90/360.0,180/360.0,1.0, 2.0, 3.0};
	//
	//div_rate.push_back(div.getLumpsum(vdate, vdate + 7) / spot / tenor[0]);
	//div_rate.push_back(div.getLumpsum(vdate, vdate + 14) / spot / tenor[1]);
	//div_rate.push_back(div.getLumpsum(vdate, vdate + 21) / spot / tenor[2]);
	//div_rate.push_back(div.getLumpsum(vdate, vdate + 30) / spot / tenor[3]);
	//div_rate.push_back(div.getLumpsum(vdate, vdate + 60) / spot / tenor[4]);
	//div_rate.push_back(div.getLumpsum(vdate, vdate + 90) / spot / tenor[5]);
	//div_rate.push_back(div.getLumpsum(vdate, vdate + 180) / spot / tenor[6]);
	//div_rate.push_back(div.getLumpsum(vdate, vdate + 360) / spot / tenor[7]);
	//div_rate.push_back(div.getLumpsum(vdate, vdate + 720) / spot / tenor[8]);
	//div_rate.push_back(div.getLumpsum(vdate, vdate + 1080) / spot / tenor[9]);
	//Rate q(div_rate, tenor);

	vol.calcLv(spot, r, q);
}

double MarketParameters::lvol(double t_axis, double s_axis) const
{
	return vol.lvol(t_axis, s_axis);
}

void MarketParameters::reset_Ivol_up()
{
	vol.reset_Ivol_up();
}

double MarketParameters::lvol_up(double t_axis, double s_axis) const
{
	return vol.lvol_up(t_axis, s_axis);
}

double MarketParameters::lvol_down(double t_axis, double s_axis) const
{
	return vol.lvol_down(t_axis, s_axis);
}

double MarketParameters::get_Lvol(int i_t, int i_k) const
{
	return vol.get_Lvol(i_t, i_k);
}

double MarketParameters::get_Lvol_hybrid(int i_t, double s_axis) const
{
	//int i_k = find_index_spot(s_axis);
	int i_k = find_index_spot2(s_axis); //SMART SEARCH 2019.12.18
	return vol.get_Lvol(i_t, i_k);
}

double MarketParameters::get_Lvol_hybrid_up(int i_t, double s_axis) const
{
	//int i_k = find_index_spot(s_axis);
	int i_k = find_index_spot2(s_axis); //SMART SEARCH 2019.12.18
	return vol.get_Lvol_up(i_t, i_k);
}

double MarketParameters::get_Lvol_hybrid_down(int i_t, double s_axis) const
{
	//int i_k = find_index_spot(s_axis);
	int i_k = find_index_spot2(s_axis); //SMART SEARCH 2019.12.18
	return vol.get_Lvol_down(i_t, i_k);
}
double MarketParameters::get_Lvol_up(int i_t, int i_k) const
{
	return vol.get_Lvol_up(i_t, i_k);

}

double MarketParameters::get_Lvol_down(int i_t, int i_k) const
{
	return vol.get_Lvol_down(i_t, i_k);
}


int MarketParameters::find_index_spot(double target) const
{
	return vol.find_index_spot(target);
}

int MarketParameters::find_index_term(double target) const
{
	return vol.find_index_term(target);
}

int MarketParameters::find_index_spot2(double target) const
{
	return vol.find_index_spot2(target);
}

