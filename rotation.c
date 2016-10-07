#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int main()
{
	int ans;	/* ユーザーからの返答 */
	int line;	/* 行番号 */
	int loop;	/* ループカウンタ */
	int pljoint[1000];	/* plate接続部分 */
	double angle;	/* 角度 */
	double R[1000];	/* Range */
	double X[1000];	/* X座標 */
	double Y[1000];	/* Y座標 */
	double Z[1000];	/* Z座標 */
	double X_out[1000];	/* 出力X座標 */
	double Y_out[1000];	/* 出力Y座標 */
	double Z_out[1000];	/* 出力Z座標 */
	char id[64];	/* particle ID */
	char linebuffer[2048];	/* 読み取り行格納 */
	FILE *read,*out,*ang;	/* Fileポインタ */
	
	/* 処理開始 */
	printf("\n------------------------------------------------\n");
	printf("\t\trotation.exe starts...\n");
	printf("\n------------------------------------------------\n\n");
	
	/* 処理手順を選択する */
	printf("> Sellect 1, 2 or 3:\n");
	printf("> \t1) XY-plane,\t2) YZ-plane,\t3) assign an angle.\n");
	printf("> [Answer] :  ");
	scanf("%d",&ans);
	
	/* 初期クリア */
	for(loop=0;loop<1000;loop++)
		pljoint[loop] = 0;
	
	/* XY平面での回転 */
	if (ans == 1){
		
		/* 読み取りFileを開く */
		if ((read = fopen("dataTR.stp", "r")) == NULL) {
			printf("Input_File[dataTR.stp] not open\n");
			exit(1);
		}
		
		/* 出力Fileを開く */
		if ((out = fopen("dataTR.stpr", "w")) == NULL) {
			printf("Output_file[dataTR.stpr] not open\n");
			exit(1);
		}
		
		/* 角度出力Fileを開く */
		if((ang = fopen("anglex.log", "w")) == NULL){
			printf("Log_file[anglex.log] not open\n");
			exit(1);
		}
		
		/* 座標の読み取り開始 */
		while(fgets(linebuffer,sizeof(linebuffer),read) != NULL){
			line = 0;
			sscanf(linebuffer,"%s %lf %lf %lf %lf %d",id,&R[line],&X[line],&Y[line],&Z[line],&pljoint[line]);
			line++;
			
			/* しきい線まで（１粒子分）読み取る */
			while(strncmp(linebuffer,"--------------------------------------------------",40) != 0){
				fgets(linebuffer,sizeof(linebuffer),read);
				sscanf(linebuffer,"%*s %lf %lf %lf %lf %d",&R[line],&X[line],&Y[line],&Z[line],&pljoint[line]);
				line++;
			}
			
			/* 角度計算 */
			angle = atan((X[line-2]-X[0])/(Y[line-2]-Y[0]));
			
			/* 回転角を表示 */
			printf("angle of trackID[%s] :\t%lf\n",id,angle*(180.0/acos(-1)));
			fprintf(ang, "%s\t%lf\n", id, angle*(180.0/acos(-1)));
			
			/* 座標系の回転 */
			for(loop=0;loop<line-1;loop++){
				X_out[loop] = (X[loop]-X[0])*(cos(angle)) - (Y[loop]-Y[0])*(sin(angle));
				Y_out[loop] = (X[loop]-X[0])*(sin(angle)) + (Y[loop]-Y[0])*(cos(angle));
				
				/* 接続部分に区別をつける */
				if(pljoint[loop] == 111)
					fprintf(out,"%s\t%lf\t%lf\t%lf\t%lf\t111\n",id,R[loop],X_out[loop],Y_out[loop],Z[loop]);
				
				/* plate内部の場合 */
				else
					fprintf(out,"%s\t%lf\t%lf\t%lf\t%lf\t000\n",id,R[loop],X_out[loop],Y_out[loop],Z[loop]);
			}
			
			/* しきい線の追記 */
			fprintf(out,"----------------------------------------\n");
			
		}	/* while [座標の読み取り開始] end */
		
		/* Fileを閉じる */
		fclose(read);
		fclose(out);
		fclose(ang);
		
	}	/* if [XY平面での回転] end */
	
	/* YZ平面での回転 */
	else if (ans == 2){
		
		/* 読み取りFileを開く */
		if ((read = fopen("dataTR.stpr", "r")) == NULL) {
			printf("Input_File[dataTR.stpr] not open\n");
			exit(1);
		}
		
		/* 出力Fileを開く */
		if ((out = fopen("dataTR.stprr", "w")) == NULL) {
			printf("Output_file[dataTR.stprr] not open\n");
			exit(1);
		}
		
		/* 角度出力Fileを開く */
		if((ang = fopen("angley.log", "w")) == NULL){
			printf("Log_file[angley.log] not open\n");
			exit(1);
		}
		
		/* 座標の読み取り開始 */
		while(fgets(linebuffer,sizeof(linebuffer),read) != NULL){
			line = 0;
			sscanf(linebuffer,"%s %lf %lf %lf %lf %d",id,&R[line],&X[line],&Y[line],&Z[line],&pljoint[line]);
			line++;
			
			/* しきい線まで（１粒子分）読み取る */
			while(strncmp(linebuffer,"--------------------------------------------------",40) != 0){
				fgets(linebuffer,sizeof(linebuffer),read);
				sscanf(linebuffer,"%*s %lf %lf %lf %lf %d",&R[line],&X[line],&Y[line],&Z[line],&pljoint[line]);
				line++;
			}
			
			/* 角度計算 */
			angle = atan((Y[line-2]-Y[0])/(Z[line-2]-Z[0]));
			
			/* 回転角を表示 */
			printf("angle of trackID[%s] :\t%lf\n",id,angle*(180.0/acos(-1)));
			fprintf(ang, "%s\t%lf\n", id, angle*(180.0/acos(-1)));
			
			/* 座標系の回転 */
			for(loop=0;loop<line-1;loop++){
				Y_out[loop] = (Y[loop]-Y[0])*(cos(angle)) - (Z[loop]-Z[0])*(sin(angle));
				Z_out[loop] = (Y[loop]-Y[0])*(sin(angle)) + (Z[loop]-Z[0])*(cos(angle));
				
				/* 接続部分に区別をつける */
				if(pljoint[loop] == 111)
					fprintf(out,"%s\t%lf\t%lf\t%lf\t%lf\t111\n",id,R[loop],X[loop],Y_out[loop],Z_out[loop]);
				
				/* plate内部の場合 */
				else
					fprintf(out,"%s\t%lf\t%lf\t%lf\t%lf\t000\n",id,R[loop],X[loop],Y_out[loop],Z_out[loop]);
			}
			/* しきい線の追記 */
			fprintf(out,"----------------------------------------\n");
			
		}	/* while [座標の読み取り開始] end */
		
		/* Fileを閉じる */
		fclose(read);
		fclose(out);
		fclose(ang);
		
	}	/* if [YZ平面での回転] end */
	
	/* 回転角を指定した場合 */
	else if(ans == 3){
		
		/* XY回転角の入力 */
		printf("XY angle (deg):  ");
		scanf("%lf",&angle);
		
		/* 0°なら何もしない */
		if(angle != 0){
			
			/* 角度をradianに変換 */
			angle = angle/(180.0/acos(-1));
			
			/* 読み取りFileを開く */
			if ((read = fopen("dataTR.stp", "r")) == NULL) {
				printf("Input_File[dataTR.stp] not open\n");
				exit(1);
			}
			
			/* 中間出力Fileを開く */
			if ((out = fopen("dataTR.stpr", "w")) == NULL) {
				printf("Output_file[dataTR.stpr] not open\n");
				exit(1);
			}
			
			/* 角度出力Fileを開く */
			if((ang = fopen("anglex.log", "w")) == NULL){
				printf("Log_file[anglex.log] not open\n");
				exit(1);
			}
			
			/* 座標の読み取り開始 */
			while(fgets(linebuffer,sizeof(linebuffer),read) != NULL){
				line = 0;
				sscanf(linebuffer,"%s %lf %lf %lf %lf %d",id,&R[line],&X[line],&Y[line],&Z[line],&pljoint[line]);
				line++;
				
				/* しきい線まで（１粒子分）読み取る */
				while(strncmp(linebuffer,"--------------------------------------------------",40) != 0){
					fgets(linebuffer,sizeof(linebuffer),read);
					sscanf(linebuffer,"%*s %lf %lf %lf %lf %d",&R[line],&X[line],&Y[line],&Z[line],&pljoint[line]);
					line++;
				}
				
				/* 回転角度を表示 */
				printf("angle of trackID[%s] :\t%lf\n",id,angle*(180.0/acos(-1)));
				fprintf(ang, "%s\t%lf\n", id, angle*(180.0/acos(-1)));
				
				/* 座標系の回転 */
				for(loop=0;loop<line-1;loop++){
					X_out[loop] = (X[loop]-X[0])*(cos(angle)) - (Y[loop]-Y[0])*(sin(angle));
					Y_out[loop] = (X[loop]-X[0])*(sin(angle)) + (Y[loop]-Y[0])*(cos(angle));
					
					/* 接続部分に区別をつける */
					if(pljoint[loop] == 111)
						fprintf(out,"%s\t%lf\t%lf\t%lf\t%lf\t111\n",id,R[loop],X_out[loop],Y_out[loop],Z[loop]);
					
					/* plate内部の場合 */
					else
						fprintf(out,"%s\t%lf\t%lf\t%lf\t%lf\t000\n",id,R[loop],X_out[loop],Y_out[loop],Z[loop]);
				}
				/* しきい線の追記 */
				fprintf(out,"----------------------------------------\n");
				
			}	/* while [座標の読み取り開始] end */
			
			/* Fileを閉じる */
			fclose(read);
			fclose(out);
			fclose(ang);
			
		}	/* if [0°なら何もしない] end */
		
		/* 続いてYZ回転角の入力 */
		printf("YZ angle (deg):  ");
		scanf("%lf",&angle);
		
		/* 0°なら何もしない */
		if(angle != 0){
			
			/* 角度をradianに変換 */
			angle = angle/(180.0/acos(-1));
			
			/* 中間Fileを開く */
			if ((read = fopen("dataTR.stpr", "r")) == NULL) {
				printf("Input_File[dataTR.stpr] not open\n");
				exit(1);
			}
			
			/* 出力Fileを開く */
			if ((out = fopen("dataTR.stprr", "w")) == NULL) {
				printf("Output_file[dataTR.stprr] not open\n");
				exit(1);
			}
			
			/* 角度出力Fileを開く */
			if((ang = fopen("angley.log", "w")) == NULL){
				printf("Log_file[angley.log] not open\n");
				exit(1);
			}
			
			/* 座標の読み取り開始 */
			while(fgets(linebuffer,sizeof(linebuffer),read) != NULL){
				line = 0;
				sscanf(linebuffer,"%s %lf %lf %lf %lf %d",id,&R[line],&X[line],&Y[line],&Z[line],&pljoint[line]);
				line++;
				
				/* しきい線まで（１粒子分）読み取る */
				while(strncmp(linebuffer,"--------------------------------------------------",40) != 0){
					fgets(linebuffer,sizeof(linebuffer),read);
					sscanf(linebuffer,"%*s %lf %lf %lf %lf %d",&R[line],&X[line],&Y[line],&Z[line],&pljoint[line]);
					line++;
				}
				
				/* 回転角度を表示 */
				printf("angle of trackID[%s] :\t%lf\n",id,angle*(180.0/acos(-1)));
				fprintf(ang, "%s\t%lf\n", id, angle*(180.0/acos(-1)));
				
				/* 座標系の回転 */
				for(loop=0;loop<line-1;loop++){
					Y_out[loop] = (Y[loop]-Y[0])*(cos(angle)) - (Z[loop]-Z[0])*(sin(angle));
					Z_out[loop] = (Y[loop]-Y[0])*(sin(angle)) + (Z[loop]-Z[0])*(cos(angle));
					
					/* 接続部分に区別をつける */
					if(pljoint[loop] == 111)
						fprintf(out,"%s\t%lf\t%lf\t%lf\t%lf\t111\n",id,R[loop],X[loop],Y_out[loop],Z_out[loop]);
					
					/* plate内部の場合 */
					else
						fprintf(out,"%s\t%lf\t%lf\t%lf\t%lf\t000\n",id,R[loop],X[loop],Y_out[loop],Z_out[loop]);
				}
				/* しきい線の追記 */
				fprintf(out,"----------------------------------------\n");
				
			}	/* while [座標の読み取り開始] end */
			
			/* Fileを閉じる */
			fclose(read);
			fclose(out);
			fclose(ang);
			
		}	/* if [0°なら何もしない] end */
		
	}	/* if [回転角を指定した場合] end */
	
	/* 1, 2, 3以外の数が入力されたら */
	else {
		printf("> You should answer by 1, 2 or 3!!\n");
		printf("> Exit soon ...\n");
		
		exit(1);
	}
	
	/* main() 戻り値 0（正常終了） */
	return 0;
	
}

