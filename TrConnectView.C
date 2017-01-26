//make 2016.11.30 inoh
//ROOT macro
#include <TRint.h>
#include <TCanvas.h>
#include <TGraph.h>

#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <math.h>

#include <TColor.h>
#include <TAxis.h>
#include <TFrame.h>
#include <TPave.h>
#include <TStyle.h>
#include <TLegend.h>
#include <TGaxis.h>
#include <TText.h>

using namespace std;

struct trconnect{
	double gx,gy,tx,ty;
};

int main(int argc, char* argv[]){
//called by TRint
	int gomic=1;
	char* gomiv[gomic];
	gomiv[0]=argv[0];
//text box
	char buf1[1024],buf2[1024],buf3[1024],buf4[1024];
//readfile
	ifstream ifs;
//call string stream
	string info,hoge;
//insert trconnect.log
	vector< trconnect > log;
//for vector
	trconnect one_log;
//for calculate standard deviation
	double gx,gy,tx,ty;
	double minx,miny,maxx,maxy;
	double gstdevx,gstdevy,gerrx,gerry;
	double tstdevx,tstdevy,terrx,terry;
	double a,b,c,d;//fuck you

//use TRint
	TRint app("app" ,&gomic ,gomiv,0,0,kTRUE);
	
/* eat TrConnect.log */
	ifs.open(argv[1]);
	if(!ifs.fail()){

//tired...
//		getline(ifs,info);
//		getline(ifs,info);
//		getline(ifs,info);

		while(ifs && getline(ifs,info)){
			stringstream ss(info);

//3columns is read
			if(!info.empty())ss>>hoge>>hoge>>info;

			if(info.compare("Grain")==0){

//past columns is read
				ss>>one_log.gx>>one_log.gy>>hoge>>hoge>>one_log.tx>>one_log.ty;
//				cerr<<hoge<<endl;
//insert vector
				log.push_back(one_log);

				info = "NULL";
			}//if

		}//while

	}//if
	else {
		cerr << "read error" << endl;
		exit(1);
	}//else

//TrConnect.log closed
	ifs.close();

//call canvas
	TCanvas *c1 = new TCanvas("TrConnectByGrain","Grain-Grain",0,100,400,400);
	TCanvas *c2 = new TCanvas("TrConnectByTrack","Track-Track",400,100,400,400);

//call two tgraphs
	TGraph *g[2];
	g[0]=new TGraph();
	g[1]=new TGraph();

//	cerr<<"TrConnectTime:\t"<<log.size()<<endl;
//	cin>>hoge;

//initialize
	minx=log[0].gx;
	miny=log[0].gy;
	maxx=log[0].gx;
	maxy=log[0].gy;
	gstdevx=0.;
	gstdevy=0.;
	tstdevx=0.;
	tstdevy=0.;
	a=0.;
	b=0.;
	c=0.;
	d=0.;

	for(int i=0;i<log.size();i++){
//vector is called tgraphs
		g[0]->SetPoint(i,log[i].gx,log[i].gy);
		g[1]->SetPoint(i,log[i].tx,log[i].ty);

//scaling...
		if(maxx<log[i].gx)maxx=log[i].gx;
		if(maxx<log[i].tx)maxx=log[i].tx;
		if(minx>log[i].gx)minx=log[i].gx;
		if(minx>log[i].tx)minx=log[i].tx;
		if(maxy<log[i].gy)maxy=log[i].gy;
		if(maxy<log[i].ty)maxy=log[i].ty;
		if(miny>log[i].gy)miny=log[i].gy;
		if(miny>log[i].ty)miny=log[i].ty;

//for calc stdev
		gstdevx+=log[i].gx;
		gstdevy+=log[i].gy;
		tstdevx+=log[i].tx;
		tstdevy+=log[i].ty;

		a+=pow(log[i].gx,2.);
		b+=pow(log[i].gy,2.);
		c+=pow(log[i].tx,2.);
		d+=pow(log[i].ty,2.);

	}//for

//calc mean
	gerrx = gstdevx/(double)log.size();
	gerry = gstdevy/(double)log.size();
	terrx = tstdevx/(double)log.size();
	terry = tstdevy/(double)log.size();

	a /= (double)log.size();
	b /= (double)log.size();
	c /= (double)log.size();
	d /= (double)log.size();

//calc stdev
	gstdevx = a - pow(gerrx,2.);
	gstdevy = b - pow(gerry,2.);
	tstdevx = c - pow(terrx,2.);
	tstdevy = d - pow(terry,2.);

	gstdevx = sqrt(gstdevx);
	gstdevy = sqrt(gstdevy);
	tstdevx = sqrt(tstdevx);
	tstdevy = sqrt(tstdevy);

//calc sterr
	gerrx = gstdevx/sqrt((double)log.size());
	gerry = gstdevy/sqrt((double)log.size());
	terrx = tstdevx/sqrt((double)log.size());
	terry = tstdevy/sqrt((double)log.size());

//scaling calc
	double max,min;
	if(maxx>maxy)max=maxx;
	else max=maxy;
	if(minx<miny)min=minx;
	else min=miny;
	
//write text
	sprintf(buf1,"Standard Deviation X:%2.2lf+-%2.2lf[micron]",gstdevx,gerrx);
	sprintf(buf2,"Standard Deviation Y:%2.2lf+-%2.2lf[micron]",gstdevy,gerry);
	sprintf(buf3,"Standard Deviation X:%2.2lf+-%2.2lf[micron]",tstdevx,terrx);
	sprintf(buf4,"Standard Deviation Y:%2.2lf+-%2.2lf[micron]",tstdevy,terry);

//first canvas
	c1->cd();
	c1->SetGrid();
//	TText *ta = new TText(-5,-5,buf1);
	TText *ta = new TText(0,0,buf1);
	ta->SetTextSize(0.04);
	ta->SetTextColor(1);
//	TText *tb = new TText(-5,-5.3,buf2);
	TText *tb = new TText(0,0,buf2);
	tb->SetTextSize(0.04);
	tb->SetTextColor(1);
	g[0]->SetMaximum(max+0.1);
//	g[0]->SetMinimum(min-0.1);
	g[0]->SetMinimum(-(max+0.1));
//	g[0]->GetXaxis()->SetLimits(min-0.1,max+0.1);
	g[0]->GetXaxis()->SetLimits(-(max+0.1),max+0.1);
	g[0]->SetTitle("TrackConnect:Grain-Grain");
	g[0]->GetXaxis()->SetTitle("dX[#mum]");
	g[0]->GetYaxis()->SetTitle("dY[#mum]");
	g[0]->SetMarkerStyle(20);
	g[0]->SetMarkerSize(0.6);
	g[0]->Draw("AP");
	ta->Draw();
	tb->Draw();
	gPad->Modified();
	
//second canvas
	c2->cd();
	c2->SetGrid();
	TText *tc = new TText(-5,-5,buf3);
	tc->SetTextSize(0.04);
	tc->SetTextColor(1);
	TText *td = new TText(-5,-5.3,buf4);
	td->SetTextSize(0.04);
	td->SetTextColor(1);
	g[1]->SetMaximum(max+0.1);
//	g[1]->SetMinimum(min-0.1);
	g[1]->SetMinimum(-(max+0.1));
//	g[1]->GetXaxis()->SetLimits(min-0.1,max+0.1);
	g[1]->GetXaxis()->SetLimits(-(max+0.1),max+0.1);
	g[1]->SetTitle("TrackConnect:Track-Track");
	g[1]->GetXaxis()->SetTitle("dX[#mum]");
	g[1]->GetYaxis()->SetTitle("dY[#mum]");
	g[1]->SetMarkerStyle(20);
	g[1]->SetMarkerSize(0.6);
	g[1]->Draw("AP");
	tc->Draw();
	td->Draw();
	gPad->Modified();

//TRint runs.
	app.Run();

	return 0;
}
