#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <math.h>
#include <stdlib.h>

#include <TRint.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TH2D.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TColor.h>
#include <TAxis.h>
#include <TPaveStats.h>
#include <TStyle.h>
#include <TLegend.h>

/* make 2016.12.07.inoh */
using namespace std;

void usage(){
	cerr << "Usage : exe RangeFile(plane^2 ,z^2 ,plane^2_error ,z^2_error)" << endl;
	exit(1);
}

int main(int argc, char **argv){
	if(argc<2)usage();

	ifstream ifs;
	int i=0;
	int gomic=1;
	char* gomiv[gomic];
	gomiv[0]=argv[0];
	string str;
	double r,z,re,ze,maxr,maxz;
	maxr=0.;
	maxz=0.;

	TRint app("app", &gomic, gomiv,0,0,kTRUE);
	
	TGraphErrors *g1;
	g1 = new TGraphErrors();

	ifs.open(argv[1]);
	if(!ifs.fail()){
		while(ifs && getline(ifs,str)){
			stringstream ss(str);
			if(!str.empty()){
				ss >> r >> z >> re >> ze;
				if(maxr < r)maxr = r + 10;
				if(maxz < z)maxz = z + 10;
				g1-> SetPoint(i,r,z);
				g1-> SetPointError(i,re,ze);
				i++;
			}
			str = "NULL";
		}//if
	}//while
	else{
		cerr << "read error " << argv[0] << endl;
		exit(1);
	}//else
	ifs.close();

	TCanvas *z1 = new TCanvas("SF_Range_Calclation","z^2 r^2 Graph",0,100,500,500);

	/* Write Graph Area */
	TH2D *h1 = new TH2D("AlphaRangeCalc","SF-AlphaRangeCalc",10,0,maxr, 10,0,maxz);
	h1->SetStats(kFALSE);
	h1->Draw("AXIS");
	
	g1->Draw("P");

	TF1 *f1 = new TF1("f1","[0]*x+[1]");
	g1->Fit(f1);
	//gStyle->SetOptFit(0111);
	//gStyle->SetOptStat("");

	g1->SetMarkerStyle(29);
	g1->SetMarkerSize(1.2);
//	g1->SetTitle("RMS by Range");
//	g1->SetMinimum(0.);
	h1->GetXaxis()->SetTitle("Plane Range squared[#mum^{2}]");
	h1->GetYaxis()->SetTitle("z Range sqared[#mum^{2}]");
	z1->SetGrid();
	gPad->Update();
//	z1->BuildLegend(0.65,0.65,0.9,0.9,"2nd diff");

	Double_t a = f1->GetParameter(0);
	Double_t b = f1->GetParameter(1);

	Double_t sf,range;
	a = sqrt(a*a);
	sf = 1/sqrt(a);
	range = sqrt(b/a);
	char buf[128];
	sprintf(buf,"SF:%1.2lf,Range:%2.2lf[micron]",sf,range);
	cout << buf << endl;

	TText *t = new TText(1100,305,buf);
	t->SetTextSize(0.035);
	t->SetTextColor(1);
	t->Draw();
	gPad->Modified();

//	TPaveStats* stats = (TPaveStats*)h1->FindObject("stats");
/*	stats->SetX1NDC(0.1);
	stats->SetX2NDC(0.3);
	stats->SetY1NDC(0.7);
	stats->SetY2NDC(0.9);
*/

	app.Run();

}//main
