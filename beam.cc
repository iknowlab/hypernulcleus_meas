#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <algorithm>
/*make 2015-10-04.inoh*/
/*.trkデータ(beam)から歪みを取り出し自身のbeamデータに補正します*/
/*vtx2dで飛跡のデータに問題がないことをチェックしてください*/
/*ベース面の連続測定に対応*/
/*飛跡は下面から測定すること*/
/*meas. = measurement*/

struct vtx {
	int trkno,plno;//number
	double emlpos,x,y,z,sz,dx,dy;//pos
};

struct nvtx {
	double x,y,z,dx,dy;//pos
};

struct mvtx {
	int trkno,plno;//number
	double emlpos,x,y,z,sz;//pos
};

int readerror(){
	std::cerr << "Usage : exe beamtrack(.trk) [output_filename]" << std::endl;
	std::cerr << "HELP : exe --help" << std::endl;
	std::cerr << "read error!" <<std::endl;
	exit(1);
}

int trkerror(){

	std::cerr << "ベース点測定を複数回行うとベース座標は平均値を使います" << std::endl;
	std::cerr << ".trkファイルのビームから歪みを取り出し、" <<std::endl;
	std::cerr << "得られた複数ビームの歪みの平均値でビームを直線に補正します" << std::endl;
	std::cerr << "mean_dis.trk,beam.trkはvtx2dに対応しています" << std::endl;

	exit(1);
}

