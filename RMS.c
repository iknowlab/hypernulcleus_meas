#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

/* 関数定義main()関数以降に記述 */
void CalcRMS(int, char*);

int main(int argc, char *fname[]){
	int select;	/* ユーザ選択 */
	char ID[64];	/* File ID */
	
	/* 計算対象に成分を選択 */
	printf("Select a coodinates :  \n");
	printf("\t1) X'-palne\t2) Y'-palne\n");
	printf("\t3) X' & Y'-plane both\n");
	scanf("%d", &select);
	
	/* 入力数値のチェック */
	if(select > 3 || select <1){
		printf("Please input by number 1, 2 or 3!!\n");
		exit(1);
	}
	
	/* RMSの計算処理 */
	CalcRMS(select-1, fname[1]);
	
	/* main() 戻り値0 (正常) */
	return 0;
	
}

void CalcRMS(int mode, char *file){
	long int Max;	/* 2次変位の最大数 */
	double deltaX;	/* 2次変位X'-plane */
	double deltaY;	/* 2次変位Y'-plane */
	double R;	/* Range */
	double SUM;	/* 2次変位の２乗値の合計 */
	double RMS;	/* RMS */
	char linebuffer[2048];	/* 読み込み行 */
	FILE *read, *out;	/* Fileポインタ */
	
	/* 読み込みFileを開く */
	if ((read = fopen(file,"r")) == NULL) {
		printf("FILE[%s] not open!!\n", file);
		exit(1);
	}
	
	/* 出力File名の決定 */
	if(mode == 0)
		sprintf(linebuffer, "RMSx.txt");
	else if(mode == 0)
		sprintf(linebuffer, "RMSy.txt");
	else
		sprintf(linebuffer, "RMSxy.txt");
	
	/* 出力Fileをアペンドモードで開く */
	if ((out = fopen(linebuffer,"a")) == NULL) {
		printf("output_file[%s] not open\n", linebuffer);
		exit(1);
	}
	
	/* 初期クリア */
	SUM = 0;
	Max = 0;
	
	/* 2次変位の読み込み */
	while(fgets(linebuffer,sizeof(linebuffer),read) != NULL){
		sscanf(linebuffer," %*s %lf %lf %lf %*lf", &R, &deltaX, &deltaY);
		
		/* X'成分選択の場合 */
		if(mode == 0){
			SUM = SUM + deltaX*deltaX;
			Max++;
		}
		/* Y'成分選択の場合 */
		else if(mode == 1){
			SUM = SUM + deltaY*deltaY;
			Max++;
		}
		/* X' Y'両成分選択の場合 */
		else{
			SUM = SUM + deltaY*deltaY + deltaX*deltaX;
			Max += 2;
		}
		
	}
	
	/* RMSを求める */
	RMS = sqrt(SUM/((double) Max));
	
	/* 計算された値を表示 */
	printf("N : %d\tRMS = %lf\n", Max, RMS);
	
	/* 出力Fileへの書き込み */
	fprintf(out,"%.0f\t%d\t%lf\n", R, Max, RMS);
	
	/* Fileを閉じる */
	fclose(read);
	fclose(out);
	
}

