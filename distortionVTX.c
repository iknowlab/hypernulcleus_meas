#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <math.h>

/* 構造体LOADPATH : フルパス名 */
struct LOADPATH{
	char pName[64];	/* 飛跡名 */
	char Path[1024];	/* フルパス */
};

/* 構造体TRNUM : tr#格納 */
struct TRNUM{
	int curr;	/* 現在のtr# */
	int prev;	/* 手前の行のtr# */
	int dist;	/* distortion.vtxのtr# */
};

/* 構造体PLATENUM : pl#格納 */
struct PLATENUM{
	int curr;	/* 現在のpl# */
	int prev;	/* 手前のpl# */
	int dist;	/* 歪みFile用 */
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
int DistortionRevision(FILE*, FILE*, FILE*, char*);

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
	
	/* 処理開始宣言 */
	printf("*** distortionVTX.exe starts ... ***\n");
	
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
	sprintf(linebuffer, "%s/vtx/", Load[select].Path);
	
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
	
	
	/* 歪みFile[distortion.vtx]を参照し、飛跡から歪みを取り除く */
	result = ProcessFrame(fname[selfile], linebuffer);
	if(result == 1){
		printf("[Error] :  Invalid File Format in '%s' !!\nexit soon ...\n", fname[selfile]);
		exit(0);
	}
	else if(result == -1)
		exit(1);
	
	/* 処理終了宣言 */
	printf("*** End distortionVTX.exe ... ***\n");
	
	/* main() 戻り値0（正常終了） */
	return 0;
	
}