int main(int argc, char **argv){
	//各種定義
	std::vector< vtx > timpo;//読み込む.trk
	std::vector< vtx >::iterator timtim;
	vtx vdat;

	std::vector< nvtx > timpoppo;//歪み補正後の.trkをぶっこむ
	nvtx newdat;

	std::vector< mvtx > mean;//歪み値
	mvtx mdat;

	int i=0;
	int j=0;
	int n=0;
	int maxj;maxj=0;
	int trkcount;trkcount=0;
	double ThickEMu,ThickEMd,Bx0,By0,SFu,SFd,BaseValue,dxdz,dydz;

	timpo.push_back(vdat);
	timpoppo.push_back(newdat);

	//歪みの値を1002個にする-＞更新の際は注意
	for(j=0;j<=1002;j++){
	mean.push_back(mdat);
	mean[j].x=0.;mean[j].y=0.;mean[j].z=0.;
	}//for
//	std::cout << mean.size() << std::endl;

	/*ファイル名読み込み*/
	if(argc == 1)readerror();
	if(argc == 2 && 0==strcmp(argv[1],"--help"))trkerror();//help mode
//	printf("%s",argv[1]);
	std::ifstream data(argv[1]);
	std::string str;

	//読み込み開始
	while(data && getline(data,str)){
		std::stringstream ss(str);
		ss >> timpo[i].trkno >> timpo[i].plno >> timpo[i].emlpos >> timpo[i].x >> timpo[i].y >> timpo[i].z ;
		timpo[i].sz=0.;
		timpo[i].dx=0.;
		timpo[i].dy=0.;
		timpo.push_back(vdat);	i++;
	}//while
	data.close();
	timpo.pop_back();	i--;

// test output
//	std::cout << "test: " << timpo[i].trkno<< " " << timpo[i].plno << " " << timpo[i].emlpos << " " << timpo[i].x << " " << timpo[i].y << " " << timpo[i].z << std::endl;

	//処理開始
	i=0;//i12n

	do{
	//i12n
		double sur[3][4]={0.};
		int count[2]={0};
	//calc by trk
		trkcount++;
		do{
			//get4POS[(0,1,2:x,y,z)0upsurface,1upbase,2downbase,3downsurface]
			if(( (int)(timpo[i].emlpos*10.0) )==30){
				sur[0][3]=timpo[i].x;sur[1][3]=timpo[i].y;sur[2][3]=timpo[i].z;}
			
			if(( (int)(timpo[i].emlpos*10.0) )==20){
				sur[0][2]+=timpo[i].x;
				sur[1][2]+=timpo[i].y;
				sur[2][2]+=timpo[i].z;
				count[1]++;//base meas. time
			}
			
			if(( (int)(timpo[i].emlpos*10.0) )==10){
				sur[0][1]+=timpo[i].x;
				sur[1][1]+=timpo[i].y;
				sur[2][1]+=timpo[i].z;
				count[0]++;//base meas. time
			}
			
			if(( (int)(timpo[i].emlpos*10.0) )== 0){
				sur[0][0]=timpo[i].x;sur[1][0]=timpo[i].y;sur[2][0]=timpo[i].z;}
			
			i++;

/*以下の条件式では乾板の位置情報が0になると飛跡を区切ります*/
/*ベース角度の再測定を行う場合は位置情報"0"に注意してください*/
		}while((int)(timpo[i-1].emlpos*10.0)!= 0);//dowhile

#ifdef TALK
	std::cout<<"Uuuuuuuunkooooooooo!!"<<std::endl;
#endif

		/*ベース座標の平均*/
		for(int k=0;k<3;k++){
			sur[k][2]=sur[k][2]/((double)count[1]);
			sur[k][1]=sur[k][1]/((double)count[0]);
//			std::cout << sur[k][2] << ":" << sur[k][1] << std::endl;
		}//for

		/* 上下Gelの厚さ */
		ThickEMu = sqrt((sur[2][0]-sur[2][1])*(sur[2][0]-sur[2][1]));
		ThickEMd = sqrt((sur[2][2]-sur[2][3])*(sur[2][2]-sur[2][3]));

		/* Shurinkage Factor & Base Thickness */
		SFu = 0.5/ThickEMu;//(本来の厚さ[mm]/測定厚)
		SFd = 0.5/ThickEMd;
		BaseValue = sqrt((sur[2][1]-sur[2][2])*(sur[2][1]-sur[2][2]));
		/* Fix BsVal at run2 */
		BaseValue = 0.040;
		
		/* Base track初期値(ベース:下xy座標) */
		Bx0 = sur[0][2];
		By0 = sur[1][2];
		
		/* Base Angle */
		dxdz = (sur[0][1]-sur[0][2])/BaseValue;
		dydz = (sur[1][1]-sur[1][2])/BaseValue;

		/*4POSをそれぞれ1点ずつに絞るためにフラッグを立てる*/
		int surfin[2]={0};//トリガーかけます
		for(j=maxj;j<i;j++){
			if((int)(timpo[j].emlpos*10)==10){
				if(surfin[0]>=1)timpo[j].plno=-100;//impossible number
				timpo[j].z=sur[2][1];
				timpo[j].y=sur[1][1];
				timpo[j].x=sur[0][1];
				surfin[0]++;
			}//if

			if((int)(timpo[j].emlpos*10)==20){
				if(surfin[1]>=1)timpo[j].plno=-100;//impossible number
				timpo[j].z=sur[2][2];
				timpo[j].y=sur[1][2];
				timpo[j].x=sur[0][2];
				surfin[1]++;
			}//if
		}//for


		/*erase mode (byebye timpo)*/
		for(timtim=timpo.begin()+maxj;timtim!=timpo.end();timtim++){/* timtimはvector<vtx>のイテレーター:初期値を代入し最終値までを調べる */
			vtx timtimtim=*timtim;/* vtxのtimtimのポインターtimtimtim */
/*			if(timpo[j].trkno==-100){*/
			if(timtimtim.plno==-100){/* timpo[j]=timtimtimみたいなもん */
				timpo.erase(timtim);
				timtim--;
			}//if
		}//for

		i-=(surfin[0]+surfin[1]-2);/* 表面の測定値を1つずつにします */

/*While文で進んだ行
 * だけ解析します(さらにWhileが続くので飛跡毎の解析ができる)*/

		 for(j=maxj;j<i;j++){
			 if(timpo[j].z >= sur[2][3] && timpo[j].z <= sur[2][2])	// if in the DownGel
				 timpo[j].sz = (timpo[j].z-sur[2][3])*SFd;
				 else if(timpo[j].z >= sur[2][1] && timpo[j].z <=sur[2][0])  // if in the UpGel
					 timpo[j].sz = 0.5 + BaseValue + (timpo[j].z-sur[2][1])*SFu;
		 }//for

			/* distortionを求める。 */
		for(j=maxj;j<i;j++){
			timpo[j].dx = timpo[j].x - (dxdz*(timpo[j].sz-timpo[maxj].sz)+Bx0);
			timpo[j].dy = timpo[j].y - (dydz*(timpo[j].sz-timpo[maxj].sz)+By0);
//timtim->dx = timtimtim.x - (dxdz*(timtimtim.sz-timtimtim.sz)+Bx0);
//timtim->dy = timtimtim.y - (dydz*(timtimtim.sz-timtimtim.sz)+By0);
		}//for

//疲れたので有賀さんのやつをコピペして調整2015/09/10.inoh
		/* ループ終了値(最終行#は含まない) */
		int line = j;
		
		/* 各種値を初期化 */
		int lay=0;
		double zz=0.;		
		int Zset;		//最下流面の位置

		for(j=maxj; j<line; j++){//下流から上流まで
			while(lay<=1002){
				if((zz-timpo[j].sz)*(zz-timpo[j].sz) < (timpo[j].sz-timpo[j+1].sz)*(timpo[j].sz-timpo[j+1].sz)){//細かなz値の値と実測SF補正後のz値の差が実SFzと次の実SFz差よりも小さければ
					mean[lay].x += timpo[j].dx + (timpo[j+1].dx-timpo[j].dx)*(zz-timpo[j].sz)/(timpo[j+1].sz-timpo[j].sz);
					mean[lay].y += timpo[j].dy + (timpo[j+1].dy-timpo[j].dy)*(zz-timpo[j].sz)/(timpo[j+1].sz-timpo[j].sz);
					mean[lay].z = zz;//そのまま

					mean[lay].plno = timpo[line].plno;//plnoを一応保持
				}//if
				else break;

		/* 出力Z座標が SF補正後のZ座標よりも下流側にいる間 */
//		while(((int) (zz*1000.)) <= ((int) (timpo[line].sz*1000.))){

				/* 出力位置の歪みを求める */
				/* Z座標をum単位に変換 */
				Zset = ((int) (zz*1000.));

				/* 出力行数を加算 */
				lay++;
		
				/* Z座標に1um足す */
				if(Zset == 500){
					zz += BaseValue;
				}//if
				else{
					zz += 0.001;
				}//else
			}//while
		}//for

//output
	std::cout << "calc...tr# " << timpo[i-1].trkno<< " pl# " << timpo[i-1].plno << " UpShrink: " << SFu << "DownShrink" << SFd << std::endl;

	maxj=i;//次の行の先頭を記録

	}while(i<timpo.size());//dowhile
	
	j=0;
	//ファイル名読み込み
	char filen[64] = "mean_dis.trk";
	std::ofstream pom;
	pom.open(filen, std::ios::out);

	/* 歪みの平均を算出 */
	while(j<mean.size()){
		/* コメント：測定したBeam trackが複数であれば trkcount > 1となる */
		mean[j].x = mean[j].x/((double) trkcount);
		mean[j].y = mean[j].y/((double) trkcount);

// test output
//	std::cout << mean.size() << " -test- " << mean[j].plno << " " << mean[j].x << " " << mean[j].y << " " << mean[j].z << std::endl;

		//Output
		if(j == 0){
			pom << "0" << " " << mean[j].plno << " " << "3.0" << " " << mean[j].x << " " << mean[j].y << " " << mean[j].z << std::endl;}
		else if(j > 0 && j < 500){
			pom << "0" << " " << mean[j].plno << " " << "2.5" << " " << mean[j].x << " " << mean[j].y << " " << mean[j].z << std::endl;}
		else if(j == 500){
			pom << "0" << " " << mean[j].plno << " " << "2.0" << " " << mean[j].x << " " << mean[j].y << " " << mean[j].z << std::endl;}
		else if(j == 501){
			pom << "0" << " " << mean[j].plno << " " << "1.0" << " " << mean[j].x << " " << mean[j].y << " " << mean[j].z << std::endl;}
		else if(j > 501 && j < 1002){
			pom << "0" << " " << mean[j].plno << " " << "0.5" << " " << mean[j].x << " " << mean[j].y << " " << mean[j].z << std::endl;}
		else if(j == 1002){
			pom << "0" << " " << mean[j].plno << " " << "0.0" << " " << mean[j].x << " " << mean[j].y << " " << mean[j].z << std::endl;}

		//Base Valueを無くします。これは、実際の飛跡のベース値を0として扱うためです。
		if(j >= 501)mean[j].z -= BaseValue;	//if
		
		j++;
	}//while

	//timpoのszを統一(0.000~1.000mm)
	for(j=0;j<=timpo.size();j++){
		if((timpo[j].emlpos>2.9)&&(timpo[j].emlpos<3.1))timpo[j].sz=0.000;
//		else if((timpo[j].emlpos>2.4)&&(timpo[j].emlpos<2.6))timpo[j].sz=timpo[j].sz;
		else if((timpo[j].emlpos>1.9)&&(timpo[j].emlpos<2.1))timpo[j].sz=0.500;
		else if((timpo[j].emlpos>0.9)&&(timpo[j].emlpos<1.1))timpo[j].sz=0.500;
		else if((timpo[j].emlpos>0.4)&&(timpo[j].emlpos<0.6))timpo[j].sz=timpo[j].sz-BaseValue;
		else if((timpo[j].emlpos>-0.1)&&(timpo[j].emlpos<0.1))timpo[j].sz=1.000;
	}//for

	//歪み補正します
	i=0;
	while(i<=timpo.size()){
		j=0;
		while(j<mean.size()-1){
			if( (timpo[i].sz>=(mean[j].z-0.0005)) && (timpo[i].sz<(mean[j+1].z+0.0005)) ){
				/* trackと同じZ座標での歪みを求める(折れ線近似) */
				timpoppo[i].dx = mean[j].x + (mean[j+1].x-mean[j].x)*(timpo[i].sz-mean[j].z)/(mean[j+1].z-mean[j].z);
				timpoppo[i].dy = mean[j].y + (mean[j+1].y-mean[j].y)*(timpo[i].sz-mean[j].z)/(mean[j+1].z-mean[j].z);		
				/* 歪みを補正 */
				timpoppo[i].x = timpo[i].x - timpoppo[i].dx;
				timpoppo[i].y = timpo[i].y - timpoppo[i].dy;
			}//if
			j++;
		}//while
		timpoppo.push_back(newdat);
		i++;
	}//while

	//ファイル名読み込み
	char filename[64] = "beam.trk";
	if(argc == 3){
	sprintf(filename,"%s",argv[2]);
//	printf("test:%s",argv[2]);
	}//if
	std::ofstream pomf_file;
	pomf_file.open(filename, std::ios::out);

	//書き出し開始
	i=0;
	while(i<timpoppo.size()-2){
		//output
//		pomf_file << timpo[i].trkno << std::endl;
		pomf_file << timpo[i].trkno<< " " << timpo[i].plno << " " << timpo[i].emlpos << " " << timpoppo[i].x << " " << timpoppo[i].y << " " << timpo[i].sz << std::endl;
	i++;
	}//while

	return 0;
}//main
