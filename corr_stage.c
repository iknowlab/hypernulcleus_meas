/*--make ariga 2008~-------*/
/*--fixed inoh 2016.01.19--*/
/*.vtx file is corrected error moved stage by this prgrm*/

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

/* 構造体PLATENUM : pl#格納 */
struct PLATENUM{
	int curr;	/* 現在のpl# */
	int prev;	/* 手前のpl# */
};

/* 構造体COODINATE : 座標 */
struct COODINATE{
	double X;
	double Y;
	double Z;
};

/* 出力行のフォーマットを指定 */
void output(FILE *fp, int tr, int pl, double pos, double posx, double posy, double posz){
	char Value[32];
	sprintf(Value,"%02.5f",posx);
	if(strcmp(Value,"-0.00000")==0)
		posx = 0.;
	
	sprintf(Value,"%02.5f",posy);
	if(strcmp(Value,"-0.00000")==0)
		posy = 0.;
	
	sprintf(Value,"%02.5f",posz);
	if(strcmp(Value,"-0.00000")==0)
		posz = 0.;
	
	if(posx >= 0. && posy >=0. && posz >= 0.)
		fprintf(fp,"%d %2.d %1.1f   %02.5f   %02.5f   %.5f\n",tr,pl,pos,posx,posy,posz);
	else if(posx < 0. && posy >=0. && posz >= 0.) 
		fprintf(fp,"%d %2.d %1.1f  %02.5f   %02.5f   %.5f\n",tr,pl,pos,posx,posy,posz);
	else if(posx < 0. && posy <0. && posz >= 0.) 
		fprintf(fp,"%d %2.d %1.1f  %02.5f  %02.5f   %.5f\n",tr,pl,pos,posx,posy,posz);
	else if(posx < 0. && posy <0. && posz < 0.) 
		fprintf(fp,"%d %2.d %1.1f  %02.5f  %02.5f  %.5f\n",tr,pl,pos,posx,posy,posz);
	else if(posx >= 0. && posy <0. && posz >= 0.) 
		fprintf(fp,"%d %2.d %1.1f   %02.5f  %02.5f   %.5f\n",tr,pl,pos,posx,posy,posz);
	else if(posx >= 0. && posy >=0. && posz < 0.) 
		fprintf(fp,"%d %2.d %1.1f   %02.5f   %02.5f  %.5f\n",tr,pl,pos,posx,posy,posz);
	else if(posx >= 0. && posy <0. && posz < 0.) 
		fprintf(fp,"%d %2.d %1.1f   %02.5f  %02.5f  %.5f\n",tr,pl,pos,posx,posy,posz);
	else if(posx < 0. && posy >=0. && posz < 0.) 
		fprintf(fp,"%d %2.d %1.1f  %02.5f   %02.5f  %.5f\n",tr,pl,pos,posx,posy,posz);
}

/* 関数定義main()関数以降に記述 */
int ProcessFrame(char*, char*);
int TrackConnection(FILE*, FILE*, FILE*, char*);

int main()
{
	int loop;	/* ループカウンタ */
	int count;	/* 項目数 */
	int select;	/* ユーザー選択 */
	int selfile;	/* ユーザー指定File */
	int result;	/* 関数処理結果 */
	char linebuffer[2048];	/* 行の格納 */
	char fname[100][256];	/* File名 */
	struct LOADPATH Load[256];	/* 読み取りFileのフルパス */
	struct dirent *entry;	/* ディレクトリ操作構造体dirent */
	DIR *beam;	/* Directoryポインタ */
	FILE *tid;	/* Fileポインタ */
	
	/* 処理開始宣言 */
	printf("*** corr_stage starts ... ***\n");
	
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
	result = ProcessFrame(fname[selfile], linebuffer);
	if(result == 1){
		printf("[Error] :  Invalid File Format in '%s' !!\nexit soon ...\n", fname[selfile]);
		exit(0);
	}
	else if(result == -1)
		exit(1);
	
	/* 処理終了宣言 */
	printf("\n8<--- Stage Variations is corrected ! 8<-------\n");
	
	/* main() 戻り値0（正常終了） */
	return 0;
	
}

