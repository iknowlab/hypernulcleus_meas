#include<stdio.h>
#include<stdlib.h>
#include<string>
#include<math.h>
#include<ieeefp.h>
#include<ctype.h>

/* 文字列の左右をトリミング */
char* strTriming(char *input){
	
	/* コメント：引数と戻り値の説明 */
	/*           ARG1 - 対象文字列 */
	/*           RTN  - トリミング後の先頭文字のポインタ */
	
	int pos;	/* 文字位置 */
	
	/* 文字列全体から検索 */
	for(pos=0; pos<strlen(input); pos++){
		
		/* 先頭からスペース文字とタブ文字を読み飛ばす */
		if(input[pos] == ' ' || input[pos] == '\t')
			continue;
		
		/* 何か文字を見つけたらループを終了 */
		break;
		
	}
	
	/* 引数の文字列の最終文字から検索 */
	for(int i=strlen(input)-1; i>pos; i--){
		
		/* スペース文字、タブ文字、改行文字でない文字ならば */
		if(input[i] != ' ' && input[i] != '\t' && input[i] != '\n'){
			
			/* １つ先の文字をNULL文字にする */
			input[i+1] = '\0';
			
			/* ループ終了 */
			break;
		}
		
	}
	
	/* 戻り値セット */
	return (input + pos);
	
}

/* 文字列を指定された文字で分割しそのポインタを返す */
int strsplit(char *output[], char *input, char *schr){
	
	/* コメント：引数と戻り値の説明 */
	/*           ARG1 - 分割後の文字ポインタ */
	/*           ARG2 - 分割前の文字列 */
	/*           ARG3 - 分割する文字 */
	/*           RTN  - 0なら正常終了、1は異常終了 */
	
	/* 変数宣言 */
	int cnum;	/* 文字数 */
	char cp[2];	/* 対象文字 */
	int index;	/* 配列番号 */
	
	/* 指定文字が１文字でなければエラー終了 */
	if(strlen(schr) != 1)
		return 1;
	
	/* 引数文字列の文字数 */
	cnum = strlen(input);
	
	/* 配列番号を初期化 */
	index = 0;
	
	/* 先頭文字にポインタを割り当てる */
	output[index] = input;
	
	/* 指定文字を検索 */
	for(int i=0; i<cnum; i++){
		
		/* 比較対象文字を取得 */
		sprintf(cp, "%c", input[i]);
		
		/* 指定文字と比較 */
		if(strcmp(cp, schr) == 0){
			
			/* 指定文字をnull文字に置き換える */
			input[i] = '\0';
			
			/* 配列番号加算 */
			index++;
			
			/* 指定文字の１つ先の文字にポインタを当てる */
			output[index] = input + i + 1;
			
			/* 次の文字へ */
			continue;
		}
		
	}
	
	/* 戻り値正常 */
	return 0;
	
}

/* ポインタ配列の整頓 */
void pntArray(char *string[]){
	
	/* nullポインタまでループ */
	for(int i=0; string[i]!=NULL; i++){
		
		/* 空文字でなければループを再帰 */
		if(strlen(strTriming(string[i])) > 0)
			continue;
		
		/* 配列の中身をシフト */
		for(int j=i; string[j]!=NULL; j++){
			if(string[j+1] !=NULL)
				string[j] = string[j+1];
			else
				string[j] = NULL;
		}
		
		/* 再評価する */
		i--;
		
	}
	
}

/* 第１引数の文字列が数値に変換可能であるかチェックする */
int CheckNumber(char *string){
	
	for(int i=0; i<strlen(string); i++){
		
		/* 小数点は許可 */
		if(string[i] == '.')
			continue;
		
		/* 符号を許可 */
		if(string[i] == '+' || string[i] == '-' || string[i] == '/' || string[i] == '*')
			continue;
		
		/* Eまたはeを許可 */
		if(string[i] == 'E' || string[i] == 'e'){
			
			if(i > 0)
				continue;
			
		}
		
		if(isdigit(string[i]) == 0)
			return 1;
		
	}
	
	return 0;
	
}

int main(int argc, char *fname[]){
	int loop;	/* ループカウンタ */
	int ent;	/* データ数 */
	int item;	/* 項目数 */
	double value;	/* 読み込みデータ */
	double ave[50];	/* 平均値 */
	double sde[50];	/* 標準偏差 */
	char *data[50];	/* 項目ポインタ */
	char linebuffer[2048];	/* 読み込み行 */
	FILE *read;	/* Fileポインタ */
	
	/* 読み込みFileを開く */
	if((read = fopen(fname[1], "r"))==NULL){
		printf("Input File[%s] not open!!\n", fname[1]);
		exit(1);
	}
	
	/* 初期クリア */
	for(loop=0; loop<50; loop++){
		ave[loop] = 0.;
		sde[loop] = 0.;
	}
	
	/* ポインタの初期化 */
	for(loop=0; loop<50; loop++)
		data[loop] = NULL;
	
	/* データ数のクリア */
	ent = 0;
	
	/* データの読み込み */
	while(fgets(linebuffer, sizeof(linebuffer), read)!=NULL){
		
		/* 改行文字を削除 */
		strcpy(linebuffer, strTriming(linebuffer));
		
		/* ポインタの初期化 */
		for(loop=0; data[loop]!=NULL; loop++)
			data[loop] = NULL;
		
		/* 項目別にポインタを取得 */
		strsplit(data, linebuffer, "\t");
		
		/* ポインタの整理 */
		pntArray(data);
		
		/* 数値変換が可能であるか確認 */
		for(loop=0; data[loop]!=NULL; loop++){
			
			/* 数値でなければ表示はNan Value */
			if(CheckNumber(data[loop]) != 0){
				ave[loop] = sqrt(-1.);
				continue;
			}
			
			/* データを保存 */
			sscanf(data[loop], "%lf", &value);
			
			/* 平均を計算するため値を加算 */
			ave[loop] += value;
			
		}
		
		/* 項目数の確認 */
		if(ent == 0)
			item = loop;
		else if(item != loop){
			printf("[Exception] :  The keys are not equal to the previous keys!!\n");
			
			fclose(read);
			exit(1);
		}
		
		/* データ数の加算 */
		ent++;
		
	}
	
	/* 見出しの表示 */
	printf("Average :  entries=%d\n", ent);
	
	/* 平均値の計算と結果の出力  */
	for(loop=0; loop<item; loop++){
		
		/* 文字を含む項目は無視 */
		if(isnan(ave[loop]) != 1)
			ave[loop] = ave[loop] / ((double) (ent));
		
		/* 値の表示 */
		printf("%lf\t", ave[loop]);
		
	}
	
	/* 改行する */
	printf("\n");
	
	/* Fileを閉じる */
	fclose(read);
	
	/* 戻り値 0(正常) */
	return 0;
	
}
