#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>
#include<math.h>

/* 構造体LOADPATH : フルパス名 */
struct LOADPATH{
	char pName[64];	/* 飛跡名 */
	char Path[1024];	/* フルパス */
};

/* 構造体PL : pl#の認識 */
struct PL{
	int curr;	/* 現在のpl# */
	int prev;	/* １つ手前行のpl# */
};

/* 構造体COODINATE : 座標 */
struct COODINATE{
	double X;
	double Y;
	double Z;
	double SZ;	/* 伸縮率補正後のZ座標 */
};

/* 構造体ShurinkageFactor : 伸縮率 */
struct ShurinkageFactor{
	double up;
	double down;
	double Base;	/* ベースの厚さ */
};

/* 関数定義main()関数以降に記述 */
int ProcessFrame(char*, char*, char*);
int StepMakingProcess(char*, FILE*, FILE*, char*, double*, double);

int main()
{
	int loop;	/* ループカウンタ */
	int count;	/* 項目数 */
	int select;	/* ユーザー選択 */
	int selfile;	/* ユーザー指定File */
	int result;	/* 関数処理結果 */
	char linebuffer[2048];	/* 行の格納 */
	char fname[100][256];	/* File名 */
	struct LOADPATH Load[100];	/* 読み取りFileのフルパス */
	struct dirent *entry;	/* ディレクトリ操作構造体dirent */
	DIR *beam;	/* Directoryポインタ */
	FILE *tid;	/* Fileポインタ */
	
	/* 以前の中間ファイルを全て削除する */
	for(loop=0; loop<5; loop++){
		switch(loop){
			case 0:	strcpy(fname[1], "dataTR.stp");
				break;
			case 1:	strcpy(fname[1], "dataTR.stpr");
				break;
			case 2:	strcpy(fname[1], "dataTR.stprr");
				break;
			case 3:	strcpy(fname[1], "dataTR.stprrn");
				break;
			case 4: strcpy(fname[1], "sample_tr.dlt");
				break;
			default:
				break;
		}
		
		if((tid = fopen(fname[1],"r"))!=NULL){
			fclose(tid);
			
			printf("... Deleting file \"%s\".\n", fname[1]);
			sprintf(linebuffer, "rm -f %s\n", fname[1]);
			system(linebuffer);
		}
	}
	
	printf("\n");

	/* 処理開始宣言 */
	printf("*** EMstepmake.exe starts ... ***\n");
	
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
	
	/* パス名を定義 */
	sprintf(linebuffer, "%svtx/", Load[select].Path);
	
	/* 無限ループ...Read Fileの選択 */
	while(1){
		/* パス名のdirectoryを開く */
		if((beam = opendir(linebuffer)) == NULL){
			printf("\n[Error] :  Invalid PATH NAME!!\n");
			
			/* 開けなかったなら終了 */
			exit(1);
		}
		
		/* 項目数Indexを初期化 */
		count = 1;
		
		/* 指定したdirectory内のFileを検索*/
		printf("\nFound these Files : \n", loop);
		while(1){
			
			/* File候補を取得、失敗したら検索終了 */
			if((entry = readdir(beam))==NULL)
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
			if((entry = readdir(beam))==NULL){
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
		
		/* directoryを閉じる */
		closedir(beam);
		
		/* ユーザーにFile番号を選択させる */
		printf("Select number of the track file  :  ");
		scanf("%d", &selfile);
		
		/* 選択番号と配列番号を合わせる */
		selfile--;
		
		/* 入力された値のチェック */
		if(selfile >= count || selfile < 0){
			printf("[Error] :  Invalid number was selected\n\nRestart soon...\n\n");
			
			/* 再表示 */
			continue;
		}
		
		/* １度だけ処理してループ終了 */
		break;
		
	}	/* while [無限ループ...Read Fileの選択] end */
	
	/* 視野移動時に生じたブレを補正する */
	result = ProcessFrame(fname[selfile], linebuffer, Load[select].pName);
	if(result == 1){
		printf("[Error] :  Invalid File Format in '%s' !!\nexit soon ...\n", fname[selfile]);
		exit(0);
	}
	else if(result == -1)
		exit(1);
	
	/* 処理終了宣言 */
	printf("\n*** End EMstepmake.exe ... ***\n");
	
	/* main() 戻り値0（正常終了） */
	return 0;
	
}

int ProcessFrame(char *fname, char *path, char *pName){
	int loop;	/* forループカウンタ */
	int counter;	/* 処理分岐 : 均一または不均一Stepの選択 */
	int ident;	/* 行の先頭文字を格納 */
	int setevent;	/* 処理分岐用変数(plate毎) Used like Boolean */
	int TNum;	/* 同じ飛跡の測定回数 */
	double Slength;	/* ユーザー指定の均一Step長 */
	double sdrms;	/* 不均一Stepのパラメータ選択 */
	double sdrmsRead;	/* step.datから読み込んだパラメータ */
	double step[1000];	/* step長格納 */
	double Rfinish;	/* 静止点からの処理範囲 */
	char pNameIndex[32];
	char linebuffer[2048];	/* １行を格納 */
	FILE *read, *out, *stp;	/* Fileポインタ */
	
	/* フルパスでFile名を定義 */
	sprintf(linebuffer,"%s%s", path, fname);
	
	/* 読み取りFileを開く */
	printf("\n... Focus On \"%s\" in process\n", linebuffer);
	if ((read = fopen(linebuffer, "r")) == NULL) {
		printf("File[%s] not open\n",linebuffer);
		return -1;
	}
	
	/* 測定回数カウントを0にする */
	TNum = 0;
	
	/* setevent を仮に条件分岐に使用 */
	setevent = 0;
	
	/* 読み込みFileの形式チェック */
	while(fgets(linebuffer, sizeof(linebuffer), read)!=NULL){
		ident = linebuffer[0];
		
		/* Scanの詳細部分があるか初めに調べる */
		if(ident == 'a' && setevent == 0)
			setevent++;
		
		/* 次にScanの詳細部分の終了文字sがあるか調べる */
		else if(ident == 's' && setevent == 1)
			setevent++;
		
		/* 最後に#で終わっているか調べる */
		else if(strncmp(linebuffer, "# approximate range ", 20) == 0 && setevent == 2){
			if(fgets(linebuffer,sizeof(linebuffer), read) == NULL)
					setevent++;
			
			/* 測定回数も加算 */
			TNum++;
			
		}
		
	}
	
	/* ３でなければ形式エラー */
	if(setevent != 3){
		fclose(read);
		return 1;
	}
	
	/* Fileストリームを戻す */
	fseek(read, 0, SEEK_SET);
	
	/* 出力Fileを開く */
	printf("... Focus On \"dataTR.stp\" in process\n\n");
	if ((out = fopen("dataTR.stp", "w")) == NULL) {
		printf("File[dataTR.stp] not open\n");
		return -1;
	}
	
	/* 均一Step長、または不均一Step長の選択 */
	/* ルール：不均一Steo長選択の場合、step.datを用意しておく */
	printf("\nWhitch do you sellect?\n\t1) Interval step \t 2) Irregular step\n\n");
	printf("[Answer by number(1, or 2) ] : ");
	scanf("%d",&counter);
	
	/* 均一Step長選択の場合 */
	if(counter == 1){
		printf("\nInput the step length (um) : ");
		scanf("%lf",&Slength);
		
		for(loop=0; loop<1000; loop++)
			step[loop] = Slength;
		
		printf("\nAnd ");
	}
	
	/* 不均一Step長選択の場合 */
	else if(counter == 2){
		
		/* step.datの呼び出し */
		if ((stp = fopen("step.dat", "r")) == NULL) {
			printf("\t[step.dat] not open\n");
			exit(1);
		}
		
		/* パラメータ入力 */
		printf("\nInput the SD(rms) Value : ");
		scanf("%lf",&sdrms);
		
		/* Step長をロード */
		loop = 0;
		while(fgets(linebuffer,sizeof(linebuffer),stp)!=NULL){
			    sscanf(linebuffer,"%lf %lf",&sdrmsRead, &step[loop]);
				if(sdrms == sdrmsRead){
			            printf("%lf %lf\n",sdrmsRead, step[loop]);
				        loop++;
			    }
		}
		
		fclose(stp);
		
		printf("\n");
	}
	
	/* 1, 2以外の数が入力された場合 */
	else{
		printf("Invalid parameter inputed !!\nexit soon...\n");
		exit(1);
	}
	
	/* デフォルト=6mm */
	Rfinish = 6000.;
	
	/* 処理範囲を指定させる */
	printf("Input the length from the stop point : \n");
	printf("(org. R = %lf )\tZ = ",Rfinish);
	scanf("%lf",&Rfinish);
	
	/* 文字sの行までストリームを移動させる */
	while(linebuffer[0] != 's')
		fgets(linebuffer, sizeof(linebuffer), read);
	
	/* 粒子名を定義 */
	strcpy(pNameIndex, pName);
	
	/* 測定回数分を処理 */
	for(loop=0;loop<TNum;loop++){
		
		/* 飛跡１本分の歪み補正処理 */
		if(StepMakingProcess(pNameIndex, read, out, path, step, Rfinish) == -1)
			return -1;
		
		/* 2回目以降は粒子名にIndexをつける */
		sprintf(pNameIndex, "%s%02d", pName, loop+1);
		
	}
	
	/* 現読み込みファイルを閉じる */
	fclose(read);
	
	/* 出力ファイルを閉じる */
	fclose(out);
	
	/* ProcessFrame() 戻り値0(正常終了) */
	return 0;
	
}
int StepMakingProcess(char *pName, FILE *read, FILE *out, char *path, double step[], double Rfinish){
	int loop;	/* forループカウンタ */
	int ident;	/* 行の先頭文字を格納 */
	int line;	/* 行数カウンタ */
	int surf;	/* 1plate分における表面数(最大で4まで) */
	int setevent;	/* 処理分岐用変数(plate毎) Used like Boolean */
	int offset;	/* 処理分岐用変数(1track毎) Used like Boolean */
	int pos;	/* Step長選択用Index Number */
	int PLchange;	/* plate接続部分の認識 Used like Boolean */
	double Position[10000];	/* vtx file内のlay# */
	double R[10000];	/* plate中最下流点からの長さ */
	double Rlength;	/* plate最下流点からの長さ(step長間隔) */
	double Range;	/* Range : 静止点からの距離 */
	char linebuffer[2048];	/* １行を格納 */
	struct PL pl;			/* vtx file内のpl# */
	struct COODINATE In[10000];	/* 読み込み座標 */
	struct COODINATE Out;		/* 出力座標 */
	struct COODINATE em[4];	/* 表面の座標 */
	struct COODINATE joint;	/* plate接続用座標 */
	struct COODINATE set;	/* 静止点のX, Y座標、Z, SZは不使用 */
	struct ShurinkageFactor SF;	/*伸縮率（MCのため基本はUP,DOWN=1.0、Baseは0.05mm）*/
	FILE *sf;	/* Fileポインタ */
	
	/* 改行文字の次へストリームを移す */
	fgets(linebuffer, sizeof(linebuffer), read);
	
	/* 初期クリア */
	line = 0;
	pos = 0;
	surf = 0;
	setevent = 0;
	offset = 0;
	Rlength = 0.;
	Range = 0.;
	PLchange = 0;
	joint.Z = 0;
	
	/* 文字#の行まで（つまり１飛跡分）処理する */
	while(linebuffer[0] != '#'){
		
		/* 座標の取得 */
		fgets(linebuffer, sizeof(linebuffer), read);
		sscanf(linebuffer,"%d %d %lf %lf %lf %lf %*lf",
						   &ident, &pl.curr, &Position[line], &In[line].X, &In[line].Y, &In[line].Z);
		
		/* line=0は静止点、手前行のpl#(pl.prev)に現段階のpl#を格納しておく */
		if(line == 0)
			pl.prev = pl.curr;
		
		/* 最終行でないこと、かつpl#が同じである間は座標を取得し続ける */
		if(linebuffer[0] != '#' && pl.curr == pl.prev){
			line++;
			continue;
		}
		
		/* コメント：この段階で1plate分の座標が得られた */
		/* 　　　　　またIn[line]には次plateの初めの行が格納されているため */
		/* 　　　　　forループの終了値はline-1までの範囲にする */
		
		/* plateの表面を認識していく */
		for(loop=0; loop<line; loop++){
			
			/* 最下流面ならば */
			if(((int) (Position[loop]*10.)) == 30){
				surf++;
				em[3].Z = In[loop].Z;
			}
			
			/* 下流乳剤とBaseの境界面ならば */
			else if(((int) (Position[loop]*10.)) == 20){
				surf++;
				em[2].Z = In[loop].Z;
				em[2].X = In[loop].X;
				em[2].Y = In[loop].Y;
			}
			
			/* 上流乳剤とBaseの境界面ならば */
			else if(((int) (Position[loop]*10.)) == 10){
				surf++;
				em[1].Z = In[loop].Z;
				em[1].X = In[loop].X;
				em[1].Y = In[loop].Y;
			}
			
			/* 最上流面ならば */
			else if(((int) (Position[loop]*10.)) == 0){
				surf++;
				em[0].Z = In[loop].Z;
			}
		}
		
		/* 伸縮率の計算 */
		/* trackがplateを貫通していた場合 */
		if(surf >= 4){
			SF.up = 0.5/sqrt((em[0].Z-em[1].Z)*(em[0].Z-em[1].Z));
			SF.down = 0.5/sqrt((em[2].Z-em[3].Z)*(em[2].Z-em[3].Z));
			SF.Base = sqrt((em[1].Z-em[2].Z)*(em[1].Z-em[2].Z));
		}
		
		/* trackの静止点が下流である場合 */
		else if(surf >= 2 && surf < 4){
			SF.up = 0.5/sqrt((em[0].Z-em[1].Z)*(em[0].Z-em[1].Z));
			SF.Base = sqrt((em[1].Z-em[2].Z)*(em[1].Z-em[2].Z));
			
			/* SFが格納されているFileを開く */
			sprintf(linebuffer,"%s/SF_pl%d.txt",path,pl.prev);
			if ((sf = fopen(linebuffer, "r")) == NULL) {
				printf("input_file[%s] not open\n",linebuffer);
				return -1;
			}
			
			/* 下流SFのみロード */
			fgets(linebuffer,sizeof(linebuffer),sf);
			fgets(linebuffer,sizeof(linebuffer),sf);
			sscanf(linebuffer,"%lf",&SF.down);
			
			/* 閉じる */
			fclose(sf);
		}
		
		/* trackの静止点が上流ならば */
		else{
			
			/* SFが格納されているFileを開く */
			sprintf(linebuffer,"%s/SF_pl%d.txt",path,pl.prev);
			if ((sf = fopen(linebuffer, "r")) == NULL) {
				printf("input_file[%s] not open\n",linebuffer);
				return -1;
			}
			
			/* 両GelのSF、またBase厚さをロード */
			fgets(linebuffer,sizeof(linebuffer),sf);
			sscanf(linebuffer,"%lf",&SF.up);
			fgets(linebuffer,sizeof(linebuffer),sf);
			sscanf(linebuffer,"%lf",&SF.down);
			fgets(linebuffer,sizeof(linebuffer),sf);
			sscanf(linebuffer,"%lf",&SF.Base);
			
			/* 閉じる */
			fclose(sf);
		}
		
//		/* Base Angleを計算する */
//		angle = atan(sqrt((em[1].X-em[2].X)*(em[1].X-em[2].X) + (em[1].Y-em[2].Y)*(em[1].Y-em[2].Y))
//					/sqrt((em[1].Z-em[2].Z)*(em[1].Z-em[2].Z)));
//		printf("  > [Track angle in pl#%d] : %lf\n", pl.prev, angle*((double) 180.0/acos(-1)));
		
		/* 初期クリア */
		In[0].SZ = 0.;
		R[0] = 0.;
		
		/* コメント：line番目は次plateの始点なので、0にしておく */
		In[line].SZ = 0.;
		R[line] = 0.;
		
		/* 伸縮率を補正する */
		for(loop=1; loop<line; loop++){
			
			/* 下流乳剤部分 */
			if(((int) (Position[loop-1]*10.)) == 25 || ((int) (Position[loop-1]*10.)) == 30)
				In[loop].SZ = In[loop-1].SZ + (In[loop].Z - In[loop-1].Z)*SF.down;
			
			/* ベース部分 */
			else if(((int) (Position[loop-1]*10.)) == 20)
				In[loop].SZ = In[loop-1].SZ + SF.Base;
			
			/* 上流乳剤部分 */
			else if(((int) (Position[loop-1]*10.)) == 5 || ((int) (Position[loop-1]*10.)) == 10)
				In[loop].SZ = In[loop-1].SZ + (In[loop].Z - In[loop-1].Z)*SF.up;
			
			/* 最下流点からの距離を計算 */
			R[loop] = R[loop-1] + sqrt((In[loop].X-In[loop-1].X)*(In[loop].X-In[loop-1].X)
								+ (In[loop].Y-In[loop-1].Y)*(In[loop].Y-In[loop-1].Y)
								+ (In[loop].SZ-In[loop-1].SZ)*(In[loop].SZ-In[loop-1].SZ));
		}
		
		/* コメント：seteventについて */
		/* 　　　　　0の場合はplate内部での処理を表す */
		/* 　　　　　1の場合はplate接続直後の処理を表す */
		/* 　　　　　2の場合は指定範囲を超えた場合の処理 */
		
		/* plate接続後は飛跡をつなぐ */
		if(setevent == 1){
			
			/* 前plateの飛跡とつなげる */
			for(loop=1; loop<line; loop++){
				In[loop].X = In[loop].X - (In[0].X - joint.X);
				In[loop].Y = In[loop].Y - (In[0].Y - joint.Y);
			}
			
			/* 初期座標は前plateの最上流点にあわせる */
			In[0].X = joint.X;
			In[0].Y = joint.Y;
			
			/* plateの内部処理に移る */
			setevent = 0;
		}
		
		/* plate内部処理の手順 */
		for(loop=0; loop<line; loop++){
			
			/* Step毎の位置を折れ線近似により取得する */
			if(Rlength >= R[loop] && Rlength < R[loop+1] && setevent == 0){
				Out.X = In[loop].X + (In[loop+1].X - In[loop].X) * (Rlength - R[loop]) / (R[loop+1] - R[loop]);
				Out.Y = In[loop].Y + (In[loop+1].Y - In[loop].Y) * (Rlength - R[loop]) / (R[loop+1] - R[loop]);
				Out.Z = joint.Z + In[loop].SZ + (In[loop+1].SZ - In[loop].SZ) * (Rlength - R[loop]) / (R[loop+1] - R[loop]);
				
				/* コメント：offsetについて・・・Booleanとして活用 */
				/* 　　　　　0は初期処理時を表す */
				/* 　　　　　1はそれ以降の処理内容を表す */
				
				/* 初期処理ならば、静止点のX, Y座標を取得する */
				if(offset == 0){
					set.X = Out.X;
					set.Y = Out.Y;
					
					/* 初期処理の終了 */
					offset = 1;
				}
				
				/* plate接続後の座標には111を追記 */
				if(PLchange == 1){
					fprintf(out,"%s\t%lf\t%lf\t%lf\t%lf\t111\n", pName, Range, (Out.X-set.X)*1000, (Out.Y-set.Y)*1000, Out.Z*1000);
					PLchange = 0;
				}
				
				/* plate内部処理中は000 */
				else
					fprintf(out,"%s\t%lf\t%lf\t%lf\t%lf\t000\n", pName, Range, (Out.X-set.X)*1000, (Out.Y-set.Y)*1000, Out.Z*1000);
				
				/* 指定範囲を超えた場合は処理終了 */
				if(Range >= Rfinish){
					setevent = 2;
					break;
				}
				
				/* step長を追加 */
				Rlength = Rlength + step[pos]/1000;
				Range = Range + step[pos];
				pos++;
				
				/* 次のstepが同じ距離以内の場合は再帰 */
				if(Rlength < R[loop+1])
					loop--;
				
				setevent = 0;
			}
			
			/* plate内の最大距離を超えたら、処理終了 */
			else if(Rlength >= R[line-1] && setevent == 0){
				
				/* plateの接続手続き */
				Rlength = Rlength - R[line-1];
				joint.X = In[line-1].X;
				joint.Y = In[line-1].Y;
				joint.Z = In[line-1].SZ + joint.Z;
				setevent = 1;
				
				break;
			}
		} /* for [plate内部処理の手順] end */
		
		/* 次のplate座標を読み込むための手続き */
		if(pl.curr != pl.prev){
			PLchange = 1;
			
			/* pl#の更新 */
			pl.prev = pl.curr;
			
			/* line番目の値を0番目に格納 */
			Position[0] = Position[line];
			In[0].X = In[line].X;
			In[0].Y = In[line].Y;
			In[0].Z = In[line].Z;
			
			/* 0番目がすでに読み込まれているので、1が初期値になる */
			line = 1;
			
			/* 表面認識数もクリア */
			surf = 0;
			
			continue;
		}
	}	/* while [文字#の行まで（つまり１飛跡分）処理する] end */

	/* しきい線を追加 */
	fprintf(out,"----------------------------------------\n");
	
	/* クリア */
	PLchange = 0;
	offset = 0;
	setevent = 0;
	
	/* ProcessFrame() 戻り値0(正常終了) */
	return 0;
	
}
