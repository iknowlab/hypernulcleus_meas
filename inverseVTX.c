#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<math.h>
#include<string.h>

/* 構造体LOADPATH : フルパス名 */
struct LOADPATH{
	char pName[64];	/* 飛跡名 */
	char Path[1024];	/* フルパス */
};

int main()
{
	int ans;	/* ユーザ返答 */
	int loop;	/* ループカウンタ */
	int count;	/* 項目数 */
	int select;	/* ユーザー選択 */
	int selfile;	/* ユーザー指定File */
	int tr;	/* track number */
	int pl;	/* plate number */
	double lay;	/* layer number */
	double x;	/* X座標 */
	double y;	/* Y座標 */
	double z;	/* Z座標 */
	char fname[100][256];	/* File名 */
	char linebuffer[2048];	/* 読み込み行 */
	char InputFile[1024];	/* 読み込みFile */
	char OutputFile[1024];	/* 出力File */
	struct LOADPATH Load[100];	/* 読み取りFileのフルパス */
	struct dirent *entry;	/* ディレクトリ操作構造体dirent */
	DIR *trk;	/* Directoryポインタ */
	FILE *tid, *read, *out;	/* Fileポインタ */
	
	/* 処理開始宣言 */
	printf("*** inverseVTX.exe starts ... ***\n");
	
	/* 識別対象の飛跡かBeam trackを選ぶ */
	printf("Which base angle do you want to calculate :\n");
	printf("\t1) Track,\t2) Beam track\n");
	printf("Select by number :  ");
	scanf("%d", &ans);
	
	/* 1、または2以外の数が入力された場合 */
	if(ans > 2 && ans <= 0){
		printf("Please input by 1 or 2, only!!\n");
		exit(1);
	}
	
	/* 無限ループ */
	while(1){
		
		/* 飛跡毎に定義したパスFileを開く */
		if((tid = fopen("ID.txt", "r")) == NULL){
			printf("Load File[ID.txt] not open!!\n\n");
			exit(1);
		}
		
		/* 初期化 */
		count = 0;
		
		/* 処理したい飛跡項目を検索 */
		while(fgets(linebuffer, sizeof(linebuffer), tid) != NULL){
			
			/* ルール：100以上はエラーとする */
			if(count > 100){
				printf("[Error] :  Too many Items to read!! -> Exit soon ...\n");
				exit(1);
			}
			
			/* 飛跡名、フルパスを取得 */
			sscanf(linebuffer, "%s %s", Load[count].pName, Load[count].Path);
			
			/* 項目数加算 */
			count++;
		}
		
		/* Fileを閉じる */
		fclose(tid);
		
		/* 検索から得られた飛跡名を表示 */
		printf("Found These Items :\n");
		for(loop=0; loop<count; loop++){
			if((loop % 2) != 0)
				printf("\t\t%d) %s\n", loop+1, Load[loop].pName);
			else
				printf("\t%d) %s", loop+1, Load[loop].pName);
		}
		
		/* 改行文字 */
		if((loop % 2) != 0)
			printf("\n");
		
		/* ユーザー選択 */
		/* コメント：ユーザーに処理する飛跡を選択させる */
		printf("[Select a number to carry on] :  ");
		scanf("%d", &select);
		
		/* 表示した番号と配列番号を合わせる */
		select--;
		
		/* 入力された値のチェック */
		if(select >= count || select < 0){
			printf("[Error] :  Invalid number was selected\n\nRestart soon...\n\n");
			
			/* 再表示 */
			continue;
		}
		
		/* １度だけ処理してループ終了 */
		break;
		
	} /* while [無限ループ] end */
	
	/* Beam trackを選択した場合、pl#を入力 */
	if(ans == 2){
		printf("Input the plate number :  ");
		scanf("%d", &pl);
	}
	
	/* 無限ループ */
	while(1){
		
		/* 選択した飛跡のあるディレクトリのパス名を定義 */
		if(ans == 1)
			sprintf(linebuffer, "%svtx/", Load[select].Path);
		else
			sprintf(linebuffer, "%sBeam/B%d/", Load[select].Path, pl);
		
		/* パス名のdirectoryを開く */
		if((trk = opendir(linebuffer)) == NULL){
			printf("\n[Error] :  Invalid PATH NAME!!\n");
			
			/* 開けなかったならエラー終了 */
			exit(1);
		}
		
		/* 項目数Indexを初期化 */
		count = 1;
		
		/* 指定したdirectory内のFileを検索*/
		printf("\n [ Found these Files ]  : \n");
		
		/* 表示directoryを示す */
		printf(" [ Directory ... \' %s \' ]\n", linebuffer);
		
		while(1){
			
			/* File候補を取得、失敗したら検索終了 */
			if((entry = readdir(trk))==NULL)
				break;
			
			/* カレントdirectoryと親directoryは無視する */
			if(strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name, "..")==0)
				continue;
			
			/* 見つけたFile名を番号を添えて表示 */
			printf("\t%d) %s", count, entry->d_name);
			
			/* 更にFile名を格納 */
			strcpy(fname[count-1], entry->d_name);
			
			/* 項目数を加算 */
			count++;
			
			/* 1000個以上Fileが見つかれば終了 */
			if(count > 1000){
				printf("\nFound Over 100 Files in this directory!!\nexit soon ...\n");
				exit(0);
			}
			
			/* File候補を取得、失敗したら検索終了 */
			if((entry = readdir(trk))==NULL){
				printf("\n");
				break;
			}
			
			/* カレントdirectoryと親directoryは無視する */
			if(strcmp(entry->d_name, ".")==0 || strcmp(entry->d_name, "..")==0)
				continue;
			
			/* 見つけたFile名を番号を添えて表示 */
			printf("\t\t%d) %s\n", count, entry->d_name);
			
			/* 更にFile名を格納 */
			strcpy(fname[count-1], entry->d_name);
			
			/* 項目数を加算 */
			count++;
			
			/* 1000個以上Fileが見つかれば終了 */
			if(count > 1000){
				printf("Found Over 100 Files in this directory!!\nexit soon ...\n");
				exit(0);
			}
		}
		
		/* ユーザーにFile番号を選択させる */
		printf("Select number of the track file  :  ");
		scanf("%d", &selfile);
		
		/* 選択番号と配列番号を合わせる */
		selfile--;
			
		/* directoryを閉じる */
		closedir(trk);
		
		/* 入力された値のチェック */
		if(selfile >= count || selfile < 0){
			printf("[Error] :  Invalid number was selected\n");
			
			/* もう一度一覧表示、選択させる */
			continue;
		}
		
		/* １度だけ処理してループ終了 */
		break;
		
	} /* while [無限ループ] end */
	
	/* 読み込みFile名を決定 */
	if(ans == 1)
		sprintf(InputFile, "%svtx/%s", Load[select].Path, fname[selfile]);
	else
		sprintf(InputFile, "%sBeam/B%d/%s", Load[select].Path, pl, fname[selfile]);
		
	/* 読み込みFileを開く */
	if((read = fopen(InputFile,"r"))==NULL){
		printf("Input_File[%s] not open!!\n", InputFile);
		exit(1);
	}
	
	/* 出力File名の決定 */
	sprintf(OutputFile, "%sfin.vtx", linebuffer);
	
	/* 出力Fileを開く */
	if((out = fopen(OutputFile,"w"))==NULL){
		printf("Output_File[%s] not open!!\n", OutputFile);
		exit(1);
	}
	
	/* loop を仮に条件分岐に使用 */
	loop = 0;
	
	/* 読み込みFileの形式チェック */
	while(fgets(linebuffer, sizeof(linebuffer), read)!=NULL){
		
		/* Scanの詳細部分があるか初めに調べる */
		if(linebuffer[0] == 'a' && loop == 0)
			loop++;
		
		/* 次にScanの詳細部分の終了文字sがあるか調べる */
		else if(linebuffer[0] == 's' && loop == 1)
			loop++;
		
		/* 最後に#で終わっているか調べる */
		else if(strncmp(linebuffer, "# approximate range ", 20) == 0 && loop == 2){
			if(fgets(linebuffer,sizeof(linebuffer), read) == NULL)
					loop++;
		}
		
	}
	
	/* ３でなければ形式エラー */
	if(loop != 3){
		fclose(read);
		fclose(out);
		
		printf("Error! : Input FIle Format Invalid\n");
		exit(0);
	}
	
	/* Fileストリームを戻す */
	fseek(read, 0, SEEK_SET);
	
	/* track情報部分を出力Fileに書き込み */
	while(fgets(linebuffer,sizeof(linebuffer),read)!=NULL){
		fprintf(out, "%s",linebuffer);
		
		if(linebuffer[0] == 's')
			break;
	}
	
	/* 改行文字の出力 */
	fgets(linebuffer,sizeof(linebuffer),read);
	fprintf(out, "%s", linebuffer);
	
	/* 座標の読み込み開始 */
	while(fgets(linebuffer,sizeof(linebuffer),read)!=NULL){
		sscanf(linebuffer,"%d %d %lf %lf %lf %lf",&tr,&pl,&lay,&x,&y,&z);
		
		/* 最終行(# approximate range ...)ならば */
		if(linebuffer[0] == '#'){
			
			/* その行を出力 */
			fgets(linebuffer,sizeof(linebuffer),read);
			
			/* 改行文字の出力 */
			fprintf(out, "%s",linebuffer);
			
			/* 次行を読み込む */
			continue;
		}
		
		/* 最上流面ならば、最下流面に */
		if(lay == 0.0)
			fprintf(out, "%d %d %.1f  %4.5f   %4.5f  %3.5f\n",tr,-1*pl,3.0,x,y,-1*z);
		
		/*上流領域ならば、下流領域に */
		if(lay == 0.5)
			fprintf(out, "%d %d %.1f  %4.5f   %4.5f  %3.5f\n",tr,-1*pl,2.5,x,y,-1*z);
		
		/* ベースと上流の境界面ならば、ベースと下流の境界面に */
		if(lay == 1.0)
			fprintf(out, "%d %d %.1f  %4.5f   %4.5f  %3.5f\n",tr,-1*pl,2.0,x,y,-1*z);
		
		/* ベースと下流の境界面ならば、ベースと上流の境界面に */
		if(lay == 2.0)
			fprintf(out, "%d %d %.1f  %4.5f   %4.5f  %3.5f\n",tr,-1*pl,1.0,x,y,-1*z);
		
		/*下流領域ならば、上流領域に */
		if(lay == 2.5)
			fprintf(out, "%d %d %.1f  %4.5f   %4.5f  %3.5f\n",tr,-1*pl,0.5,x,y,-1*z);
		
		/* 最下流面ならば、最上流面に */
		if(lay == 3.0)
			fprintf(out, "%d %d %.1f  %4.5f   %4.5f  %3.5f\n",tr,-1*pl,0.0,x,y,-1*z);
		
	}
	
	/* Fileを閉じる */
	fclose(read);
	fclose(out);
	
	/* main() 戻り値0 (正常) */
	return 0;
	
}

