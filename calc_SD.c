/* 2016.6.7.inoh remake */
/* calc theta_plane */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

/* 構造体SecondDifference : 位置の2次変位 */
struct SecondDifference{
	double x;
	double y;
	double z;
	/* calc theta */
	double theta;
};

int main(int argc, char *para[])
{
	int loop;	/* ループ変数 */
	int Global;	/* １飛跡中の2次変位の数 */
	int Inner;	/* plate接続部分を含まない2次変位の数 */
	int Pnum;	/* 粒子数 */
	int pljoint[3];	/* plate接続部分 */
	int sel;	/* ユーザ指定の番号 */
	double R[3];	/* Range */
	double x[3];	/* X'座標 */
	double y[3];	/* Y'座標 */
	double z[3];	/* Z'座標 */
	double Xpre;	/* 予測X'座標*/
	double Ypre;	/* 予測Y'座標*/
	double Zpre;	/* 予測Z'座標*/
	char linebuffer[2048];	/* 読み取り行格納 */
	char ID[512];	/* particle ID */
	struct SecondDifference Diff;	/* 2次変位 */
	struct SecondDifference Dave;	/* 2次変位の１飛跡分の平均値 */
	struct SecondDifference Drms;	/* １飛跡から得られる2次変位のRMS値 */
	FILE *read, *smp, *log;	/* Fileポインタ */
	
	/* 表示する内容の設定 */
	printf("Select a number from below :\n");
	printf("\t1) Display Average of second differences\n");
	printf("\t2) Display RMS value of second differences\n");
	printf("The number :  ");
	scanf("%d", &sel);
	
	/* 入力数値の確認 */
	if(sel < 1 || sel > 2){
		printf("Please input 1 or 2!!\n");
		exit(1);
	}
	
	/* 初期クリア */
	for(loop=0;loop<3;loop++)
		pljoint[loop] = 5;
	
	/* 読み込みFileを開く */
	if ((read = fopen("dataTR.stprr", "r")) == NULL) {
		printf("input_file[dataTR.stprr] not open\n");
		exit(1);
	}
	
	/* Sample file(出力Fileとして)を作成 */
	if ((smp = fopen("smpl_tr.dlt", "w")) == NULL) {
		printf("output_file[sample_tr.dlt] not open\n");
		exit(1);
	}
	
	/* Log fileを作成 */
	if ((log = fopen("delta_v1.log", "w")) == NULL) {
		printf("output_file[delta_v1.log] not open\n");
		exit(1);
	}
	
	/* 粒子数を初期化 */
	Pnum = 0;
	
	/* 読み込み開始 */
	while(fgets(linebuffer,sizeof(linebuffer),read) != NULL){
		
		/* 初期化 */
		Global = 0;
		Inner = 0;
		Dave.x = 0;
		Dave.y = 0;
		Drms.x = 0;
		Drms.y = 0;
		
		/* 1点目を取得 */
		sscanf(linebuffer," %s %lf %lf %lf %lf %d",ID,&R[0],&x[0],&y[0],&z[0],&pljoint[0]);
		
		/* 2点目を取得 */
		fgets(linebuffer,sizeof(linebuffer),read);
		sscanf(linebuffer," %s %lf %lf %lf %lf %d",ID,&R[1],&x[1],&y[1],&z[1],&pljoint[1]);
		
		/* しきい線まで（１飛跡分）取得していく */
		while(1){
			fgets(linebuffer,sizeof(linebuffer),read);
			
			/* しきい線であれば次の粒子の処理へ */
			if(strncmp(linebuffer,"--------------------------------------------------",40) == 0)
				break;
			
			/* 3点目を取得 */
			sscanf(linebuffer," %s %lf %lf %lf %lf %d",ID,&R[2],&x[2],&y[2],&z[2],&pljoint[2]);
			
			/* 予測X'座標を算出 */
			Xpre = x[1] + (x[1]-x[0])*(R[2]-R[1])/(R[1]-R[0]);
			
			/* X'成分の2次変位 */
			Diff.x = Xpre - x[2];
			
			/* 予測Y'座標を算出 */
			Ypre = y[1] + (y[1]-y[0])*(R[2]-R[1])/(R[1]-R[0]);
			
			/* Y'成分の2次変位 */
			Diff.y = Ypre - y[2];
			
			/* 予測Z'座標を算出 */
			Zpre = z[1] + (z[1]-z[0])*(R[2]-R[1])/(R[1]-R[0]);
			
			/* Z'成分の2次変位 */
			Diff.z = Zpre - z[2];

			/* calc theta_plane (only X'-axis) */
			
			Diff.theta = atan( (x[2]-x[0])/(z[2]-z[0]) );

//			printf("debug mode: %lf",z[2]-z[0]);//debug

			/* plate内部である場合 */
			if(pljoint[1] == 0 && pljoint[2] == 0){
				
				/* Sample Fileに出力 */
				fprintf(smp,"%s\t%lf\t%lf\t%lf\t%lf\t%lf\t000\n",ID,R[2],Diff.x,Diff.y,Diff.z,Diff.theta);
				
				/* 2次変位の2乗を加算 */
				Drms.x = Drms.x + Diff.x*Diff.x;
				Drms.y = Drms.y + Diff.y*Diff.y;
				
				/* 2次変位の数を加算(plate内部のみ) */
				Inner++;
			}
			
			/* 接続部分を含む場合 */
			else{
				
				/* Sample Fileに出力するだけ */
				fprintf(smp,"%s\t%lf\t%lf\t%lf\t%lf\t%lf\t111\n",ID,R[2],Diff.x,Diff.y,Diff.z,Diff.theta);
			}
			
			/* 2次変位の絶対値を加算 */
			Dave.x = Dave.x + sqrt(Diff.x*Diff.x);
			Dave.y = Dave.y + sqrt(Diff.y*Diff.y);
			
			/* 2次変位の数を加算 */
			Global++;
			
			/* 1点目に2点目を代入 */
			R[0] = R[1];
			x[0] = x[1];
			y[0] = y[1];
			z[0] = z[1];
			pljoint[0] = pljoint[1];
			
			/* 2点目に3点目を代入 */
			R[1] = R[2];
			x[1] = x[2];
			y[1] = y[2];
			z[1] = z[2];
			pljoint[1] = pljoint[2];
		}
		
		/* 引数に"-nl"があれば、しきい線を書き込まない */
		if(para[1] == NULL || strcmp(para[1],"-nl")!=0)
			fprintf(smp,"--------------------------------------------------\n");
		
		/* 2次変位の平均値を求める */
		Dave.x = Dave.x/(double)Global;
		Dave.y = Dave.y/(double)Global;
		
		/* 2成分の平均値の和 */
		Dave.z = Dave.x + Dave.y;
		
		/* 2次変位の１飛跡から得られるRMSを求める */
		Drms.x = sqrt(Drms.x/(double)Inner);
		Drms.y = sqrt(Drms.y/(double)Inner);
		
		/* 表示内容の分岐 */
		if(sel != 1){
			
			/* RMS値を表示 */
			printf("rmsSD(%s) :\t%.9f\t%.9f\t%d\n", ID, Drms.x, Drms.y, Inner);
			fprintf(log, "%s\t%.9f\t%.9f\t%d\n", ID, Drms.x, Drms.y, Inner);
			
		}
		else{
			
			/* 平均値を表示ずる場合 */
			printf("aveSD(%s) :\t%.9f\t%.9f\t%.9f\n", ID, Dave.x, Dave.y, Dave.z);
			fprintf(log, "%s\t%.9f\t%.9f\t%.9f\n", ID, Dave.x, Dave.y, Dave.z);
			
		}
		
		/* 粒子数を加算 */
		Pnum++;
		
	}	/* while [しきい線まで（１飛跡分）取得していく] end */
	
	/* Fileを閉じる */
	fclose(read);
	fclose(smp);
	
	/* main() 戻り値 0(正常終了) */
	return 0;
}