int ProcessFrame(char *fname, char *path){
	int loop;	/* ループカウンタ */
	int TNum;	/* 同じ飛跡の測定回数 */
	char linebuffer[2048];	/* 行読み取り変数 */
	char c[8];	/* tr# */
	char string[2048];	/* 文字列の格納 */
	FILE *read, *out, *dst;	/* Fileポインタ */
	
	/* フルパスでFile名を定義 */
	sprintf(linebuffer,"%s%s", path, fname);
	
	/* 読み込みFileを開く */
	printf("... Focus On \"%s\" in process\n", linebuffer);
	if ((read = fopen(linebuffer, "r")) == NULL) {
		printf("File[%s] not open\n", linebuffer);
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
	if(loop != 3){
		fclose(read);
		return 1;
	}
	
	/* Fileストリームを戻す */
	fseek(read, 0, SEEK_SET);
	
	/* 出力File名を決定 */
	sprintf(string, "%c", fname[0]);
	loop = 0;
	strcpy(c, "");
	while(strcmp(string,".") != 0){
		strcat(c,string);
		loop++;
		sprintf(string, "%c", fname[loop]);
	}
	sprintf(linebuffer, "%s_d.vtx", c);
	
	/* 出力Fileを開く */
	sprintf(linebuffer,"%sfin_d.vtx", path);
	printf("... Focus On \"%s\" in process\n", linebuffer);
	if ((out = fopen(linebuffer, "w")) == NULL) {
		printf("output_file[%s] not open\n",linebuffer);
		return -1;
	}
	
	/* distortion.vtxを開く */
	sprintf(linebuffer,"%sdistortion.vtx", path);
	printf("... Focus On \"%s\" in process\n\n", linebuffer);
	if ((dst = fopen(linebuffer, "r")) == NULL) {
		printf("input_file[%s] not open\n",linebuffer);
		return -1;
	}
	
	/* Scan情報（a～sまで）を読み飛ばす */
	while(fgets(linebuffer,sizeof(linebuffer),read)!=NULL){
		fprintf(out,"%s",linebuffer);
		
		/* summaryまで来たらループ終了 */
		if(linebuffer[0] == 's')
			break;
		
	}
	
	/* 測定回数分を処理 */
	for(loop=0;loop<TNum;loop++){
		
		/* 飛跡１本分の歪み補正処理 */
		if(DistortionRevision(read, dst, out, path) == -1)
			return -1;
		
	}
	
	/* Fileを閉じる */
	fclose(read);
	fclose(out);
	fclose(dst);
	
	/* ProcessFrame() 戻り値0（正常終了） */
	return 0;
	
}

int DistortionRevision(FILE *read, FILE *dst, FILE *out, char *path){
	int loop;	/* ループカウンタ */
	int Tline;	/* Track File用読み取り行番号 */
	int Dline;	/* distortion.vtx用読み取り行番号*/
	int Max;	/* ループ終了値 */
	int surf;	/* 表面カウンタ */
	double Position[300];	/* lay# */
	double Z[300];	/* SF補正後のZ座標 */
	double Dpos[1003];	/* distortion.vtxのlay# */
	double surface[3][4];	/* 表面座標 */
	double SFu, SFd;	/* 伸縮率(Shurinkage Factor) */
	struct TRNUM tr;	/* tr# */
	struct PLATENUM pl;	/* pl# */
	struct COODINATE Track[300];	/* Track座標 */
	struct COODINATE Distr[1003];	/* distrtion */
	struct COODINATE Out;	/* 歪み補正後の座標 */
	struct COODINATE Med;	/* 中間座標 */
	char linebuffer[2048];	/* 行読み取り変数 */
	char string[2048];	/* 文字列の格納 */
	FILE *sf;	/* Fileポインタ */
	
	/* 行番号を初期化 */
	Tline = 0;
	
	/* 改行文字 */
	fprintf(out,"\n");
	
	/* 初期座標を取得 */
	fgets(linebuffer,sizeof(linebuffer),read);
	fgets(linebuffer,sizeof(linebuffer),read);
	sscanf(linebuffer,"%d %d %lf %lf %lf %lf %*lf",&tr.curr,&pl.curr,&Position[Tline],&Track[Tline].X,&Track[Tline].Y,&Track[Tline].Z);
	
	/* tr#を格納 */
	tr.prev = tr.curr;
	
	/* pl#を格納 */
	pl.prev = pl.curr;
	
	/* 行番号追加 */
	Tline++;
	
	/* trackまた、distortion座標を読み取る */
	while(1){
		
		/* track座標の読み取り(1plate分) */
		while(linebuffer[0] != '#' && pl.curr == pl.prev){
			fgets(linebuffer,sizeof(linebuffer),read);
			sscanf(linebuffer,"%d %d %lf %lf %lf %lf %*lf",&tr.curr,&pl.curr,&Position[Tline],&Track[Tline].X,&Track[Tline].Y,&Track[Tline].Z);
			
			/* 行番号を加算 */
			Tline++;
		}
		
		/* 最終行("# approximate range ... "という部分)を保存 */
		strcpy(string, linebuffer);
			
		/* 最大行数を保存(最終行は除く) */
		Max = Tline-1;
		
		/* 表面認識変数の初期化 */
		surf = 0;
		
		/* 各表面の座標を取得 */
		for(Tline=0;Tline<Max;Tline++){
			
			/* 最上流面 */
			if(((int) (Position[Tline]*10.)) == 0){
				surface[0][0] = Track[Tline].X;
				surface[1][0] = Track[Tline].Y;
				surface[2][0] = Track[Tline].Z;
				surf++;
			}
			
			/* ベースと上流乳剤の境目 */
			else if(((int) (Position[Tline]*10.)) == 10){
				surface[0][1] = Track[Tline].X;
				surface[1][1] = Track[Tline].Y;
				surface[2][1] = Track[Tline].Z;
				surf++;
			}
			
			/* ベースと下流乳剤の境目 */
			else if(((int) (Position[Tline]*10.)) == 20){
				surface[0][2] = Track[Tline].X;
				surface[1][2] = Track[Tline].Y;
				surface[2][2] = Track[Tline].Z;
				surf++;
			}
			
			/* 最下流面 */
			else if(((int) (Position[Tline]*10.)) == 30){
				surface[0][3] = Track[Tline].X;
				surface[1][3] = Track[Tline].Y;
				surface[2][3] = Track[Tline].Z;
				surf++;
			}
		}
		
		/* trackがplateを貫通していた場合 */
		if(surf >= 4){
			SFu = 0.5/sqrt((surface[2][0]-surface[2][1])*(surface[2][0]-surface[2][1]));
			SFd = 0.5/sqrt((surface[2][2]-surface[2][3])*(surface[2][2]-surface[2][3]));
		}
		
		/* trackの静止点が上流である場合 */
		else if(surf >= 2 && surf < 4){
			SFu = 0.5/sqrt((surface[2][0]-surface[2][1])*(surface[2][0]-surface[2][1]));
			
			/* SFが格納されているFileを開く */
			sprintf(linebuffer,"%s/SF_pl%d.txt",path,pl.prev);
			if ((sf = fopen(linebuffer, "r")) == NULL) {
				printf("input_file[%s] not open\n",linebuffer);
				return -1;
			}
			
			/* 下流SFのみロード */
			fgets(linebuffer,sizeof(linebuffer),sf);
			fgets(linebuffer,sizeof(linebuffer),sf);
			sscanf(linebuffer,"%lf",&SFd);
			
			/* 閉じる */
			fclose(sf);
		}
		
		/* trackの静止点が下流ならば */
		else{
			
			/* SFが格納されているFileを開く */
			sprintf(linebuffer,"%s/SF_pl%d.txt",path,pl.prev);
			if ((sf = fopen(linebuffer, "r")) == NULL) {
				printf("input_file[%s] not open\n",linebuffer);
				return -1;
			}
			
			/* 両GelのSFをロード */
			fgets(linebuffer,sizeof(linebuffer),sf);
			sscanf(linebuffer,"%lf",&SFu);
			fgets(linebuffer,sizeof(linebuffer),sf);
			sscanf(linebuffer,"%lf",&SFd);
			
			/* 閉じる */
			fclose(sf);
		}
		
		/* SF補正後のZ座標を取得 */
		for(Tline=0;Tline<Max;Tline++){
			if(((int) (Position[Tline]*10.)) >= 0 && ((int) (Position[Tline]*10.)) <= 10)
				Z[Tline] = 1. - (Track[Max-1].Z - Track[Tline].Z)*SFu;
			else if(((int) (Position[Tline]*10.)) >= 20 && ((int) (Position[Tline]*10.)) <= 30)
				Z[Tline] = 0.5 - (surface[2][2] - Track[Tline].Z)*SFd;
		}
		
		/* 歪みFileの読み取り行番号を初期化 */
		Dline = 0;
		
		/* distortion.vtxの座標を読み取る */
		while(fgets(linebuffer,sizeof(linebuffer),dst)!=NULL){
			sscanf(linebuffer,"%d %d %lf %lf %lf %lf %*lf",&tr.dist,&pl.dist,&Dpos[Dline],&Distr[Dline].X,&Distr[Dline].Y,&Distr[Dline].Z);
			
			/* 同じpl#の座標のみ取得 */
			if(pl.dist == pl.prev)
				Dline++;
		}
		
		/* ストリームをFileの頭に戻す */
		fseek(dst, 0, SEEK_SET);
		
		/* (※1) 最下流面のZ座標だけ0.05um引く */
		Distr[0].Z = Distr[0].Z - 0.00005;
		
		/* (※2) 最上流面のZ座標も1um加算 */
		Distr[Dline].Z = Distr[Dline-1].Z + 0.001;
		
		/* (※3) 最上流面の座標を1つ上の配列にコピーしておく */
		Distr[Dline].X = Distr[Dline-1].X;
		Distr[Dline].Y = Distr[Dline-1].Y;
		
		/* コメント：(※)の処理理由 */
		/* 　　　　　以下のループ処理において、if()の条件に対応させるため。 */
		/* 　　　　　1) 基本的に浮動小数点を含む条件式はナンセンス。精度が悪い。 */
		/* 　　　　　2) loop=Max(ループ終了値)の時、loop+1は次のplateの最下流点(つまりZ=0.)。
		/* 　　　　　   この状態ではif()の条件をクリアできず、最上流点のみ未処理になってしまうから1umだけ追加した。 */
		/* 　　　　　3) loop=Max(ループ終了値)の時、loop+1は次のplateの最下流点(つまりX,Yともに0)。 */
		/* 　　　　　   このままだとMed.XおよびMed.Yが正しく求められないため。この方法はあまり得策ではない。 */
		
		/* track側のループ処理 */
		for(Tline=0;Tline<Max;Tline++){
			
			/* 歪みFile側のループ処理 */
			for(loop=0;loop<Dline;loop++){
				
				/* trackのZ座標に対応する歪みを探す */
				if(Z[Tline] >= Distr[loop].Z && Z[Tline] < Distr[loop+1].Z){
					
					/* trackと同じZ座標での歪みを求める(折れ線近似) */
					Med.X = Distr[loop].X + (Distr[loop+1].X-Distr[loop].X)*(Z[Tline]-Distr[loop].Z)/(Distr[loop+1].Z-Distr[loop].Z);
					Med.Y = Distr[loop].Y + (Distr[loop+1].Y-Distr[loop].Y)*(Z[Tline]-Distr[loop].Z)/(Distr[loop+1].Z-Distr[loop].Z);
					
					/* 歪みを補正 */
					Out.X = Track[Tline].X - Med.X;
					Out.Y = Track[Tline].Y - Med.Y;
					
					/* 出力 */
					output(out, tr.prev, pl.prev, Position[Tline], Out.X, Out.Y, Track[Tline].Z);
				}
			}
		}
		
		/* 最終行ならばループ終了 */
		if(string[0] == '#')
			break;
		
		/* track側の次plate最下流点を、最初の配列に格納 */
		Track[0].X = Track[Max].X;
		Track[0].Y = Track[Max].Y;
		Track[0].Z = Track[Max].Z;
		
		/* lay#の引き継ぎ */
		Position[0] = Position[Max];
		
		/* 行番号の初期化 */
		Tline = 1;
		
		/* pl#の引き継ぎ */
		pl.prev = pl.curr;
		
		/* tr#の引き継ぎ */
		tr.prev = tr.curr;
		
	}	/* while [trackまた、distortion座標を読み取る] end */
	
	/* vtx fileの最終行を出力 */
	fprintf(out,"%s",string);
	
	/* DistortionRevision 戻り値 0(正常終了) */
	return 0;
	
}	/* 関数終了 */
