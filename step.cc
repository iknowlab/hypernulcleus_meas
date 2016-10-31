#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <math.h>
#include <stdlib.h>
/*make 2015-09-18.inoh*/
/*repair 2015-10-14.inoh*/
/* 文字化けでコメントが死にました */
/* step毎に区切って2nddiffを求める */
/* 2つのデータが出力される */
/* i,j,size()等などに問題あり注意 */

//typedef 
struct vtx {
	int trkno,plno;//number
	double emlpos,x,y,z;//pos
};//vtx ;

struct vvtx {
	int trkno,plno;//number
	double emlpos,x,y,z;//pos
	double tanx,tany;
};

struct pred {
	double x,y,dx,dy;
	int base;
};

int readerror(){
	std::cerr << "Usage : exe trackfile(.trk) step[mm] option:(filename1 filename2)" << std::endl;
	std::cerr << "filename1 : stepfile" << std::endl;
	std::cerr << "filename2 : RMSfile" << std::endl;
	exit(1);
}

int main(int argc, char **argv){
/*------変数・初期値設定-----------------------*/
	std::vector< vtx > timpo;
	vtx rdat;
	timpo.push_back(rdat);

	std::vector< vvtx > timpoppo;
	vvtx wdat;	
	timpoppo.push_back(wdat);

	std::vector< pred > rms;
	pred ddat;
	int t=0;	
	int i=0;
	int j=0;
	int k=0;
	int m=0;
	int maxj=0;
	int maxk=0;
	int flag=0;
	int trkcount=0;//debug
	double sur[3][4],step,mean,delta;
	mean = 0.0;
	delta = 0.0;
	//rms.dat -> clean mode
	double cleanmax = 1.0e-4;
	double cleanmin = 1.0e-20;

/* ------read data-------------------------- */
	if(argc < 3)readerror();
	std::ifstream data(argv[1]);
	std::string str;
	//read step(micron)
	step = atof(argv[2]);
	
	while(data && getline(data,str)){
		std::stringstream ss(str);
		ss >> timpo[i].trkno >> timpo[i].plno >> timpo[i].emlpos >> timpo[i].x >> timpo[i].y >> timpo[i].z ;
		timpo.push_back(rdat);	i++;
	}//while
	data.close();
	timpo.pop_back();

/* ------------------start calc------------------- */
	i=0;//i12n

	do{//Analized all tracks, but calculate one by one
		trkcount++;//trk#

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

			if(i==timpo.size())break;//EOF
		}while((int)(timpo[i-1].emlpos*10.0)!= 0);//dowhile

/*
		for(int n=0;n<4;n++){
			for(int l=0;l<3;l++){
				printf("sur value %1.6lf\t",sur[l][n]);		
			}//for
			printf("\n");
		}//for
*/
/* --------------main  program----------------------------------------- */
		//zero pos(z' = 0) initialize
		timpoppo[k].x = timpo[maxj].x;
		timpoppo[k].y = timpo[maxj].y;
		timpoppo[k].z = timpo[maxj].z;
		timpoppo[k].trkno = timpo[maxj].trkno;
		timpoppo[k].plno = timpo[maxj].plno;
		timpoppo[k].emlpos = timpo[maxj].emlpos;

/* -------------------calc  step---------------------------------- */
		for(j=maxj;j<i;j++){
			if((timpoppo[k].z + step) <= timpo[j].z){//測定値が記録された値+stepを上回ったとき
				timpoppo.push_back(wdat);
				k++;//i12n

				/*numbering*/
				timpoppo[k].trkno = timpo[j].trkno;
				timpoppo[k].plno = timpo[j].plno;
				timpoppo[k].emlpos = timpo[j].emlpos;//rough

				timpoppo[k].z = timpoppo[k-1].z + step;//plus step

				/* present x(or y) *
				 * ( z- z(j1) )*tan( (x(j2)-x(j1) )/( z(j2)-z(j1) ) */
				/* z:step ,j2:present trigered j ,j1:before "j2" */
				timpoppo[k].x = timpo[j-1].x+(timpoppo[k].z-timpo[j-1].z)*(timpo[j].x - timpo[j-1].x)/(timpo[j].z-timpo[j-1].z);
				timpoppo[k].y = timpo[j-1].y+(timpoppo[k].z-timpo[j-1].z)*(timpo[j].y - timpo[j-1].y)/(timpo[j].z-timpo[j-1].z);

				/* calc an each tangent(step by step) */
				timpoppo[k].tanx = (timpoppo[k].x - timpoppo[k-1].x) / step;
				timpoppo[k].tany = (timpoppo[k].y - timpoppo[k-1].y) / step;

			}//if
		}//for
		timpoppo[k].emlpos = 0.;//(last value)
		
		/* next i12n */
		timpoppo.push_back(wdat);
		k++;

/*-------------------calc 2nd diff and RMS---------------------------*/
		for(t=maxk;t<timpoppo.size();t++){
			rms.push_back(ddat);
			rms[t].base = 0;
			rms[t].x = timpoppo[t].tanx * (2.0*step);
			rms[t].y = timpoppo[t].tany * (2.0*step);
			rms[t].dx = rms[t].x - timpoppo[t+1].x;
			rms[t].dy = rms[t].y - timpoppo[t+1].y;
			if((timpoppo[t].z >= 0.5) && (flag == 0)){//base cut mode
				rms[t-1].base = 1;//without base
				rms[t].base = 1;//without base
				flag = 1;
			}//if		
		}//for

		flag=0;//flag i12n
		maxj=i;//memorize i
		maxk=timpoppo.size();//memorize timpoppo.size

	}while(i<timpo.size());//dowhile
	timpoppo.pop_back();//remove last data

	//check
	std::cout << "calc end" << std::endl;

/*-----start output--------------------------------------*/

//step trkfile
	char stepfile[64] = "step.trk";
	if(argc > 3)sprintf(stepfile,"%s",argv[3]);

	std::ofstream hage;
	hage.open(stepfile, std::ios::out);

	for(i=0;i<t-1;i++){
		hage << timpoppo[i].trkno << " " << timpoppo[i].plno << " " << timpoppo[i].emlpos << " " << timpoppo[i].x << " " << timpoppo[i].y << " " << timpoppo[i].z << std::endl;
	}//for
	hage.close();

//rms.dat
	char rmsfile[64] = "rms.dat";
	if(argc > 4)sprintf(rmsfile,"%s",argv[4]);
	
	std::ofstream bald;
	bald.open(rmsfile, std::ios::out);

	for(i=0;i<rms.size();i++){

		if(rms[i].base == 1)continue;//base cut
	/*CAUTION*/
		if( (rms[i].dx*rms[i].dx) < cleanmin ||(rms[i].dx*rms[i].dx) > cleanmax )continue;//clean mode

		bald << rms[i].dx << std::endl;
		mean +=(rms[i].dx*rms[i].dx);
		m++;

	}//for
	bald.close();
	mean=sqrt(mean/(double)m);
	std::cout << "RMS(X') is " << mean << "\nNmDat are " << m << std::endl;

	return 0;
}//main
