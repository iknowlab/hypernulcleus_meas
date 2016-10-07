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

/* 構造体TRNUM : tr#格納 */
struct TRNUM{
	int curr;	/* 現在のtr# */
	int prev;	/* 手前の行のtr# */
	int dist;	/* distortion.vtxのtr# */
};

/* 行ストリームを1行戻す(ただしVTX FILEのみ適用可) */
void BackStreem(FILE *fp){
	char linebuffer[2048];
	fseek(fp, -90, SEEK_CUR);
	fgets(linebuffer,sizeof(linebuffer),fp);
}

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

int main(){
	int loop;	/* ループカウンタ */
	int count;	/* 項目数 */
	int select;	/* ユーザー選択 */
	int selfile;	/* ユーザー指定File */
	int flag;	/* 処理課程の分岐 used like Boolean */
	int result;	/* 関数処理結果 */
	char linebuffer[2048];	/* 行の格納 */
	char fname[100][256];	/* File名 */
	char command[1024];	/* 外部コマンド文字列(Unixのみ使用可能) */
	struct LOADPATH Load[100];	/* 読み取りFileのフルパス */
	struct dirent *entry;	/* ディレクトリ操作構造体dirent */
	DIR *beam;	/* Directoryポインタ */
	FILE *tid;	/* Fileポインタ */
		
	/* 処理開始宣言 */
	printf("*** distortion.exe starts ... ***\n");
	
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
	
	/* 処理過程変数を初期化 */
	flag = 1;
	
	/* plate数分ループ */
	for(loop=12; loop>1; loop--){
		
		/* パス名を定義 */
		sprintf(linebuffer, "%s/Beam/B%d/", Load[select].Path, loop);
		
		/* パス名のdirectoryを開く */
		if((beam = opendir(linebuffer)) == NULL){
			
			/* 開けなかったなら次のplateへ */
			continue;
		}
		
		/* 項目数Indexを初期化 */
		count = 1;
		
		/* 指定したdirectory内のFileを検索*/
		printf("\n[pl#%d] :  Found these Files : \n", loop);
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
		
		/* ユーザーにFile番号を選択させる */
		printf("Select number of the beam track file  :  ");
		scanf("%d", &selfile);
		
		/* 選択番号と配列番号を合わせる */
		selfile--;
		
		/* 入力された値のチェック */
		if(selfile >= count || selfile < 0){
			printf("[Error] :  Invalid number was selected\n");
			
			/* もう一度一覧表示、選択させる */
			loop++;
			continue;
		}
		
		/* directoryを閉じる */
		closedir(beam);
		
		/* 処理状況を表示 */
		printf("\nexecute ... \n\n");
		
		/* 1plate毎に歪みを取得し、distortion.vtxを生成 */
		result = ProcessFrame(fname[selfile], linebuffer);
		if(result == 1){
			printf("[Error] :  Invalid File Format in '%s' !!\nexit soon ...\n", fname[selfile]);
			exit(0);
		}
		else if(result == -1)
			exit(1);

		/* 処理変数による分岐 */
		/* コメント：flag=1ならば、生成したFileを識別対象の飛跡があるdirectoryにコピー */
		/* 　　　　　flag=0ならば、アペンドしていく */
		if(flag == 1)
			sprintf(command, "cp %sdistortion.vtx %s/vtx/\n", linebuffer, Load[select].Path);
		else
			sprintf(command, "cat %sdistortion.vtx >> %s/vtx/distortion.vtx\n",linebuffer, Load[select].Path);
		
		/* 外部コマンドとして実行 */
		system(command);
		
		/* 処理過程を変更 */
		flag = 0;
	}	/* for [plate数分ループ] end */
	
	/* 処理終了を告げる */
	printf("\n*** End distortion.exe ... ***\n");
	
	/* main() 戻り値 0（正常終了） */
	return 0;
}