int ProcessFrame(char *fname, char *path){
	int pos;	/* 文字位置 */
	int loop;	/* ループカウンタ */
	int TNum;	/* 同じ飛跡の測定回数 */
	char linebuffer[2048];	/* 読み取り行格納 */
	char string[2048];	/* 文字列格納 */
	char cp[4];	/* １文字格納 */
	FILE *read, *out, *log;	/* Fileポインタ */
	
	/* 文字位置、文字列を初期化 */
	pos = 0;
	strcpy(string, "");
	
	/* Fileから_d手前までの文字列を取得 */
	while(fname[pos] != '_'){
		sprintf(cp, "%c", fname[pos]);
		strcat(string, cp);
		pos++;
	}
	
	/* フルパスでFile名を定義 */
	sprintf(linebuffer,"%s%s_d.vtx", path, string);
	
	/* 読み込みFileを開く */
	printf("\n... Focus On \"%s\" in process\n", linebuffer);
	if ((read = fopen(linebuffer, "r")) == NULL) {
		printf("input_file[%s] not open\n",linebuffer);
		return -1;
	}
	
	/* 測定回数カウントを0にする */
	TNum = 0;
	
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
			
			/* 測定回数も加算 */
			TNum++;
			
		}
		
	}
		
	/* ３でなければエラー */
	if(loop != 3){
		fclose(read);
		return 1;
	}
	
	/* Fileストリームを戻す */
	fseek(read, 0, SEEK_SET);
	
	/* 出力File名を決定 */
	sprintf(linebuffer,"%s%s_r.vtx", path, string);
	
	/* 出力Fileを開く */
	printf("... Focus On \"%s\" in process\n", linebuffer);
	if ((out = fopen(linebuffer, "w")) == NULL) {
		printf("output_file[%s] not open\n",linebuffer);
		return -1;
	}
	
	/* ログFile名を決定 */
	sprintf(linebuffer,"%sTrConnect.log", path);
	
	/* ログFileを開く */
	printf("... Focus On \"%s\" in process\n\n", linebuffer);
	if ((log = fopen(linebuffer, "w")) == NULL) {
		printf("Log_file[%s] not open\n",linebuffer);
		return -1;
	}
	
	/* ログFileに読み取りFile名を記述 */
		fprintf(log, "FILE = \"%s.vtx\"\n\n", string);

	/* 行の読み取り開始 */
	while(fgets(linebuffer,sizeof(linebuffer),read)!=NULL){
		
		/* Scan情報部分（a〜s）を出力 */
		fprintf(out,"%s",linebuffer);
		
		/* s文字に到達したら、ループ終了 */
		if(linebuffer[0] == 's')
			break;
	}
	
	/* 測定回数分を処理 */
	for(loop=0;loop<TNum;loop++){
		
		/* ログに処理対象の飛跡情報を記述 */
		fprintf(log, "TR# : %d --\n", loop+1);
		fprintf(log, "pl#,\tZ (mm),\tGrain,\tdX(um),\tdY(um)\tdZ(um),\tTrack,\tdX(um),\tdY(um),\tdZ(um),\tmove\n");
		
		/* 飛跡１本毎に視野移動前後を接続していく */
		if(TrackConnection(read, out, log, path) == -1)
			return -1;
		
	}
	
	/* Fileポインタを閉じる */
	fclose(read);
	fclose(out);
	fclose(log);
	
	/* ProcessFrame() 戻り値0（正常終了） */
	return(0);
	
}

