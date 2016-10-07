#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main(int argc, char *para[]){
	int pljoint;	/* plate接続部分 */
	int Cut;	/* CutするRange */
	double Range;	/* Range */
	char fname[256];	/* 読み込みFile名 */
	char linebuffer[2048];	/* 読み込み行 */
	FILE *read, *out;	/* Fileポインタ */
	
	/* CutするRangeを入力 */
	printf("Input the cut range :  ");
	scanf("%d", &Cut);
	
	/* 読み込みFileを開く */
	if((read = fopen("sample_tr.dlt", "r"))==NULL){
		printf("Input File [sample_tr.dlt] not open!!\n");
		exit(1);
	}
	
	/* 出力Fileを開く */
	if((out = fopen("rcut.dlt", "w"))==NULL){
		printf("Output File [rcut.dlt] not open!!\n");
		exit(1);
	}
	
	/* データの読み込み */
	while(fgets(linebuffer, sizeof(linebuffer), read)!=NULL){
		
		/* しきい線であれば出力して次の行へ */
		if(strncmp(linebuffer,"--------------------------------------------------",40) == 0){
			fprintf(out, "%s", linebuffer);
			continue;
		}
		
		/* Rangeとplate接続認識コードの取得 */
		sscanf(linebuffer, "%*s %lf %*lf %*lf %*lf %d", &Range, &pljoint);
		
		/* 指定Range以下であれば次の行へ */
		if(((int) Range) < Cut)
			continue;
		
		/* 第１引数に"-e"の指定があり、かつplate接続部分であれば次の行へ */
		if(para[1] != NULL && strcmp(para[1],"-e") == 0){
			
			if(pljoint > 100)
				continue;
			
		}
		
		/* 取得した行を出力 */
		fprintf(out, "%s", linebuffer);
		
	}
	
	/* Fileを閉じる */
	fclose(read);
	fclose(out);
	
	/* 戻り値 0 (正常) */
	return 0;
	
}