int ProcessFrame(char *fname, char *path){
	int loop;	/* ループカウンタ */
	int line;	/* 読み取り行数 */
	int tNum;	/* Beam track本数 */
	int Max;	/* ループ終了値 */
	int lay;	/* 出力行番号 */
	int Zset;	/* 出力Z座標（μm単位） */
	
	// 測定fileのチェック用変数
	int SCheck[4];	/* 表面認識用変数 used like Boolean */
	double ThickEMu;	/* Up-Gelの厚さ */
	double ThickEMd;	/* Down-Gelの厚さ */
	double SFu;	/* Up-Gelの伸縮率 */
	double SFd;	/* Down-Gelの伸縮率 */
	double BaseValue;	/* ベース厚さ */
	double fx[1000];	/* 読み取りX座標 */
	double fy[1000];	/* 読み取りY座標 */
	double fz[1000];	/* 読み取りZ座標 */
	double sz[1000];	/* SF補正後のZ座標 */
	double surface[3][4];	/* emulsin layers : Vtx Fileと同じ  surface[a][b] */
							/* a:0=x,1=y,2=z */
							/* b=0 : 最上流面 */
							/*   1 : ベース上流面 */
							/*   2 : ベース下流面 */
							/*   3 : 最下流面 */
	double Bx0;	/* 最下流表面でのX座標 */
	double By0;	/* 最下流表面でのX座標 */
	double dxdz;	/* Base Angle of X-plane */
	double dydz;	/* Base Angle of Y-plane */
	char linebuffer[2048];	/* 読み取り行格納 */
	
	// 関数に渡す変数
	struct TRNUM tr;	/* tr# */
	int pl;	/* pl# */
	double pos;	/* Vtx Fileのlay# */
	double x;	/* 出力用X成分の歪み */
	double y;	/* 出力用Y成分の歪み */
	double z;	/* 出力用Z成分の歪み */
	double meanx[1003];	/* X成分の歪みの総和 */
	double meany[1003];	/* Y成分の歪みの総和 */
	double meanz[1003];	/* Z成分の歪みの総和 */
	
	FILE *read, *out;	/* Fileポインタ */
	
	/* 歪み格納変数の初期化 */
	for(loop=0; loop<1003; loop++){
		meanx[loop] = 0.;
		meany[loop] = 0.;
	}
	
	/* フルパスでFile名を定義 */
	sprintf(linebuffer,"%s%s", path, fname);
	
	/* 読み込みFileを開く */
	if((read = fopen(linebuffer,"r"))==NULL){
		printf("Input file \"%s\" not open!!\n",linebuffer);
		return -1;
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
	if(loop != 3){
		fclose(read);
		return 1;
	}
	
	/* Fileストリームを戻す */
	fseek(read, 0, SEEK_SET);
	
	/* 出力File名を決定 */
	sprintf(linebuffer,"%sdistortion.vtx",path);
	
	/* 出力Fileを開く */
	if((out = fopen(linebuffer,"w"))==NULL){
		printf("Input file \"%s\" not open!!\n",linebuffer);
		return -1;
	}
	
	/* VTX FILE中ののscan情報部分a～sを読み飛ばす */
	/* コメント：sを認識するまでストリームを移動 */
	while(linebuffer[0] != 's')
		fgets(linebuffer,sizeof(linebuffer),read);
	
	/* 測定回数の初期化 */
	tNum = 0;
	
	/* 座標の読み取り開始 */
	while(fgets(linebuffer,sizeof(linebuffer),read)!=NULL){
		
		/* tr#の取得 */
		sscanf(linebuffer, "%d %*d ", &tr.prev);

		/* 行の頭文字が#(VTXFILEの文末文字)まで読み飛ばす */
		if(linebuffer[0] != '#')
			continue;
		
		/* 表面チェック用変数の初期化 */
		for(loop=0; loop<4; loop++)
			SCheck[loop] = 0;
		
		/* ループカウンタを初期化 */
		loop = 0;
		
		/* Fileの最終行から座標の読み取りを始める(#部分は含まない) */
		while(pos < 3.0){
			
			/* ストリームを1行手前に戻す */
			BackStreem(read);
			
			/* １行を読み取る */
			fgets(linebuffer,sizeof(linebuffer),read);
			sscanf(linebuffer,"%d %d %lf %lf %lf %lf %*lf",&tr.curr,&pl,&pos,&fx[loop],&fy[loop],&fz[loop]);
			
			/* s文字またはtr#が変化したら */
			if(linebuffer[0] == 's' || tr.curr != tr.prev){
				
				/* カウンタを除算 */
				loop--;
				
				/* File冒頭部分の座標を取得(Scan情報部分a～sまでは含まない) */
				fgets(linebuffer,sizeof(linebuffer),read);
				fgets(linebuffer,sizeof(linebuffer),read);
				
				/* tr#に差異があればもう一行移動 */
				if(tr.curr != tr.prev)
					fgets(linebuffer,sizeof(linebuffer),read);

				sscanf(linebuffer,"%d %d %lf %lf %lf %lf %*lf",&tr.curr,&pl,&pos,&fx[loop], &fy[loop], &fz[loop]);
			}
						
			/* 最上流面の座標取得 */
			if(((int) (pos*10.)) == 0){
				surface[0][0] = fx[loop];
				surface[1][0] = fy[loop];
				surface[2][0] = fz[loop];
				
				/* 表面認識 */
				SCheck[0] += 1;
			}
			
			/* 上流乳剤とベースの境界座標を取得 */
			else if(((int) (pos*10.)) == 10){
				surface[0][1] = fx[loop];
				surface[1][1] = fy[loop];
				surface[2][1] = fz[loop];
				
				/* 表面認識 */
				SCheck[1] += 1;
			}
			
			/* 下流乳剤とベースの境界座標を取得 */
			else if(((int) (pos*10.)) == 20){
				surface[0][2] = fx[loop];
				surface[1][2] = fy[loop];
				surface[2][2] = fz[loop];
				
				/* 表面認識 */
				SCheck[2] += 1;
			}
			
			/* 最下流面の座標取得 */
			else if(((int) (pos*10.)) == 30){
				surface[0][3] = fx[loop];
				surface[1][3] = fy[loop];
				surface[2][3] = fz[loop];
				
				/* 表面認識 */
				SCheck[3] += 1;
			}
			
			/* ループカウンタ追加 */
			loop++;
		}
		
		/* ループ終了値 */
		Max = loop;
		
		/* Error Check */
		for(loop=0; loop<Max; loop++)
			
			/* Z方向への折り返しがあれば通知 */
			if(fz[loop] >= fz[loop-1])
				printf("  Error in tr#%d - pl#%d, -> [fz[%d]=%.5f] is bigger than [fz[%d]=%.5f] !!\n", tr.curr, pl, loop, fz[loop], loop-1, fz[loop-1]);
		
		/* Error Check */
		for(loop=0; loop<4; loop++)
			
			/* ２度以上同じ表面がある場合も通知 */
			if(SCheck[loop] > 1)
				printf("  Error in Surface Identification -> tr#%d - pl#%d !!\n", tr.curr, pl);
		
		
		/* 再びストリームを最終行に移動させる(while loop終了を促す) */
		while(linebuffer[0] != '#')
			fgets(linebuffer, sizeof(linebuffer), read);
		
		/* 上下Gelの厚さ */
		ThickEMu = sqrt((surface[2][0]-surface[2][1])*(surface[2][0]-surface[2][1]));
		ThickEMd = sqrt((surface[2][2]-surface[2][3])*(surface[2][2]-surface[2][3]));
		
		/* Shurinkage Factor & Base Thickness */
		SFu = 0.5/ThickEMu;
		SFd = 0.5/ThickEMd;
		BaseValue = sqrt((surface[2][1]-surface[2][2])*(surface[2][1]-surface[2][2]));
		
		/* Base track初期値 */
		Bx0 = surface[0][3];
		By0 = surface[1][3];
		
		/* Base Angle */
		dxdz = (surface[0][1]-surface[0][2])/BaseValue;
		dydz = (surface[1][1]-surface[1][2])/BaseValue;
		
		/* szの計算 */
		for(loop=0; loop<Max; loop++){
			if(fz[loop] >= surface[2][3] && fz[loop] <= surface[2][2])	// if in the DownGel
				sz[loop] = (fz[loop]-surface[2][3])*SFd;
			else if(fz[loop] >= surface[2][1] && fz[loop] <=surface[2][0])	// if in the UpGel
				sz[loop] = 0.5 + BaseValue + (fz[loop]-surface[2][1])*SFu;
		}
		
		/* distortionを求める。 */
		for(loop=0; loop<Max; loop++){
			fx[loop] = fx[loop] - (dxdz*(sz[loop]-sz[Max-1])+Bx0);
			fy[loop] = fy[loop] - (dydz*(sz[loop]-sz[Max-1])+By0);
		}
		
		/* ループ終了値(最終行#は含まない) */
		line = Max-1;
		
		/* 各種値を初期化 */
		lay = 0;
		z = 0.;		//最下流面の位置
		
		/* 出力Z座標が SF補正後のZ座標よりも下流側にいる間 */
		while(((int) (z*1000.)) <= ((int) (sz[0]*1000.))){
			
			/* 出力位置の歪みを求める */
			for(loop=line; loop>=0; loop--)
				if((z-sz[loop])*(z-sz[loop]) < (sz[loop-1]-sz[loop])*(sz[loop-1]-sz[loop])){
					meanx[lay] = meanx[lay] + fx[loop] + (fx[loop-1]-fx[loop])*(z-sz[loop])/(sz[loop-1]-sz[loop]);
					meany[lay] = meany[lay] + fy[loop] + (fy[loop-1]-fy[loop])*(z-sz[loop])/(sz[loop-1]-sz[loop]);
					meanz[lay] = z;
					
					break;
				}
			
				/* Z座標をum単位に変換 */
			Zset = ((int) (z*1000.));
			
			/* 出力行数を加算 */
			lay++;
			
			/* Z座標に1um足す */
			if(Zset == 500)
				z += BaseValue;
			else
				z += 0.001;
		}
		
		/* lay#を0.0にする */
		pos = 0.0;
		
		/* tr#の更新 */
		tr.prev = tr.curr;

		/* track数追加 */
		tNum++;
		
	}	/* while [座標の読み取り開始] end */
	
	/* Fileへの出力処理 */
	for(loop=0; loop<1002; loop++){
		
		/* 歪みの平均を算出 */
		/* コメント：測定したBeam trackが複数であれば tNum > 1となる */
		x = meanx[loop]/((double) tNum);
		y = meany[loop]/((double) tNum);
		
		/* Z座標はそのまま */
		z = meanz[loop];
		
		/* Output */
		if(loop == 0)
			output(out, tNum, pl, 3.0, x, y, z);
		else if(loop > 0 && loop < 500)
			output(out, tNum, pl, 2.5, x, y, z);
		else if(loop == 500)
			output(out, tNum, pl, 2.0, x, y, z);
		else if(loop == 501)
			output(out, tNum, pl, 1.0, x, y, z-BaseValue);
		else if(loop > 500 && loop < 1001)
			output(out, tNum, pl, 0.5, x, y, z-BaseValue);
		else if(loop == 1001)
			output(out, tNum, pl, 0.0, x, y, z-BaseValue);
		
	}
	
	/* Fileを閉じる */
	fclose(read);
	fclose(out);
	
	/* ProcessFrame() 戻り値0（正常終了） */
	return 0;
	
}	/* 関数終了 */