int TrackConnection(FILE *read, FILE *out, FILE *log, char *path){
	int line;	/* 読み取り行数 */
	int move;	/* 視野移動回数 */
	int tr;	/* tr# */

/* inoh */
	int store=0;	/* パターンマッチ判定を書き込まない蓄積トリガー */
	double STOREPOS[10];/* store trackfile */
	struct COODINATE STORE[10]; /* store trackfile */

/* inoh end */	

	double Position[10000];	/* lay# */
	struct PLATENUM pl;	/* pl# */
	struct COODINATE Track[10000];	/* 読み取り座標 */
	struct COODINATE Med;	/* 視野移動により生じた座標差 */
	char linebuffer[2048];	/* 読み取り行格納 */
	
	/* 行数を初期化 */
	line = 0;
	
	/* 視野移動数を初期化 */
	move = 0;
	
	/* 改行文字 */
	fprintf(out,"\n");
	
	/* 初めの座標を取得 */
	fgets(linebuffer,sizeof(linebuffer),read);
	fgets(linebuffer,sizeof(linebuffer),read);
	sscanf(linebuffer,"%d %d %lf %lf %lf %lf %*lf",&tr,&pl.curr,&Position[line],&Track[line].X,&Track[line].Y,&Track[line].Z);
	
	/* 初期座標の出力 */
	fprintf(out,"%s",linebuffer);
	
	/* 手前のpl#に読み取ったpl#を保存 */
	pl.prev = pl.curr;
	
	/* 行数加算 */
	line++;
	
	/* 視野移動による差を初期化 */
	Med.X = 0;
	Med.Y = 0;
	Med.Z = 0;/* inoh */

	/* 以降の行を読んでいく */
	while(1){

		/* 視野移動トリガー初期化@inoh */
		int trigerA=0;
		int trigerB=0;
		
		/* 座標を取得 */
		fgets(linebuffer,sizeof(linebuffer),read);
		sscanf(linebuffer,"%d %d %lf %lf %lf %lf %*lf",&tr,&pl.curr,&Position[line],&Track[line].X,&Track[line].Y,&Track[line].Z);
		
		/* 最終行(# approximate range ...の部分)であれば、最終行を出力して終了 */
		if(linebuffer[0] == '#'){
			fprintf(out,"%s",linebuffer);
			break;
		}

/* ---------------------fixed by inoh------------------------ */

		/*パターンマッチを利用します*/
		/*Positionの変化が4連続で存在せず(TrigerAB)、かつ全体でz値の変化が10um以下の場合とする(TrigerAB only pokes coution)*/
		/*飛跡 <-> grain相互間の変化はないと考えているので変化を2um以下とする(TrigerA and B)*/

		/* triger (A) */

		if(Track[line].Z > Track[line-1].Z-0.002 && Track[line].Z < Track[line-1].Z+0.002 && trigerA!=1
			&&( ((int)(10.*Position[line-1]))==((int)(10.*Position[line])) )
			){/* Posの変化がないことを含める(測定点からベースまでの距離が近い時の対処) */
			trigerA=1;

			/* read2times */
			fgets(linebuffer,sizeof(linebuffer),read);
			sscanf(linebuffer,"%d %d %lf %lf %lf %lf %*lf",&tr,&pl.curr,&STOREPOS[0],&STORE[0].X,&STORE[0].Y,&STORE[0].Z);

			/* 最終行(# approximate range ...の部分)であれば、最終行を出力して終了 */
			if(linebuffer[0] == '#'){
				fprintf(out,"%s",linebuffer);
				trigerA=0;/* reset */
				break;
			}/* trigerA end */

			fgets(linebuffer,sizeof(linebuffer),read);
			sscanf(linebuffer,"%d %d %lf %lf %lf %lf %*lf",&tr,&pl.curr,&STOREPOS[1],&STORE[1].X,&STORE[1].Y,&STORE[1].Z);

		/* triger B */

			if(STORE[1].Z > STORE[0].Z-0.002 && STORE[1].Z < STORE[0].Z+0.002 && trigerA==1){
				trigerB=1;
			}
			else{
				/* trigetBに引っかからない場合(単なる測定の問題) */
				line = line + 2;

				/* STOREにあるデータをTRACK,Positionに移します */
				Track[line-1].X=STORE[0].X;
				Track[line-1].Y=STORE[0].Y;
				Track[line-1].Z=STORE[0].Z;
				Position[line-1]=STOREPOS[0];
				Track[line].X=STORE[1].X;
				Track[line].Y=STORE[1].Y;
				Track[line].Z=STORE[1].Z;
				Position[line]=STOREPOS[1];

				/* output */
				output(out, tr, pl.curr, Position[line-2], Track[line-2].X-Med.X, Track[line-2].Y-Med.Y, Track[line-2].Z-Med.Z);
				output(out, tr, pl.curr, Position[line-1], Track[line-1].X-Med.X, Track[line-1].Y-Med.Y, Track[line-1].Z-Med.Z);
				trigerA=0;/* reset */

			}/* trigerB end */
		
		}/* triger end */

		/* 8<-------本当はif文分岐なので繋げてもいいのですが、見にくいので区切ります----- */

		/* trigerAB */
		if(trigerA==1 && trigerB==1){
			if( ((int)(10.*STOREPOS[1])==(int)(10.*STOREPOS[0])) 
			&& ((int)(10.*STOREPOS[0])==(int)(10.*Position[line]))
			&& ((int)(10.*Position[line])==(int)(10.*Position[line-1])) ){/* 4つの乾板位置が一致 */

				/* 修正深度(z)が大きすぎる(over 10micron)場合 */
				if(STORE[1].Z - Track[line-1].Z >0.0100 || STORE[1].Z - Track[line-1].Z <-0.0100)
					printf("Caution: Stage.Z was corrected too large in Z%lf at pl%d.\n",STORE[1].Z,pl.curr);

				/* ステージ移動誤差の積算 */
				Med.X = Med.X + STORE[0].X - Track[line].X;
				Med.Y = Med.Y + STORE[0].Y - Track[line].Y;
				/* 測定方法は一緒だが、旧式の測定法を用いた場合はこちらを使う */
//				Med.X += STORE[1].X -Track[line-1].X;
//				Med.Y += STORE[1].Y -Track[line-1].Y;

				/* 深度(z)を修正する場合(over 5micron) */
				if(STORE[1].Z - Track[line-1].Z >0.0050 || STORE[1].Z - Track[line-1].Z <-0.0050)
					Med.Z = Med.Z + STORE[0].Z - Track[line].Z;
				/* 測定方法は一緒だが、旧式の測定法を用いた場合はこちらを使う */
//					Med.Z += STORE[1].Z -Track[line-1].Z;

				/* 積算終了 */

				/* ログに差を書き込む */
				fprintf(log, "%d\t%.4f\tGrain\t%.4f\t%.4f\t%.4f\tTrack\t%.4f\t%.4f\t%.4f\t%d\n", pl.curr, STORE[1].Z, (STORE[0].X-Track[line].X)*1000 ,(STORE[0].Y-Track[line].Y)*1000 ,(STORE[0].Z-Track[line].Z)*1000 , (STORE[1].X-Track[line-1].X)*1000 ,(STORE[1].Y-Track[line-1].Y)*1000 ,(STORE[1].Z-Track[line-1].Z)*1000 ,move);

				/* STOREにある実際の飛跡点を移す*/
				Track[line].X=STORE[1].X;
				Track[line].Y=STORE[1].Y;
				Track[line].Z=STORE[1].Z;
				
				/* 視野移動数 */
				move++;

			}//if
			else{
				line = line + 2;
				/* STOREにあるデータをTRACK,Positionに移します */
				Track[line-1].X=STORE[0].X;
				Track[line-1].Y=STORE[0].Y;
				Track[line-1].Z=STORE[0].Z;
				Position[line-1]=STOREPOS[0];
				Track[line].X=STORE[1].X;
				Track[line].Y=STORE[1].Y;
				Track[line].Z=STORE[1].Z;
				Position[line]=STOREPOS[1];

				/* output */
				output(out, tr, pl.curr, Position[line-2], Track[line-2].X-Med.X, Track[line-2].Y-Med.Y, Track[line-2].Z-Med.Z);
				output(out, tr, pl.curr, Position[line-1], Track[line-1].X-Med.X, Track[line-1].Y-Med.Y, Track[line-1].Z-Med.Z);
			}
			
			/* reset */
			trigerA=0;
			trigerB=0;
			/* next line */
			continue;
		}
		/* trigerAB end */

		/* 同じplate内であれば座標を出力していく */
		if(pl.curr == pl.prev){
		output(out, tr, pl.curr, Position[line], Track[line].X-Med.X, Track[line].Y-Med.Y, Track[line].Z-Med.Z);

/* ちょきちょき 8<------------------------ */

#ifdef ariga//有賀さんの使用していたプログラムの場合(パターンマッチ無)

		/* 視野移動の部分を認識する */
		/* ルール：前のZ座標との間隔が±0.5umの範囲内ならば、視野移動部分とみなす */
		/* 　　　　測定後にもし視野移動部分のZ座標差が0.5um以上になっていた場合は、*/
		/* 　　　　このプログラムを実行する前に、差を0.0umにしておく */	

		if(Track[line].Z > Track[line-1].Z-0.0005 && Track[line].Z < Track[line-1].Z+0.0005){
			
			/* 座標差を取得する */
			Med.X = Med.X + Track[line].X - Track[line-1].X;
			Med.Y = Med.Y + Track[line].Y - Track[line-1].Y;
			
			/* 視野移動数を加算 */
			move++;
			
			/* 移動により生じた座標差をlogに書き込む */
			fprintf(log, "%d\t%.4f\t%.4f\t%.4f\t%d\n", pl.curr, Track[line].Z, (Track[line].X-Track[line-1].X)*1000 ,(Track[line].Y-Track[line-1].Y)*1000 ,move);
			
			/* 次の行へ */
			continue;
		}
		
		
		/* 同じplate内であれば座標を出力していく */
		if(pl.curr == pl.prev){
			output(out, tr, pl.curr, Position[line], Track[line].X-Med.X, Track[line].Y-Med.Y, Track[line].Z);
#endif			
			/* 行数加算 */
			line++;
			
		}
		
		/* 次のplateへ移るときは */
		else{
			
			/* pl#を保存 */
			pl.prev = pl.curr;
			
			/* lay#を保存 */
			Position[0] = Position[line];
			
			/* 初期座標の引き継ぎ */
			Track[0].X = Track[line].X;
			Track[0].Y = Track[line].Y;
			Track[0].Z = Track[line].Z;
			
			/* 生じた座標差もクリアしておく */
			Med.X = 0;
			Med.Y = 0;
			
			/* 次のplateの初期位置を出力 */
			output(out, tr, pl.curr, Position[line], Track[line].X-Med.X, Track[line].Y-Med.Y, Track[line].Z-Med.Z);
			
			/* 行数も初期化 */
			line = 1;
		}
		
	}	/* while [以降の行を読んでいく] end */
	
	/* TrackConnection() 戻り値0（正常終了） */
	return(0);
}
