#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <math.h>
#include <stdlib.h>
/*make 2015-09-14.inoh*/
/*update 2015-10-15.inoh*/
/*始点と終点のxy座標を0にする(回転系の補正)*/
/*球面収差の誤差をy‘に与えることでx’の測定誤差を減らす*/
/*飛跡は下面から測定すること*/

struct vtx {
	int trkno,plno;//number
	double emlpos,x,y,z,sz;//pos
};

struct nvtx {
	double x,y1,y2,sz;
};

int readerror(){
	std::cerr << "Usage : exe beamtrack(.trk) [output_filename]" << std::endl;
	exit(1);
}

int main(int argc, char **argv){
	//各種定義
	std::vector< vtx > timpo;//読み込む.trk
	vtx vdat;
	std::vector< nvtx > timpoppo;//変換後のデータ.trk
	nvtx ndat;
	int i=0;
	int j=0;
	int maxj;maxj=0;
	int trkcount;trkcount=0;
	double sur[3][4],ThickEMu,ThickEMd,SFu,SFd,BaseValue,phai,theta;

	timpo.push_back(vdat);
	timpoppo.push_back(ndat);

	//ファイル名読み込み
	if(argc == 1)readerror();
	std::ifstream data(argv[1]);
	std::string str;
	
	//読み込み開始
	while(data && getline(data,str)){
		std::stringstream ss(str);
		ss >> timpo[i].trkno >> timpo[i].plno >> timpo[i].emlpos >> timpo[i].x >> timpo[i].y >> timpo[i].z ;
		timpo[i].sz=0.;
		timpo.push_back(vdat);	i++;
		timpoppo.push_back(ndat);
	}//while
	data.close();
	timpo.pop_back();	i--;
	timpoppo.pop_back();

	//処理開始
	i=0;//i12n
	do{
		int flagship=0;
		trkcount++;//trk#
		/*4POS用の値で初期値を設定します*/
		for(int n=0;n<4;n++)sur[2][n]=1000000.0;//

		do{
			//get4POS[(0,1,2:x,y,z)0upsurface,1upbase,2downbase,3downsurface]
			if(( (int)(timpo[i].emlpos*10.0) )==30){
				sur[0][3]=timpo[i].x;sur[1][3]=timpo[i].y;sur[2][3]=timpo[i].z;}
			if(( (int)(timpo[i].emlpos*10.0) )==20){
				sur[0][2]=timpo[i].x;sur[1][2]=timpo[i].y;sur[2][2]=timpo[i].z;}	
			if(( (int)(timpo[i].emlpos*10.0) )==10){
				sur[0][1]=timpo[i].x;sur[1][1]=timpo[i].y;sur[2][1]=timpo[i].z;}
			if(( (int)(timpo[i].emlpos*10.0) )== 0){
				sur[0][0]=timpo[i].x;sur[1][0]=timpo[i].y;sur[2][0]=timpo[i].z;}
			i++;
		}while((int)(timpo[i-1].emlpos*10.0)!= 0);//dowhile

		/* 乾板の4POS(only z axis)が読み取れていない飛跡を無視します */
		for(int n=0;n<4;n++)if(sur[2][n]>999999.0){maxj=i;flagship=1;}
		if(flagship==1)continue;

		//While文で進んだ行だけ解析します(さらにWhileが続くので飛跡毎の解析ができる)
/*
		for(int k=0;k<4;k++){
			for(int l=0;l<3;l++){
				printf("sur value %3.2lf\t",sur[l][k]);		
			}//for
			printf("\n");
		}//for
*/
		/* 上下Gelの厚さ */
		ThickEMu = sqrt((sur[2][0]-sur[2][1])*(sur[2][0]-sur[2][1]));
		ThickEMd = sqrt((sur[2][2]-sur[2][3])*(sur[2][2]-sur[2][3]));

		/* Shurinkage Factor & Base Thickness */
		SFu = 0.5/ThickEMu;
		SFd = 0.5/ThickEMd;
		BaseValue = sqrt((sur[2][1]-sur[2][2])*(sur[2][1]-sur[2][2]));
		
		for(j=maxj;j<i;j++){
			//SF補正後のzの値を出す
			if(timpo[j].z >= sur[2][3] && timpo[j].z <= sur[2][2])	// if in the DownGel
				timpo[j].sz = (timpo[j].z-sur[2][3])*SFd;
			else if(timpo[j].z >= sur[2][1] && timpo[j].z <=sur[2][0])	// if in the UpGel
				timpo[j].sz = 0.5 + BaseValue + (timpo[j].z-sur[2][1])*SFu;

			//SF補正後のzに表面を変換
			if(( (int)(timpo[j].emlpos*10.0) )==30){
				sur[2][3]=timpo[j].sz;}
			if(( (int)(timpo[j].emlpos*10.0) )==20){
				sur[2][2]=timpo[j].sz;}	
			if(( (int)(timpo[j].emlpos*10.0) )==10){
				sur[2][1]=timpo[j].sz;}
			if(( (int)(timpo[j].emlpos*10.0) )== 0){
				sur[2][0]=timpo[j].sz;}

		}//for

		/* Track Angle1 (phai)*/
		phai = atan((sur[0][0]-sur[0][3])/(sur[1][0]-sur[1][3]));//x/y
		
		/* 回転補正をかけます(始点をゼロ点に処理) */
		for(j=maxj;j<i;j++){
			//phai compensation	
			timpoppo[j].x = (timpo[j].x-timpo[maxj].x)*cos(phai) - 
						(timpo[j].y-timpo[maxj].y)*sin(phai);
			timpoppo[j].y1 = (timpo[j].x-timpo[maxj].x)*sin(phai) + 
					(timpo[j].y-timpo[maxj].y)*cos(phai);
			
			//save Surface-Y'
			if(( (int)(timpo[j].emlpos*10.0) )==30){
				sur[1][3]=timpoppo[j].y1;}
			if(( (int)(timpo[j].emlpos*10.0) )== 0){
				sur[1][0]=timpoppo[j].y1;}

		}//for

		/* Track Angle1 (theta)*/
		theta = atan((sur[1][0]-sur[1][3])/(sur[2][0]-sur[2][3]));//y'/z

		for(j=maxj;j<i;j++){
			//theta compensation
			timpoppo[j].y2 = (timpoppo[j].y1 - timpoppo[maxj].y1)*cos(theta) - 
						(timpo[j].sz - timpo[maxj].sz)*sin(theta);
			timpoppo[j].sz = (timpoppo[j].y1 - timpoppo[maxj].y1)*sin(theta) + 
						(timpo[j].sz - timpo[maxj].sz)*cos(theta);
		}//for
		
		//output
		std::cout << "phai=" << phai << "\n" << "theta=" << theta << std::endl;

		maxj=i;//次の先頭行を記録

// output
		std::cout << "calc...tr# " << timpo[i-1].trkno << " pl# " << timpo[i-1].plno << " last eml.pos: " << timpo[i-1].emlpos << std::endl;

	}while(i<timpo.size());//dowhile

	//ファイル名読み込み
	char filename[64] = "zerobeam.trk";
	if(argc == 3){
		sprintf(filename,"%s",argv[2]);
	}//if
	std::ofstream pomf_file;
	pomf_file.open(filename, std::ios::out);

	//書き出し開始
	i=0;
	while(i<timpoppo.size()){

		//output
		pomf_file << timpo[i].trkno<< " " << timpo[i].plno << " " << timpo[i].emlpos << " " << timpoppo[i].x << " " << timpoppo[i].y2 << " " << timpoppo[i].sz << std::endl;

	i++;
	}//while

	return 0;
}//main
