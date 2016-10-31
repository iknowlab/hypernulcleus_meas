#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <math.h>
#include <stdlib.h>
/*下面から飛跡をとらないとエラー吐きますミスったらtocコマンドでごまかして*/
/*6列構成の.vtx(.trk)ファイルを読み込みます*/
/*tr# pl# emlpos x y zを読み込んでxz,yz Graphを作ります*/

void trk2d(void){
	std::cerr << "Usage : E07,373trackfile(.vtx .trk)" << std::endl;
	std::cerr << "Usage : trk2d [option] *.trk (is not \"trk2d.C2\")" << std::endl;
	exit(1);
}

void trk2d(char const *filename, char const *filetype,
char const *epsname,	int titleflag = 0,
char const *titletext){

/*-------10% aborting system---------*/	
	TRandom3 abortcheck;
	abortcheck.SetSeed((unsigned)time(NULL));
	if(abortcheck.Uniform()>0.9){
	printf("アボートしました？（コアダンプ）\n");
	abort();
	}//if

/*main*/
	//ファイル名読み込み
	std::ifstream data(filename);
	std::string str;
	//各種定義
	int trkno,plno,i;
	char trname[64];
	double lastemlpos=-3.0;//初期値、一枚目の乾板の始点をどこでも許容する
	double emlpos,x,y,z,dz;

	//2つのキャンバス指定	
	TCanvas *z1 = new TCanvas("z1","z1",0,100,600,400);
	TCanvas *z2 = new TCanvas("z2","z2",600,100,600,400);

	//Trkの数(plate別)とキャンバス(グラフ)の数
	TGraph *g[100][2];//pointer
	TMultiGraph *mg1 = new TMultiGraph();
	TMultiGraph *mg2 = new TMultiGraph();
	char trnamex[64],trnamey[64];

	//Colour Value Settings
	const Int_t NRGBs = 5;
	const Int_t NCont = 255;
	Int_t MyPalette[NCont];
	Double_t red[NRGBs] = {0.0,0.0,0.87,1.0,1.0};
	Double_t green[NRGBs] = {0.0,0.81,1.0,0.20,0.0};
	Double_t blue[NRGBs] = {0.51,1.00,0.12,0.00,0.00};
	Double_t stop[NRGBs] = {0.0,0.34,0.61,0.84,1.00};
	Int_t FI = TColor::CreateGradientColorTable(NRGBs,stop,red,green,blue,NCont);
	for(int i=0;i<NCont;++i){ MyPalette[i] = FI+i; }

	//初期値
	i=0;
	int counter=0;
	
	//読み込み開始
	while(getline(data,str)){
		stringstream ss(str);
	/* check filetype */
		if(filetype=="stp"){
			ss >> trname >> z >> x >> y >> dz >> emlpos;
		}
		else{
			ss >> trkno >> plno >> emlpos >> x >> y >> z;
		}
		//異なるプレートや異なる飛跡に移動した際の基準をプレート位置(0.0~3.0)の差で判定
		if(TMath::Abs(lastemlpos-emlpos)> 2.5 && filetype!="stp"){
			i=0;//initialization
			g[counter][0] = new TGraph();//initialization
			g[counter][1] = new TGraph();//initialization
			counter++;//new track
		}//if
		if(filetype=="stp"){
			if(i==0){
				g[0][0] = new TGraph();
				g[0][1] = new TGraph();
				counter++;//new event
			}
			i++;
			g[0][0]-> SetPoint(i,x,z);
			g[0][1]-> SetPoint(i,y,z);
		}
		else{
			g[counter-1][0]-> SetPoint(i,x,z);//g[][]にx,zを入れる、値に順序iを与える
			g[counter-1][1]-> SetPoint(i,y,z);
			lastemlpos=emlpos;//プレート位置保持
			++i;//次の行番号作成
		}
	}//while

	//飛跡別のグラフデータをマルチグラフに挿入
	for(i=0;i<counter;i++){

		g[i][0]->SetMarkerStyle(3);
		g[i][0]->SetMarkerColor(MyPalette[i*NCont/(counter+1)]);
		sprintf(trnamex,"tr#_%d",i+1);
		g[i][0]->SetTitle(trnamex);
		mg1 -> Add(g[i][0]);

		g[i][1]->SetMarkerStyle(3);
		g[i][1]->SetMarkerColor(MyPalette[i*NCont/(counter+1)]);
		sprintf(trnamey,"tr#_%d",i+1);
		g[i][1]->SetTitle(trnamey);
		mg2 -> Add(g[i][1]);

	}

	if(titleflag != 0){
		z1->cd();
		mg1->SetTitle(titletext);
		z2->cd();
		mg2->SetTitle(titletext);
	}

	//アッラーフアクバル!!
	z1->cd();
	mg1->Draw("APL");
	if(filetype!="stp")mg1->GetXaxis()->SetTitle("X[mm]");
	if(filetype!="stp")mg1->GetYaxis()->SetTitle("Z[mm]");
	gPad->Modified();
	z1->BuildLegend(0.1,0.75,0.25,0.9);

	//allahu acbar!!
	z2->cd();
	mg2->Draw("APL");
	if(filetype!="stp")mg2->GetXaxis()->SetTitle("Y[mm]");
	if(filetype!="stp")mg2->GetYaxis()->SetTitle("Z[mm]");
	gPad->Modified();
	z2->BuildLegend(0.1,0.75,0.25,0.9);

	if(epsname && strlen(epsname)>0){
		char epsnamex[64];sprintf(epsnamex,"X_%s",epsname);
		char epsnamey[64];sprintf(epsnamey,"Y_%s",epsname);
		z1->cd();
		z1->SaveAs(epsnamex);
		z2->cd();
		z2->SaveAs(epsnamey);
	}

}//void
