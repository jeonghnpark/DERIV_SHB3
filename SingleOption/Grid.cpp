#include "Grid.h"
#include "j_fd.h"
#include "k_miscellaneous.hpp"

double Grid::getpv(double s0) const
{
	return intp1d(s0,px,vold,1,maxassetnodeindex-1);
}
void Grid::backward(signed int d, const MarketParam& para)
{
	Vol vol=para.get_vol();
	
	Rate R=para.get_rfrate();
	Rate Q=para.get_q();
	vol.calcLv(para.get_spot(),R,Q);

	double tau=(d-para.get_vdate())/365.0; //time from vdate
	double dt=1/365.0;
	
	double r_forward=R.getForward(tau);
	double q_forward=Q.getForward(tau);

	for(int i=0;i<=maxassetnodeindex;i++){	
		double short_vol=vol.lvol(tau,px[i]);
		alpha[i]=0.5*short_vol*short_vol*dt;
		beta[i]=(r_forward-q_forward)*dt;
	}

	trimatrix1d(A,B,C,alpha,beta, r_forward, dt, px, dpx,1, maxassetnodeindex-1);
	trimxsolve1d(A,B, C, vold,  vnew, 0, maxassetnodeindex, 0,0);
		
	for(int i=0;i<=maxassetnodeindex;i++){
		vold[i]=vnew[i];
	}
}
void Grid::finalPayoff(const EuropeanVanilla& ev)
{
	for(int i=0;i<=maxassetnodeindex;i++)
		vold[i]=ev.OptionPayoff(px[i]);
}
void Grid::init(const EuropeanVanilla& ev,  const MarketParam& para)
{
	
	px[0]=0.0;
	double tmp_ds=ev.GetReferencePrice() *3.0/maxassetnodeindex;
	for(int i=1;i<=maxassetnodeindex;i++)
		px[i]=px[i-1]+tmp_ds;
	for(int i=0;i<maxassetnodeindex;i++) //max index of dp is max index of px -1
		dpx[i]=px[i+1]-px[i];
	

}
Grid::Grid(int maxi_):maxassetnodeindex(maxi_)
{
	px=new double[maxassetnodeindex+1];
	dpx=new double[maxassetnodeindex+1];
	alpha=new double[maxassetnodeindex+1];
	beta=new double[maxassetnodeindex+1];
	vold=new double[maxassetnodeindex+1];
	vold_next=new double[maxassetnodeindex+1];  //reserve for theta
	vnew=new double[maxassetnodeindex+1];
	A=new double[maxassetnodeindex+1];
	B=new double[maxassetnodeindex+1];
	C=new double[maxassetnodeindex+1];
}
Grid::~Grid()
{
	delete[] px;
	delete[] dpx;
	delete[] alpha;
	delete[] beta;
	delete[] vold;
	delete[] vnew;
	delete[] vold_next;
	delete[] A;;
	delete[] B;
	delete[] C;
}