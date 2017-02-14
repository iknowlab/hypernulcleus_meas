#include <TCanvas.h>
#include <TRint.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TColor.h>
#include <TAxis.h>
#include <TLegend.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <math.h>
#include <stdlib.h>

/* make inoh 2017.2.14 */
/*下面から飛跡をとらないとエラー吐きますミスったらtocコマンドでごまかして*/
/*6列構成の.vtx(.trk)ファイルを読み込みます*/
/*tr# pl# emlpos x y zを読み込んでxz,yz Graphを作ります*/

void howto(char *filename){
	std::cerr << "View Track Position" << std::endl;
	std::cerr << "Usage : "<< filename <<" *.trk " << std::endl;
	exit(1);
}

int main(int argc, char* argv[]){
	if(argc<2)howto(argv[0]);

	TRint app("app",0,0,0,0,kTRUE);

/*
void trk2d(char const *filename, char const *filetype,
char const *epsname,	int titleflag = 0,
char const *titletext){
*/

	//各種定義
	int trkno,plno,i;
	char trname[64];
	double lastemlpos=-3.0;//初期値、一枚目の乾板の始点をどこでも許容する
	double emlpos,x,y,z,dz;
	double max_x=0.0;
	double max_y=0.0;
	std::string str;

	//2つのキャンバス指定	
	TCanvas *z1 = new TCanvas("bf_rm_dist","bf_rm_dist",0,100,600,600);
	TCanvas *z2 = new TCanvas("af_rm_dist","af_rm_dist",610,100,600,600);

	//Trkの数(plate別)とキャンバス(グラフ)の数
	TGraph *g[100][2];//pointer
	TMultiGraph *mg1 = new TMultiGraph();
	TMultiGraph *mg2 = new TMultiGraph();
	char trnamex[64],trnamey[64];

	//Colour Value Settings
	const Int_t NRGBs = 4;
	const Int_t NCont = 50;
	Int_t MyPalette[NCont];
	Double_t red[NRGBs] =	{1.00,0.50,0.00,0.00};
	Double_t green[NRGBs] = {0.00,0.00,0.60,1.00};
	Double_t blue[NRGBs] =	{0.00,1.00,1.00,0.00};
	Double_t stop[NRGBs] =	{0.00,0.33,0.64,1.00};
	Int_t FI = TColor::CreateGradientColorTable(NRGBs,stop,red,green,blue,NCont);
	for(int k=0;k<NCont;++k){ MyPalette[k] = FI+k; }

	//初期値
	i=0;
	int counter=0;
	/* separator */
	int file_sep[argc];
	//読み込み開始
	for(int j=1;j<argc;j++){
	std::ifstream data(argv[j]);

		while(getline(data,str)){
			std::stringstream ss(str);
			ss >> trkno >> plno >> emlpos >> x >> y >> z ;//>> dz >> emlpos;
			x *= 1000.0;
			y *= 1000.0;
			//異なるプレートや異なる飛跡に移動した際の基準をプレート位置(0.0~3.0)の差で判定
			if(TMath::Abs(lastemlpos-emlpos)> 2.5){
				i=0;//initialization
				g[counter][0] = new TGraph();//initialization
				g[counter][1] = new TGraph();//initialization
				counter++;//new track
			}//if
		
			if(x * x > max_x * max_x)max_x = sqrt(x * x);//AbsMax
			if(y * y > max_y * max_y)max_y = sqrt(y * y);//AbsMax

			g[counter-1][0]-> SetPoint(i,x,z);//g[][]にx,zを入れる、値に順序iを与える
			g[counter-1][1]-> SetPoint(i,y,z);
			lastemlpos=emlpos;//プレート位置保持
			++i;//次の行番号作成
			
		}//while
		file_sep[j]=counter;

	}//for

	//飛跡別のグラフデータをマルチグラフに挿入
	for(i=0;i<file_sep[1];i++){
		g[i][0]->SetMarkerStyle(3);
		g[i][0]->SetMarkerColor(MyPalette[i*NCont/(file_sep[1]+1)]);
		sprintf(trnamex,"tr#_%d",i+1);
		g[i][0]->SetTitle(trnamex);
		mg1 -> Add(g[i][0]);
	}//for

	for(i=file_sep[1];i<file_sep[2];i++){
		g[i][0]->SetMarkerStyle(3);
		g[i][0]->SetMarkerColor(MyPalette[(i-file_sep[1])*NCont/(file_sep[1]+1)]);
		sprintf(trnamey,"tr#_%d",i+1-file_sep[1]);
		g[i][0]->SetTitle(trnamey);
		mg2 -> Add(g[i][0]);
	}

	z1->cd();
	mg1->SetTitle("before remove distortion");
	mg1->Draw("APL");
	mg1->GetXaxis()->SetTitle("X[#mum]");
	mg1->GetYaxis()->SetTitle("Z[mm]");
	mg1->GetXaxis()->SetLimits(-(max_x+0.1),max_x+0.1);
	gPad->Modified();
	z1->BuildLegend(0.88,0.55,0.99,0.95);

	z2->cd();
	mg2->SetTitle("after remove distortion");
	mg2->Draw("APL");
	mg2->GetXaxis()->SetTitle("X[#mum]");
	mg2->GetYaxis()->SetTitle("Z[mm]");
	mg2->GetXaxis()->SetLimits(-(max_x+0.1),max_x+0.1);
	gPad->Modified();
	z2->BuildLegend(0.88,0.55,0.99,0.95);

	app.Run();

	return 0;

}//main
